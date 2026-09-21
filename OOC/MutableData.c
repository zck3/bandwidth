/*============================================================================
  MutableData, an object-oriented C string manipulation class.
  Copyright (C) 2019, 2022 by Zack T Smith.

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

#include "MutableData.h"

MutableDataClass *_MutableDataClass = NULL;

#define kDefaultMutableDataSize (4096)

void MutableData_destroy (Any* self)
{
	if (!self) {
		return;
	}
	verifyCorrectClass(self,MutableData);
	
	Data_destroy (self);
}

MutableData* MutableData_init (MutableData* restrict self)
{
	ENSURE_CLASS_READY(MutableData);

	if (!self) {
		return NULL;
	}
	
	Data_init ((Data*) self);
	self->is_a = _MutableDataClass;

	self->bytes = malloc (sizeof(Char) * kDefaultMutableDataSize);
	self->allocatedSize = kDefaultMutableDataSize;
	self->length = 0;

	return self;
}

static MutableData* MutableData_initWithBytes (MutableData* restrict self, const uint8_t *bytes, size_t length)
{
	ENSURE_CLASS_READY(MutableData);

	if (bytes == NULL)
		return self;

	if (self) {
		Data_init ((Data*) self);
		self->is_a = _MutableDataClass;

		void *newBytes = ooc_alloc_memory(length);
		if (newBytes) {
			self->bytes = newBytes;
			self->allocatedSize = length;
			self->length = length;
			memcpy (newBytes, bytes, length);
		}
	}
	return self;
}

MutableData* MutableData_withBytes (const uint8_t *bytes, size_t length)
{
	// NULL pointer is allowed.
	MutableData* newData = new(MutableData);
	return MutableData_initWithBytes (newData, bytes, length);
}

static MutableData* MutableData_initWithCString (MutableData* restrict self, const char *string)
{
	ENSURE_CLASS_READY(MutableData);

	if (string == NULL)
		return self;

	if (self) {
		Data_init ((Data*) self);
		self->is_a = _MutableDataClass;

		size_t length = ooc_strlen (string);
		MutableData_initWithBytes (self, (uint8_t*) string, length);
	}
	return self;
}

MutableData* MutableData_withCString (const char *string)
{
	// NULL pointer is allowed.
	MutableData* newData = new(MutableData);
	return MutableData_initWithCString (newData, string);
}

static void reallocIfNecessary (MutableData* restrict self, size_t newLength)
{
	if (!self) {
		return;
	}

	if (newLength > self->allocatedSize) {
		size_t allocatedSize = self->allocatedSize;
		size_t newSize = allocatedSize + newLength;
		size_t additional = allocatedSize >> 2;
		if (additional > kDefaultMutableDataSize) 
			additional = kDefaultMutableDataSize;
		newSize += additional;
		self->bytes = realloc (self->bytes, sizeof(Char) * newSize);
		self->allocatedSize = newSize;
	}
}

static void MutableData_appendBytes (MutableData* restrict self, uint8_t* bytes, size_t length)
{
	if (!self || !bytes || !length) {
		return;
	}
	verifyCorrectClass(self,MutableData);

	size_t currentLength = self->length;
	size_t newLength = currentLength + length;
	reallocIfNecessary (self, newLength + 1);

	memcpy (self->bytes + currentLength, bytes, length);
	self->bytes[newLength] = 0;
	self->length = newLength;
}

static void MutableData_appendByte (MutableData* restrict self, uint8_t byte_)
{
	uint8_t byte = byte_;
	MutableData_appendBytes (self, &byte, 1);
}

static void MutableData_appendData (MutableData* restrict self, void *other_)
{
	if (!self) {
		return;
	}
	verifyCorrectClass(self,MutableData);
	Data *other = (Data*)other_;
	verifyCorrectClassOrSubclass(other,Data);

	uint8_t *bytes = $(other, bytes);
	size_t length = $(other, length);
	MutableData_appendBytes (self, bytes, length);
}

static void MutableData_truncateAt (MutableData* restrict self, size_t index)
{
	if (!self) {
		return;
	}
	verifyCorrectClass(self,MutableData);

	if (index < self->length) {
		self->bytes[index] = 0;
		self->length = index;
	}
}

MutableDataClass* MutableDataClass_init (MutableDataClass *class)
{
	SET_SUPERCLASS(Data);

	// Overridden method
	SET_OVERRIDDEN_METHOD_POINTER(MutableData,destroy);

	// Additional methods
	SET_METHOD_POINTER(MutableData,appendData);
	SET_METHOD_POINTER(MutableData,appendByte);
	SET_METHOD_POINTER(MutableData,appendBytes);
	SET_METHOD_POINTER(MutableData,truncateAt);
	
	VALIDATE_CLASS_STRUCT(class);
	return class;
}

MutableData* MutableData_fromFile (FILE* file)
{
	if (!file) {
		return NULL;
	}

	if (feof(file)) {
		return NULL;
	}

	MutableData *data = new(MutableData);
#define READFILE_BUFFER_SIZE (1<<20)
	uint8_t buffer[READFILE_BUFFER_SIZE];
	while (!feof (file)) {
		size_t count = fread (buffer, 1, READFILE_BUFFER_SIZE, file);
		if (!count) {
			break;
		}
		$(data, appendBytes, buffer, count);
	}
	return data;
}

