/*============================================================================
  OOC, a library for object-oriented programming using C.
  Copyright (C) 2023-2024, 2026 by Zack T Smith.

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

#ifdef TESTING

#include <stdio.h>
#include <time.h>

#include "MutableDictionary.h"
#include "Testing.h"

int main()
{
	TEST(MutableDictionary,"empty") {
		MutableDictionary *mut = new(MutableDictionary);
		unsigned total = $(mut, count);
		EXPECT_ZERO(total, "New mutable dict should be empty");

		release (mut);
		EXPECT_NULL(mut, "Released mutable dict's pointer should be null");
	}

	TEST(MutableDictionary,"2 keys") {
		MutableDictionary *mut;
		unsigned total;

		mut = new(MutableDictionary);
		String *key1 = _String("ok");
		String *value1 = _String("what");
		$(mut, set, key1, (Object*) value1);
		total = $(mut, count);
		EXPECT_EQUAL_INT (total, 1, "Mutable dict with 1 key-value pair should have count of 1");

		String *value = (String*) $(mut, get, String_withCString("ok"));
		EXPECT_NONNULL (value, "Dict values for key is non-null");
		EXPECT_TRUE ($(value, equals, value1), "Dict value for key is correct");

		String *key2 = _String("something");
		value = (String*) $(mut, get, _String("ok2"));
		EXPECT_NULL (value, "Mutable dict value for bogus key is null");

		$(mut, set, key2, (Object*) _String("else"));
		//$(mut, print, NULL);

		$(mut, remove, key1);
		total = $(mut, count);
		EXPECT_EQUAL_INT(total, 1, "Mutable dict originally with 2 key-value pairs, but one removed, should have a count of 1");

		$(mut, remove, key2);
		total = $(mut, count);
		EXPECT_ZERO(total, "After mutable dict keys removed, dict should be empty");

		release (mut);
		EXPECT_NULL(mut, "Released mutable dict's pointer should be null");
	}

	// Empty immutable dictionary
	TEST(Dictionary,"empty") {
		Dictionary *dict = new(Dictionary);
		unsigned total = $(dict, count);
		EXPECT_ZERO(total, "New dict should be empty");

		release (dict);
		EXPECT_NULL(dict, "Released dict's pointer should be null");
	}

	TEST(Dictionary,"1-pair") {
		Dictionary *dict;
		unsigned total;

		String *key = String_withCString("hey");
		String *value = _String("what");
		dict = Dictionary_newWith(key, (Object*) value);

		total = $(dict, count);
		EXPECT_EQUAL_INT(total, 1, "Dictionary instantiated with 1 key-value pair should have a count of 1");

		Object *object = $(dict, get, key);
		EXPECT_NONNULL(object, "Dict values for key is non-null");
		EXPECT_TRUE($(value, equals, object), "Dict value for key is correct");

		//$(dict, print, NULL);
		release (dict);
		EXPECT_NULL(dict, "Released dict's pointer should be null");
	}

	// 1-pair CString dictionary
	TEST(Dictionary,"1-pair CString") {
		Dictionary *dict = Dictionary_newWithCStrings ("some key", "some value");

		unsigned total = $(dict, count);
		EXPECT_EQUAL_INT(total, 1, "Dictionary instantiated with 1 key-value pair of C-Strings should have a count of 1");

		String *key = _String("some key");
		String *value = _String("some value");

		Object *foundValue = $(dict, get, key);
		EXPECT_NONNULL(foundValue, 
			"Dict values for key is non-null");
		EXPECT_TRUE($(foundValue, equals, value), 
			"Dict value for key is correct");

		//$(dict, print, NULL);
		release (dict);
		EXPECT_NULL(dict, "Released dict's pointer should be null");
	}

	int retval = Testing_result();
	deallocateClasses();
	return retval;
}

#endif // TESTING
