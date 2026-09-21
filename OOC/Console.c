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

#include <stdarg.h>

#include "Console.h"

ConsoleClass *_ConsoleClass = NULL;

static Console *singleton = NULL;

Console *Console_singleton ()
{
	if (!singleton) {
		singleton = new(Console);
	}
	return singleton;
}

void Console_destroy (Any *self)
{
        DEBUG_DESTROY;

	if (!self) {
		return;
	}
	verifyCorrectClassOrSubclass(self,Console);
}

static void Console_describe (Console* restrict self, FILE *outputFile) 
{ 
	if (!self) {
		return;
	}
	verifyCorrectClass(self,Console);

	if (!outputFile)
		outputFile = stdout;

	fprintf (outputFile, "%s", $(self, className));
}

static void Console_flush (Console* restrict self)
{ 
	if (!self) {
		return;
	}
	verifyCorrectClass(self,Console);

	fflush (stdout);
}

static void Console_newline (Console* restrict self)
{ 
	if (!self) {
		return;
	}
	verifyCorrectClass(self,Console);

	putchar ('\n');
}

static void Console_printf (Console* restrict self, const char *format, ...)
{ 
	if (!self) {
		return;
	}
	verifyCorrectClass(self,Console);

#define kMaxVarargsStringLength (16384)
	char buffer [kMaxVarargsStringLength];
	va_list args;
	va_start (args, format);
	vsnprintf (buffer, sizeof(buffer)-1, format, args);
	va_end (args);
	printf ("%s", buffer);
}

static void Console_printInt (Console* restrict self, int value)
{
	if (!self) {
		return;
	}
	verifyCorrectClass(self,Console);

	printf ("%d", value);
}

static void Console_printUnsigned (Console* restrict self, unsigned value)
{
	if (!self) {
		return;
	}
	verifyCorrectClass(self,Console);

	printf ("%u", value);
}

static void Console_printObject (Console* restrict self, Any *object_)
{
	if (!self) {
		return;
	}
	verifyCorrectClass(self,Console);
	if (!isObject (object_)) {
		return;
	}

	Object *object = object_;
	$(object, print, NULL);
}

static void Console_puts (Console* restrict self, const char *string)
{ 
	if (!self) {
		return;
	}
	verifyCorrectClass(self,Console);
	if (string) {
		puts(string);
	}
}

static void Console_putchar (Console* restrict self, const Char ch)
{ 
	if (!self) {
		return;
	}
	verifyCorrectClass(self,Console);
	if (ch) {
		// Unicode not supported yet
		if (ch > 0 && ch < 128) {
			putchar(ch);
		}
	}
}

ConsoleClass* ConsoleClass_init (ConsoleClass* restrict class)
{
	SET_SUPERCLASS(Object);

	ObjectClass_init ((void*) class);

	SET_OVERRIDDEN_METHOD_POINTER(Console,destroy);
	SET_OVERRIDDEN_METHOD_POINTER(Console,describe);

	SET_METHOD_POINTER(Console,newline);
	SET_METHOD_POINTER(Console,flush);
	SET_METHOD_POINTER(Console,printf);
	SET_METHOD_POINTER(Console,puts);
	SET_METHOD_POINTER(Console,putchar);
	SET_METHOD_POINTER(Console,printInt);
	SET_METHOD_POINTER(Console,printUnsigned);
	SET_METHOD_POINTER(Console,printObject);
	
        VALIDATE_CLASS_STRUCT(class);
	return class;
}

Console* Console_init (Console* restrict self)
{
	ENSURE_CLASS_READY(Console);

	Object_init ((Object*) self);
	self->is_a = _ConsoleClass;

	return self;
}

