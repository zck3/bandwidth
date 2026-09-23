/*============================================================================
  CPU, an object-oriented C CPU information class.
  Copyright (C) 2019, 2023-2026 by Zack T Smith.

  Object-Oriented C is free software: you can redistribute it and/or modify
  it under the terms of the GNU Lesser General Public License as published
  by the Free Software Foundation, either version 3 of the License, or
  (at your option) any later version.
 
  Object-Oriented C is distributed in the hope that it will be useful,
  but WITHOUT ANY WARRANTY; without even the implied warranty of
  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
  GNU Lesser General Public License for more details.
 
  You should have received a copy of the GNU Lesser General Public License
  along with this software.  If not, see <http://www.gnu.org/licenses/>.

  The author may be reached at 3 at zs3 dot me.
 *===========================================================================*/

#define _GNU_SOURCE // Needs to be here to expose sched_getcpu().

#include "CPU.h"
#include "String.h"
#include "FileSystem.h"
#include "Utility.h"

#include <sys/types.h>
#include <dirent.h> // opendir
#include <ctype.h> // isdigit
#include <unistd.h>
#include <stdbool.h>

#if defined(__linux__) 
#include <sched.h>
#include <sys/utsname.h>
#include <sys/sysinfo.h>
#include <sys/auxv.h>
#endif

#if defined(__CYGWIN__)
#include <sys/utsname.h>
#include <sys/sysinfo.h>
#endif

#if defined(_WIN32)
#include <windows.h>
#endif

#ifdef __APPLE__
#include <sys/sysctl.h>
#endif

CPUClass *_CPUClass = NULL;

/* This is a fallback if no native solution exists to determine CPU make (as does for x86).
 */
static String* CPU_make (CPU* restrict self) 
{
	if (!self) {
		return NULL;
	}
	verifyCorrectClassOrSubclass(self,CPU);

#if defined(__ANDROID__) && defined(__TERMUX__)
	char *android_result = execute_and_return_first_line ("cat /sys/devices/soc0/vendor 2> /dev/null");
	if (android_result) {
		return _String(android_result);
	}
#endif

#if defined(__linux__) || defined(__CYGWIN__) 
	char *result = execute_and_return_first_line ("grep -i '^vendor_id.*:' /proc/cpuinfo | sed 's/^.*: //' | tail -1");
	if (result) {
		return String_withCString (result);
	}
#endif

	return NULL;
}

static String* CPU_model (CPU* restrict self) 
{
	if (!self) {
		return NULL;
	}
	verifyCorrectClassOrSubclass(self,CPU);

#if defined(__ANDROID__) && defined(__TERMUX__)
	char android_cpu_family[64];
	unsigned family_len = FileSystem_read_trimmed_line_from_file("/sys/devices/soc0/family",
			android_cpu_family, sizeof(android_cpu_family));
	if (family_len) {
		return _String(android_cpu_family);
	} 
#endif

	return NULL;
}

static unsigned CPU_registerSize (CPU* restrict self) 
{
	if (!self) {
		return 0;
	}
	verifyCorrectClassOrSubclass(self,CPU);

	// Size in bits.
	return 8 * sizeof(long);
}

static String* CPU_instructionSet (CPU* restrict self) 
{
	if (!self) {
		return NULL;
	}
	verifyCorrectClassOrSubclass(self,CPU);

	return NULL;
}

static void CPU_describe (CPU* restrict self, FILE *file)
{
	if (!self) {
		return;
	}
	verifyCorrectClassOrSubclass(self,CPU);
	
	fprintf (file ?: stdout, "%s\n", $(self, className));
}

void CPU_destroy (Any* self_)
{
        DEBUG_DESTROY;

	if (!self_) {
		return;
	}
	verifyCorrectClassOrSubclass(self_,CPU);

	CPU *self = self_;

        releaseAndClear (self->features);

	if (self->level1d_sizes) {
		free (self->level1d_sizes);
		self->level1d_sizes = NULL;
	}
	if (self->level1i_sizes) {
		free (self->level1i_sizes);
		self->level1i_sizes = NULL;
	}
	if (self->level2_sizes) {
		free (self->level2_sizes);
		self->level2_sizes = NULL;
	}
	if (self->level3_sizes) {
		free (self->level3_sizes);
		self->level3_sizes = NULL;
	}

	Object_destroy (self);
}

CPU* CPU_init (CPU* restrict self)
{
	ENSURE_CLASS_READY(CPU);

	if (self) {
		Object_init ((Object*)self);
		self->is_a = _CPUClass;

		MutableSet *mut = new(MutableSet);
		self->features = retain(mut);
	}
	return self;
}

static MutableSet* CPU_features (CPU* restrict self)
{
	if (!self) {
		return NULL;
	}
	verifyCorrectClassOrSubclass(self,CPU);

	// Features fetching is implemented in subclasses.

	return self->features;
}

static bool CPU_has128bitVectors (CPU* restrict self)
{
	if (!self) {
		return false;
	}
	verifyCorrectClassOrSubclass(self,CPU);

	return self->has128bitVectors;
}

static bool CPU_has256bitVectors (CPU* restrict self)
{
	if (!self) {
		return false;
	}
	verifyCorrectClassOrSubclass(self,CPU);

	return self->has256bitVectors;
}

static bool CPU_has512bitVectors (CPU* restrict self)
{
	if (!self) {
		return false;
	}
	verifyCorrectClassOrSubclass(self,CPU);

	return self->has512bitVectors;
}

static bool CPU_hasVectorUnit (CPU* restrict self)
{
	if (!self) {
		return false;
	}
	verifyCorrectClassOrSubclass(self,CPU);

	if (!$(self->features, count)) {
		(void) $(self,features);
	}

	return self->has128bitVectors 
	    || self->has256bitVectors
	    || self->has512bitVectors;
}

static int CPU_currentCore (CPU* restrict self)
{
	return cpu_current_core();
#ifdef __linux__
	int cpu = sched_getcpu ();
	if (cpu < 0) {
		return -1;
	}
	return cpu;
#elif defined(_WIN32)
	return GetCurrentProcessorNumber();
#elif defined(__APPLE__) && defined(__aarch64__)
	uint64_t mpidr;
	asm volatile("mrs %0, mpidr_el1" : "=r" (mpidr));
	uint32_t core_id = mpidr & 0xFF; 
	return core_id;
#else
	return -1;
#endif
}

static unsigned CPU_nCores (CPU* restrict self)
{
	if (!self) {
		return false;
	}
	verifyCorrectClassOrSubclass(self,CPU);

	if (!self->nCores) {
#if defined(__linux__) || defined(__CYGWIN__)
		long value = sysconf(_SC_NPROCESSORS_ONLN);
		if (value < 1) {
			value = get_nprocs_conf();
		}
		if (value > 0) {
			self->nCores = value;
		}
#endif

#ifdef __APPLE__
		int mib[2];
		int value = 0;
		size_t len = sizeof(int);
		mib[0] = CTL_HW;
		mib[1] = HW_NCPU;
		if (!sysctl (mib, 2, &value, (void*)&len, NULL, 0)) {
			if (value > 0) {
				self->nCores = value;
			}
		}

		char temp[16] = {0};
		size_t size = sizeof(temp);
		if (!sysctlbyname("machdep.cpu.core_count", temp, &size, NULL, 0)) {
			int ncores = atoi(temp);
			if (ncores > 0) {
				self->nCores = ncores;
			}
		}

		size = sizeof(temp);
		if (!sysctlbyname("machdep.cpu.cores_per_package", temp, &size, NULL, 0)) {
			int ncores = atoi(temp);
			if (ncores > 0) {
				self->nCores = ncores;
			}
		}
#endif

		// RULE: The first time we determine the number of cores,
		// also allocate the cache size arrays.
		//
		if (self->nCores) {
			size_t nBytes = sizeof(unsigned) * self->nCores;
			self->level1d_sizes = (unsigned *) malloc (nBytes);
			self->level1i_sizes = (unsigned *) malloc (nBytes);
			self->level2_sizes = (unsigned *) malloc (nBytes);
			self->level3_sizes = (unsigned *) malloc (nBytes);
			
			memset (self->level1d_sizes, 0, nBytes);
			memset (self->level1i_sizes, 0, nBytes);
			memset (self->level2_sizes, 0, nBytes);
			memset (self->level3_sizes, 0, nBytes);
		}
	}

	return self->nCores;
}

//-----------------------------------------------------------------------------
// Name:	macos_fetch_cache_info
// Purpose:	Fetches CPU cache size info from macOS.
// Returns:	True if the data has been fetched into our ivars.
//-----------------------------------------------------------------------------

#if defined(__APPLE__)
static bool macos_fetch_cache_info (CPU* restrict self)
{
	size_t l1d = 0;
	size_t l1i = 0;
	size_t l2 = 0;
	size_t l3 = 0;

	size_t value = 0;
	size_t len = sizeof(value);
	
	char *name = "hw.l1dcachesize";
	if (!sysctlbyname (name, &value, &len, NULL, 0)) {
		if (value > 0) {
			l1d = value >> 10;
		}
	}

	name = "hw.l1icachesize";
	value = 0;
	if (!sysctlbyname (name, &value, &len, NULL, 0)) {
		if (value > 0) {
			l1i = value >> 10;
		}
	}

	name = "hw.l2cachesize";
	value = 0;
	if (!sysctlbyname (name, &value, &len, NULL, 0)) {
		if (value > 0) {
			l2 = value >> 10;
		}
	}

	name = "hw.l3cachesize";
	value = 0;
	if (!sysctlbyname (name, &value, &len, NULL, 0)) {
		if (value > 0) {
			l3 = value >> 10;
		}
	}

	if (!l1d && !l1i && !l2 && !l3) {
		return false;
	}

	unsigned ncores = CPU_nCores(self);
	for (unsigned core = 0; core < ncores; core++) {
		self->level1i_sizes[core] = l1i;
		self->level1d_sizes[core] = l1d;
		self->level2_sizes[core] = l2;
		self->level3_sizes[core] = l3;
	}

	return true;
}
#endif 

//-----------------------------------------------------------------------------
// Name:	linux_fetch_cache_info
// Purpose:	Fetches CPU cache size info from (some) Linux kernels.
// Returns:	True if the data has been fetched into our ivars.
//-----------------------------------------------------------------------------
#if defined(__linux__) 
static bool linux_fetch_cache_info (CPU* restrict self)
{
	if (self->level1d_sizes[0]) {
		return true;
	}

	char path[PATH_MAX];

	bool got_data = false;

	unsigned ncores = CPU_nCores(self);
	for (unsigned core = 0; core < ncores; core++) {
		int index = 0;

		// This do-while block acts like a try-catch. 
		// Each 'break' exits the block when there's any error.
		do {
			snprintf (path, sizeof(path), "/sys/devices/system/cpu/cpu%u/cache/index%d/type", core, index);
	
			char cacheType[64] = {0};
			if (!FileSystem_read_trimmed_line_from_file (path, cacheType, sizeof(cacheType))) {
				break;
			}

			unsigned size;
			snprintf (path, sizeof(path), "/sys/devices/system/cpu/cpu%u/cache/index%d/size", core, index);
			bool got_size = FileSystem_read_unsigned_from_file (path, &size);
			if (!got_size) {
				break;
			}

			unsigned level;
			snprintf (path, sizeof(path), "/sys/devices/system/cpu/cpu%u/cache/index%d/level", core, index);
			bool got_level = FileSystem_read_unsigned_from_file (path, &level);
			if (!got_level) {
				break;
			}

			switch (level) {
			case 1:
				if (!strcasecmp ("instruction", cacheType)) {
					self->level1i_sizes[core] = size;
				}
				else if (!strcasecmp ("data", cacheType)) {
					self->level1d_sizes[core] = size;
				}
				got_data = true;
				break;

			case 2:
				self->level2_sizes[core] = size;
				got_data = true;
				break;

			case 3:
				self->level3_sizes[core] = size;
				got_data = true;
				break;

			default: break;
			}

			index++;
		} while(true);
	}

	return got_data;
}
#endif

static float CPU_maximumSpeed (CPU* restrict self, unsigned core)
{
	if (!self) {
		return false;
	}
	verifyCorrectClassOrSubclass(self,CPU);

	if (core >= CPU_nCores(self)) {
		return 0;
	}

#if defined(__linux__)
	char path[PATH_MAX];
	snprintf (path, sizeof(path), "/sys/devices/system/cpu/cpu%u/cpufreq/cpuinfo_max_freq", core);
	unsigned value = 0;
	if (FileSystem_read_unsigned_from_file (path, &value)) {
		float f = (float) value;
		f /= 1000000.f; // Convert kHz to GHz
		return f;
	}
#endif

	return 0.f;
}

static float CPU_minimumSpeed (CPU* restrict self, unsigned core)
{
	if (!self) {
		return false;
	}
	verifyCorrectClassOrSubclass(self,CPU);

	if (core >= CPU_nCores(self)) {
		return 0;
	}

#if defined(__linux__)
	char path[PATH_MAX];
	snprintf (path, sizeof(path), "/sys/devices/system/cpu/cpu%u/cpufreq/cpuinfo_min_freq", core);
	unsigned value = 0;
	if (FileSystem_read_unsigned_from_file (path, &value)) {
		float f = (float) value;
		f /= 1000000.f; // Convert kHz to GHz
		return f;
	}
#endif

	return 0.f;
}

static float CPU_currentSpeed (CPU* restrict self, unsigned core)
{
	if (!self) {
		return false;
	}
	verifyCorrectClassOrSubclass(self,CPU);

	if (core >= CPU_nCores(self)) {
		return 0;
	}

#if defined(__linux__)
	char path[PATH_MAX];
	snprintf (path, sizeof(path), "/sys/devices/system/cpu/cpu%u/cpufreq/cpuinfo_cur_freq", core);
	unsigned value = 0;
	if (FileSystem_read_unsigned_from_file (path, &value)) {
		float f = (float) value;
		f /= 1000000.f; // Convert kHz to GHz
		return f;
	}
#endif

	return 0.f;
}

//-----------------------------------------------------------------------------
// Name:	CPU_levelNCacheSize 
// Purpose:	Fetches one of the four CPU caches' size from the OS.
// Returns:	The cache size.
// Note:	This is also a fallback if the CPU-specific call fails.
//-----------------------------------------------------------------------------
unsigned CPU_levelNCacheSize (CPU *self, unsigned core, unsigned level, bool is_data) 
{
	if (!self) {
		return 0;
	}
	verifyCorrectClassOrSubclass(self,CPU);

	if (core >= CPU_nCores(self)) {
		return 0;
	}

	// If the cache data isn't loaded, fetch it now.
	if (!self->level1d_sizes || !self->level1d_sizes[0]) {
		bool got_data = false;
#if defined(__linux__)
		got_data = linux_fetch_cache_info (self);
#elif defined(__APPLE__)
		got_data = macos_fetch_cache_info (self);
#endif
		if (!got_data) {
			return 0;
		}
	}

	unsigned size = 0;

	switch (level) {
	case 1:
		if (is_data) {
			size = self->level1d_sizes[core];
		} else {
			size = self->level1i_sizes[core];
		}
		break;
	case 2:
		size = self->level2_sizes[core];
		break;
	case 3:
		size = self->level3_sizes[core];
		break;
	}

	return size;
}

static String* CPU_family (CPU* restrict self) 
{
	// Implemented in subclass.
	return NULL;
}

static unsigned CPU_nInstructionTLBEntries (CPU* restrict self) 
{
	// Implemented in subclass.
	return 0;
}

static unsigned CPU_nDataTLBEntries (CPU* restrict self) 
{ 
	// Implemented in subclass.
	return 0;
}

static unsigned CPU_nUnifiedTLBEntries (CPU* restrict self) 
{
	// Implemented in subclass.
	return 0;
}

/* Get the CPU temperature by physical core.
 * If hyperthreading is enabled, divide the core number by 2.
 */
static float CPU_temperature (CPU* restrict self, unsigned sought_core) 
{
	if (!self) {
		return false;
	}
	verifyCorrectClassOrSubclass(self,CPU);

	if (sought_core >= CPU_nCores(self)) {
		return 0.f;
	}

#if defined(__linux__) 
	/* For reference, my BASH function.
	function t3 {
		  pushd /sys/bus/platform/devices > /dev/null
			  cd coretemp.0/hwmon/hwmon?
			  for a in temp?_label; do
				I=`echo $a | sed "s/label/input/"`
				L=`cat $a`
				T1000=`cat $I`
				T=$(($T1000/1000))
				echo $L: $T"°C"
			  done
		  popd > /dev/null
	}
	*/

	// NOTE: We have to examine the labels to find the
	// correct file with the core number that we want.

	char hwmon_path[PATH_MAX];
	snprintf (hwmon_path, sizeof(hwmon_path), "/sys/bus/platform/devices/coretemp.0/hwmon");
	DIR *dir = opendir (hwmon_path);
	if (!dir) {
		return 0.;
	}
	// Just get the first directory.
	struct dirent *de;
	bool found = false;
	while ((de = readdir(dir))) {
		if (has_prefix (de->d_name, "hwmon")) {
			if (isdigit(de->d_name[5])) {
				strncat (hwmon_path, "/", sizeof(hwmon_path)-1);
				strncat (hwmon_path, de->d_name, sizeof(hwmon_path)-1);
				strncat (hwmon_path, "/", sizeof(hwmon_path)-1);
				found = true;
				break;
			}
		}
	}
	closedir (dir);
	if (!found) {
		return false;
	}
	int temperature_index = 1;
	int pathlen = strlen (hwmon_path);
	while (temperature_index < 128) {
		snprintf (hwmon_path+pathlen, sizeof(hwmon_path) - pathlen, "temp%d_label", temperature_index);
		char line [256];
		unsigned len = FileSystem_read_trimmed_line_from_file (hwmon_path, line, 256);
		if (!len) {
			break;
		}
		if (has_prefix (line, "Core ")) {
			int found_core = atoi (line + 5);
			if (found_core == sought_core) {
				hwmon_path[pathlen] = 0;
				snprintf (hwmon_path+pathlen, sizeof(hwmon_path) - pathlen, "temp%d_input", temperature_index);
				len = FileSystem_read_trimmed_line_from_file (hwmon_path, line, 256);
				if (len) {
					int value = atoi (line);
					return (float)value / 1000.f;
				}
			}
		}
		temperature_index++;
	}
#endif

	return 0.f;
}

static void CPU_flushDataCacheAt (CPU* restrict self, void* address, size_t size)
{
	// Implemented in subclass.
}

static void CPU_memoryFence (CPU* restrict self)
{
	// Implemented in subclass.
}

CPUClass* CPUClass_init (CPUClass *class)
{
	SET_SUPERCLASS(Object);

	SET_OVERRIDDEN_METHOD_POINTER(CPU,describe);
        SET_OVERRIDDEN_METHOD_POINTER(CPU,destroy);

	SET_METHOD_POINTER(CPU,family);
	SET_METHOD_POINTER(CPU,make);
	SET_METHOD_POINTER(CPU,model);
	SET_METHOD_POINTER(CPU,nCores);
	SET_METHOD_POINTER(CPU,currentCore);
	SET_METHOD_POINTER(CPU,features);
	SET_METHOD_POINTER(CPU,registerSize);
	SET_METHOD_POINTER(CPU,instructionSet);
	SET_METHOD_POINTER(CPU,levelNCacheSize);
	SET_METHOD_POINTER(CPU,hasVectorUnit);
	SET_METHOD_POINTER(CPU,has128bitVectors);
	SET_METHOD_POINTER(CPU,has256bitVectors);
	SET_METHOD_POINTER(CPU,has512bitVectors);
	SET_METHOD_POINTER(CPU,maximumSpeed);
	SET_METHOD_POINTER(CPU,minimumSpeed);
	SET_METHOD_POINTER(CPU,currentSpeed);
	SET_METHOD_POINTER(CPU,temperature);
	SET_METHOD_POINTER(CPU,nInstructionTLBEntries);
	SET_METHOD_POINTER(CPU,nDataTLBEntries);
	SET_METHOD_POINTER(CPU,nUnifiedTLBEntries);
	SET_METHOD_POINTER(CPU,flushDataCacheAt);
	SET_METHOD_POINTER(CPU,memoryFence);

	VALIDATE_CLASS_STRUCT(class);
	return class;
}

