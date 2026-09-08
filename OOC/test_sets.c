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

#include "MutableSet.h"
#include "String.h"
#include "Int.h"
#include "Double.h"
#include "Testing.h"

int main()
{
	// Empty mutable set
	TEST(MutableSet,"empty") {
		MutableSet *mut = new(MutableSet);
		EXPECT_ZERO($(mut, count), "New mutable set should have 0 items");
		EXPECT_TRUE($(mut, isEmpty), "New mutable set should be empty");

		release (mut);
		EXPECT_NULL(mut, "Released mutable set's pointer should be null");
	}

	// Empty set
	TEST(Set,"empty") {
		Set *set = new(Set);
		EXPECT_ZERO($(set, count), "New set should have 0 items");
		EXPECT_TRUE($(set, isEmpty), "New set should be empty");

		release (set);
		EXPECT_NULL(set, "Released set's pointer should be null");
	}

	TEST(MutableSet,"add") {
		unsigned total;
		MutableSet *mut = new(MutableSet);
		String *object1 = String_withCString("fromage");

		// Re-add same item 100 times, resulting in set of 1 item.
		repeat(100) {
			$(mut, add, object1);
		}
		total = $(mut, count);
		EXPECT_EQUAL_INT(1, total, "After re-adding same item 100 times to a new mutable set, count should be 1");

		EXPECT_FALSE($(mut, isEmpty), "Modified mutable set should be non-empty");

		EXPECT_TRUE($(mut, contains, String_withCString("fromage")),
			"string added to mutable set is present");
		EXPECT_FALSE($(mut, contains, String_withCString("cheese")),
			"string not added to mutable set is not present");

		release (mut);
		EXPECT_NULL(mut, "Released mutable set's pointer should be null");
	}

	TEST(MutableSet,"removal") {
		unsigned total;
		MutableSet *mut = new(MutableSet);

		String *object1 = String_withCString("Kaese");
		$(mut, add, object1);
		String *object2 = String_withCString("Kaas");
		$(mut, add, object2);
		EXPECT_TRUE ($(mut, contains, String_withCString("Kaese")),
			"string added to mutable set is present");
		EXPECT_TRUE ($(mut, contains, String_withCString("Kaas")),
			"string added to mutable set is present");
		EXPECT_FALSE($(mut, contains, String_withCString("Oranges")),
			"string not added to mutable set is not present");
		EXPECT_FALSE($(mut, contains, String_withCString("")),
			"empty string not added to mutable set is not present");

		$(mut, remove, object1);
		total = $(mut, count);
		EXPECT_EQUAL_INT(1, total, "After removing 1 object from a mutable set of 2, count should be 1");

		$(mut, remove, String_withCString("Kaese"));
		total = $(mut, count);
		EXPECT_EQUAL_INT(1, total, "After the same object a second time from a mutable set, count should be 1");

		$(mut, remove, String_withCString("Fromage"));
		total = $(mut, count);
		EXPECT_EQUAL_INT(1, total, "After trying to remove an object that is not in the mutable set, count should be 1");

		$(mut, remove, String_withCString("Kaas"));
		total = $(mut, count);
		EXPECT_ZERO(total, "Emptied mutable set should have 0 items");
		release (mut);
		EXPECT_NULL(mut, "Released mutable set's pointer should be null");
	}

	TEST(MutableSet,"union") {
		MutableSet *mut = new(MutableSet);
		EXPECT_EQUAL_INT(0, $(mut, count), "New mutable set should have 0 items");
		$(mut, add, _String("cheese"));
		$(mut, add, _String("fromage"));
		EXPECT_EQUAL_INT(2, $(mut, count), "After adding 2 objects, the mutable set should have 2 items");

		Set *set = Set_with(_String("formaggio"), NULL);
		EXPECT_EQUAL_INT(1, $(set , count), "New set instantiated with just 1 item");

		$(mut, unionWith, set);
		EXPECT_EQUAL_INT(3, $(mut, count), "A union of mutable set and set should result in the mutable set having 3 items");

		release (mut);
		EXPECT_NULL(mut, "Released mutable set's pointer should be null");

		release (set);
		EXPECT_NULL(set, "Released set's pointer should be null");
	}

	TEST(MutableSet,"intersect 2-vs-1") {
		MutableSet *mut = new(MutableSet);

		EXPECT_EQUAL_INT(0, $(mut, count), "New mutable set should be empty");
		$(mut, add, _String("cheese"));
		$(mut, add, _String("fromage"));
		EXPECT_EQUAL_INT(2, $(mut, count), "After adding 2 objects, the mutable set should have 2 items");
		
		Set *set = Set_with(_String("cheese"), NULL);
		EXPECT_EQUAL_INT(1, $(set , count), "New set instantiated with just 1 item");

		$(mut, intersect, set);
		EXPECT_EQUAL_INT(1, $(mut, count), "The intersect of the mutable and immutable sets should result in the mutable set having only 1 item");

		release (mut);
		EXPECT_NULL(mut, "Released mutable set's pointer should be null");

		release (set);
		EXPECT_NULL(set, "Released set's pointer should be null");
	}

	TEST(MutableSet,"intersect 2-vs-0") {
		MutableSet *mut = new(MutableSet);
		EXPECT_EQUAL_INT(0, $(mut, count), "New mutable set should be empty");
		$(mut, add, _String("cheese"));
		$(mut, add, _String("fromage"));
		EXPECT_EQUAL_INT(2, $(mut, count), "After adding 2 objects, the mutable set should have 2 items");
		
		Set *set = new(Set);
		EXPECT_EQUAL_INT(0, $(set, count), "New empty set should have 0 items");

		$(mut, intersect, set);
		EXPECT_EQUAL_INT(0, $(mut, count), "An intersect between the mutable set(2 items) and empty set should result in the mutable set having 0 items");

		release (mut);
		EXPECT_NULL(mut, "Released mutable set's pointer should be null");

		release (set);
		EXPECT_NULL(set, "Released set's pointer should be null");
	}

	TEST(MutableSet,"intersect 1-vs-2") {
		MutableSet *mut = new(MutableSet);
		EXPECT_EQUAL_INT(0, $(mut, count), "New mutable set should be empty");

		$(mut, add, _String("cheese"));
		EXPECT_EQUAL_INT(1, $(mut, count), "After adding 1 object, the mutable set should have 1 item");
		
		Set *set = Set_with(
			_String("cheese"), 
			_String("Kaese"), 
			NULL);
		EXPECT_EQUAL_INT(2, $(set, count), "New set instantiated with 2 objects should have count of 2");

		$(mut, intersect, set);
		EXPECT_EQUAL_INT(1, $(mut, count), "Intersect of mutable set with 1 item and set with 2 items results in mutable set having 1 item");

		release (mut);
		EXPECT_NULL(mut, "Released mutable set's pointer should be null");

		release (set);
		EXPECT_NULL(set, "Released set's pointer should be null");
	}

	TEST(MutableSet,"intersect 0-vs-2") {
		MutableSet *mut = new(MutableSet);
		EXPECT_EQUAL_INT(0, $(mut, count), "New mutable set should be empty");
		
		Set *set = Set_with(
			_String("cheese"), 
			_String("Kaese"), 
			NULL);
		EXPECT_EQUAL_INT(2, $(set, count), "New set instantiated with 2 items should have a count of 2");

		$(mut, intersect, set);
		EXPECT_EQUAL_INT(0, $(mut, count), "After the intersect with an empty set, the mutable set should have a count of 0");
		EXPECT_TRUE($(mut, isEmpty), "After the intersect with an empty set, the mutable set should be empty");

		release (mut);
		EXPECT_NULL(mut, "Released mutable set's pointer should be null");

		release (set);
		EXPECT_NULL(set, "Released set's pointer should be null");
	}

	TEST(MutableSet,"subtract") {
		MutableSet *mut = new(MutableSet);
		EXPECT_EQUAL_INT(0, $(mut, count), "New mutable set should be empty");

		$(mut, add, _String("cheese"));
		$(mut, add, _String("fromage"));
		$(mut, add, _String("formaggio"));
		EXPECT_EQUAL_INT(3, $(mut, count), "After adding 3 objects, the mutable set should have 3 items");

		Set *set = Set_with(_String("formaggio"), NULL);
		EXPECT_EQUAL_INT(1, $(set, count), "New set with 1 object should have a count of 1");

		$(mut, subtract, set);
		EXPECT_EQUAL_INT(2, $(mut, count), "Subtracting the set from the mutable set should result in the mutable set having 2 items");

		release (mut);
		EXPECT_NULL(mut, "Released mutable set's pointer should be null");

		release (set);
		EXPECT_NULL(set, "Released set's pointer should be null");
	}

	//-----------
	// Remove all
	TEST(MutableSet,"remove all") {
		MutableSet *mut = new(MutableSet);
		$(mut, add, _Int(123));
		$(mut, add, _Int(456));
		$(mut, add, _Double(M_PI));
		$(mut, add, _String("mutton"));
		$(mut, add, _String("chicken"));
		$(mut, add, _String("beef"));
		EXPECT_EQUAL_INT(6, $(mut, count), "New mutable set created, then 6 objects added to it, the count should be 6");

		$(mut, removeAll);
		EXPECT_ZERO($(mut, count), "Mutable set emptied using removeAll should have 0 items");
		EXPECT_TRUE($(mut, isEmpty), "Emptied mutable set should be empty");

		release(mut);
		EXPECT_NULL(mut, "Released mutable set's pointer should be null");
	}

	TEST(MutableSet,"1 item") {
		String *obj = String_withCString("formaggio");
		MutableSet *mut = MutableSet_with(obj, NULL);
		EXPECT_EQUAL_INT(1, $(mut, count), "New mutable set instantiated with 1 object should have count of 1");
		EXPECT_TRUE($(mut, contains, obj), "The 1-item mutable set should contain the item it was instantiated with");
		
		release (mut);
		EXPECT_NULL(mut, "Released mutable set's pointer should be null");
	}

	TEST(Set,"1 item") {
		String *obj = String_withCString("formaggio");
		Set *set = Set_with(obj, NULL);
		EXPECT_EQUAL_INT(1, $(set, count), "New set instantiated with 1 object should have count of 1");
		EXPECT_TRUE($(set, contains, obj), "The 1-item set should contain the item it was instantiated with");
		
		release (set);
		EXPECT_NULL(set, "Released set's pointer should be null");
	}

	TEST(Set,"10 items") {
		Set *set = Set_with(
			_Int(123), _Double(4.5), _Int(-1), _String("xyz"), _String("ABC"),
			_Int(-123), _Double(-4.5), _Int(1), _String("XYZ"), _String("abc"),
			NULL);

		EXPECT_EQUAL_INT(10, $(set, count), "New set instantiated with 10 objects should have a count of 10");
		EXPECT_TRUE($(set, contains, _Int(-123)), "New set initialized with 10 items should contain specific the matching integer");
	
		release (set);
		EXPECT_NULL(set, "Released set's pointer should be null");
	}

	TEST(MutableSet,"10 items") {
		MutableSet *mut = MutableSet_with(
			_Int(123), _Double(4.5), _Int(-1), _String("xyz"), _String("ABC"),
			_Int(-123), _Double(-4.5), _Int(1), _String("XYZ"), _String("abc"),
			NULL);

		EXPECT_EQUAL_INT(10, $(mut, count), "New mutable set instantiated with 10 objects should have a count of 10");
		EXPECT_TRUE($(mut, contains, _Int(-123)), "New mutable set initialized with 10 items should contain specific the matching integer");
		
		release (mut);
		EXPECT_NULL(mut, "Released mutable set's pointer should be null");
	}

	TEST(Set,"array") {
		Set *set = Set_with(
				_String("ABC"), 
				_String("XYZ"), 
				_String("abc"),
				_String("A1"), 
				_String("mutton"),
				_String("chicken"),
				_String("beef"),
				_String("A2"), 
				_String("B1"), 
				_String("B2"),
				_String("xyz"), 
				NULL);

		Array *ary = $(set, asArray, false);
		Array *sorted = $(set, asArray, true);

		EXPECT_EQUAL_INT(11, $(set, count), "New set instantiated with 11 objects should have a count of 11");
		EXPECT_EQUAL_INT(11, $(ary, count), "Array from set with 11 objects should have a count of 11");
		EXPECT_EQUAL_INT(11, $(ary, count), "Sorted array from set with 11 objects should have a count of 11");
	
		printf ("Array from set: "); $(ary, print, 0);puts("");
		printf ("Sorted array from set: "); $(sorted, print, 0);puts("");
		release (set);
		release (ary);
		release (sorted);
	}

	int retval = Testing_result();
	deallocateClasses();
	return retval;
}

#endif // TESTING
