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
#include <math.h>

#include "Stack.h"
#include "String.h"
#include "RectStack.h"
#include "Testing.h"

int main()
{
	// Empty stack
	TEST(Stack,"empty") {
		Stack *stack = NULL;

		// Test new, push, pop.
		stack = new(Stack);
		EXPECT_TRUE($(stack, isEmpty), "New stack should be empty");

		EXPECT_NULL($(stack, pop), "Pop from new stack should return null");

		release(stack);
		EXPECT_NULL(stack, "Released stack's pointer should be null");
	}

	// Push pop
	TEST(Stack,"push pop") {
		Stack *stack = new(Stack);
		String *str = _String("abc");

		$(stack, push, str);
		EXPECT_FALSE($(stack, isEmpty), "Modified stack should not be empty");
		
		String *str2 = $(stack, pop);
		EXPECT_TRUE(str == str2, "Pushed and popped item is same object");

		EXPECT_TRUE($(stack, isEmpty), "Emptied stack should be empty");

		release(str);
		EXPECT_NULL(str, "Released string's pointer should be null");

		release(stack);
		EXPECT_NULL(stack, "Released stack's pointer should be null");
	}

	// Push pop order
	TEST(Stack,"order") {
		Stack *stack = new(Stack);

		// Check order of push/pop.
		String *str1 = _String("A");
		String *str2 = _String("B");
		$(stack, push, str1);
		$(stack, push, str2);

		String *s2 = $(stack, pop);
		EXPECT_TRUE(s2 == str2, "Popped item same as last pushed.");
		EXPECT_TRUE($(s2, equals, str2), "Popped item same as last pushed.");
		
		String *s1 = $(stack, pop);
		EXPECT_TRUE(s1 == str1, "Popped item same as first pushed.");
		EXPECT_TRUE($(s1, equals, str1), "Popped item same as first pushed.");

		EXPECT_TRUE($(stack, isEmpty), "Emptied stack should be empty");
		
		String *s = $(stack, pop);
		EXPECT_NULL(s, "Pop from empty stack should return null");
		
		release(stack);
		EXPECT_NULL(stack, "Released stack's pointer should be null");
	}

	// Swap
	TEST(Stack,"swap") {
		Stack *stack = new(Stack);
		EXPECT_TRUE($(stack, isEmpty), "New stack should be empty");

		$(stack, swap);
		EXPECT_TRUE($(stack, isEmpty), "New stack should be empty after useless swap");

		$(stack, push, _String("e"));
		EXPECT_EQUAL_INT($(stack, count), 1, "After pushing onto an empty stack, the count should be 1");

		$(stack, swap);
		EXPECT_EQUAL_INT($(stack, count), 1, "After performing a swap on a stack with only 1 item, the count should still be 1");

		String *str = _String("x");
		String *str2 = _String("y");
		$(stack, push, str);
		$(stack, push, str2);
		$(stack, swap);
		String *s = $(stack, pop);
		EXPECT_TRUE(s == str, "Popped item after swap is correctly first pushed");
		EXPECT_TRUE($(s, equals, str), "Popped item after swap is correctly first pushed");

		s = $(stack, pop);
		EXPECT_TRUE(s == str2, "Popped item after swap-pop is correctly second pushed");
		EXPECT_TRUE($(s, equals, str2), "Popped item after swap-pop is correctly second pushed");

		release(stack);
		EXPECT_NULL(stack, "Released stack's pointer should be null");
	}

	// Empty pop
	TEST(Stack,"empty pop") {
		Stack *stack = new(Stack);

		// Check pop of empty stack.
		String *s = $(stack, pop);
		EXPECT_NULL(s, "Pop from empty stack should return null");

		release(stack);
		EXPECT_NULL(stack, "Released stack's pointer should be null");
	}

	TEST(RectStack,"push pop") {
		RectStack *rects = new(RectStack);
		EXPECT_TRUE($(rects, isEmpty), "New rectangle stack should be empty");

		Rect r = Rect_new(1,2,3,4);
		$(rects, push, r);
		EXPECT_FALSE($(rects, isEmpty), "Modified rectangle stack should not be empty");
			
		Rect r2 = $(rects, pop);
		EXPECT_TRUE($(rects, isEmpty), "Emptied rectangle stack should be empty");
		EXPECT_FALSE(r2.origin.x != 1 || r2.origin.y != 2 || r2.size.width != 3 || r2.size.height != 4, "Popped rectangle verified to be correct");
		
		release(rects);
		EXPECT_NULL(rects, "Released rectangle stack's pointer should be null");
	}

	int retval = Testing_result();
	deallocateClasses();
	return retval;
}

#endif // TESTING
