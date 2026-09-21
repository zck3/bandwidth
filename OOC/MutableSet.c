/*============================================================================
  MutableSet, an object-oriented C mutable set class.
  Copyright (C) 2023, 2024 by Zack T Smith.

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

#include "MutableSet.h"
#include "String.h"
#include "Log.h"

MutableSetClass *_MutableSetClass = NULL;

#ifdef UNUSED
static MutableSet* MutableSet_initWithSize (MutableSet* restrict self, SetSize size)
{
	ENSURE_CLASS_READY(MutableSet);

	if (self) {
		Object_init ((Object*)self);

		// Skip call to Set_init since that doesn't do anything 
		// more than this, and it will allocate buckets.

		self->is_a = _MutableSetClass;
		switch (size) {
		default:
		case SetSizeSmall:
			self->nBuckets = 127;
			break;
		case SetSizeMedium:
			self->nBuckets = 1279;
			break;
		case SetSizeLarge:
			self->nBuckets = 12721;
			break;
		}
		if (self->buckets) {
			free(self->buckets);
		}
		size_t nBytes = sizeof(SetListItem*) * self->nBuckets;
		self->buckets = (SetListItem**) malloc(nBytes);
		if (self->buckets) {
			ooc_bzero (self->buckets, nBytes);
		}
	}
	return self;
}
#endif

void MutableSet_destroy (Any* restrict self)
{
	if (!self) {
		return;
	}
	verifyCorrectClassOrSubclass(self,MutableSet);

	Set_destroy ((Set*) self);
}

MutableSet* MutableSet_init (MutableSet* restrict self)
{
	ENSURE_CLASS_READY(MutableSet);

	if (self) {
		Set_init ((Set*)self);
		self->is_a = _MutableSetClass;
	}

	return self;
}

static void MutableSet_add (MutableSet* restrict self, Any *object_)
{
	if (!self || !object_) {
		return;
	}
	verifyCorrectClassOrSubclass(self,MutableSet);
	if (!isObject(object_)) {
		return;
	}
	Object *object = object_;

	unsigned hash = $(object, hash);
	unsigned index = hash % self->nBuckets;

	SetListItem *item = self->buckets[index];
	bool found = false;
	while (item) {
		if ($(object, equals, item->object)) {
			found = true;
			break;
		}
		item = item->next;
	}
	if (!found) {
		SetListItem *item = (SetListItem*) malloc(sizeof(SetListItem));
		item->object = retain(object);
		item->next = self->buckets[index];
		self->buckets[index] = item;
	}
}

static void MutableSet_addCString (MutableSet* restrict self, const char *cstring)
{
	if (!self || !cstring) {
		return;
	}
	String *string = _String(cstring);
	MutableSet_add (self, string);
}

static void MutableSet_remove (MutableSet* restrict self, Any *object_)
{
	if (!self || !object_) {
		return;
	}
	verifyCorrectClassOrSubclass(self,MutableSet);
	if (!isObject(object_)) {
		return;
	}
	Object *object = object_;

	unsigned hash = $(object, hash);
	unsigned index = hash % self->nBuckets;

	SetListItem *item = self->buckets[index];
	SetListItem *prev = NULL;
	while (item) {
		if (areSameClass (object, item->object)) {
			if ($(object, equals, item->object)) {
				if (!prev) {
					self->buckets[index] = item->next;
				} else {
					prev->next = item->next;
				}
				release(item->object);
				free (item);
				break;
			}
		}
		prev = item;
		item = item->next;
	}
}

static void MutableSet_unionWith (MutableSet* restrict self, Set *set_)
{
	if (!self || !set_) {
		return;
	}
	verifyCorrectClassOrSubclass(self,MutableSet);
	verifyCorrectClassOrSubclass(set_,Set);
	Set *set = set_;

	for (unsigned i=0; i < set->nBuckets; i++) {
		SetListItem *item = set->buckets[i];
		while (item) {
			MutableSet_add (self, item->object);
			item = item->next;
		}
	}
}

static void MutableSet_subtract (MutableSet* restrict self, Set *set_)
{
	if (!self || !set_) {
		return;
	}
	verifyCorrectClassOrSubclass(self,MutableSet);
	verifyCorrectClassOrSubclass(set_,Set);
	Set *set = set_;

	for (unsigned i=0; i < set->nBuckets; i++) {
		SetListItem *item = set->buckets[i];
		while (item) {
			MutableSet_remove (self, item->object);
			item = item->next;
		}
	}
}

/*---------------------------------------------------------------------------
 * Name:        intersect
 * Purpose:     Obtains the intersection of sets, changing self.
 *-------------------------------------------------------------------------*/
static void MutableSet_intersect (MutableSet* restrict self, Set *set_)
{
	if (!self || !set_) {
		return;
	}
	verifyCorrectClassOrSubclass(self,MutableSet);
	verifyCorrectClassOrSubclass(set_,Set);
	Set *set = set_;

	// Traverse self's hash buckets.
	for (unsigned i=0; i < self->nBuckets; i++) {

		// Traverse current bucket.
		SetListItem *prev = NULL;
		SetListItem *item = self->buckets[i];
		while (item) {
			Object *object = item->object;
			SetListItem *next = item->next;

			// If any of our objects is NOT the other set, remove it.
			if (!$(set, contains, object)) {
				if (!prev) {
					// Remove first item
					self->buckets[i] = next;
				} else {
					// Remove subsequent item
					prev->next = next;
				}
				release(object);
				free (item);
			} else {
				prev = item;
			}
			item = next;
		}
	}
}

MutableSet* MutableSet_with (Any* first, ...)
{
	MutableSet *mut = new(MutableSet);

	va_list list;
	va_start(list, first);
	Object *object = first;
	do {
		if (isObject(object)) {
			$(mut, add, object);
		} else {
			Log_error (__FUNCTION__, "Parameter is not an Object.");
		}
		object = va_arg(list, Object*);
	} while (object != NULL);
	va_end(list);

	return mut;
}

static void MutableSet_removeAll (MutableSet* restrict self)
{
	if (!self) {
		return;
	}
	verifyCorrectClassOrSubclass(self,MutableSet);

	for (unsigned i=0; i < self->nBuckets; i++) {
		SetListItem *item = self->buckets[i];
		while (item) {
			SetListItem *next = item->next;
			release(item->object);
			free(item);
			item = next;
		}
		self->buckets[i] = NULL;
	}
}

MutableSetClass* MutableSetClass_init (MutableSetClass *class)
{
	SET_SUPERCLASS(Set);

        SET_METHOD_POINTER(MutableSet,add);
        SET_METHOD_POINTER(MutableSet,addCString);
        SET_METHOD_POINTER(MutableSet,remove);
        SET_METHOD_POINTER(MutableSet,unionWith);
        SET_METHOD_POINTER(MutableSet,subtract);
        SET_METHOD_POINTER(MutableSet,intersect);
        SET_METHOD_POINTER(MutableSet,removeAll);

	VALIDATE_CLASS_STRUCT(class);
	return class;
}

