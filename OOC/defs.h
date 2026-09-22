/*============================================================================
  OOC, a library for object-oriented programming using C.
  Copyright (C) 2019, 2022-2024, 2026 by Zack T Smith.

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

#ifndef _OBJECTORIENTED_DEFS_H
#define _OBJECTORIENTED_DEFS_H

#define OOC_RELEASE "0.40"

//-----------------------------------------------------------------------------
// CHANGES
//-----------------------------------------------------------------------------
// 0.29	Added URL class.
// 0.30	Added JPEG reader.
// 0.31	Added TIFF reader.
// 0.32	Added Model3D with STL writer.
// 0.33	Added Display, Window, View, Controller classes.
// 0.34	Ported Label from FrugalWidgets.
// 0.35	Ported Button from FrugalWidgets.
// 0.36	Ported ImageView from FrugalWidgets.
// 0.37	Ported ProgressBar from FrugalWidgets.
// 0.38	Pointer focus logic. 
// 0.39	- Ported TextField from FrugalWidgets: highlight, select-all, insert/delete, Enter, listener, delete selection.
// 	- Fixed normal vector logic in Model3D. 
//     	- MutableArray: Added setAt method and arrayWithArray.
//     	- Began porting TextView/TextEdit from FrugalWidgets.
//     	- Added general scroll wheel support. TextView scroll-wheel support.
//     	- Views with subviews, using GL scissor region to crop.
//     	- Moved arrays of Model3D and View objects into Controller.
//     	- Added support in Window for OpenGL full-screen mode.
//     	- Added Display class methods for OpenGL version lookup.
//     	- Added Char routines due to mbstowcs failing on basic UTF-8.
//     	- Improved unit testing e.g. added EXPECT_CLASS, EXPECT_NEVER.
//	- Improved OpenGL test.
//     	- Added lookup of OS name on macOS.
//	- Added macros MAKE_[GS]ETTER_SCALAR, MAKE_SETTER_OBJECT
//	- Added ANSI text color logic to Console class.
//	- Fixed header file & framework issues with OpenGL & GLUT on MacOS.
//	- Added 'restrict' keyword for better compiler optimization.
//	- Fixed memory leak discovered with Valgrind.
// 0.40 - Fixed bug with Set_print.
//	- Added addCString to MutableSet.
//	- Added currentCore to CPU.
//	- Added Table class.
//	- Two-stage make for test_opengl and test_gui.
//	- Simplified #includes.
// 0.41	- Various fixes for macOS including #includes and ./configure.
//-----------------------------------------------------------------------------

#if __WORDSIZE == 64 && !defined(IS_64BIT)
 #define IS_64BIT
#endif

//#define GRATUITOUS_DEBUGGING

#define OBJECT_MAGIC_NUMBER (0x7ac4c0de)

#define kDefaultDPI 100

#ifndef ONE_MEGABYTE
#define ONE_MEGABYTE (1LU << 20LU)
#endif
#ifndef ONE_GIGABYTE
#define ONE_GIGABYTE (1LU << 30LU)
#endif
#ifndef ONE_MILLION
#define ONE_MILLION (1000000LU)
#endif

#ifndef MAXUNSIGNED
#define MAXUNSIGNED (~(0U))
#endif

#endif

