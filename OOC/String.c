/*============================================================================
  String, an object-oriented C string class.
  Copyright (C) 2019, 2023-2024 by Zack T Smith.

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

#include "String.h"
#include "MutableString.h"
#include "MutableArray.h"
#include "Int.h"
#include "Double.h"
#include "Log.h"

#include <ctype.h> 
#include <wchar.h> 

StringClass *_StringClass = NULL;

#define kDefaultStringSize (32)

static int String_parseInt (String* restrict self)
{
	if (!self) {
		return 0;
	}
	verifyCorrectClassOrSubclass(self,String);

	Char *string = self->chars;
	return string? Char_strtol(string) : 0;
}

static double String_parseDouble (String* restrict self)
{
	if (!self) {
		return 0;
	}
	verifyCorrectClassOrSubclass(self,String);

	Char *string = self->chars;
	return string? Char_strtod(string) : 0.0;
}

static unsigned long String_parseHex (String* restrict self)
{
	if (!self) {
		return 0;
	}
	verifyCorrectClassOrSubclass(self,String);

	Char *string = self->chars;
	if (!string) {
		return 0;
	}
	if (*string == '0' && string[1] == 'x') {
		string += 2;
	}
	return Char_strtod (string);
}

static unsigned long String_length (String* restrict self)
{
	if (!self) {
		return 0;
	}
	verifyCorrectClassOrSubclass(self,String);

	if (!self->chars) {
		return 0;
	}
	return Char_strlen(self->chars);
}

static void String_describe (String* restrict self, FILE *file)
{
	if (!self) {
		return;
	}
	verifyCorrectClassOrSubclass(self,String);
	
	fprintf (file ?: stdout, "%s(%lu chars)\n", $(self, className), $(self, length));
}

static void String_print (String* restrict self, FILE* file)
{
	if (!self) {
		return;
	}
	verifyCorrectClassOrSubclass(self,String);

	if (!file) {
		file = stdout;
	}

	if (self->chars != NULL) {
		size_t len = Char_strlen(self->chars);
		for (size_t i=0; i < len; i++) {
			Char ch = self->chars[i];
			// TODO convert to UTF8
			if (ch > 0 && ch < 128) {
				fputc (ch, file);
			}
		}
	}
}

static void String_printWithEscapedChar (String* restrict self, FILE* file, char escapedChar)
{
	if (!self) {
		return;
	}
	verifyCorrectClassOrSubclass(self,String);

	if (escapedChar == 0) {
		String_print (self, file);
		return;
	}

	if (!file) {
		file = stdout;
	}

	if (self->chars != NULL) {
		size_t len = Char_strlen (self->chars);
		for (size_t i = 0; i < len; i++) {
			Char ch = self->chars[i];
			if (ch == escapedChar) {
				fputc ('\\', file);
			}

			// XX Unicode not supported yet.
			if (ch > 0 && ch <= 127) {
				fputc (ch, file);
			}
		}
	}
}

static bool String_equals (String* restrict self, void *other_) 
{ 
	if (!self || !other_) {
		return false;
	}
	verifyCorrectClassOrSubclass(self,String);
	String *other = (String*) other_;
	verifyCorrectClassOrSubclass(other,String);

	if (self->chars == NULL && other->chars == NULL) {
		return true;
	}
	if (self->chars == NULL || other->chars == NULL) {
		return false;
	}
	size_t lenSelf = Char_strlen(self->chars);
	size_t lenOther = Char_strlen(other->chars);
	if (!lenSelf && !lenOther) {
		return true;
	}
	if (lenSelf != lenOther) {
		return false;
	}
	if (Char_strcmp (self->chars, other->chars)) {
		return false;
	}

	return true;
}

static unsigned String_totalLines (String* restrict self)
{
	if (!self) {
		return 0;
	}
	verifyCorrectClassOrSubclass(self,String);

	unsigned nLines = 1;
	Char *ptr = self->chars;
	while (*ptr) {
		if (*ptr == '\n')
			nLines++;
		ptr++;
	}

	return nLines;
}

void String_destroy (Any* self_)
{
        DEBUG_DESTROY;

	if (!self_) {
		return;
	}
	verifyCorrectClassOrSubclass(self_,String);

	String *self = self_;

	if (self->chars) {
		ooc_free (self->chars);
		self->chars = NULL;
	}

	Object_destroy (self);
}

String* String_init (String* restrict self)
{
	ENSURE_CLASS_READY(String);

	if (self) {
		Object_init ((Object*)self);
		self->is_a = _StringClass;

		self->chars = NULL;
	}
	return self;
}

static String* String_initWithCString (String* restrict self, const char *cstring)
{
	ENSURE_CLASS_READY(String);

	if (self) {
		Object_init ((Object*)self);
		self->is_a = _StringClass;

		if (cstring) {
			unsigned length = ooc_strlen (cstring);
			if (length > 0) {
				self->chars = ooc_alloc_memory(sizeof(Char) * (length+1));
				if (self->chars != NULL) {
					uint8_t *bytes = (uint8_t*)cstring;

					for (int i = 0; i < length; i++) {
						self->chars[i] = bytes[i];
					}

					self->chars[length] = 0;
				}
			}
		}
	}
	return self;
}

static String* String_initWithWideStringAndLength (String* restrict self, const Char *string, size_t length)
{
	ENSURE_CLASS_READY(String);

	if (self) {
		Object_init ((Object*)self);
		self->is_a = _StringClass;

		if (string && length) {
			void *newChars = ooc_alloc_memory (sizeof(Char) * (length+1));
			if (newChars != NULL) {
				self->chars = newChars;
				memcpy (newChars, string, length * sizeof(Char));
				self->chars[length] = 0;
			}
		}
	}
	return self;
}

static String* String_initWithWide (String* restrict self, const Char *string)
{
	if (string)
		return String_initWithWideStringAndLength (self, string, Char_strlen(string));
	else
		return String_init(self);
}

String* String_withChars (const Char *chars)
{
	String *string = allocate(String);
	return String_initWithWide (string, chars);
}

String* String_withCharsStringAndLength (const Char *chars, size_t length)
{
	String *string = allocate(String);
	return String_initWithWideStringAndLength (string, chars, length);
}

static bool String_hasSuffix (String* restrict self, void *other_)
{
	if (!self || !other_) {
		return false;
	}

	verifyCorrectClassOrSubclass(self,String);
	verifyCorrectClassOrSubclass(other_,String);

	Char *selfPtr = self->chars;
	size_t selfLength = selfPtr ? Char_strlen(selfPtr) : 0;

	String *other = (String*)other_;
	verifyCorrectClassOrSubclass(other,String);

	size_t otherLength = $(other, length);
	if (otherLength > selfLength) {
		return false;
	}

	size_t selfIndex = selfLength - otherLength;
	size_t otherIndex = 0;
	Char *otherPtr = other->chars;
	while (selfIndex < selfLength) {
		Char selfChar = selfPtr[selfIndex];
		if (selfChar != otherPtr[otherIndex]) {
			return false;
		}
		selfIndex++;
		otherIndex++;
	}

	return true;
}

static bool String_hasCStringPrefix (String* restrict self, const char *other_)
{
	if (!self || !other_ || !self->chars) {
		return false;
	}

	size_t selfLength = Char_strlen(self->chars);
	size_t otherLength = strlen (other_);
	if (otherLength > selfLength) {
		return false;
	}

	uint8_t *other = (uint8_t*) other_;

	size_t index = 0;
	Char *selfPtr = self->chars;

	while (index < otherLength) {
		Char selfChar = selfPtr[index];
		if (selfChar != (Char) other[index]) {
			return false;
		}
		index++;
	}

	return true;
}

static bool String_hasCStringSuffix (String* restrict self, const char *other_)
{
	if (!self || !other_ || !self->chars) {
		return false;
	}

	size_t selfLength = Char_strlen(self->chars);
	size_t otherLength = strlen (other_);
	if (otherLength > selfLength) {
		return false;
	}

	size_t selfIndex = selfLength - otherLength;

	uint8_t *other = (uint8_t*) other_;

	size_t index = 0;
	Char *selfPtr = self->chars;

	while (selfIndex < selfLength) {
		Char selfChar = selfPtr[selfIndex++];
		Char otherChar = (Char) other[index++];
		if (selfChar != otherChar) {
			return false;
		}
	}

	return true;
}

static bool String_hasPrefix (String* restrict self, Any *other_)
{
	if (!self || !other_) {
		return false;
	}

	verifyCorrectClassOrSubclass(self,String);
	verifyCorrectClassOrSubclass(other_,String);
	String *other = other_;

	Char *selfPtr = self->chars;
	size_t selfLength = selfPtr ? Char_strlen(selfPtr) : 0;

	size_t otherLength = $(other, length);
	if (otherLength > selfLength) {
		return false;
	}

	size_t index = 0;
	Char *otherPtr = other->chars;
	while (index < otherLength) {
		Char selfChar = selfPtr[index];
		if (selfChar != otherPtr[index]) {
			return false;
		}
		index++;
	}
	return true;
}

static Char* String_characters (String* restrict self)
{
	if (!self) {
		return NULL;
	}
	verifyCorrectClassOrSubclass(self,String);

	return self->chars;
}

static Char String_at (String* restrict self, int index)
{
	if (!self || index < 0) {
		return 0;
	}
	verifyCorrectClassOrSubclass(self,String);

	if (self->chars == NULL) {
		return 0;
	}

	int length = Char_strlen(self->chars);
	if (index < length) {
		return self->chars[index];
	}

	return 0;
}

typedef enum {
	TypeString = 0,
	TypeInteger = 1,
	TypeFloat = 2,
	TypeNull = 3,
#ifdef TODO
	TypeBool = 4, 
	TypeUnsigned = 5, // e.g. hex
#endif
} ObjType;
static ObjType wchar_string_determine_type (Char *str, size_t length)
{
	if (!str || !*str || !length) {
		return TypeString;
	}

	Char null[5] = { 'N', 'U', 'L', 'L', 0 };
	if (length == 4 && !Char_strcasecmp (null, str)) {
		return TypeNull;
	}

	int n_digits = 0;
	int n_dots = 0;
	bool minus_prefix = *str == '-';
	if (minus_prefix) {
		str++;
	}

	while (*str && length > 0) {
		Char ch = *str++;
		length--;

		if ('.' == ch) {
			n_dots++;
		}
		else if (isdigit(ch)) {
			n_digits++;
		}
		else {
			return TypeString;
		}
	}

	if (n_digits >= 1) {
		if (n_dots == 0) {
			return TypeInteger;
		} else {
			return TypeFloat;
		}
	}
	return TypeString;
}

/* Convert null-terminated Char string to an object of an appropriate type.
 */
static Any *wchar_string_to_object (Char *str, size_t length)
{
	Any *newObject = NULL;
	switch (wchar_string_determine_type(str, length)) {
	case TypeFloat:
		newObject = Double_withLongLong(Char_strtod(str));
		break;
	case TypeInteger:
		newObject = Int_withLongLong(Char_strtol(str));
		break;
	case TypeNull:
		break;
	default:
		newObject = String_withCharsStringAndLength (str, length);
	}
	return newObject;
}

static Array *String_explode (String* restrict self, Char separator, 
			bool respectQuotes,
			bool doTrimming,
			bool convertToAppropriateType)
{
	if (!self || !separator) {
		return NULL;
	}
	verifyCorrectClassOrSubclass(self,String);
	Char *originalChars = self->chars;
	if (!originalChars || !*originalChars) {
		return NULL;
	}

	size_t originalLength = Char_strlen(originalChars);

	bool inQuotes = false;

	unsigned stringSize = 1024;
	Char *string = malloc(stringSize);
	if (!string) {
		return NULL;
	}

	MutableArray *mut = new(MutableArray);

	size_t index = 0;
	size_t stringIndex = 0;
	Char *ptr = originalChars;
	Char ch = 0;

	while (index < originalLength) {
		ch = ptr[index++];

		// Respect backslashed characters.
		bool backslashed = false;
		if (ch == '\\' && index != originalLength-1) {
			backslashed = true;
			ch = ptr[index++];
		}

		if (!backslashed) {
			// Respect double quotes if so directed.
			if (ch == '"' && respectQuotes) {
				inQuotes = !inQuotes;
				continue;
			}

			// Second priority is, find separators.
			if (!inQuotes && ch == separator) {
				Char *str = string;
				size_t len = stringIndex;
				str[len] = 0;

				if (doTrimming) {
					str = Char_trimString (str, &len);
				}
				Any *newObject;
				if (convertToAppropriateType) {
					newObject = wchar_string_to_object (str, len);
				} else {
					newObject = String_withCharsStringAndLength (str, len);
				}
				$(mut, append, newObject);
				stringIndex = 0;
				continue;
			}
		}
		
		// Third priority is, store characters and, if needed, resize array.
		string[stringIndex++] = ch;
		if (stringIndex >= stringSize) {
			stringSize <<= 1;
			string = realloc(string, stringSize);
			if (!string) {
				Log_perror(__FUNCTION__, "realloc");
				release(mut);
				return NULL;
			}
		}
	}

	if (stringIndex || ch == separator) {
		Char *str = string;
		size_t len = stringIndex;
		str[len] = 0;

		if (doTrimming) {
			str = Char_trimString (str, &len);
		}
		Any *newObject;
		if (convertToAppropriateType) {
			newObject = wchar_string_to_object (str, len);
		} else {
			newObject = String_withCharsStringAndLength (str, len);
		}
		$(mut, append, newObject);
	}

	Array *result = Array_withArray (mut);
	release(mut);

	return result;
}

static unsigned String_hash (String* restrict self)
{
	if (!self) {
		return 0;
	}
	verifyCorrectClassOrSubclass(self,String);

	unsigned long sum = 0;
	Char *ptr = self->chars;
	if (!ptr) {
		return 0;
	}

	while (*ptr) {
		sum <<= 1;
		sum += *ptr++;
	}

	return sum;
}

static int String_compare (String* restrict self, Any *other_)
{
	if (!self) {
		return 0;
	}
	verifyCorrectClassOrSubclass(self,String);
	verifyCorrectClassOrSubclass(other_,String);

	String *other = other_;

	Char *a = self->chars;
	Char *b = other->chars;
	if (!a && b) {
		return CharLessThan;
	}
	if (a && !b) {
		return CharGreaterThan;
	}
	if (!a && !b) {
		return CharEqual;
	}
	return Char_strcmp (a, b);
}

static const char *String_asUTF8 (String* restrict self)
{
	if (!self) {
		return NULL;
	}
	verifyCorrectClassOrSubclass(self,String);

	Char *source = self->chars;
	if (!source || !*source) {
		return NULL;
	}

	size_t len = Char_strlen(source);

	size_t bytesNeeded = len; // Not supporting Unicode yet.
	bytesNeeded++;

	char *destination = ooc_alloc_memory (bytesNeeded);
	(void)Char_toUTF8 (destination, source, bytesNeeded);

	// RULE: Caller frees the UTF8 string.
	return destination;
}

static bool String_containsChar (String* restrict self, char ch)
{
	if (!self || !ch) {
		return false;
	}
	verifyCorrectClassOrSubclass(self,String);

	Char *source = self->chars;
	if (!source) {
		return false;
	}

	for (size_t index = 0; source[index]; index++) {
		if (ch == source[index]) {
			return true;
		}
	}

	return false;
}

static bool String_equalsCString (String* restrict self, const char* cstring)
{
	if (!self || !cstring) {
		return false;
	}
	verifyCorrectClassOrSubclass(self,String);

	Char *source = self->chars;
	size_t len = source ? Char_strlen (source) : 0;
	size_t cslen = strlen (cstring);

	if (!len && !cslen) {
		return true;
	}
	if (len != cslen) {
		return false;
	}

	for (unsigned i=0; i < len; i++) {
		Char ch = cstring[i];
		if (source[i] != ch) {
			return false;
		}
	}

	return true;
}

static bool String_containsCString (String* restrict self, const char *cstring)
{
	if (!self || !cstring || !*cstring) {
		return false;
	}
	verifyCorrectClassOrSubclass(self,String);

	Char *source = self->chars;
	if (!source || !*source) {
		return false;
	}

	size_t len = Char_strlen (source);
	size_t cslen = strlen (cstring);

	for (int i=0; i <= len-cslen; i++) {
		if (source[i] == (Char) cstring[0]) {
			bool match = true;
			for (int j=1; j < cslen; j++) {
				char ch = cstring[j];
				if (!ch) {
					return true;
				}
				if (ch != source[i+j]) {
					match = false;
					break;
				}
				if (match) {
					return true;
				}
			}
		}
	}
	return false;
}

String* String_withCString (const char* str)
{
	return String_initWithCString (allocate(String), str);
}

String* String_withWide (const wchar_t *wide)
{
	if (!wide) {
		return NULL;
	}
	size_t length = wcslen(wide);
	if (!length) {
		return new(String);
	}
	Char *chars = malloc(sizeof(Char) * (length+1));
	if (!chars) {
		Log_perror(__FUNCTION__, "malloc");
		return NULL;
	}
	String *string = new(String);
	string->chars = chars;
	for (int i = 0; i < length; i++) {
		chars[i] = wide[i];
	}
	chars[length] = 0;
	return string;
}

static String* String_substring (String* restrict self, int index, int length)
{
	if (!self) {
		return NULL;
	}
	verifyCorrectClassOrSubclass(self,String);

	int stringLength = Char_strlen(self->chars);

	String *string = new(String);

	if (!self->chars || length <= 0 || index >= stringLength) {
		return string;
	}

	// RULE: If the index is negative, determine what Chars in the range will be within the string and return those.
	if (index < 0) {
		length += index;
		index = 0;
		if (length <= 0) {
			return string;
		}
	}

	// RULE: If the range extends beyond the end of the string, reduce the length to only include Chars in the string.
	if (index+length >= stringLength) {
		length = stringLength - index;
	}

	string->chars = Char_strndup (&self->chars[index], length);
	return string;
}

static String* String_prefixOfLength (String* restrict self, int length)
{
	return String_substring (self, 0, length);
}

static String* String_suffixOfLength (String* restrict self, int length)
{
	if (!self) {
		return NULL;
	}
	verifyCorrectClassOrSubclass(self,String);

	Char *chars = self->chars;
	if (chars) {
		int stringLength = Char_strlen(chars);
		return String_substring (self, stringLength-length, length);
	} 

	return new(String);
}


StringClass* StringClass_init (StringClass *class)
{
	SET_SUPERCLASS(Object);

	SET_OVERRIDDEN_METHOD_POINTER(String,describe);
        SET_OVERRIDDEN_METHOD_POINTER(String,destroy);
        SET_OVERRIDDEN_METHOD_POINTER(String,equals);
        SET_OVERRIDDEN_METHOD_POINTER(String,compare);
        SET_OVERRIDDEN_METHOD_POINTER(String,hash);

        SET_METHOD_POINTER(String,length);
        SET_METHOD_POINTER(String,print);
	SET_METHOD_POINTER(String,at);
        SET_METHOD_POINTER(String,characters);
        SET_METHOD_POINTER(String,totalLines);
        SET_METHOD_POINTER(String,explode);
        SET_METHOD_POINTER(String,asUTF8);
	SET_METHOD_POINTER(String,parseInt);
	SET_METHOD_POINTER(String,parseDouble);
	SET_METHOD_POINTER(String,parseHex);
	SET_METHOD_POINTER(String,containsCString);
	SET_METHOD_POINTER(String,containsChar);
	SET_METHOD_POINTER(String,printWithEscapedChar);
	SET_METHOD_POINTER(String,hasCStringSuffix);
	SET_METHOD_POINTER(String,hasCStringPrefix);
        SET_METHOD_POINTER(String,hasSuffix);
        SET_METHOD_POINTER(String,hasPrefix);
	SET_METHOD_POINTER(String,equalsCString);
	SET_METHOD_POINTER(String,prefixOfLength);
	SET_METHOD_POINTER(String,suffixOfLength);
	SET_METHOD_POINTER(String,substring);
	
	VALIDATE_CLASS_STRUCT(class);
	return class;
}

