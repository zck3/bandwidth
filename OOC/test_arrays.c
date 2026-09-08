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
#include <string.h>
#include <time.h>

#include "String.h"
#include "Array.h"
#include "MutableArray.h"
#include "Int.h"
#include "Double.h"
#include "Testing.h"

int main()
{
	TEST(Array,"empty") {
		Array *ary;
		size_t count;

		ary = new(Array);
		count = $(ary, count);
		EXPECT_ZERO(count, "New array has zero elements");
		EXPECT_ZERO(ary->retainCount, "New array has zero retain count");
		release (ary);
		EXPECT_NULL(ary, "Released array's pointer should be null");
	}

	TEST(Array,"1-element") {
		Array *ary;
		String *str;
		size_t count;
		Object *obj;

		str = String_withCString("abc123");
		ary = Array_withObject((Object*)str, 1);
		count = $(ary, count);
		EXPECT_EQUAL_INT(count, 1, "Array should have 1 element.");

		obj = $(ary, at, 0);
		EXPECT_NONNULL(obj, "Read of existing object at valid index should return non-null");

		EXPECT_TRUE($(str, equals, obj), "Single element added correctly");

		obj = $(ary, at, 100);
		EXPECT_NULL(obj, "Array read at out of range index should return null");

		release(ary);
		EXPECT_NULL(ary, "Released array's pointer should be null");
	}

	TEST(Array,"1001-element") {
#define NUM 1001
		String *str = String_withCString("abc123");
		Array *ary = Array_withObject((Object*)str, NUM);
		size_t count = $(ary, count);
		EXPECT_EQUAL_INT(count, NUM, "Array should have 1001 elements.");

		bool all_found = true;
		bool all_correct = true;
		Object *obj;
		for (int i=0; i < NUM; i++) {
			obj = $(ary, at, i);
			if (!obj) {
				all_found = false;
			} else {
				if (obj != (void*)str) {
					all_correct = false;
				}
			}
		}
		EXPECT_TRUE(all_found, "All 1001 elements should be non-null");
		EXPECT_TRUE(all_correct, "All 1001 elements should be specific string");
		EXPECT_EQUAL_INT(obj->retainCount, NUM, "String's retain count should be 1001");

		release(ary);
		EXPECT_NULL(ary, "Released array's pointer should be null");
	}

	TEST(MutableArray,"1001-element") {
#define NUM 1001
		String *str = String_withCString("abc123");
		MutableArray *mut = MutableArray_withObject((Object*)str, NUM);
		size_t count = $(mut, count);
		EXPECT_EQUAL_INT(count, NUM, "MutableArray should have 1001 elements.");

		bool all_found = true;
		bool all_correct = true;
		Object *obj;
		for (int i=0; i < NUM; i++) {
			obj = $(mut, at, i);
			if (!obj) {
				all_found = false;
			} else {
				if (obj != (void*)str) {
					all_correct = false;
				}
			}
		}
		EXPECT_TRUE(all_found, "All 1001 elements should be non-null");
		EXPECT_TRUE(all_correct, "All 1001 elements should be specific string");
		EXPECT_EQUAL_INT(obj->retainCount, NUM, "String's retain count should be 1001");

		release(mut);
		EXPECT_NULL(mut, "Released array's pointer should be null");
	}

	TEST(Array,"bad array index") {
		Array *ary;
		String *str;
		Object *obj;

		str = String_withCString("abc123");
		ary = Array_withObject((Object*)str, 1);
		obj = $(ary, at, -1);
		EXPECT_NULL(obj, "Array read at negative index should return null");

		obj = $(ary, at, 10);
		EXPECT_NULL(obj, "Array read at invalid large index should return null");

		obj = $(ary, at, 1);
		EXPECT_NULL(obj, "Array read at invalid index should return null");

		obj = $(ary, at, 0);
		EXPECT_NONNULL(obj, "Array read at valid index should return non-null");

		release(ary);
		EXPECT_NULL(ary, "Released array's pointer should be null");
	}

	TEST(MutableArray,"big array") {
		MutableArray *mut = new(MutableArray);

		// Save a value at a large index, causing the MutableArray to resize its internal array.
		Int *integer = _Int(12345);
		long big_index = 216387;
		$(mut, setAt, integer, big_index);

		// Make sure all values are NULL up to the set value.
		int n_non_null = 0;
		for (int i=0; i < big_index; i++) {
			if ($(mut, at, i) != NULL) {
				++n_non_null;
			}
		}
		EXPECT_ZERO(n_non_null, "Mutable array has zero non-null values before set value");

		// Check that the set value is still correct.
		Int *int2 = $(mut, at, big_index);
		EXPECT_NONNULL(int2, "Value already set at big index reads as non-null");
		EXPECT_TRUE($(int2, equals, integer), "Value set at big index is, when read, still what was set");

		// Check some values beyond the set value.
		n_non_null = 0;
		for (int i=1; i < 100; i++) {
			if ($(mut, at, big_index + i) != NULL) {
				++n_non_null;
			}
		}
		EXPECT_ZERO(n_non_null, "Mutable array has all null values beyond set value");
		release(mut);
		EXPECT_NULL(mut, "Released mutable array's pointer should be null");
	}

	TEST(MutableArray,"add remove") {
		MutableArray *mut;
		String *str;
		size_t count;

		str = String_withCString("xyz");
		mut = new(MutableArray);
		count = $(mut, count);
		EXPECT_EQUAL_INT(count, 0, "Array count should be 0");

		$(mut, append, str);
		count = $(mut, count);
		EXPECT_EQUAL_INT(count, 1, "Array count should be 1 after append");

		$(mut, append, str);
		count = $(mut, count);
		EXPECT_EQUAL_INT(count, 2, "Array count should be 2 after append");

		$(mut, removeAt, 0);
		count = $(mut, count);
		EXPECT_EQUAL_INT(count, 1, "Array count should be 1 after removal");

		$(mut, removeAt, 0);
		count = $(mut, count);
		EXPECT_EQUAL_INT(count, 0, "Array count should be 0 after removal");
		release(mut);
		EXPECT_NULL(mut, "Released mutable array's pointer should be null");
	}

	TEST(MutableArray,"empty") {
		MutableArray *mut;
		size_t count;

		mut = new(MutableArray);
		$(mut, removeAll);
		count = $(mut, count);
		EXPECT_ZERO(count, "Mutable array should have 0 items after removeAll");

		srand (1234);
		repeat(1000) {
			$(mut, append, _Int(rand()));
		}
		EXPECT_EQUAL_INT($(mut,count), 1000, "Mutable array should have 1000 pseudorandom integers after appends");

		int n_equal = 0;
		srand (1234);
		repeat_as(1000,i) {
			Int *value = $(mut, at, i);
			if (rand() == $(value, asInt)) {
				++n_equal;
			}
		}
		EXPECT_EQUAL_INT(n_equal, 1000, "Integers in mutable array should have identical pseudorandom values to those we generate a second time");

		$(mut, removeAll);

		count = $(mut, count);
		EXPECT_ZERO(count, "Mutable array should have 0 items after removeAll call");
		release(mut);
		EXPECT_NULL(mut, "Released mutable array's pointer should be null");
	}

	TEST(MutableArray,"append, removeAll") {
		MutableArray *mut = new(MutableArray);
		$(mut, append, _String("abc"));
		$(mut, append, _Int(123));
		$(mut, append, _Double(1.23));
		EXPECT_EQUAL_INT(3, $(mut, count), "Mutable array should have 3 objects after 3 appends");

		String *str = $(mut, at, 0);
		Int *integer = $(mut, at, 1);
		Double *floating = $(mut, at, 2);

		EXPECT_ZERO(strcmp("String", classNameOf(str)),
			"String object's class is String");
		EXPECT_ZERO(strcmp("Int", classNameOf(integer)),
			"Int object's class is Int");
		EXPECT_ZERO(strcmp("Double", classNameOf(floating)),
			"Double object's class is Double");
		
		$(mut, removeAll);
		EXPECT_ZERO($(mut, count), "Emptied mutable array should be empty");
		release(mut);
		EXPECT_NULL(mut, "Released mutable array's pointer should be null");
	}

	TEST(Array,"empty-at") {
		Array *ary = new(Array);
		EXPECT_ZERO($(ary, count), 
			"New array should be empty");
		EXPECT_NULL($(ary, at, 12345000), 
			"Array read at large out of range index should return null");
		EXPECT_NULL($(ary, at, 1000), 
			"Read at large out of range index of empty array should return null");
		EXPECT_NULL($(ary, at, 1), 
			"Read at small index of empty array should return null");
		EXPECT_NULL($(ary, at, 0), 
			"Read at zero index of empty array should return null");
		EXPECT_NULL($(ary, at, -1), 
			"Read at negative index of empty array should return null");
		release(ary);
		EXPECT_NULL(ary, "Released array's pointer should be null");
	}

	TEST(MutableArray,"insertAt(0)") {
		MutableArray *mut = new(MutableArray);

		// Insert 99..0
		repeat_as(100,i) {
			$(mut, insertAt, _Int(i), 0);
		}
		EXPECT_EQUAL_INT(100, $(mut,count), "Mutable array should have 100 objects after 100 insertAt calls");

		// Check all Ints
		int n_different = 0;
		repeat_as(100,i) {
			int expectedValue = 99 - i;
			Int *object = $(mut, at, i);
			int value = $(object, asInt);

			if (value != expectedValue) {
				n_different++;
			}
		}
		EXPECT_ZERO(n_different, "Mutable array decreasing values are correct");
		$(mut, removeAll);
		EXPECT_ZERO($(mut, count), "Emptied mutable array should be empty");
		release(mut);
		EXPECT_NULL(mut, "Released mutable array's pointer should be null");
	}

	// Remove random item
	TEST(MutableArray,"append, removeAt") {
		MutableArray *mut = new(MutableArray);

		srand (time (NULL));

		// Append 0..99
		repeat_as(100,i) {
			$(mut, append, _Int(i));
		}

		// Remove one random value.
		unsigned removedValue = rand() % 100;
		$(mut, removeAt, removedValue);

		bool found[100];
		memset (found, 0, 100);

		repeat_as(100,i) {
			Int *object = $(mut, at, i);
			if (object) {
				int foundValue = $(object, asInt);
				found[foundValue] = 1;
			}
		}

		int n_missing = 0;
		for (int i=0; i < 100; i++) {
			if (!found[i]) {
				n_missing++;
			}
		}
		EXPECT_EQUAL_INT(n_missing, 1, "Only 1 integer should be missing, which is the one that was removed");

		$(mut, removeAll);
		EXPECT_ZERO($(mut, count), "Emptied mutable array should be empty");

		release(mut);
		EXPECT_NULL(mut, "Released mutable array's pointer should be null");
	}

	// Append NULL 
	TEST(MutableArray,"append NULL") {
		MutableArray *mut;
		size_t count;

		mut = new(MutableArray);
		count = $(mut, count);
		EXPECT_EQUAL_INT(count, 0, "New mutable array should have 0 objects");
		$(mut, append, NULL);
		$(mut, append, NULL);
		$(mut, append, NULL);
		$(mut, append, NULL);
		count = $(mut, count);
		EXPECT_EQUAL_INT(count, 4, "Mutable array should have 4 objects after 4 NULLs were appended");

		release(mut);
		EXPECT_NULL(mut, "Released mutable array's pointer should be null");
	}

	TEST(MutableArray,"append NULL, removeAt") {
		MutableArray *mut;
		size_t count;

		mut = new(MutableArray);
		$(mut, append, NULL);
		$(mut, removeAt, 0);
		count = $(mut, count);
		EXPECT_EQUAL_INT(count, 0, "Mutable array should have 0 objects after 1 object was added, then removed");

		release(mut);
		EXPECT_NULL(mut, "Released mutable array's pointer should be null");
	}

	TEST(Array,"with objects") {
		Array *ary = Array_with(
			_Int(321),
			_Double(1.1e100),
			_String("fubar"),
			Array_with(_Int(-1), _String("ok"), NULL), // Subarray
			_Int(1 << 20),
			NULL
		);
		// $(ary,print,0);puts("");
		EXPECT_EQUAL_INT($(ary,count), 5, "New array instantiated with objects should have 5 objects in it");

		release(ary);
		EXPECT_NULL(ary, "Released array's pointer should be null");
	}

	TEST(MutableArray,"with objects") {
		MutableArray *mut = MutableArray_with(
			_Int(123),
			_Double(45.6),
			_String("Fubar"),
			Array_with(_Int(1), _String("OK"), NULL),
			_Int(0xffff),
			NULL
		);
		
		unsigned count = $(mut, count);
		EXPECT_EQUAL_INT(count, 5, "New mutable array instantiated with objects should have 5 objects in it");
		release(mut);
		EXPECT_NULL(mut, "Released mutable array's pointer should be null");
	}

	int retval = Testing_result();
	deallocateClasses();
	return retval;
}

#endif // TESTING
