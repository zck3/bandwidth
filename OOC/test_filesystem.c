/*============================================================================
  OOC, a library for object-oriented programming using C.
  Copyright (C) 2026 by Zack T Smith.

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

#include "FileSystem.h"
#include "MutableData.h"
#include "Testing.h"

int main()
{
	TEST_NON_CLASS("FileSystem","total lines") {
		FILE *file = FileSystem_openFile(_String("test_cars.csv"), NULL);
		String *str;
		unsigned count = 0;
		while ((str = FileSystem_readLine(file))) {
			count++;
		}
		EXPECT_EQUAL_INT(count, 6, "CSV file should have 6 lines");
		FileSystem_closeFile (file);
	}

	int retval = Testing_result();
	deallocateClasses();
	return retval;
}

#endif // TESTING
