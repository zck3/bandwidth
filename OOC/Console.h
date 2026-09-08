/*============================================================================
  Console, an object-oriented C console I/O class.
  Copyright (C) 2019 by Zack T Smith.

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

#ifndef _OOC_CONSOLE_H
#define _OOC_CONSOLE_H

#include <stdbool.h>
#include <math.h>

#include "Object.h"
#include "String.h"

#define DECLARE_CONSOLE_INSTANCE_VARS(FOO) 

#define DECLARE_CONSOLE_METHODS(TYPE_POINTER) \
	void (*newline) (TYPE_POINTER); \
	void (*flush) (TYPE_POINTER); \
	void (*puts) (TYPE_POINTER, const char*); \
	void (*putchar) (TYPE_POINTER, Char); \
	void (*printInt) (TYPE_POINTER, int); \
	void (*printUnsigned) (TYPE_POINTER, unsigned); \
	void (*printf) (TYPE_POINTER, const char* fmt, ...); \
	void (*printObject) (TYPE_POINTER, Any*);

struct console;

typedef struct consoleclass {
	DECLARE_OBJECT_CLASS_VARS
        DECLARE_OBJECT_METHODS(struct console*)
        DECLARE_CONSOLE_METHODS(struct console*)
} ConsoleClass;

extern ConsoleClass *_ConsoleClass;
extern ConsoleClass* ConsoleClass_init (ConsoleClass*);

typedef struct console {
        ConsoleClass *is_a;
	DECLARE_OBJECT_INSTANCE_VARS(struct console*)
	DECLARE_CONSOLE_INSTANCE_VARS(struct console*)
} Console;

extern Console *Console_singleton ();

extern void Console_destroy (Any*);
extern Console *Console_init (Console *self);

#define ANSIForegroundPlain "\033[0m"
#define ANSIForegroundBold "\033[1m"
#define ANSIForegroundUnderline "\033[4m"
#define ANSIForegroundBlack "\033[30m" 
#define ANSIForegroundRed "\033[31m" 
#define ANSIForegroundGreen "\033[32m" 
#define ANSIForegroundYellow "\033[33m"
#define ANSIForegroundBlue "\033[34m"
#define ANSIForegroundMagenta "\033[35m"
#define ANSIForegroundCyan "\033[36m"
#define ANSIForegroundWhite "\033[37m"
#define ANSIBackgroundBlack "\033[40m" 
#define ANSIBackgroundRed "\033[41m" 
#define ANSIBackgroundGreen "\033[42m" 
#define ANSIBackgroundYellow "\033[43m"
#define ANSIBackgroundBlue "\033[44m"
#define ANSIBackgroundMagenta "\033[45m"
#define ANSIBackgroundCyan "\033[46m"
#define ANSIBackgroundWhite "\033[47m"
#endif
