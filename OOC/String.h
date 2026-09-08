/*============================================================================
  String, an object-oriented C string class.
  Copyright (C) 2009-2019, 2023 by Zack T Smith.

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

#ifndef _OOC_STRING_H
#define _OOC_STRING_H

#include "Array.h"
#include "Char.h"

#define DECLARE_STRING_METHODS(TYPE_POINTER) \
	unsigned long (*length) (TYPE_POINTER); \
	Char (*at) (TYPE_POINTER, int); \
	Char* (*characters) (TYPE_POINTER); \
	bool (*hasSuffix) (TYPE_POINTER, Any*); \
	bool (*hasPrefix) (TYPE_POINTER, Any*); \
	Array* (*explode) (TYPE_POINTER, Char, bool respectQuotes, bool doTrimming, bool convertToAppropriateTypes); \
	unsigned (*totalLines) (TYPE_POINTER); \
	const char* (*asUTF8) (TYPE_POINTER);\
	int (*parseInt) (TYPE_POINTER); \
	double (*parseDouble) (TYPE_POINTER); \
	unsigned long (*parseHex) (TYPE_POINTER); \
	bool (*containsCString) (TYPE_POINTER, const char *); \
	bool (*containsChar) (TYPE_POINTER, char); \
	void (*printWithEscapedChar) (TYPE_POINTER, FILE*, char); \
	bool (*equalsCString) (TYPE_POINTER, const char*); \
	bool (*hasCStringSuffix) (TYPE_POINTER, const char *other); \
	bool (*hasCStringPrefix) (TYPE_POINTER, const char *other); \
	struct string *(*prefixOfLength) (TYPE_POINTER, int); /* Only returns String* */ \
	struct string *(*suffixOfLength) (TYPE_POINTER, int); /* Only returns String* */ \
	struct string *(*substring) (TYPE_POINTER, int index, int length); /* Only returns String* */ 

struct string;

typedef struct stringclass {
	DECLARE_OBJECT_CLASS_VARS
	DECLARE_OBJECT_METHODS(struct string*)
	DECLARE_STRING_METHODS(struct string*)
} StringClass;

extern StringClass *_StringClass;
extern StringClass* StringClass_init (StringClass*);

#define DECLARE_STRING_INSTANCE_VARS(TYPE_POINTER) \
	Char *chars; /* null terminated */ 

typedef struct string {
	StringClass *is_a;
	DECLARE_OBJECT_INSTANCE_VARS(struct string*)
	DECLARE_STRING_INSTANCE_VARS(struct string*)
} String;

extern String* String_init (String* object);
extern void String_destroy (Any* object);
extern String* String_withCString (const char*);
extern String* String_withChars (const Char *string);
extern String* String_withCharsStringAndLength (const Char *wide, size_t length);
extern String* String_withWide (const wchar_t *string);

// Convenience instantiator macro
#define _String(VALUE) String_withCString(VALUE)

#endif

