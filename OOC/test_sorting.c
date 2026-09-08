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
#include "MutableArray.h"
#include "Testing.h"

int main()
{
	TEST(MutableArray,"sort ascending and descending") {
		Array *ary_ascending = Array_with(
			_String("adipiscing"),
			_String("aliqua"),
			_String("amet"),
			_String("consectetur"),
			_String("do"),
			_String("dolor"),
			_String("dolore"),
			_String("eiusmod"),
			_String("elit"),
			_String("et"),
			_String("incididunt"),
			_String("ipsum"),
			_String("labore"),
			_String("lorem"),
			_String("magna"),
			_String("sed"),
			_String("sit"),
			_String("tempor"),
			_String("ut"),
			NULL
		);

		Array *ary_descending = Array_with(
			_String("ut"),
			_String("tempor"),
			_String("sit"),
			_String("sed"),
			_String("magna"),
			_String("lorem"),
			_String("labore"),
			_String("ipsum"),
			_String("incididunt"),
			_String("et"),
			_String("elit"),
			_String("eiusmod"),
			_String("dolore"),
			_String("dolor"),
			_String("do"),
			_String("consectetur"),
			_String("amet"),
			_String("aliqua"),
			_String("adipiscing"),
			NULL
		);

		MutableArray *mut = new(MutableArray);
		$(mut, append, _String("lorem"));
		$(mut, append, _String("ipsum"));
		$(mut, append, _String("dolor"));
		$(mut, append, _String("sit"));
		$(mut, append, _String("amet"));
		$(mut, append, _String("consectetur"));
		$(mut, append, _String("adipiscing"));
		$(mut, append, _String("elit"));
		$(mut, append, _String("sed"));
		$(mut, append, _String("do"));
		$(mut, append, _String("eiusmod"));
		$(mut, append, _String("tempor"));
		$(mut, append, _String("incididunt"));
		$(mut, append, _String("ut"));
		$(mut, append, _String("labore"));
		$(mut, append, _String("et"));
		$(mut, append, _String("dolore"));
		$(mut, append, _String("magna"));
		$(mut, append, _String("aliqua"));
			
		$(mut, quicksort, true);
		EXPECT_TRUE($(ary_ascending, equals, mut), "Ascending sort successful");

		$(mut, quicksort, false);
		// $(mut,print,0);
		EXPECT_TRUE($(ary_descending, equals, mut), "Descending sort successful");
	}

	int retval = Testing_result();
	deallocateClasses();
	return retval;
}

#endif // TESTING
