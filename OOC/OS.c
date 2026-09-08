/*============================================================================
  OS, an object-oriented C operating system information class.
  Copyright (C) 2019, 2023-2024, 2026 by Zack T Smith.

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

#include "OS.h"
#include "Utility.h"
#include "Log.h"

#include <sys/types.h>
#include <unistd.h>
#include <ctype.h>

#ifdef __linux__
#include <sys/utsname.h>
#include <sys/sysinfo.h>
#endif

#ifdef __CYGWIN__
#include <sys/utsname.h>
#include <sys/sysinfo.h>
#endif

#ifdef __APPLE__
#include <sys/sysctl.h>
#endif

OSClass *_OSClass = NULL;

String* OS_type () 
{
	char *type = NULL;
#ifdef __CYGWIN__
	type = "Windows/Cygwin";
#elif defined(__ANDROID__)
	type = "Android";
#elif defined(__linux__)
	type = "Linux";
#elif defined(__APPLE__)
	type = "macOS";
#elif defined(__WIN32__) || defined(__WIN64__)
	type = "Windows";
#endif

	return type ? String_withCString (type) : NULL;
}

String* OS_name () 
{
#if defined(__linux__) 
 #if !defined(__ANDROID__)
	//char *name = execute_and_return_first_line("grep ^ID= /etc/os-release|sed 's/^ID=//'|sed 's/\"//g'");
	char *name = execute_and_return_first_line("grep ^NAME= /etc/os-release|sed 's/^NAME=//'|sed 's/\"//g'");
	if (name && strlen(name)) {
		return _String(name);
	}
 #endif
#elif defined(__APPLE__)
	char *name = execute_and_return_first_line("grep 'LICENSE AGREEMENT FOR macOS' '/System/Library/CoreServices/Setup Assistant.app/Contents/Resources/en.lproj/OSXSoftwareLicense.rtf' | sed 's/^.*macOS //' | sed 's/\\\\//'");
	if (name && strlen(name)) {
		return _String(name);
	}
#endif

	return NULL;
}

float OS_version () 
{
#if defined(__linux__)
	char *name = execute_and_return_first_line("grep ^VERSION_ID= /etc/os-release|sed 's/^VERSION_ID=//'|sed 's/\"//g'");
	if (name && strlen(name)) {
		return atof(name);
	}
#elif defined(__APPLE__)
        char *result = execute_and_return_first_line ("system_profiler SPSoftwareDataType | grep 'System Version:' | sed 's/^.*System Version: //' | sed 's/(.*//' | sed 's/macOS //'");
	if (result) {
		return atof(result);
	}
#endif

	return 0.f;
}

String* OS_kernelName () 
{
	char *type = NULL;
#ifdef __CYGWIN__
	type = "WindowsNT";
#elif defined(__ANDROID__)
	type = "Linux";
#elif defined(__linux__)
	type = "Linux";
#elif defined(__APPLE__)
	type = "Darwin";
#elif defined(__WIN32__) || defined(__WIN64__)
	type = "WindowsNT";
#endif

	return type ? String_withCString (type) : NULL;
}

String* OS_kernelRelease () 
{
#ifdef __linux__
	struct utsname uts;
	uname (&uts);
	const char *cstring = uts.release;

	// Truncate to leave out extraneous build info.
	static char truncated_str [512];
	strncpy (truncated_str, cstring, sizeof(truncated_str)-1);
	char *ptr = strchr (truncated_str, '(');
	if (ptr) {
		*ptr = 0;
	}
	ptr = strchr (truncated_str, '-');
	if (ptr) {
		*ptr = 0;
	}

	return strlen(cstring) ? String_withCString (truncated_str) : NULL;
#endif

#if defined(__APPLE__)
	// E.g. Kernel Version: Darwin 24.2.0
        char *result = execute_and_return_first_line ("system_profiler SPSoftwareDataType | grep 'Kernel Version:' | sed 's/^.*Kernel Version: Darwin//' | sed 's/(.*//'");
	if (result) {
		while (isspace(*result)) {
			result++;
		}
		return String_withCString (result);
	}
#endif

#if defined(__WIN32__) || defined(__WIN64__)
	// If have windows.h, call GetVersionEx() or VerifyVersionInfo()
	// If have uname(), just use Linux code.
#endif

	return NULL;
}

void OS_describe (OS* self, FILE *file)
{
	if (!self) {
		return;
	}
	verifyCorrectClassOrSubclass(self,OS);
	
	fprintf (file ?: stdout, "%s\n", $(self, className));
}

void OS_print (OS *self, FILE* file)
{
	if (!self) {
		return;
	}
	verifyCorrectClassOrSubclass(self,OS);

	if (!file) {
		file = stdout;
	}

	String *name = OS_name();
	$(name, print, file);
	release(name);

	fprintf (file ?: stdout, " %g\n", OS_version());
}

bool OS_equals (OS *self, void *other_) 
{ 
	if (!self || !other_) 
		return false;
	verifyCorrectClassOrSubclass(self,OS);
	OS *other = (OS*) other_;
	verifyCorrectClassOrSubclass(other,OS);

	return false;
}

void OS_destroy (Any* self)
{
        DEBUG_DESTROY;

	if (!self) {
		return;
	}
	verifyCorrectClassOrSubclass(self,OS);

	Object_destroy ((Object*)self);
}

OS* OS_init (OS* restrict self)
{
	ENSURE_CLASS_READY(OS);

	if (self) {
		Object_init ((Object*)self);
		self->is_a = _OSClass;
	}
	return self;
}

long OS_load ()
{
#if defined(__linux__) 
	struct sysinfo info;
	if (!sysinfo (&info)) {
		return info.loads[0];
	} else {
		perror("sysinfo");
	}
#endif

	return 0;
}

long OS_uptime ()
{
#if defined(__linux__) 
	struct sysinfo info;
	if (!sysinfo (&info)) {
		return info.uptime;
	} else {
		perror("sysinfo");
	}
#endif

	return 0;
}

unsigned OS_memoryPageSize ()
{
	return getpagesize();
}

unsigned OS_userid ()
{
	return (unsigned) getuid();
}

String* OS_username ()
{
	char *username = getlogin() ?: "";
	String *result = String_withCString(username);
	return result;
}

size_t OS_freeRAM ()
{
	unsigned long freeMegabytes = 0;

#if defined(__linux__) 
	struct sysinfo info;

	if (sysinfo(&info)) {
		Log_perror(__FUNCTION__, "sysinfo");
		return 0;
	}

	freeMegabytes = info.freeram / ONE_MEGABYTE;

#elif defined(__APPLE__)
	int mib[] = { CTL_HW, HW_PHYSMEM };
	size_t size = 0;
	size_t len = sizeof(size);
	if (0 == sysctl (mib, 2, &size, &len, NULL, 0)) {
		freeMegabytes = size / ONE_MEGABYTE;
	}
#endif

	return freeMegabytes;
}

size_t OS_totalRAM ()
{
	unsigned long totalMegabytes = 0;

#if defined(__linux__) 
	struct sysinfo info;

	if (sysinfo(&info)) {
		Log_perror(__FUNCTION__, "sysinfo");
		return 0;
	}

	totalMegabytes = info.totalram / ONE_MEGABYTE;

#elif defined(__APPLE__)
	int mib[] = { CTL_HW, HW_MEMSIZE };
	size_t size = 0;
	size_t len = sizeof(size);
	if (0 == sysctl (mib, 2, &size, &len, NULL, 0)) {
		totalMegabytes = size / ONE_MEGABYTE;
	}
#endif

	return totalMegabytes;
}

OSClass* OSClass_init (OSClass *class)
{
	SET_SUPERCLASS(Object);

	SET_OVERRIDDEN_METHOD_POINTER(OS,describe);
        SET_OVERRIDDEN_METHOD_POINTER(OS,destroy);
        SET_OVERRIDDEN_METHOD_POINTER(OS,equals);

	VALIDATE_CLASS_STRUCT(class);
	return class;
}

bool OS_isRoot ()
{
	uid_t uid = geteuid ();
	return uid == 0;
}

