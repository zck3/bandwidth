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

#include "OS.h"
#include "Testing.h"

int main()
{
	TEST(OS,"is root") {
		bool isRoot1 = OS_isRoot();
		const char *username = getenv("USER");
		bool isRoot2 = username? 0 == strcmp ("root", username) : false;

		EXPECT_TRUE(isRoot1 == isRoot2, "OS_isRoot should be confirmed by $USER");
	}

	int retval = Testing_result();
	return retval;
}

#endif // TESTING
