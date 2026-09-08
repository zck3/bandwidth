/*============================================================================
  FileSystem, an object-oriented C filesystem class.
  Copyright (C) 2021, 2023, 2026 by Zack T Smith.

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

// Some of this code is part of a refactoring to break apart Utility.c
// into classes.

#ifndef _OOC_FILESYSTEM_H
#define _OOC_FILESYSTEM_H

#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
#include <sys/types.h>
#include <dirent.h>

#include "MutableArray.h"
#include "String.h"

// FileSystem is not an OOC class per se, because
// there's no need to encapsulate or save state.

extern DIR* FileSystem_openDirectory (Any *path); 
extern DIR* FileSystem_openDirectoryWithCString (const char *); 
extern MutableArray* FileSystem_readDirectory (DIR*); 
extern void FileSystem_closeDirectory (DIR*); 
extern FILE* FileSystem_openFile (Any* path, const char *mode); 
extern FILE* FileSystem_openFileWithCString (const char *path, const char *mode); 
extern void FileSystem_closeFile (FILE*); 
extern bool FileSystem_fileExistsWithCString (const char*);
extern unsigned long long FileSystem_fileSizeWithCString (const char *path);
extern String *FileSystem_readLine (FILE*); 
extern MutableArray *FileSystem_readLinesWithCString (const char* path);
extern unsigned FileSystem_read_trimmed_line_from_file (const char *path, char *result, unsigned maxlen);
extern bool FileSystem_read_unsigned_from_file (const char *path, unsigned *return_value);
extern bool FileSystem_read_longlong_from_file (const char *path, long long *return_value);
extern bool FileSystem_read_hex_from_file (const char *path, unsigned long *return_value);

#endif
