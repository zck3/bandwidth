/*=========================================================================
 * This file is part of zBenchmark, which is a system benchmarking tool.
 * Copyright (C) 2021 Zack T Smith.
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

#ifndef _UTILITY_H
#define _UTILITY_H

#define _GNU_SOURCE
#include <stdio.h>
#include <string.h>
#include <stdint.h>
#include <stdbool.h>
#include <sched.h>
#include <limits.h>
#include <unistd.h>
#include <time.h>
#include <sys/time.h>

extern bool has_prefix (const char *string, const char *prefix);
extern bool has_suffix (const char *string, const char *suffix);

extern bool Network_isReachable (void);
extern const char *ipv4_to_string (unsigned value);

extern char *execute_and_return_first_line (char *cmd);

extern int cpu_current_core();

#endif
