/*============================================================================
  Array, an object-oriented C array class.
  Copyright (C) 2019, 2023, 2026 by Zack T Smith.

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

#ifndef _OOC_ARRAY_H
#define _OOC_ARRAY_H

#include <stdlib.h>
#include <stdbool.h>
#include <stdarg.h>

#include "Object.h"

#define DEFAULT_ARRAY_SIZE (32)

#define DECLARE_ARRAY_INSTANCE_VARS(FOO) \
	size_t count;\
	size_t size;\
	Object **array;

#define DECLARE_ARRAY_METHODS(TYPE_POINTER) \
	size_t (*count) (TYPE_POINTER); \
	Any* (*at) (TYPE_POINTER, long); \
	Any* (*first) (TYPE_POINTER); \
	Any* (*last) (TYPE_POINTER); \
	Any* (*separatedByCString) (TYPE_POINTER, const char*); \
	bool (*contains) (TYPE_POINTER, Any *object); 

struct array;

typedef struct arrayclass {
	DECLARE_OBJECT_CLASS_VARS
        DECLARE_OBJECT_METHODS(struct array*)
        DECLARE_ARRAY_METHODS(struct array*)
} ArrayClass;

extern ArrayClass *_ArrayClass;

typedef struct array {
        ArrayClass *is_a;
	DECLARE_OBJECT_INSTANCE_VARS(struct array*)
	DECLARE_ARRAY_INSTANCE_VARS(struct array*)
} Array;

extern ArrayClass* ArrayClass_init (ArrayClass*);
extern Array *Array_init (Array *self);
extern void Array_destroy (Any *self);
extern Array* Array_withObject (Any *object, unsigned count);
extern Array* Array_withArray (Any *arrayToCopy);
extern Array* Array_with (Any*, ...); // RULE: Last item must be NULL.

#endif
