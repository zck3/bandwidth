/*============================================================================
  MutableString, an object-oriented C string manipulation class.
  Copyright (C) 2019, 2024 by Zack T Smith.

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

#include "MutableString.h"
#include "Log.h"

#define _GNU_SOURCE
#include <stdio.h>
#include <string.h>
#include <stdarg.h>
#include <wctype.h> // towlower, towupper

MutableStringClass *_MutableStringClass = NULL;

#define kDefaultMutableStringSize (32)

void MutableString_destroy (Any* self)
{
        DEBUG_DESTROY;

	if (!self) {
		return;
	}
	verifyCorrectClass(self,MutableString);

	String_destroy (self);
}

MutableString* MutableString_init (MutableString* restrict self)
{
	ENSURE_CLASS_READY(MutableString);

	if (!self) {
		return NULL;
	}
	
	String_init ((String*)self);
	self->is_a = _MutableStringClass;

	self->chars = ooc_alloc_memory(sizeof(Char) * kDefaultMutableStringSize);
	self->_allocatedSize = kDefaultMutableStringSize;

	return self;
}

static MutableString* MutableString_initWithCString (MutableString* restrict self, const char *str)
{
	ENSURE_CLASS_READY(MutableString);

	if (self) {
		String_init ((String*)self);
		self->is_a = _MutableStringClass;

		int len = str ? ooc_strlen (str) : 0;
		self->_allocatedSize = len? len*2+1 : kDefaultMutableStringSize;
		self->chars = ooc_alloc_memory(sizeof(Char) * self->_allocatedSize);
		if (str) {
			for (int i=0; i < len; i++)
				self->chars[i] = str[i];
		}
	}
	return self;
}

static void reallocIfNecessary (MutableString* restrict self, int newLength)
{
	if (newLength >= self->_allocatedSize-1) {
		int newSize = self->_allocatedSize  * 2;
		if (newLength > newSize)
			newSize = newLength * 2;
		self->chars = realloc (self->chars, sizeof(Char) * newSize);
		self->_allocatedSize = newSize;
	}
}

static void MutableString_setCString (MutableString* restrict self, const char* string)
{
	if (!self) { 
		return;
	}
	verifyCorrectClass(self,MutableString);

	if (!string || !*string) {
		if (self->chars)
			self->chars[0] = 0;
		return;
	}

	int len = ooc_strlen (string);
	reallocIfNecessary (self, len+1);
	int i;
	for (i=0; i < len; i++) {
		self->chars[i] = string[i];
	}
	self->chars[i] = 0;
}

#ifdef UNUSED
static void MutableString_setWide (MutableString* restrict self, wchar_t* string)
{
	// TODO
}
#endif

static void MutableString_setChars (MutableString* restrict self, const Char* string)
{
	if (!self) { 
		return;
	}
	verifyCorrectClass(self,MutableString);

	if (!string || !*string) {
		if (self->chars)
			self->chars[0] = 0;
		return;
	}
	int len = Char_strlen (string);
	reallocIfNecessary (self, len+1);
	memcpy (self->chars, string, len * sizeof(Char));
	self->chars[len] = 0;
}

static void MutableString_setString (MutableString* restrict self, String *other)
{
	if (!self) { 
		return;
	}
	verifyCorrectClass(self,MutableString);

	if (!other || !other->chars) {
		if (self->chars)
			self->chars[0] = 0;
		return;
	}

	int otherLength = Char_strlen(other->chars);
	reallocIfNecessary (self, otherLength+1);
	memcpy (self->chars, other->chars, otherLength * sizeof(Char));
	self->chars[otherLength] = 0;
}

static void MutableString_appendCString (MutableString* restrict self, const char *string)
{
	if (!self) {
		return;
	}
	verifyCorrectClass(self,MutableString);

	if (!string || !*string)
		return;
	if (!self->chars) {
		MutableString_setCString (self, string);
		return;
	}

	int otherLength = ooc_strlen (string);
	int currentLength = Char_strlen(self->chars);
	int newLength = currentLength + otherLength;
	reallocIfNecessary (self, newLength+1);

	int i = 0;
	int j = currentLength;
	while (i < otherLength) {
		// XX Need to convert UTF8 to Char.
		self->chars[j++] = (uint8_t) string[i++];
	}
	self->chars[j] = 0;
}

static void MutableString_appendFormat (MutableString* restrict self, const char* format, ...)
{
	if (!self) {
		return;
	}
	verifyCorrectClass(self,MutableString);

	if (!format || !*format)
		return;

        va_list args;

        va_start (args, format);
        long spaceNeeded = vsnprintf (NULL, 0, format, args);
        va_end (args);

	if (spaceNeeded <= 0) {
		return;
	}

        va_start (args, format);
        char *buffer = ooc_alloc_memory(spaceNeeded + 1);
        vsnprintf (buffer, spaceNeeded, format, args);
        va_end (args);

	MutableString_appendCString (self, buffer);
	ooc_bzero (buffer, spaceNeeded);
	ooc_free (buffer);
}

static void MutableString_appendChars (MutableString* restrict self, const Char *string)
{
	if (!self) {
		return;
	}
	verifyCorrectClass(self,MutableString);

	if (!string || !*string)
		return;
	if (!self->chars) {
		MutableString_setChars (self, string);
		return;
	}

	int currentLength = Char_strlen(self->chars);
	int otherLength = Char_strlen (string);
	int newLength = currentLength + otherLength;
	reallocIfNecessary (self, newLength+1);
	memcpy (self->chars + currentLength, string, otherLength * sizeof(Char));
	self->chars [newLength] = 0;
}

static void MutableString_appendString (MutableString* restrict self, String *other)
{
	if (!self) {
		return;
	}
	verifyCorrectClass(self,MutableString);

	if (!other)
		return;
	verifyCorrectClasses(other,String,MutableString);

	if (!self->chars) {
		MutableString_setString (self, other);
		return;
	}

	int currentLength = Char_strlen(self->chars);
	int otherLength = $(other, length);
	int newLength = currentLength + otherLength;
	reallocIfNecessary (self, newLength+1);
	memcpy (self->chars + currentLength, other->chars, otherLength * sizeof(Char));
	self->chars [newLength] = 0;
}

static void MutableString_insertCharacterAt (MutableString* restrict self, Char ch, unsigned index)
{
	if (!self) {
		return;
	}
	verifyCorrectClass(self,MutableString);

	if (!ch) {
		return;
	}

	int length = Char_strlen(self->chars);
	reallocIfNecessary (self, 2+length);

	if (index >= length) {
		self->chars [length++] = ch;
		self->chars [length] = 0;
	}
	else {
		for (int i = length; i > index; i--) {
			self->chars[i] = self->chars[i-1];
		}
		length++;
		self->chars[index] = ch;
		self->chars[length] = 0;
	}
}

static void MutableString_appendCharacter (MutableString* restrict self, Char ch)
{
	return MutableString_insertCharacterAt (self, ch, MAXUNSIGNED);
}

static void MutableString_toUpper (MutableString* restrict self)
{
	if (!self || !self->chars) {
		return;
	}
	verifyCorrectClass(self,MutableString);

	Char *ptr = self->chars;
	size_t length = Char_strlen(ptr);
	for (register size_t index=0; index < length; index++) {
		register Char ch = towupper(ptr[index]);
		ptr[index] = ch;
	}
}

static void MutableString_toLower (MutableString* restrict self)
{
	if (!self || !self->chars) {
		return;
	}
	verifyCorrectClass(self,MutableString);

	Char *ptr = self->chars;
	size_t length = Char_strlen(ptr);
	for (register size_t index=0; index < length; index++) {
		register Char ch = towlower(ptr[index]);
		ptr[index] = ch;
	}
}

static void MutableString_reverse (MutableString* restrict self)
{
	if (!self || !self->chars) {
		return;
	}
	verifyCorrectClass(self,MutableString);

	int length = Char_strlen(self->chars);

	if (length <= 1) {
		return;
	}

	register int end = length-1;
	register Char *ptr = self->chars;
	for (register int start=0; start < end; start++, end--) {
		register Char ch = ptr[start];
		ptr[start] = ptr[end];
		ptr[end] = ch;
	}
}

static void MutableString_removeStartingChars (MutableString* restrict self, int index)
{
	if (!self || index <= 0) {
		return;
	}
	verifyCorrectClass(self,MutableString);

	size_t length = Char_strlen (self->chars);
	if (index >= length) {
		self->chars[0] = 0;
		return;
	}

	size_t newLength = length - index;
	for (size_t i = 0; i < newLength; i++) {
		self->chars[i] = self->chars[index];
		index++;
	}
	self->chars[newLength] = 0;
}

static void MutableString_truncateAt (MutableString* restrict self, int index)
{
	if (!self) {
		return;
	}
	verifyCorrectClass(self,MutableString);

	if (!self->chars) {
		return;
	}

	int currentLength = Char_strlen (self->chars);
	if (index < currentLength) {
		self->chars[index] = 0;
	}
}

MutableString* MutableString_withCString (const char* str)
{
	MutableString* obj = allocate(MutableString);
	return MutableString_initWithCString (obj, str);
}

MutableStringClass* MutableStringClass_init (MutableStringClass *class)
{
	SET_SUPERCLASS(String);

	SET_OVERRIDDEN_METHOD_POINTER(MutableString,destroy);

	SET_METHOD_POINTER(MutableString,setCString);
	SET_METHOD_POINTER(MutableString,setChars);
	SET_METHOD_POINTER(MutableString,setString);
	SET_METHOD_POINTER(MutableString,appendCString);
	SET_METHOD_POINTER(MutableString,appendChars);
	SET_METHOD_POINTER(MutableString,appendCharacter);
	SET_METHOD_POINTER(MutableString,appendString);
	SET_METHOD_POINTER(MutableString,insertCharacterAt);
	SET_METHOD_POINTER(MutableString,truncateAt);
	SET_METHOD_POINTER(MutableString,removeStartingChars);
	SET_METHOD_POINTER(MutableString,appendFormat);
	SET_METHOD_POINTER(MutableString,reverse);
	SET_METHOD_POINTER(MutableString,toUpper);
	SET_METHOD_POINTER(MutableString,toLower);
	
	VALIDATE_CLASS_STRUCT(class);
	return class;
}

