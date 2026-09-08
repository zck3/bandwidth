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

#include "Model3D.h"
#include "FileSystem.h"
#include "Utility.h"
#include "Testing.h"

int main()
{
	TEST(Model3D,"empty") {
		Model3D *m = new(Model3D);

		// Empty model 
		m = new(Model3D);
		EXPECT_ZERO(m->nTriangles, "New model should have 0 triangles");
		EXPECT_NONNULL(m->triangles, "New model has triangles array.");

		release(m);
		EXPECT_NULL(m, "Released model's pointer should be null");
	}
	
	TEST(Model3D,"cube") {
		Model3D *m = new(Model3D);
		
		$(m, addSphere, 1.f, 36, RGB_RED); // BUG: value of 10 leaves gap.
		$(m, translate, 1.5, 0, 0);
		$(m, addCube, 1.f, RGB_GREEN);
		EXPECT_NONZERO(m->nTriangles, "Modified model should have >0 triangles");
		EXPECT_NONNULL(m->triangles, "Modified model has triangles array.");

		release(m);
		EXPECT_NULL(m, "Released model's pointer should be null");
	}

	TEST(Model3D,"STL writer") {
		Model3D *m = new(Model3D);
		$(m, addSphere, 1.f, 36, RGB_RED); // BUG: value of 10 leaves gap.
		$(m, translate, 1.5, 0, 0);
		$(m, addCube, 1.f, RGB_GREEN);

		// Write to file
		EXPECT_TRUE($(m, writeSTL, "test.stl", "fubar"), "Wrote STL file");
		EXPECT_TRUE(FileSystem_fileExistsWithCString("test.stl"), "Written STL file should exist");

		release(m);
		EXPECT_NULL(m, "Released model's pointer should be null");
	}

#ifdef __APPLE__
	system ("open test.stl");
#else
	if (!system ("which fstl")) {
		system ("fstl test.stl");
	}
	unlink ("test.stl");
#endif

	int retval = Testing_result();
	deallocateClasses();
	return retval;
}

#endif // TESTING
