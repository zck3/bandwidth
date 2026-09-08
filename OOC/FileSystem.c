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

#include "FileSystem.h"
#include "Log.h"

#include <sys/stat.h> // stat
#include <ctype.h>    // isspace

DIR* FileSystem_openDirectoryWithCString (const char *path)
{
	DIR *dir = opendir (path);
	if (!dir) {
		Log_perror (__FUNCTION__, "opendir");
	}
	return dir;
}

DIR* FileSystem_openDirectory (Any *path_)
{
	if (!path_) {
		return NULL;
	}
	verifyCorrectClassOrSubclass(path_,String);

	String *path = path_;
	const char *cpath = $(path, asUTF8);

	DIR *dir = opendir (cpath);
	if (!dir) {
		Log_perror (__FUNCTION__, "opendir");
	}
	free ((void*) cpath);
	return dir;
}

MutableArray* FileSystem_readDirectory (DIR* dir)
{
	if (!dir) {
		return NULL;
	}
	MutableArray *ary = new(MutableArray);
	struct dirent *de;
	while ((de = readdir(dir))) {
		const char *name = de->d_name;
		String *string = String_withCString(name);
		$(ary, append, string);
	}
	return ary;
}

void FileSystem_closeDirectory (DIR* dir)
{
	if (!dir) {
		return;
	}
	closedir (dir);
}

FILE* FileSystem_openFileWithCString (const char *path, const char *mode)
{
	if (!path) {
		return NULL;
	}

	FILE *file = fopen (path, mode ?: "rb");
	if (!file) {
		Log_perror (__FUNCTION__, "fopen");
	}
	return file;
}

FILE* FileSystem_openFile (Any *path_, const char *mode)
{
	if (!path_) {
		return NULL;
	}
	verifyCorrectClassOrSubclass(path_,String);

	String *path = path_;
	const char *cpath = $(path, asUTF8);

	FILE *file = fopen (cpath, mode ?: "rb");
	if (!file) {
		Log_perror (__FUNCTION__, "fopen");
	}

	free ((void*) cpath);
	return file;
}

void FileSystem_closeFile (FILE* file)
{
	if (!file) {
		return;
	}
	fclose (file);
}

String *FileSystem_readLine (FILE* file)
{
	if (!file) {
		return NULL;
	}

	if (feof(file)) {
		return NULL;
	}

	size_t bufsize = 1024;
	uint8_t *line = malloc(bufsize);
	if (!line) {
		return NULL;
	}
	int ix = 0;
	bool eof_detected = false;
	while (!feof(file)) {
		int ch = fgetc(file);
		if (ch == EOF) {
			eof_detected = true;
			break;
		}
		if (ch == '\n') {
			break;
		}
		line[ix++] = ch;
		if (ix == bufsize) {
			bufsize += bufsize/2;
			line = realloc(line, bufsize);
			if (!line) {
				return NULL;
			}
		}
	}
	line[ix] = 0;
	String *string = NULL;
	if (eof_detected && !ix) {
		// It's the normal end-of-file. No need to create a string.
	} else {
		string = String_withCString((const char*)line);
	}
	free ((void*) line);
	return string;
}

MutableArray *FileSystem_readLinesWithCString (const char *path)
{
	if (!path) {
		return NULL;
	}
	FILE *file = fopen (path, "rb");
	if (!file) {
		return NULL;
	}

	MutableArray *mut = new(MutableArray);
	do {
		String *line = FileSystem_readLine (file);
		if (!line) {
			break;
		}
		$(mut, append, line);
	} 
	while (!feof(file));

	fclose(file);
	return mut;
}

//----------------------------------------------------------------------------
// Name:	FileSystem_fileSizeWithCString
// Purpose:	Obtain the size of a file.
// Returns:	Size in bytes or -1 on error.
//----------------------------------------------------------------------------
unsigned long long FileSystem_fileSizeWithCString (const char *path)
{
	struct stat st;
	if (stat (path, &st)) {
		perror ("stat");
		return -1;
	}
	return (long long) st.st_size;
}

//----------------------------------------------------------------------------
// Name:	FileSystem_fileExistsWithCString
// Purpose:	Determine if a file exists at specified path.
// Returns:	True if it exists, false if not.
//----------------------------------------------------------------------------
bool FileSystem_fileExistsWithCString (const char *path)
{
	struct stat st;
	if (!stat (path, &st)) {
		return true;
	}
	return false;
}

//----------------------------------------------------------------------------
// Name:	FileSystem_read_trimmed_line_from_file	
// Purpose:	Reads first line of a file, removes whitespace from start+end.
// Returns:	Line length.
//----------------------------------------------------------------------------
unsigned FileSystem_read_trimmed_line_from_file (const char *path, char *result, unsigned maxlen)
{
	if (!path || !result || !maxlen) {
		return 0;
	}

	FILE *f = fopen (path, "r");
	if (!f) {
		return 0;
	}

	int ch;
	unsigned index = 0;
	bool found_nonwhitespace = false;
	while (EOF != (ch = fgetc(f))) {
		if (ch == '\n') {
			break;
		}

		// Trim the start.
		if (!found_nonwhitespace && isspace(ch)) {
			continue;
		}

		found_nonwhitespace = true;
		result[index++] = ch;

		if (index == maxlen-1) {
			break;
		}
	}
	result[index] = 0;

	// Trim the end.
	unsigned length = index;
	while (index > 0) {
		index--;
		uint8_t *result_unsigned = (uint8_t*) result;
		uint8_t ch = result_unsigned[index];
		if (isspace((int) ch)) {
			result[index] = 0;
			length--;
		}
		else {
			break;
		}
	}

	fclose (f);
	return length;
}

//----------------------------------------------------------------------------
// Name:	FileSystem_read_float_from_file
// Purpose:	Reads a single floating-point number from a file.
// Returns:	Stores float in result, returns true if success, else false.
//----------------------------------------------------------------------------
bool FileSystem_read_float_from_file (const char *path, float *return_value)
{
	if (!path || !return_value) {
		return false;
	}
	FILE *f = fopen (path, "r");
	if (!f) {
		return false;
	}

	int count = fscanf (f, "%f", return_value);
	fclose (f);
	return count == 1;
}

//----------------------------------------------------------------------------
// Name:	FileSystem_read_hex_from_file
// Purpose:	Reads a single hexadecimal number from a file.
// Returns:	Stores hex in result, returns true if success, else false.
//----------------------------------------------------------------------------
bool FileSystem_read_hex_from_file (const char *path, unsigned long *return_value)
{
	if (!path || !return_value) {
		return false;
	}
	FILE *f = fopen (path, "r");
	if (!f) {
		return false;
	}

	int count = fscanf (f, "0x%lx", return_value);
	fclose (f);
	return count == 1;
}

//----------------------------------------------------------------------------
// Name:	read_unsigned_from_file
// Purpose:	Reads a single unsigned int from a file.
// Returns:	Stores long in result, returns true if success, else false.
//----------------------------------------------------------------------------
bool FileSystem_read_unsigned_from_file (const char *path, unsigned *return_value)
{
	if (!path || !return_value) {
		return false;
	}
	FILE *f = fopen (path, "r");
	if (!f) {
		return false;
	}

	int count = fscanf (f, "%u", return_value);
	fclose (f);

	return count == 1;
}

//----------------------------------------------------------------------------
// Name:	read_longlong_from_file
// Purpose:	Reads a single long long int from a file.
// Returns:	Stores long in result, returns true if success, else false.
//----------------------------------------------------------------------------
bool FileSystem_read_longlong_from_file (const char *path, long long *return_value)
{
	if (!path || !return_value) {
		return false;
	}
	FILE *f = fopen (path, "r");
	if (!f) {
		return false;
	}

	int count = fscanf (f, "%lld", return_value);
	fclose (f);

	return count == 1;
}

//----------------------------------------------------------------------------
// Name:	get_file_line_count
// Purpose:	Counts the number of text lines in a text file.
// Returns:	Count, or -1 on error.
//----------------------------------------------------------------------------
long FileSystem_get_file_line_count (const char *path)
{
	FILE *f = fopen (path, "r");
	if (!f) {
		return -1;
	}

	long lineCount = 0;

#define BUFSIZE 4096
	char buffer[BUFSIZE];
	char lastChar = 0;
	while (!feof (f)) {
		long len = fread (buffer, 1, BUFSIZE, f);
		if (len <= 0)
			break;
		for (long i=0; i < len; i++) {
			lastChar = buffer[i];
			if (lastChar == '\n')
				lineCount++;
		}
	}

	if (lastChar != '\n') {
		lineCount++; // Partial last line.
	}

	return lineCount;
}

