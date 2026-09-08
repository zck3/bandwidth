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

#include "Data.h"
#include "MutableData.h"
#include "Testing.h"
#include "FileSystem.h"

int main()
{
	TEST(Data,"read nonexistent file") {
		Data *data = Data_fromFilePath("/laksdjflkasdjflkajdsldfas");
		size_t length = $(data, length);
		EXPECT_ZERO(length, "Nonexistent file length should be zero");
		release(data);
	}

	TEST(Data,"read memory-mapped files") {
		Data *data = Data_fromFilePath("./PIA25970.tif");
		size_t length = $(data, length);
		EXPECT_EQUAL_INT(length, 1664670, "File length as read should be 1664670");
		release(data);

		data = Data_fromFilePath("./PIA25970.jpg");
		length = $(data, length);
		EXPECT_EQUAL_INT(length, 213170, "File length as read should be 213170");
		release(data);
	}

	TEST(MutableData,"file size") {
		FILE *file = fopen("PIA25970.tif", "rb");
		if (file) {
			MutableData *mut = MutableData_fromFile(file);
			size_t length = $(mut, length);
			EXPECT_EQUAL_INT(length, 1664670, "TIFF file should have 1664670 bytes length");
			FileSystem_closeFile (file);
			release(mut);
		}
	}

	int retval = Testing_result();
	deallocateClasses();
	return retval;
}

#endif // TESTING
