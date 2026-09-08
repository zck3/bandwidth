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

#ifndef _OOC_MUTABLESTRING_H
#define _OOC_MUTABLESTRING_H

#include <stdint.h>

#include "Object.h"
#include "String.h"

#define DECLARE_MUTABLESTRING_METHODS(TYPE_POINTER) \
	void (*setCString) (TYPE_POINTER, const char*); \
	void (*setChars) (TYPE_POINTER, const Char*); \
	void (*setString) (TYPE_POINTER, String*); \
	void (*appendCString) (TYPE_POINTER, const char *); \
	void (*appendChars) (TYPE_POINTER, const Char *string); \
	void (*appendString) (TYPE_POINTER, String*); \
	void (*appendCharacter) (TYPE_POINTER, Char); \
	void (*insertCharacterAt) (TYPE_POINTER, Char, unsigned index); \
	void (*truncateAt) (TYPE_POINTER, int index); \
	void (*removeStartingChars) (TYPE_POINTER, int index); \
	void (*appendFormat) (TYPE_POINTER, const char* fmt, ...); \
	void (*reverse) (TYPE_POINTER); \
	void (*toUpper) (TYPE_POINTER); \
	void (*toLower) (TYPE_POINTER); 

struct mutablestring;

typedef struct mutablestringclass {
	DECLARE_OBJECT_CLASS_VARS
	DECLARE_OBJECT_METHODS(struct mutablestring*)
	DECLARE_STRING_METHODS(struct mutablestring*)
	DECLARE_MUTABLESTRING_METHODS(struct mutablestring*)
} MutableStringClass;

extern MutableStringClass *_MutableStringClass;
extern MutableStringClass* MutableStringClass_init (MutableStringClass*);

#define DECLARE_MUTABLESTRING_INSTANCE_VARS(TYPE_POINTER) \
	int _allocatedSize; 

typedef struct mutablestring {
	MutableStringClass *is_a;
	DECLARE_OBJECT_INSTANCE_VARS(struct mutablestring*)
	DECLARE_STRING_INSTANCE_VARS(struct mutablestring*)
	DECLARE_MUTABLESTRING_INSTANCE_VARS(struct mutablestring*)
} MutableString;

extern MutableString* MutableString_init (MutableString* object);
extern MutableString* MutableString_withCString (const char*);
extern void MutableString_destroy (Any *);

#define _MutableString(STR) MutableString_withCString(STR)

#endif

