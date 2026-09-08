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
#include <unistd.h>

#include "Image.h"
#include "FileSystem.h"
#include "MutableImage.h"
#include "Utility.h"
#include "FontPK.h"
#include "Testing.h"
#include "colors.h"

#if defined(__ANDROID__) && defined(__TERMUX__)
#define BMP_FILEPATH "./foo.bmp"
#else
#define BMP_FILEPATH "/tmp/foo.bmp"
#endif

static unsigned totalPixelsOfColor (MutableImage *mut, RGB color)
{
	int width = $(mut, width);
	int height = $(mut, height);

	unsigned count = 0;

	for (int i= -20; i < width + 20; i++) {
		for (int j= -20; j < height + 20; j++) {
			RGB foundColor = $(mut, pixelAt, i, j);
			if (foundColor == color) {
				count++;
			}
		}
	}

	return count;
}

int main()
{
	TEST(Image,"empty") {
		Image *img = NULL;

		img = new(Image);
		EXPECT_ZERO($(img, width), 
			"New image width should be 0");
		EXPECT_ZERO($(img, height), 
			"New image height should be 0");

		EXPECT_NULL($(img, pixels), 
			"New image has null pixels pointer");
		EXPECT_NULL($(img, pixelRowAddress, 400),
			"New image has null pixel row pointers");
		EXPECT_NULL($(img, pixelRowAddress, 800),
			"New image has null pixel row pointers");

		release(img);
		EXPECT_NULL(img, "Released image's pointer should be null");
	}

	TEST(MutableImage,"blank image") {
		MutableImage *mut = NULL;

		mut = MutableImage_withSize (123, 456);
		EXPECT_EQUAL_INT($(mut, width), 123, "Mutable image with fixed size should have correct width");
		EXPECT_EQUAL_INT($(mut, height), 456, "Mutable image with fixed size should have correct height");

		EXPECT_NONNULL($(mut, pixels), 
		  "Mutable image with valid size has non-null pixels pointer");
		EXPECT_NONNULL($(mut, pixelRowAddress, 400), 
		  "Mutable image with valid size has non-null row pixels pointer for valid row");
		EXPECT_NULL($(mut, pixelRowAddress, 800),
		  "Mutable image with valid size has null row pixels pointer for invalid row");

		release(mut);
		EXPECT_NULL(mut, "Released mutable image's pointer should be null");
	}

#ifdef HAVE_JPEGLIB
	TEST(Image, "JPEG image size") {
		Image *img = NULL;

		img = Image_fromFile("MarsRover.jpg");
		EXPECT_NONNULL(img, "Image file should be read correctly");
		if (img) {
			EXPECT_EQUAL_INT($(img, width), 1300, "Image from test JPEG should have correct width");
			EXPECT_EQUAL_INT($(img, height), 1300, "Image from test JPEG should have correct height");

			EXPECT_TRUE($(img, writeBMP, BMP_FILEPATH), 
				"JPEG write should succeed");
				
			EXPECT_TRUE(FileSystem_fileExistsWithCString(BMP_FILEPATH), 
				"Written JPEG file should exist");

			EXPECT_NONZERO(FileSystem_fileSizeWithCString(BMP_FILEPATH),
				"Written JPEG file should have nonzero length");

			unlink(BMP_FILEPATH);
			release(img);
			EXPECT_NULL(img, "Released image's pointer should be null");
		}
	}
#else
	TEST_SKIP(Image, "JPEG image size", "JPEG support unavailable");
#endif

	TEST(MutableImage,"resize") {
		MutableImage *mut = new(MutableImage);
		EXPECT_ZERO($(mut, width), "New mutable image width should be 0");
		EXPECT_ZERO($(mut, height), "New mutable image height should be 0");

		$(mut, clear);
		$(mut, resize, 1920, 1080);
		EXPECT_EQUAL_INT($(mut, width), 1920, "Resized mutable image from test JPEG should have correct width");
		EXPECT_EQUAL_INT($(mut, height), 1080, "Resized mutable image from test JPEG should have correct height");

		release(mut);
		EXPECT_NULL(mut, "Released mutable image's pointer should be null");
	}

	TEST(MutableImage,"drawing") {
		MutableImage *mut = MutableImage_withSize(1920,1080);

		$(mut, fillWithColor, RGB_CYAN);
		$(mut, drawLine, 0, 0, 1919, 0, RGB_RED);
		$(mut, drawLine, 0, 0, 0, 1079, RGB_RED);
		unsigned nCyan = totalPixelsOfColor (mut, RGB_CYAN);
		unsigned nRed = totalPixelsOfColor (mut, RGB_RED);
		unsigned total = nCyan + nRed;
		EXPECT_EQUAL_INT(total, 1920*1080, "Mutable image should have 1920*1080 pixels");
		EXPECT_EQUAL_INT(nRed, 1920+1080-1, "After line draws, mutable image should have 1920+1080-1 red pixels");
		
		$(mut, clear);
		unsigned nWhite = totalPixelsOfColor (mut, RGB_WHITE);
		EXPECT_EQUAL_INT(nWhite, 1920*1080, "After clearing, mutable image should have 1920*1080 white pixels");

		$(mut, fillRectangle, 10, 10, 57, 91, RGB_BLUE);
		unsigned nBlue = totalPixelsOfColor (mut, RGB_BLUE);
		EXPECT_EQUAL_INT(nBlue, 57 * 91, "After fill rect call, mutable image should have 57*91 blue pixels");

		$(mut, clear);
		$(mut, fillRectangle, -30, -30, 30, 30, RGB_RED);
		$(mut, fillRectangle, 1920, 30, 30, 30, RGB_RED);
		$(mut, fillRectangle, 33, -30, 30, 30, RGB_RED);
		$(mut, fillRectangle, 550, 1080, 30, 30, RGB_RED);
		nWhite = totalPixelsOfColor (mut, RGB_WHITE);
		EXPECT_EQUAL_INT(nWhite, 1920*1080, "After clearing, and drawing filled rectangles with out-of-bounds coords, the mutable image should still be entirely white");

		$(mut, drawRect, Rect_new(-30, -30, 30, 30), RGB_RED);
		$(mut, drawRectangle, 1920, 30, 30, 30, RGB_RED);
		$(mut, drawRectangle, 33, -30, 30, 30, RGB_RED);
		$(mut, drawRectangle, 550, 1080, 30, 30, RGB_RED);
		nWhite = totalPixelsOfColor (mut, RGB_WHITE);
		EXPECT_EQUAL_INT(nWhite, 1920*1080, "After clearing, and drawing rectangles out-of-bounds, the mutable image should still be entirely white");

		$(mut, drawRectangle, -29, -29, 30, 30, RGB_RED);
		nWhite = totalPixelsOfColor (mut, RGB_WHITE);
		EXPECT_EQUAL_INT(nWhite, 1920*1080-1, "After drawing only red 1 pixel within the mutable image, it should have 1920*1080-1 white pixels");

		$(mut, putPixel, 0, 0, RGB_WHITE);
		nWhite = totalPixelsOfColor (mut, RGB_WHITE);
		EXPECT_EQUAL_INT(nWhite, 1920*1080, "After setting the red pixel to white, the mutable image should have 1920*1080 white pixels");

		$(mut, drawLine, 0, 0, 1919, 1079, RGB_GREEN);
		nWhite = totalPixelsOfColor (mut, RGB_WHITE);
		EXPECT_EQUAL_INT(nWhite, 1920*1079, "After drawing a diagonal green line, the mutable image should have 1920*1079 white pixels");

		RGB color = $(mut, pixelAt, 0, 0);
		EXPECT_EQUAL_INT(color, RGB_GREEN, "Reading the top-left pixel from the mutable image, it should be green");

		$(mut, putPixel, 0, 0, RGB_RED);
		color = $(mut, pixelAt, 0, 0);
		EXPECT_EQUAL_INT(color, RGB_RED, "After setting the mutable image's top-left pixel to red, it should read back as red");

		$(mut, putPixel, 0, 0, RGB_BLUE);
		color = $(mut, pixelAt, 0, 0);
		EXPECT_EQUAL_INT(color, RGB_BLUE, "After setting the mutable image's top-left pixel to blue, it should read back as blue");

		$(mut, clear);
		color = $(mut, pixelAt, 0, 0);
		EXPECT_EQUAL_INT(color, RGB_WHITE, "After clearing mutable image, its top-left pixel should read as white");

		release(mut);
		EXPECT_NULL(mut, "Released mutable image's pointer should be null");
	}

	TEST(MutableImage,"zero dimension") {
		MutableImage *mut;

		// An image with a zero dimension should retain the nonzero dimension.
		mut = MutableImage_withSize (0, 456);
		EXPECT_EQUAL_INT(0, $(mut, width), "New mutable image with zero width, the width should read as 0");
		EXPECT_EQUAL_INT(456, $(mut, height), "New mutable image with non-zero height, the height should read as non-zero");

		EXPECT_NULL($(mut, pixels), 
			"New mutable image with zero width has null pixels pointer");
		EXPECT_NULL($(mut, pixelRowAddress, 400),
			"New mutable image with zero width has null pixel row pointers");
		EXPECT_NULL($(mut, pixelRowAddress, 800),
			"New mutable image with zero width has null pixel row pointers");

		Size size = $(mut, size);
		EXPECT_EQUAL_INT(0, size.width, "Reading the Size struct, the previously initialized zero width should be zero");
		EXPECT_EQUAL_INT(456, size.height, "Reading the Size struct, the previously initialized non-zero height should be non-zero");
		release(mut);
		EXPECT_NULL(mut, "Released mutable image's pointer should be null");

		// An image with a zero dimension should retain the nonzero dimension.
		mut = MutableImage_withSize (4000, 0);
		EXPECT_EQUAL_INT(4000, $(mut, width), "New mutable image with non-zero width, the width should read as non-zero");
		EXPECT_EQUAL_INT(0, $(mut, height), "New mutable image with zero height, the height should read as 0");
		
		EXPECT_NULL($(mut, pixels), 
			"New mutable image with zero height has null pixels pointer");
		EXPECT_NULL($(mut, pixelRowAddress, 400),
			"New mutable image with zero height has null pixel row pointers");
		EXPECT_NULL($(mut, pixelRowAddress, 800),
			"New mutable image with zero height has null pixel row pointers");

		size = $(mut, size);
		EXPECT_EQUAL_INT(4000, size.width, "Reading the Size struct, the previously initialized non-zero width should be non-zero");
		EXPECT_EQUAL_INT(0, size.height, "Reading the Size struct, the previously initialized zero height should be zero");

		release(mut);
		EXPECT_NULL(mut, "Released mutable image's pointer should be null");
	}

	TEST(MutableImage,"oversized image") {
		MutableImage *mut;

		// Oversized image, dimension will be rejected.
		mut = MutableImage_withSize (1<<30, 1<<30);
		EXPECT_ZERO($(mut, width), "Impossibly large image width should be 0");
		EXPECT_ZERO($(mut, height), "Impossibly large image height should be 0");
		EXPECT_NULL($(mut, pixels), "Impossibly large image has null pixels pointer");

		release(mut);
		EXPECT_NULL(mut, "Released mutable image's pointer should be null");
	}

	TEST(MutableImage,"draw text") {
		MutableImage *mut = new(MutableImage);
		EXPECT_ZERO($(mut, width), "New mutable image width should be 0");
		EXPECT_ZERO($(mut, height), "New mutable image width should be 0");

		$(mut, resize, 320, 240);
		unsigned foundWidth = $(mut, width);
		unsigned foundHeight = $(mut, height);
		EXPECT_EQUAL_INT(foundWidth, 320, "Resized mutable image should have correct width");
		EXPECT_EQUAL_INT(foundHeight, 240, "Resized mutable image should have correct height");
	
		unsigned nWhite = totalPixelsOfColor (mut, RGB_WHITE);
		EXPECT_EQUAL_INT(nWhite, 320*240, "Cleared mutable image should have correct number of white pixels (default color)");

		String *s = _String("this is a test.");
		FontPK *font = FontPK_with(NULL, 0);
		$(mut, drawString, s, 20, 20, (Font*)font, RGB_RED);
		nWhite = totalPixelsOfColor (mut, RGB_WHITE);
		EXPECT_NONEQUAL_INT(nWhite, 320*240, "After drawing red text on white background, the total number of white pixels should be reduced");

		release(mut);
		EXPECT_NULL(mut, "Released mutable image's pointer should be null");
	}

	int retval = Testing_result();
	deallocateClasses();
	return retval;
}

#endif // TESTING
