/*=========================================================================
 * This file is part of Object-Oriented C.
 * Copyright (C) 2021, 2023, 2024 Zack T Smith.
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU Lesser General Public License as published by
 * the Free Software Foundation; either version 3 of the License, or
 * (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU Lesser General Public License for more details.
 *
 * You should have received a copy of the GNU Lesser General Public License
 * along with this program; if not, write to the Free Software
 * Foundation, Inc., 51 Franklin St, Fifth Floor, Boston, MA  02110-1301  USA
 *
 * The author may be reached at 3 at zs3 dot me.
 *=======================================================================*/

// This code is borrowed from my zBenchmark project.

#include "ObjectOriented.h"
#include "Utility.h"
#include "FileSystem.h"

#include <stdio.h>
#include <stdlib.h>
#include <sys/stat.h>
#include <sys/time.h>
#include <ctype.h> // isspace

#include "Log.h"

//----------------------------------------------------------------------------
// Name:	has_prefix
// Purpose:	Check whether C string has specified prefix, case-sensitive.
// Returns:	True if it has the prefix.
//----------------------------------------------------------------------------
bool has_prefix (const char *string, const char *prefix)
{
	if (!string || !prefix)
		return false;

	size_t stringLength = strlen(string);
	size_t prefixLength = strlen(prefix);
	if (prefixLength > stringLength)
		return false;

	return 0 == strncmp (string, prefix, prefixLength);
}

//----------------------------------------------------------------------------
// Name:	has_suffix
// Purpose:	Check whether C string has specified suffix, case-sensitive.
// Returns:	True if it has the suffix.
//----------------------------------------------------------------------------
bool has_suffix (const char *string, const char *suffix)
{
	if (!string || !suffix)
		return false;

	size_t stringLength = strlen(string);
	size_t suffixLength = strlen(suffix);
	if (suffixLength > stringLength)
		return false;

	char *ending = (char*)string + stringLength - suffixLength;
	return 0 == strcmp (ending, suffix);
}

//----------------------------------------------------------------------------
// Name:	Network_isReachable 
// Purpose:	Check whether any network connection is up.
// Returns:	True if one or more connections are up.
//----------------------------------------------------------------------------
bool Network_isReachable (void)
{
	FILE *output = popen ("ifconfig | grep inet | grep netmask | grep -v 127.0.0.1 | wc -l", "r");
	if (!output) {
		Log_perror (__FUNCTION__, "popen");
		return false;
	}

	long nConnections = 0;
	if (1 != fscanf (output, "%ld", &nConnections)) {
		nConnections = 0;
	}

	(void)pclose (output);

	return nConnections > 0;
}

static char ipv4_string [18];
const char *ipv4_to_string (unsigned value)
{
        unsigned long a, b, c, d;
        a = (value >> 24) & 0xff;
        b = (value >> 16) & 0xff;
        c = (value >> 8) & 0xff;
        d = value & 0xff;
        snprintf (ipv4_string, sizeof(ipv4_string), "%lu.%lu.%lu.%lu", a, b, c, d);
	return ipv4_string;
}

/*! NOTE: Caller must not free the returned string.
 */
char *execute_and_return_first_line (char *cmd)
{
	if (!cmd || !*cmd) {
		return NULL;
	}
#define MAX_EXECUTED_RESULT_LINE_LEN 4096
	bool success = false;
	FILE *output = popen (cmd, "r");
	if (!output) {
		return NULL;
	}

	static char result [MAX_EXECUTED_RESULT_LINE_LEN];
	if (fgets (result, MAX_EXECUTED_RESULT_LINE_LEN, output)) {
		success = true;
		char *newline = strchr(result, '\n');
		if (newline) {
			*newline = 0;
		}
		char *carriage_return = strchr(result, '\r');
		if (carriage_return) {
			*carriage_return = 0;
		}
	}

	(void)pclose (output);
	return success ? result : NULL;
}

