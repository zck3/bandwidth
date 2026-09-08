/*============================================================================
  Array, an object-oriented C array class.
  Copyright (C) 2019, 2026 by Zack T Smith.

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

#include "Array.h"
#include "MutableArray.h"
#include "MutableString.h"
#include "Log.h"

ArrayClass *_ArrayClass = NULL;

MAKE_GETTER_SCALAR(Array,size_t,count)

static Any *Array_at (Array* restrict self, long index) 
{
	if (!self) {
		return NULL;
	}
	verifyCorrectClassOrSubclass(self,Array);

	if (index < 0 || index >= self->count) {
		return NULL;
	}

	Any *value = self->array [index];
	return value;
}

/* Duplicate an array as a new Array.
 * Works also for MutableArray.
 */
Array *Array_withArray (Any* array_)
{
	Array* restrict self = new(Array);

	if (array_ && isMemberOfClassOrSubclass(array_, Array)) {
		Array *array = array_;

		size_t n = self->count = self->size = Array_count (array);
		unsigned nBytes = sizeof(Any*) * self->size;
		self->array = (Object**) malloc(nBytes);
		ooc_bzero (self->array, nBytes);

		for (long i=0; i < n ; i++) {
			Object *object = Array_at (array, i);
			retain(object);
			self->array[i] = object;
		}
	}
	return self;
}

static Any *Array_first (Array* restrict self) 
{
	if (!self) {
		return NULL;
	}
	verifyCorrectClassOrSubclass(self,Array);
	if (!self->count) {
		return NULL;
	}

	return self->array [0];
}

static Any *Array_last (Array* restrict self) 
{
	if (!self) {
		return NULL;
	}
	verifyCorrectClassOrSubclass(self,Array);

	if (!self->count) {
		return NULL;
	}
	return self->array [self->count - 1];
}

static bool Array_contains (Array* restrict self, Any *object_) 
{
	if (!self || !object_) {
		return false;
	}
	verifyCorrectClassOrSubclass(self,Array);
	if (!isObject(object_)) {
		// XX
		return false;
	}

	size_t n = self->count;
	if (!n) {
		return false;
	}

	Object *object = object_;

	for (size_t i=0; i < n; i++) {
		Object *other = self->array[i];
		if (object == other) {
			// Pointer match
			return true;
		}
		if ($(object, equals, other)) {
			// Content match e.g. two Strings.
			return true;
		}
	}
	return false;
}

void Array_destroy (Any *self_)
{
	DEBUG_DESTROY;
	if (!self_) {
		return;
	}
	verifyCorrectClassOrSubclass(self_,Array);

	Array* restrict self = self_;
	Object **array = self->array;
	if (self->size && array) {
		for (size_t index = 0; index < self->count; index++) {
			Object *object = self->array[index];
			release(object);
		}
		ooc_free (array);
		self->array = NULL;
		self->size = 0;
		self->count = 0;
	}
}

static void Array_describe (Array* restrict self, FILE *outputFile) 
{ 
	if (!self) {
		return;
	}
	verifyCorrectClassOrSubclass(self,Array);

	if (!outputFile)
		outputFile = stdout;

	fprintf (outputFile, "%s", $(self, className));
	fprintf (outputFile, "(%lu)", (unsigned long) self->count);
}

static void Array_print (Array* restrict self, FILE *outputFile) 
{ 
	if (!self) {
		return;
	}
	verifyCorrectClassOrSubclass(self,Array);

	if (!outputFile) {
		outputFile = stdout;
	}

	fputc ('[', outputFile);

	int n=self->count;
	for (int i=0; i < n; i++) {
		Object *object = $(self, at, i);
		if (object) {
			$(object, print, outputFile);
		} else {
			fputs ("NULL", outputFile);
		}

		if (i++ != n-1) {
			fputc (',', outputFile);
		}
	}

	fputc (']', outputFile);
}

static bool Array_equals (Array* restrict self, Any *other_)
{ 
	if (!self || !other_) {
		return false;
	}

	verifyCorrectClassOrSubclass(self,Array);
	Array *other = (Array*)other_;

	// RULE: We an only compare an Array to an (Mutable)Array.
	if (!isMemberOfClassOrSubclass(other,Array)) {
		return false; 
	}

	if (self->count != other->count) {
		return false;
	}

	for (int i=0; i < self->count; i++) {
		Object *first = Array_at (self, i);
		Object *second = Array_at (other, i);

		if (!first && !second) {
			// Arrays can contain NULL pointers.
			continue;
		}
		else if (first && second) {
			if (!$(first, equals, second)) {
				return false;
			}
		}
		else {
			// Either one or the other is NULL.
			return false;
		}
	}

	return true;
}

/* Makes a mutable string from an array, inserting separator between items.
 */
static Any *Array_separatedByCString (Array* restrict self, const char* separator)
{
	if (!self) {
		return NULL;
	}
	verifyCorrectClassOrSubclass(self,Array);
	
	MutableString *mut = new(MutableString);

	int n=$(self, count);
	for (int i=0; i < n; i++) {
		Object *object = $(self, at, i);
		// TODO Need general asString method.

		if (isMemberOfClassOrSubclass(object, String)) {
			$(mut, appendString, (String*) object);
		}
#ifdef LATER
		else if (isMemberOfClassOrSubclass(object, Int)) {
			// HERE
		}
		else if (isMemberOfClassOrSubclass(object, Double)) {
			// HERE
		}
#endif
		if (i != n-1) {
			if (separator && *separator) {
				$(mut, appendCString, separator);
			}
		}
	}

	// XX Convert to String?
	return mut;
}

ArrayClass* ArrayClass_init (ArrayClass *class)
{
	SET_SUPERCLASS(Object);

	// Overridden methods
	SET_OVERRIDDEN_METHOD_POINTER(Array,describe);
	SET_OVERRIDDEN_METHOD_POINTER(Array,destroy);
	SET_OVERRIDDEN_METHOD_POINTER(Array,print);
	SET_OVERRIDDEN_METHOD_POINTER(Array,equals);

	// Array methods
	SET_METHOD_POINTER(Array,count);
	SET_METHOD_POINTER(Array,first);
	SET_METHOD_POINTER(Array,last);
	SET_METHOD_POINTER(Array,at);
	SET_METHOD_POINTER(Array,contains);
	SET_METHOD_POINTER(Array,separatedByCString);

	VALIDATE_CLASS_STRUCT(class);

        return class;
}

Array* Array_init (Array* restrict self)
{
        ENSURE_CLASS_READY(Array);

	Object_init ((Object*) self);
	self->is_a = _ArrayClass;

	self->size = 0;
	self->count = 0;
	self->array = NULL;

	return self;
}

Array *Array_withObject (Any *object_, unsigned count) 
{
	Array *self = new(Array);

	if (!object_ || !count) {
		return self;
	}

	unsigned long nBytes = sizeof(Any*) * count;
	self->array = (Object**) malloc(nBytes);
	if (!self->array) {
		Log_perror(__FUNCTION__, "malloc");
		release(self);
		return NULL;
	}

	self->size = count;
	self->count = count;

	Object *object = (Object*)object_;
	if (object) {
		for (unsigned i=0; i < count; i++) {
			self->array[i] = retain(object);
		}
	} else {
		ooc_bzero (self->array, nBytes);
	}
	return self;
}

Array* Array_with (Any* first, ...) // NOTE: Last item must be NULL.
{
	Array *self = new(Array);

	self->size = DEFAULT_ARRAY_SIZE;
	self->count = 0;
	size_t nBytes = sizeof(Any*) * self->size;
	self->array = (Object**) calloc(nBytes, 1);
	if (!self->array) {
		// RULE: Can't allocate buffer, return NULL array pointer.
		Log_perror (__FUNCTION__, "calloc");
		release(self);
		return NULL;
	}

	va_list list;
	va_start(list, first);
	Object *object = first;
	do {
		if (self->count >= self->size) {
			size_t newSize = self->size + self->size / 2;
			size_t nBytes = sizeof(Any*) * self->size;
			void *newArray = realloc(self->array, nBytes);
			if (!newArray) {
				// RULE: Can't reallocate buffer, return NULL array pointer.
				release(self);
				return NULL;
			}
			self->size = newSize;
			self->array = newArray;
		}

		if (isObject(object)) {
			self->array[self->count++] = object;
		} else {
			Log_error (__FUNCTION__, "Parameter is not an Object.");
		}
		object = va_arg(list, Object*);
	} while (object != NULL);
	va_end(list);

	return self;
}

