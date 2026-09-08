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
#include <math.h>

#include "List.h"
#include "String.h"
#include "Int.h"
#include "Double.h"
#include "Testing.h"

int main()
{
	TEST(List,"empty") {
		List *list = new(List);
		EXPECT_TRUE($(list, isEmpty), "New list is empty");

		Object *obj;
		obj = $(list, first);
		EXPECT_NULL(obj, "New list has null first object");

		obj = $(list, at, 2);
		EXPECT_NULL(obj, "New list has null 3rd object");

		obj = $(list, at, -1);
		EXPECT_NULL(obj, "New list has null object at negative index");

		release(list);
		EXPECT_NULL(list, "Released list's pointer should be null");
	}

	TEST(List,"append") {
		List *list = new(List);
		$(list, appendCString, "string");
		EXPECT_FALSE($(list, isEmpty), "Modified list should be non-empty");

		$(list, appendInt, -10001);
		$(list, appendDouble, M_PI);
		EXPECT_FALSE($(list, isEmpty), "Appended-to list should be non-empty");
		EXPECT_EQUAL_INT(3, $(list, count), "After appending 3 items to list, count should be 3");

		Int *i = $(list, at, 1);
		EXPECT_NONNULL(i, "Modified list has non-null object at 2nd position");

		int value = $(i, asInt);
		EXPECT_EQUAL_INT(-10001, value, "The 2nd item in the list, should be the correct integer");
		
		//printf ("List=");
		//$(list, print, NULL);
		//puts ("");

		release(list);
		EXPECT_NULL(list, "Released list's pointer should be null");
	}

	TEST(List,"removals, insertions") {
		List *list = new(List);
		EXPECT_TRUE($(list, isEmpty), "New list should be empty");
		$(list, appendCString, "A");
		EXPECT_FALSE($(list, isEmpty), "Modified list should be non-empty");
		$(list, appendCString, "B");
		$(list, appendCString, "C");
		$(list, appendCString, "D");
		EXPECT_EQUAL_INT(4, $(list, count), "After appending 4 C-Strings, the list count should be 4");

		$(list, removeFirst);
		EXPECT_EQUAL_INT(3, $(list, count), "After moving the 1st item, list should have 3 items");

		$(list, removeLast);
		EXPECT_EQUAL_INT(2, $(list, count), "After moving the last item, list should have 2 items");

		$(list, removeLast);
		EXPECT_EQUAL_INT(1, $(list, count), "After moving the last item, list should have 1 item");

		$(list, removeLast);
		EXPECT_ZERO($(list, count), "Emptied list should have 0 items");
		EXPECT_TRUE($(list, isEmpty), "Emptied list should be empty");

		$(list, appendInt, 12345);
		$(list, appendDouble, M_PI);
		EXPECT_EQUAL_INT(2, $(list, count), "After adding 2 items to the empty list, its count should be 2");
		$(list, removeAll);
		EXPECT_EQUAL_INT(0, $(list, count), "After removing all items from the list, its count should be 0");

		$(list, appendInt, 555);
		EXPECT_EQUAL_INT(1, $(list, count), "After appending 1 integer, the list's count should be 1");
		EXPECT_FALSE($(list, isEmpty), "After appending an integer to the list, it should be non-empty");

		Object *object;
		object = $(list, at, 1000);
		EXPECT_NULL(object, "List read at excessive index should return null");

		$(list, appendInt, -111);
		EXPECT_EQUAL_INT(2, $(list, count), "Append object, bringing list count to 2");

		$(list, prepend, _Int(210));
		EXPECT_EQUAL_INT(3, $(list, count), "Append object, bringing list count to 3");

		$(list, removeAt, 1);
		EXPECT_EQUAL_INT(2, $(list, count), "Remove 2nd object, bringing list count to 2");

		// Excessive removals.
		repeat(1000) {
			if (rand() & 1) {
				$(list, removeFirst);
			} else {
				$(list, removeLast);
			}
		}
		EXPECT_ZERO($(list, count), "After excessive removals, list with all items removed using removeFirst/Last should be empty");

		repeat(1000) {
			$(list, appendDouble, -1.234);
		}

		EXPECT_EQUAL_INT(1000, $(list, count), "After appending 1000 doubles to the list, its count should be 1000");

		Double *i = $(list, at, 0);
		EXPECT_NONNULL(i, "Modified list has item at position 0");

		double value = $(i, asDouble);
		EXPECT_EQUAL_INT(-1.234, value, "The item at index 0 should be the correct value");

		$(list, removeAt, 2000); // no effect
		EXPECT_EQUAL_INT(1000, $(list, count), "After attempting to remove an object at an out-of-bounds index, the list should still have 1000 items");

		$(list, removeAt, 1); 
		EXPECT_EQUAL_INT(999, $(list, count), "After removing an object at a valid index, the list should have 999 items");

		repeat(1000) {
			$(list, removeLast);
		}
		EXPECT_ZERO($(list, count), "List with all items removed using removeLast should be empty");

		$(list, prepend, _Int(10));
		$(list, append, _Int(20));
		EXPECT_EQUAL_INT(2, $(list, count), "After adding 2 objects to the emptied list, its count should be 2");

		release(list);
		EXPECT_NULL(list, "Released list's pointer should be null");
	}

	int retval = Testing_result();
	deallocateClasses();
	return retval;
}

#endif // TESTING
