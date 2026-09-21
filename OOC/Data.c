/*============================================================================
  Data, an object-oriented C immutable byte array class.
  Copyright (C) 2021, 2024 by Zack T Smith.

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

#include "Data.h"
#include "Log.h"

#include <unistd.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <fcntl.h>
#include <locale.h>
//#include <openssl/sha.h>
#include <sys/mman.h>

DataClass *_DataClass = NULL;

static void Data_print (Data* restrict self, FILE *file)
{
	if (!self) {
		return;
	}
	verifyCorrectClassOrSubclass(self,Data);

	if (!file) {
		file = stdout;
	}
	size_t length = self->length;
	for (unsigned i=0; i < length; i++) {
		fprintf (file, "%02x", (unsigned)self->bytes[i]);
	}
}

#if UNUSED
static bool Data_hash (Data* restrict self)
{
	verifyCorrectClass(self,Data);

	// TODO
	SHA256_CTX context;
	SHA256_Init (&context);
	SHA256_Update (&context, input, self->length);
	SHA256_Final (output, &context);
	self->print (self, output);

	return true;
}
#endif

static bool Data_equals (Data* restrict self, void *other_)
{
	if (!self || !other_) {
		return false;
	}
	verifyCorrectClass(self,Data);
	Data *other = (Data*)other_;
	verifyCorrectClassOrSubclass(other,Data);

	if (!self->length && !other->length) {
		return true;
	}
	if (self->length != other->length) {
		return false;
	}
	if (!self->bytes && !other->bytes) {
		return true;
	}
	if (!self->bytes || !other->bytes) {
		return false;
	}

	return !memcmp (self->bytes, other->bytes, self->length);
}

static void Data_describe (Data* restrict self, FILE *file)
{
	if (!self) {
		return;
	}
	verifyCorrectClass(self,Data);
	
#ifdef IS_64BIT
	fprintf (file ?: stdout, "%s(%lu bytes)\n", $(self, className), self->length);
#else
	fprintf (file ?: stdout, "%s(%u bytes)\n", $(self, className), (unsigned)self->length);
#endif
}

void Data_destroy (Any* self_)
{
        DEBUG_DESTROY;

	if (!self_) {
		return;
	}
	verifyCorrectClassOrSubclass(self_,Data);
	
	Data *self = (Data*) self_;
	if (self->bytes) {
		if (self->isMemoryMappedFile) {
			munmap (self->bytes, self->length);
		} else {
			ooc_bzero (self->bytes, self->length);
			ooc_free (self->bytes);
		}
	}
}

Data* Data_init (Data* restrict self)
{
	ENSURE_CLASS_READY(Data);

	if (self) {
		Object_init ((Object*)self);
		self->is_a = _DataClass;

		self->length = 0;
		self->bytes = NULL;
	}
	return self;
}

#ifdef UNUSED
static Data* Data_initWithCString (Data* restrict self, const char *str)
{
	ENSURE_CLASS_READY(Data);

	if (!self) {
		return NULL;
	}
	verifyCorrectClassOrSubclass(self,Data);

	Object_init ((Object*)self);
	self->is_a = _DataClass;

	size_t len = ooc_strlen (str);
	self->length = len;
	self->bytes = malloc (1 + self->length); 
	if (str) {
		for (size_t i=0; i < len; i++)
			self->bytes[i] = str[i];
	}

	return self;
}
#endif

static Data* Data_initFromFilePath (Data* restrict self, const char *path)
{
	ENSURE_CLASS_READY(Data);

	if (!self) {
		return NULL;
	}
	if (!path) {
		return self;
	}

	Object_init ((Object*)self);
	self->is_a = _DataClass;

	int fd = open (path, O_RDONLY);
	if (-1 == fd) {
		perror ("open");
	}
	else {
		struct stat st;
		size_t length;
		if (stat (path, &st)) {
			perror ("stat");
		}
		else {
			length = st.st_size;
			void *address = mmap (NULL, length, PROT_READ, 
				MAP_ANONYMOUS | MAP_FILE | MAP_PRIVATE, fd, 0);
			if (!address) {
				perror ("mmap");
			}
			else {
				self->bytes = address;
				self->length = length;
				self->isMemoryMappedFile = true;
			}
		}
		close(fd);
	}

	return self;
}

static Data* Data_initWithData (Data* restrict self, void *other_)
{
	ENSURE_CLASS_READY(Data);

	if (!self) {
		return NULL;
	}
	if (!other_)
		return self;

	verifyCorrectClass(self,Data);
	Data *other = (Data*)other_;
	verifyCorrectClassOrSubclass(other,Data);

	Object_init ((Object*)self);
	self->is_a = _DataClass;

	size_t len = other->length;
	self->length = len;
	self->bytes = malloc (len); 
	memcpy ((char*) self->bytes, (char*) other->bytes, len);

	return self;
}

// NOTE: This does not duplicate the bytes.
static Data* Data_initWithBytes (Data* restrict self, uint8_t *bytes, size_t length)
{
	ENSURE_CLASS_READY(Data);

	if (!self) {
		return NULL;
	}
	verifyCorrectClass(self,Data);

	Object_init ((Object*)self);
	self->is_a = _DataClass;

	if (bytes && length > 0) {
		self->length = length;
		self->bytes = bytes;
	}

	return self;
}

static size_t Data_length (Data* restrict self)
{
	if (!self) {
		return 0;
	}
	verifyCorrectClassOrSubclass(self,Data);

	return self->length;
}

static char *Data_asCString (Data* restrict self)
{
	if (!self) {
		return NULL;
	}
	verifyCorrectClassOrSubclass(self,Data);

	size_t length = self->length;
	if (!length || !self->bytes) {
		return NULL;
	}

	char *string = malloc (length + 1);
	if (!string) {
		Log_perror(__FUNCTION__, "malloc");
		return NULL;
	}
	memcpy (string, self->bytes, length);
	string[length] = 0;

	/* Caller frees the string */ 
	return string;
}

static String *Data_asString (Data* restrict self)
{
	if (!self) {
		return NULL;
	}
	verifyCorrectClassOrSubclass(self,Data);

	size_t length = self->length;
	if (!length || !self->bytes) {
		return NULL;
	}

	// Allocate space for wide string including terminating null.
	Char *wide = malloc(sizeof(Char) * (length+1));
	if (!wide) {
		Log_perror(__FUNCTION__, "malloc");
		return NULL;
	}

	Char_fromUTF8 (wide, (const char*)self->bytes, length);

	String* string = String_withChars(wide);
	return string;
}

static uint8_t *Data_bytes (Data* restrict self)
{
	if (!self) {
		return 0;
	}
	verifyCorrectClassOrSubclass(self,Data);

	return self->bytes;
}

static uint8_t Data_byteAt (Data* restrict self, size_t index)
{
	if (!self) {
		return 0;
	}
	verifyCorrectClassOrSubclass(self,Data);

	if (index < self->length) {
		return self->bytes[index];
	}
	return 0;
}

static unsigned long Data_sum (Data* restrict self)
{
	if (!self) {
		return 0;
	}
	verifyCorrectClassOrSubclass(self,Data);

	unsigned long sum = 0;
	for (size_t index = 0; index < self->length; index++) {
		sum += self->bytes[index];
	}
	return sum;
}

static bool Data_writeToFilePath (Data* restrict self, const char *path)
{
	if (!self || !path) {
		return false;
	}
	verifyCorrectClassOrSubclass(self,Data);

	FILE *file = fopen (path, "wb");
	if (!file) {
		return false;
	}

	if (self->length && self->bytes) {
		fwrite (self->bytes, 1, self->length, file);
	}

	fflush (file);
	fclose (file);

	return true;
}

Data* Data_withData (Any *other_)
{
	if (!other_)
		return NULL;
	verifyCorrectClassOrSubclass(other_,Data);

	Data *other = other_;
	Data* newData = new(Data);
	return Data_initWithData (newData, other);
}

Data* Data_withBytes (uint8_t *bytes, size_t length)
{
	// NULL pointer is allowed.
	Data* newData = new(Data);
	return Data_initWithBytes (newData, bytes, length);
}

Data* Data_fromFilePath (const char *path)
{
	if (!path) {
		return NULL;
	}

	// NULL pointer is allowed.
	Data* newData = new(Data);
	return Data_initFromFilePath (newData, path);
}

DataClass* DataClass_init (DataClass* restrict class)
{
	SET_SUPERCLASS(Object);

	SET_OVERRIDDEN_METHOD_POINTER(Data,describe);
        SET_OVERRIDDEN_METHOD_POINTER(Data,destroy);
        SET_OVERRIDDEN_METHOD_POINTER(Data,equals);

        SET_METHOD_POINTER(Data,length);
        SET_METHOD_POINTER(Data,print);
	SET_METHOD_POINTER(Data,byteAt);
	SET_METHOD_POINTER(Data,bytes);
	SET_METHOD_POINTER(Data,sum);
	SET_METHOD_POINTER(Data,asCString);
	SET_METHOD_POINTER(Data,asString);
	SET_METHOD_POINTER(Data,writeToFilePath);

	VALIDATE_CLASS_STRUCT(class);
	return class;
}

