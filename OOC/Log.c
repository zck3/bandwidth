/*============================================================================
  Log, a simple logging facility.
  Copyright (C) 2018, 2021, 2024 by Zack T Smith.

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

#include <stdarg.h>
#include <errno.h>

#if defined(__ANDROID__) && !defined(__TERMUX__)
#include <android/log.h>
#include <android_native_app_glue.h>
#include <errno.h>
#define LOG_TAG "ObjectOrientedC"
#endif

#include "ObjectOriented.h"
#include "Log.h"

#define kMaxLineLength (2048)
#define kMaxVarargsStringLength (8192)

static FILE *_outputFile = NULL;

bool Log_show_debug_output = false;

void Log_close()
{
#ifndef __ANDROID__
	if (_outputFile != NULL) {
		fclose (_outputFile);
	}
	_outputFile = NULL;
#endif
}

bool Log_reopen (const char *path)
{
#ifndef __ANDROID__
	if (!path) {
		return false;
	}
	FILE *f = fopen (path, "ab");
	if (!f) {
		perror ("fopen");
		return false;
	}
	Log_close ();
	_outputFile = f;
#endif
	return true;
}

void Log_print (const char *message)
{
	if (!message) {
		return;
	}
#if !defined(__ANDROID__) || defined(__TERMUX__)
	FILE *file = _outputFile == NULL ? stderr : _outputFile;
	fprintf (file, "%s", message);
#else
	__android_log_print(ANDROID_LOG_VERBOSE, LOG_TAG, "%s", message);
#endif
}

void Log_println (const char *message)
{
	if (!message) {
		return;
	}
#if !defined(__ANDROID__) || defined(__TERMUX__)
	FILE *file = _outputFile == NULL ? stderr : _outputFile;
	fprintf (file, "%s\n", message);
#else
	__android_log_print(ANDROID_LOG_VERBOSE, LOG_TAG, "%s", message);
#endif
}

void Log_error_printf (const char *funcName, const char *format, ...)
{ 
	char line [kMaxVarargsStringLength];
	snprintf (line, sizeof(line)-1, "Error(%s): ", funcName);
	size_t length = strlen(line);
	va_list args;
	va_start (args, format);
	vsnprintf (line+length, sizeof(line)-1-length, format, args);
	va_end (args);
	Log_println (line);
	fflush (NULL);
	exit (-1);
}

void Log_warning_printf (const char *funcName, const char *format, ...)
{ 
	char line [kMaxVarargsStringLength];
	snprintf (line, sizeof(line)-1, "Warning(%s): ", funcName);
	size_t length = strlen(line);
	va_list args;
	va_start (args, format);
	vsnprintf (line+length, sizeof(line)-1-length, format, args);
	va_end (args);
	Log_println (line);
	fflush (NULL);
}

void Log_debug_printf (const char *funcName, const char *format, ...)
{ 
	if (Log_show_debug_output) {
		char line [kMaxVarargsStringLength];
		snprintf (line, sizeof(line)-1, "Debug(%s): ", funcName);
		size_t length = strlen(line);
		va_list args;
		va_start (args, format);
		vsnprintf (line+length, sizeof(line)-1-length, format, args);
		va_end (args);
		Log_println (line);
		fflush (NULL);
	}
}

void Log_error (const char *funcName, const char *message)
{
#if !defined(__ANDROID__) || defined(__TERMUX__)
	char line [kMaxLineLength];
	snprintf (line, sizeof(line)-1, "Error(%s): %s", funcName, message);
	Log_println (line);
	fflush (NULL);
	exit (-1);
#else
	__android_log_print(ANDROID_LOG_ERROR, LOG_TAG, "%s", message);
#endif
}

void Log_errorNullParameter (const char *funcName)
{
	Log_error (funcName, "Null parameter.");
}

void Log_warning (const char *funcName, const char *message)
{
#if !defined(__ANDROID__) || defined(__TERMUX__)
	size_t needed = snprintf (NULL, 0, "Warning(%s): %s", funcName, message);
	char line [needed+1];
	snprintf (line, sizeof(line)-1, "Warning(%s): %s", funcName, message);
	Log_println (line);
	fflush (NULL);
#else
	__android_log_print(ANDROID_LOG_WARN, LOG_TAG, "%s", message);
#endif
}

void Log_perror (const char *funcName, const char *calledFunction)
{
	const char *message = strerror(errno);
	char line [kMaxLineLength];
	snprintf (line, sizeof(line)-1, "%s: %s", calledFunction ?: "", message ?: "");
	Log_warning (funcName, line);
}

void Log_debug (const char *funcName, const char *message)
{
	if (Log_show_debug_output) {
#if !defined(__ANDROID__) || defined(__TERMUX__)
		char line [kMaxLineLength];
		snprintf (line, sizeof(line)-1, "Debug(%s): %s", funcName, message);
		Log_println (line);
		fflush (NULL);
#else
		__android_log_print(ANDROID_LOG_DEBUG, LOG_TAG, "%s", message);
#endif
	}
}

