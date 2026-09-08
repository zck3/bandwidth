/*============================================================================
  Hardware, an object-oriented C hardware information class.
  Copyright (C) 2019, 2023-2024 by Zack T Smith.

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

#include "Hardware.h"
#include "FileSystem.h"
#include "Utility.h"

#include <sys/types.h>
#include <unistd.h>
#include <ctype.h>

#if defined(__linux__) || defined(__CYGWIN__)
#include <sys/sysinfo.h>
#endif

#ifdef HAVE_LIBUSB
#include <libusb.h>
#endif

#ifdef __APPLE__
#include <sys/sysctl.h>
#endif

HardwareClass *_HardwareClass = NULL;

void Hardware_describe (Hardware* self, FILE *file)
{
	if (!self) {
		return;
	}
	verifyCorrectClassOrSubclass(self,Hardware);
	
	fprintf (file ?: stdout, "%s\n", $(self, className));
}

void Hardware_print (Hardware *self, FILE* file)
{
	if (!self) {
		return;
	}
	verifyCorrectClassOrSubclass(self,Hardware);

	if (!file) {
		file = stdout;
	}
}

void Hardware_destroy (Any* self_)
{
        DEBUG_DESTROY;

	if (!self_) {
		return;
	}
	verifyCorrectClassOrSubclass(self_,Hardware);

	Hardware *self = self_;

	Object_destroy (self);
}

Hardware* Hardware_init (Hardware* restrict self)
{
	ENSURE_CLASS_READY(Hardware);

	if (self) {
		Object_init ((Object*)self);
		self->is_a = _HardwareClass;
	}
	return self;
}

static float Hardware_temperature (Hardware* restrict self, unsigned thermalZone)
{
	if (!self) {
		return false;
	}
	verifyCorrectClassOrSubclass(self,Hardware);

#if defined(__linux__) || defined(__CYGWIN__)
	char path [PATH_MAX];
	snprintf (path, sizeof(path), "/sys/class/thermal/thermal_zone%u/temp", thermalZone);
	if (FileSystem_fileExistsWithCString (path)) {
		long long temp;
		if (FileSystem_read_longlong_from_file (path, &temp)) {
			return 0.001f * (float)temp;
		}
	}
#endif

#if defined(__APPLE__) 
	// Temperature is no longer available from ioreg utility.
#endif

	return -1.f;
}

static unsigned Hardware_totalRAM (Hardware* restrict self)
{
	if (!self) {
		return false;
	}
	verifyCorrectClassOrSubclass(self,Hardware);

	unsigned long totalMegabytes = 0;

#if defined(__linux__) || defined(__APPLE__) || defined(__CYGWIN__)
        unsigned long n_pages = sysconf(_SC_PHYS_PAGES);
	unsigned long bytes_per_page = getpagesize();

	// In the event that this is a 32-bit system with exactly 4GB of RAM,
	// it's necessary to avoid an arithmetic overflow by careful
	// shifting and multiplication.
	totalMegabytes = ((n_pages >> 2) * (bytes_per_page >> 2) >> 16);
#endif

	return totalMegabytes;
}

bool Hardware_onBatteryPower (Hardware *self) 
{
	if (!self) {
		return NULL;
	}
	verifyCorrectClassOrSubclass(self,Hardware);

	FILE *file = fopen ("/sys/class/power_supply/BAT0/status", "r");
	if (!file) {
		return false;
	}

	char line[64];
	if (!fgets (line, sizeof(line)-1, file)) {
		fclose (file);
		return false;
	}

	fclose(file);

	line[sizeof(line)-1] = 0;
	return 0 == strcasecmp (line, "Discharging");
}

String* Hardware_systemMake (Hardware* restrict self) 
{
	if (!self) {
		return NULL;
	}
	verifyCorrectClassOrSubclass(self,Hardware);

	char *result = NULL;

#if defined(__ANDROID__) && defined(__TERMUX__)
	result = execute_and_return_first_line ("getprop ro.product.brand");
	if (result && strlen(result)) {
		result[0] = toupper(result[0]);
	}
#endif

#if defined(__APPLE__)
	result = "Apple";
#endif

#if defined(__linux__)
    #if defined(__x86_64__) || defined(__i386__)
	if (!result && FileSystem_fileExistsWithCString("/sys/devices/platform/thinkpad_acpi")) {
		result = "Lenovo";
	}
    #endif
#endif

	return result? _String(result) : NULL;
}

String* Hardware_systemModel (Hardware *self) 
{
	if (!self) {
		return NULL;
	}
	verifyCorrectClassOrSubclass(self,Hardware);

	char *result = NULL;

#if defined(__ANDROID__) && defined(__TERMUX__)
	result = execute_and_return_first_line ("getprop ro.product.model");
	if (result && !strlen(result)) {
		result = NULL;
	}
#endif

#ifdef __APPLE__
	char name[48];
	size_t size = sizeof(name);
	if (!sysctlbyname("hw.model", name, &size, NULL, 0)) {
		if (name[0]) {
			name[size] = 0;
			return String_withCString (name);
		}
	}
#endif

#if defined(__linux__) 
	// Best case, the kernel provides the model name, as with AsahiLinux.
	if (!result) {
		if (FileSystem_fileExistsWithCString("/proc/device-tree/model")) {
			char name[256] = {0};
			if (0 < FileSystem_read_trimmed_line_from_file ("/proc/device-tree/model", name, sizeof(name))) {
				return _String (name);
			}
		}

		if (FileSystem_fileExistsWithCString("/sys/devices/platform/thinkpad_acpi")) {
			result = "Thinkpad";
		}
	}
#endif

	return result? _String(result) : NULL;
}

HardwareClass* HardwareClass_init (HardwareClass *class)
{
	SET_SUPERCLASS(Object);

	SET_OVERRIDDEN_METHOD_POINTER(Hardware,describe);
        SET_OVERRIDDEN_METHOD_POINTER(Hardware,destroy);

	SET_METHOD_POINTER(Hardware,totalRAM);
	SET_METHOD_POINTER(Hardware,systemMake);
	SET_METHOD_POINTER(Hardware,systemModel);
	SET_METHOD_POINTER(Hardware,temperature);
	SET_METHOD_POINTER(Hardware,onBatteryPower);
	
	VALIDATE_CLASS_STRUCT(class);
	return class;
}

