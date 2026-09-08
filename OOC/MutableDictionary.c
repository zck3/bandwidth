/*============================================================================
  MutableDictionary, an object-oriented C mutable dictionary class.
  Copyright (C) 2023 by Zack T Smith.

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

#include "MutableDictionary.h"
#include "Log.h"

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

MutableDictionaryClass *_MutableDictionaryClass = NULL;

#ifdef UNUSED
static MutableDictionary* MutableDictionary_initWithSize (MutableDictionary* restrict self, DictionarySize size)
{
	ENSURE_CLASS_READY(MutableDictionary);

	if (self) {
		Object_init ((Object*)self);

		// Skip call to Dictionary_init since that doesn't do anything 
		// more than this, and it will allocate buckets.

		self->is_a = _MutableDictionaryClass;
		switch (size) {
		default:
		case DictionarySizeSmall:
			self->nBuckets = 127;
			break;
		case DictionarySizeMedium:
			self->nBuckets = 1279;
			break;
		case DictionarySizeLarge:
			self->nBuckets = 12721;
			break;
		}
		size_t nBytes = sizeof(DictionaryListItem*) * self->nBuckets;
		self->buckets = (DictionaryListItem**) malloc(nBytes);
		ooc_bzero (self->buckets, nBytes);
	}
	return self;
}
#endif

MutableDictionary* MutableDictionary_init (MutableDictionary* restrict self)
{
	ENSURE_CLASS_READY(MutableDictionary);

	if (self) {
		Dictionary_init ((Dictionary*)self);
		self->is_a = _MutableDictionaryClass;
	}

	return self;
}

static void MutableDictionary_set (MutableDictionary* restrict self, String *key, Object *value)
{
	if (!self || !key || !value) {
		return;
	}
	verifyCorrectClassOrSubclass(self,MutableDictionary);
	verifyCorrectClassOrSubclass(key,String);

	unsigned hash = $(key, hash);
	unsigned index = hash % self->nBuckets;

	DictionaryListItem *item = self->buckets[index];
	bool found = false;
	while (item) {
		if ($(key, equals, item->key)) {
			found = true;
			if (item->value != value) {
				release(item->value);
				item->value = retain(value);
			}
			break;
		}
		item = item->next;
	}
	if (!found) {
		DictionaryListItem *item = (DictionaryListItem*) malloc(sizeof(DictionaryListItem));
		item->key = retain(key);
		item->value = retain(value);
		item->next = self->buckets[index];
		self->buckets[index] = item;
	}
}

static void MutableDictionary_removeAll (MutableDictionary* restrict self)
{
	if (!self) {
		return;
	}
	verifyCorrectClassOrSubclass(self,MutableDictionary);

	for (unsigned i=0; i < self->nBuckets; i++) {
		DictionaryListItem *item = self->buckets[i];
		while (item) {
			DictionaryListItem *next = item->next;
			release(item->key);
			release(item->value);
			free(item);
			item = next;
		}
		self->buckets[i] = NULL;
	}
}

static void MutableDictionary_remove (MutableDictionary* restrict self, String *key)
{
	if (!self || !key) {
		return;
	}
	verifyCorrectClassOrSubclass(self,MutableDictionary);
	verifyCorrectClassOrSubclass(key,String);

	unsigned hash = $(key, hash);
	unsigned index = hash % self->nBuckets;

	DictionaryListItem *item = self->buckets[index];
	DictionaryListItem *prev = NULL;
	while (item) {
		if ($(key, equals, item->key)) {
			if (!prev) {
				self->buckets[index] = item->next;
			} else {
				prev->next = item->next;
			}
			release(item->key);
			release(item->value);
			free (item);
			break;
		}
		prev = item;
		item = item->next;
	}
}

MutableDictionaryClass* MutableDictionaryClass_init (MutableDictionaryClass *class)
{
	SET_SUPERCLASS(Dictionary);

        SET_METHOD_POINTER(MutableDictionary,set);
        SET_METHOD_POINTER(MutableDictionary,remove);
        SET_METHOD_POINTER(MutableDictionary,removeAll);

	VALIDATE_CLASS_STRUCT(class);
	return class;
}

