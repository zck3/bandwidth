/*============================================================================
  MutableArray, an object-oriented C mutable array class.
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

#include "Array.h"
#include "MutableArray.h"
#include "Log.h"

MutableArrayClass *_MutableArrayClass = NULL;

void MutableArray_destroy (Any *self)
{
	DEBUG_DESTROY;
	if (!self) {
		return;
	}
	verifyCorrectClassOrSubclass(self,MutableArray);
	Array_destroy (self);
}
	
static void MutableArray_describe (MutableArray* restrict self, FILE *outputFile) 
{ 
	if (!self) {
		return;
	}
	verifyCorrectClass(self, MutableArray);

	if (!outputFile) {
		outputFile = stdout;
	}
	fprintf (outputFile, "%s", $(self, className));
	fprintf (outputFile, "(%lu)", (unsigned long) self->count);
}

static void MutableArray_removeAt (MutableArray* restrict self, unsigned index) 
{
	if (!self) {
		return;
	}
	verifyCorrectClassOrSubclass(self,MutableArray);

	if (!self->array) {
		return;
	}
	size_t count = self->count;
	if (index >= count) {
		return;
	}
	if (index == count-1) {
		Object *object = self->array [count-1];
		release(object);
		self->array [count-1] = NULL;
		self->count = count-1;
		return;
	}

	Object *object = self->array [index];
	if (object)
		release(object);

	int i = index;
	while (i <= count-2) {
		self->array [i] = self->array [i+1];
		i++;
	}
	self->array [i+1] = NULL;
	self->count = count-1;
}

static void MutableArray_remove (MutableArray* restrict self, Any *object_) 
{ 
	if (!self) {
		return;
	}
	if (!object_)
		return;
	verifyCorrectClassOrSubclass(self,MutableArray);

	if (!isObject(object_)) 
		error_not_an_object(__FUNCTION__);

	Object *object = object_;

	size_t count = self->count;
	if (!count)
		return;

	size_t where = 0;
	bool found = false;

	for (int index=0; index < count; index++) {
		if (object == self->array[index]) {
			where = index;
			found = true;
			break;
		}
	}
	if (!found) {
		warning (__FUNCTION__, "OBJECT NOT IN ARRAY\n");
		return;
	}

	MutableArray_removeAt (self, where);
}

static void MutableArray_removeFirst (MutableArray* restrict self) 
{
	if (!self) {
		return;
	}
	verifyCorrectClassOrSubclass(self,MutableArray);

	MutableArray_removeAt (self, 0);
}

static void MutableArray_removeLast (MutableArray* restrict self) 
{
	if (!self) {
		return;
	}
	verifyCorrectClassOrSubclass(self,MutableArray);

	size_t count = self->count;
	if (!count)
		return;
	
	MutableArray_removeAt (self, count-1);
}

static int quicksort_compare_ascending(const void* a, const void* b)
{
	Object **ptr1 = (Object**) a;
	Object **ptr2 = (Object**) b;
	Object *obj1 = (Object*) *ptr1;
	Object *obj2 = (Object*) *ptr2;
	return $(obj1, compare, (Any*) obj2);
}

static int quicksort_compare_descending(const void* a, const void* b)
{
	Object **ptr1 = (Object**) a;
	Object **ptr2 = (Object**) b;
	Object *obj1 = (Object*) *ptr1;
	Object *obj2 = (Object*) *ptr2;
	return $(obj2, compare, (Any*) obj1);
}

static void MutableArray_quicksort (MutableArray* restrict self, bool ascending)
{
	if (!self) {
		return;
	}
	verifyCorrectClassOrSubclass(self,MutableArray);

	size_t count = self->count;
	if (count <= 1) {
		return;
	}

	qsort (self->array, count, sizeof(Object*), 
		ascending ?  quicksort_compare_ascending : quicksort_compare_descending);
}

static void MutableArray_reverse (MutableArray* restrict self) 
{
	if (!self) {
		return;
	}
	verifyCorrectClassOrSubclass(self,MutableArray);

	size_t count = self->count;
	if (count <= 1) {
		return;
	}

	size_t i = 0;
	size_t j = count-1;
	while (i < j) {
		void *object1 = self->array[i];
		void *object2 = self->array[j];
		self->array[i++] = object2;
		self->array[j--] = object1;
	}
}

static void MutableArray_randomize (MutableArray* restrict self) 
{
	if (!self) {
		return;
	}
	verifyCorrectClassOrSubclass(self,MutableArray);

	size_t count = self->count;
	if (count <= 1) {
		return;
	}

	for (size_t i = 0; i < count; i++) {
		size_t j = rand() % count;
		if (i != j) {
			void *object1 = self->array[i];
			void *object2 = self->array[j];
			self->array[i] = object2;
			self->array[j] = object1;
		}
	}
}

static void MutableArray_removeAll (MutableArray* restrict self) 
{ 
	if (!self) {
		return;
	}
	verifyCorrectClassOrSubclass(self,MutableArray);

	Object **array = self->array;
	if (self->array) {
		size_t count = self->count;
		if (count) {
			for (unsigned i = 0; i < count; i++) {
				Object *object = array[i];
				if (object) {
					release(object);
				}
			}
		}
		size_t nBytes = sizeof(Object*) * self->size;
		if (nBytes) {
			ooc_bzero ((void*) self->array, nBytes);
		}
	}

	self->count = 0; 
}

#ifdef UNUSED
// TODO print as JSON
static void MutableArray_printJSON (MutableArray* restrict self, FILE *outputFile)
{
	if (!self) {
		return;
	}
	verifyCorrectClassOrSubclass(self,MutableArray);

	if (!outputFile) {
		outputFile = stdout;
	}

	if (!self->array) {
		return;
	}

	fputc ('[', outputFile);
	for (size_t index = 0; index < self->count; index++) {
		Object *object = self->array[index];
		if (!object) {
        		fprintf (outputFile, "null");
		}
		else {
			$(object, print, outputFile);
		}
		
		if (index != self->count-1) {
			fprintf (outputFile, ", ");
		}
	}
	fputc (']', outputFile);
}
#endif

static void MutableArray_append (MutableArray* restrict self, Any *object_)
{
	if (!self) {
		return;
	}
	verifyCorrectClassOrSubclass(self,MutableArray);

	if (!self->array) {
		// This means the array malloc operation failed at instantiation.
		return;
	}

	// RULE: It's OK to append a NULL.
	Object *object = object_;
	if (object) {
		if (!isObject(object)) {
			error_not_an_object(__FUNCTION__);
		}
		retain(object);
	}

	size_t count = self->count;
	if (count >= self->size) {
		size_t newSize = self->size * 2;
		Object **ary = reallocarray (self->array, newSize, sizeof(Object*));
		if (ary) {
			self->array = ary;
			self->size = newSize;
		} else {
			Log_perror(__FUNCTION__, "reallocarray");
		}
	}

	self->array [count++] = object;
	self->count = count;
}

static bool allocateInitialArray (MutableArray* restrict self)
{
	if (!self) {
		return false;
	}

	self->size = DEFAULT_ARRAY_SIZE;
	self->count = 0;
	size_t nBytes = sizeof(Object*) * self->size;
	self->array = (Object**) ooc_alloc_memory (nBytes);
	if (!self->array) {
		Log_perror (__FUNCTION__, "ooc_alloc_memory");
		self->size = 0;
	}
	return self->array != NULL;
}

static void MutableArray_setAt (MutableArray* restrict self, Any* object_, unsigned index)
{
	if (!self) {
		return;
	}
	verifyCorrectClassOrSubclass(self,MutableArray);

	if (object_ != NULL && !isObject(object_)) {
		error_not_an_object(__FUNCTION__);
	}

	Object *object = object_;

	// Resize the array if necessary.
	// The new pointers will be cleared in the next section.
	//
	if (index >= self->size) {
		// Ensure the new index is covered.
		size_t newSize = index+4;
		// Slow exponential growth to avoid wasting memory.
		newSize += newSize / 4;
		// Use an entire cache line.
		if (newSize & 0xf) {
			newSize = (newSize | 0xf) + 1;
		}
		self->array = realloc (self->array, newSize * sizeof(Object*));
		if (!self->array) {
			Log_perror (__FUNCTION__, "realloc");
			return;
		} else {
			self->size = newSize;
		}
	}

	// If there's a gap between the last item and the new one,
	// fill it in with NULLs.
	//
	if (index >= self->count) {
		for (unsigned i = self->count; i <= index; i++) {
			self->array[i] = NULL;
		}
	}

	Object *existingObject = self->array[index];
	if (existingObject && existingObject != object) {
		release (existingObject);
	}
	self->array[index] = object;
	if (index >= self->count) {
		self->count = index+1;
	}
}

static void MutableArray_insertAt (MutableArray* restrict self, Any* object_, unsigned index) 
{
	if (!self) {
		return;
	}
	verifyCorrectClassOrSubclass(self,MutableArray);

	// Allow NULL but if it's non-NULL, make sure it's an object.
	if (object_ && !isObject(object_)) {
		error_not_an_object(__FUNCTION__);
		return;
	}
	Object *object = object_;

	size_t count = self->count;
	if (!count || index >= count) {
		MutableArray_append (self, object);
		return;
	}

	if (index < 0) {
		index = 0;
	}

	if (count >= self->size) {
		unsigned n = self->size;
		if (!n) {
			self->size = DEFAULT_ARRAY_SIZE;
		}
		size_t newSize = n + n/2;
		Object **ary = realloc (self->array, newSize * sizeof(Object*));
		if (!self->array) {
			Log_perror (__FUNCTION__, "realloc");
			return;
		} else {
			self->array = ary;
			self->size = newSize;
		}
	}

	if (index >= self->count) {
		for (unsigned i = self->count; i < index; i++) {
			self->array[i] = NULL;
		}
	}

	retain(object);

	// Insert: O(n)
	for (unsigned i = count; i > index; i--) {
		self->array[i] = self->array[i-1];
	}
	self->array[index] = object;
	self->count = count + 1;
}

static void MutableArray_prepend (MutableArray* restrict self, Any* object) 
{
	if (!self) {
		return;
	}
	verifyCorrectClassOrSubclass(self,MutableArray);

	MutableArray_insertAt (self, object, 0);
}

MutableArrayClass* MutableArrayClass_init (MutableArrayClass* restrict class)
{
        SET_SUPERCLASS(Array);

        SET_OVERRIDDEN_METHOD_POINTER(MutableArray,describe);
        SET_OVERRIDDEN_METHOD_POINTER(MutableArray,destroy);

	SET_METHOD_POINTER(MutableArray,append);
	SET_METHOD_POINTER(MutableArray,prepend);
	SET_METHOD_POINTER(MutableArray,remove);
	SET_METHOD_POINTER(MutableArray,removeAt);
	SET_METHOD_POINTER(MutableArray,removeAll);
	SET_METHOD_POINTER(MutableArray,removeFirst);
	SET_METHOD_POINTER(MutableArray,removeLast);
	SET_METHOD_POINTER(MutableArray,insertAt);
	SET_METHOD_POINTER(MutableArray,reverse);
	SET_METHOD_POINTER(MutableArray,randomize);
	SET_METHOD_POINTER(MutableArray,quicksort);
	SET_METHOD_POINTER(MutableArray,setAt);

	VALIDATE_CLASS_STRUCT(class);
        return class;
}

MutableArray *MutableArray_init (MutableArray* restrict self) 
{
	ENSURE_CLASS_READY(MutableArray);

	if (!self) {
		return NULL;
	}

	Array_init ((Array*) self);
	self->is_a = _MutableArrayClass;

	if (!allocateInitialArray(self)) {
		release(self);
		return NULL;
	}

	return self;
}

MutableArray* MutableArray_withArray (Any *arrayToCopy_)
{
	MutableArray *self = new(MutableArray);
	if (arrayToCopy_) {
		verifyCorrectClassOrSubclass(arrayToCopy_,Array);
		Array *arrayToCopy = (Array*) arrayToCopy_;

		unsigned n = $(arrayToCopy, count);
		for (unsigned i=0; i < n; i++) {
			Object* obj = $(arrayToCopy, at, i);
			if (obj) {
				$(self, setAt, obj, i);
			}
		}
	}
	return self;
}

MutableArray* MutableArray_with (Any *first, ...) // NOTE: Last item must be NULL.
{
	MutableArray *self = new(MutableArray);

	va_list list;
	va_start(list, first);
	Object *object = first;
	do {
		if (isObject(object)) {
			$(self, append, object);
		} else {
			Log_error (__FUNCTION__, "Parameter is not an Object.");
		}
		object = va_arg(list, Object*);
	} while (object != NULL);
	va_end(list);

	return self;
}

MutableArray *MutableArray_withObject (Any *object_, unsigned count) 
{
	MutableArray *self = new(MutableArray);

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

