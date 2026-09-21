/*============================================================================
  Table, an object-oriented C mutable table class.
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

#include "Table.h"
#include "Log.h"
#include "FileSystem.h"
#include "Utility.h"
#include "String.h"
#include "Int.h"
#include "Double.h"
#include "Console.h"

#include <stdlib.h>

TableClass *_TableClass = NULL;

void Table_destroy (Any *self_)
{
	DEBUG_DESTROY;
	if (!self_) {
		return;
	}
	verifyCorrectClassOrSubclass(self_,Table);

	Table *self = self_;

	releaseAndClear (self->columnNames);
	releaseAndClear (self->rows);
}

//----------------------------------------------------------------------------
// Name:	Table_print
// Purpose:	Prints the Table in CSV format.
//----------------------------------------------------------------------------
static void Table_print (Table* self, FILE *outputFile) 
{ 
	if (!self) {
		return;
	}
	verifyCorrectClassOrSubclass(self,Table);

	if (!outputFile) {
		outputFile = stdout;
	}

	if (self->columnNames) {
		unsigned count = $(self->rows, count);
		for (unsigned j = 0; j < count; j++) {
			Object *item = $(self->columnNames, at, j);
			if (item) {
				$(item, print, outputFile);
			}
			if (j != count-1) {
				fputc(',', outputFile);
			}
		}
		fputc('\n', outputFile);
	}

	if (self->rows) {
		// XX Need iterator
		unsigned nRows = $(self->rows, count);
		for (unsigned i = 0; i < nRows; i++) {
			Array *row = $(self->rows, at, i);
			if (row) {
				unsigned nColumns = $(row, count);
				for (unsigned j = 0; j < nColumns; j++) {
					Object *item = $(row, at, j);
					if (item) {
						$(item, print, outputFile);
					}
					if (j != nColumns-1) {
						fputc(',', outputFile);
					}
				}
			} else {
				unsigned nColumns = $(self->columnNames, count);
				if (nColumns) {
					for (unsigned k = 0; k < nColumns-1; k++) {
						fprintf (outputFile, ",");
					}
				}
			}
			fputc ('\n', outputFile);
		}
	}
}

static void Table_describe (Table* self, FILE *outputFile) 
{ 
	if (!self) {
		return;
	}
	verifyCorrectClassOrSubclass(self,Table);

	if (!outputFile) {
		outputFile = stdout;
	}

	fprintf (outputFile, "%s", $(self, className));
	fprintf (outputFile, "(%lu rows %lu columns)", 
		(unsigned long) $(self, nRows),
		(unsigned long) $(self, nColumns));
}

static unsigned Table_nRows (Table* self)
{ 
	if (!self) {
		return 0;
	}
	verifyCorrectClassOrSubclass(self,Table);

	return $(self->rows, count);
}

static unsigned Table_nColumns (Table* self)
{ 
	if (!self) {
		return 0;
	}
	verifyCorrectClassOrSubclass(self,Table);

	if (!self->columnNames) {
		return self->nColumns;
	}

	unsigned nNames = $(self->columnNames, count);
	if (nNames == 0) {
		return self->nColumns;
	}
	
	return nNames;
}

// TODO Need some kind of printingLength method for Object class.
static unsigned Object_printingLength (Object *obj)
{
	if (!obj) {
		return 0;
	}
	if (isMemberOfClassOrSubclass(obj, String)) {
		String *str = (String*) obj;
		return $(str, length);
	}
	else if (isMemberOfClassOrSubclass(obj, Int)) {
		Int *integer = (Int*)obj;
		int value = $(integer, asInt);
		int len = snprintf(NULL, 0, "%d", value);
		return len >= 0 ? (unsigned) len : 0U;
	}
	else if (isMemberOfClassOrSubclass(obj, Double)) {
		Double *dbl = (Double*)obj;
		double value = $(dbl, asDouble);
		int len = snprintf(NULL, 0, "%g", value); // XX What about various formatting.
		return len >= 0 ? (unsigned) len : 0U;
	}

	return 0U;
}

unsigned Table_maximumWidthOfColumn (Table* self, unsigned column)
{
	if (!self) {
		return 0;
	}
	verifyCorrectClassOrSubclass(self,Table);

	unsigned nCols = $(self, nColumns);
	if (column >= nCols) {
		return 0;
	}

	unsigned maximum = 0;
	unsigned nNames = self->columnNames ? $(self->columnNames, count) : 0;
	if (nNames && column < nNames) {
		String *name = $(self->columnNames, at, column);
		maximum = Object_printingLength ((Object*)name);
	}

	unsigned nRows = $(self, nRows);
	for (unsigned i=0; i < nRows; i++) {
		Array *ary = $(self->rows, at, i);
		if (ary) {
			Object *obj = $(ary, at, column);
			if (obj) {
				unsigned len = Object_printingLength (obj);
				if (len > maximum) {
					maximum = len;
				}
			}
		}
	}
	return maximum;
}

static unsigned Table_minimumRequiredColumns (Table* self)
{ 
	if (!self) {
		return 0;
	}
	verifyCorrectClassOrSubclass(self,Table);

	if (!self->rows) {
		return $(self, nColumns);
	}

	unsigned nRows = $(self->rows, count);
	unsigned nCols = 0;
	for (unsigned row = 0; row < nRows; row++) {
		MutableArray *ary = $(self->rows, at, row);
		if (ary) {
			unsigned n = $(ary, count);
			if (n > nCols) {
				nCols = n;
			}
		}
	}
	return nCols;
}

static unsigned Table_count (Table* self)
{ 
	if (!self) {
		return 0;
	}
	verifyCorrectClassOrSubclass(self,Table);

	return $(self->columnNames, count) * $(self->rows, count);
}

String *Table_columnName (Table *self, unsigned column)
{
	if (!self) {
		return NULL;
	}
	verifyCorrectClassOrSubclass(self,Table);

	if (!self->columnNames) {
		return NULL;
	}
	unsigned nNames = $(self->columnNames, count);
	if (column >= nNames) {
		return NULL;
	}

	return $(self->columnNames, at, column);
}

static Any *Table_at (Table* self, unsigned row, unsigned column)
{
	if (!self) {
		return NULL;
	}
	verifyCorrectClassOrSubclass(self,Table);

	Object *value = NULL;
	Array *rowArray = $(self->rows, at, row);
	if (rowArray) {
		if (column < $(rowArray, count)) {
			value = $(rowArray, at, column);
		}
	}
	return value;
}

static void Table_append (Table* self, Array *row)
{
	if (!self) {
		return;
	}
	verifyCorrectClassOrSubclass(self,Table);
	verifyCorrectClassOrSubclass(row,Array);

	if (!self->rows) {
		error (__FUNCTION__, "Internal inconsistency: Table is missing rows array.");
	}

	unsigned nItems = $(row, count);
	if (nItems > self->nColumns) {
		self->nColumns = nItems;
	}

	$(self->rows, append, row);
}

static Array *Table_rowAt (Table* self, unsigned row)
{
	if (!self) {
		return NULL;
	}
	verifyCorrectClassOrSubclass(self,Table);

	unsigned nRows = $(self->rows, count);
	if (row >= nRows) {
		return NULL;
	}

	return $(self->rows, at, row);
}

#if not_yet_used 
static MutableString* Table_asCSV (Table *self)
{
	if (!self) {
		return NULL;
	}
	verifyCorrectClassOrSubclass(self,Table);
	// TODO
	return NULL;
}
#endif

/*-----------------------------------------------------------------------------
 * Name:	join
 * Purpose:	Inner join of two tables. Column names must be the same.
 * Parameter:	column: String, common column name.
 *---------------------------------------------------------------------------*/
#if not_yet_used
static Table *Table_join (Table *self, Any *column)
{
	if (!self) {
		return NULL;
	}
	verifyCorrectClassOrSubclass(self,Table);

	// TODO

	return NULL;
}
#endif

static void Table_setColumnNames (Table* self, Array* names)
{
	if (!self) {
		return;
	}
	verifyCorrectClassOrSubclass(self,Table);
	verifyCorrectClassOrSubclass(names,Array);

	if (!names) {
		if (self->columnNames) {
			release(self->columnNames);
		}
		self->nColumns = 0;
		return;
	}

	if ((void*)names != (void*)self->columnNames) {
		if (self->columnNames) {
			release(self->columnNames);
		}
		self->columnNames = retain(names);
	}

	self->nColumns = $(names, count);
}

static bool Table_parseFile (Table *self, const char *path, bool readColumnNames, bool doTrimming, Char separator)
{
	if (!self) {
		return NULL;
	}
	verifyCorrectClassOrSubclass(self,Table);

	FILE *file = fopen (path, "r");
	if (!file) {
		return false;
	}

	if (!separator) {
        	separator = ',';
	}

	if (readColumnNames) {
		String *namesLine = FileSystem_readLine (file);
		if (!namesLine) {
			return false;
		}
		release(self->columnNames);
		self->nColumns = 0;

		Array *ary = $(namesLine, explode, separator, true, doTrimming, true);
		if (ary) {
			self->nColumns = $(ary, count);
			if (self->nColumns > 0) {
				self->columnNames = retain(MutableArray_withArray(ary));
				Log_debug_printf (__FUNCTION__, "Total columns = %lu\n", (unsigned long) $(self->columnNames, count));
			}
		} 

		if (!self->nColumns) {
			Log_debug_printf (__FUNCTION__, "No columns\n");
			fclose (file);
			return false;
		}
	}

	String *line = NULL;
	while (NULL != (line = FileSystem_readLine(file))) {
		Array *values = $(line, explode, separator, true, doTrimming, true);
		if (values && $(values, count) > 0) {
			$(self->rows, append, values);
			Log_debug_printf (__FUNCTION__, "Got %lu data columns\n", (unsigned long) $(values, count));
		} else {
			if (values) {
				release(values);
			}
			//$(self->rows, append, NULL);
			Log_debug (__FUNCTION__, "Found empty row.\n");
		}
	}

	return true;
}

static bool Table_parseCSV (Table *self, const char *path, bool readColumnNames, bool doTrimming)
{
	return Table_parseFile (self, path, readColumnNames, doTrimming, ',');
}

static bool Table_parseTSV (Table *self, const char *path, bool readColumnNames, bool doTrimming)
{
	return Table_parseFile (self, path, readColumnNames, doTrimming, '\t');
}

static void print_spaces (unsigned count, FILE *file)
{
	while (count-- > 0) {
		fputc (' ', file ?: stdout);
	}
}
static void print_hbar (unsigned count, FILE *file)
{
	if (count < 2) {
		return;
	}
	if (!file) {
		file = stdout;
	}
	fputc ('+', file);
	count -= 2;
	while (count-- > 0) {
		fputc ('-', file);
	}
	fputc ('+', file);
	fputc ('\n', file);
}

void Table_prettyPrint (Table *self, FILE* output, bool printRowBars)
{
	if (!self) {
		return;
	}
	verifyCorrectClassOrSubclass(self,Table);

	unsigned nColumns = $(self, nColumns);
	unsigned nRows = $(self, nRows);
	if (!nColumns || !nRows) {
		return;
	}

	if (!output) {
		output = stdout;
	}

	unsigned maxColumnWidths[nColumns];
	memset (maxColumnWidths, 0, sizeof(maxColumnWidths));

	unsigned total_chars = 1;
	for (unsigned i=0; i < nColumns; i++) {
		maxColumnWidths[i] = Table_maximumWidthOfColumn (self, i);
		total_chars += maxColumnWidths[i] + 1;
	}
	
	unsigned nNames = self->columnNames ? $(self->columnNames, count) : 0;
	if (!printRowBars || nNames > 0) {
		print_hbar (total_chars, output);
	}

	if (nNames) {
		fputc ('|', output);
		bool print_boldface = output == stdout;
		for (unsigned j=0; j < nColumns; j++) {
			unsigned cellLength = 0;
			Object *obj = j < nNames ? $(self->columnNames, at, j) : NULL;
			if (obj) {
				cellLength = Object_printingLength (obj);
				if (print_boldface) {
					fwrite (ANSIForegroundBold, 1, strlen(ANSIForegroundBold), output);
				}

				$(obj, print, output);

				if (print_boldface) {
					fwrite (ANSIForegroundPlain, 1, strlen(ANSIForegroundBold), output);
				}
			}
			if (cellLength < maxColumnWidths[j]) {
				unsigned difference = maxColumnWidths[j] - cellLength;
				print_spaces (difference, output);
			}
			fputc ('|', output);
		}
		fputc ('\n', output);
	}

	for (unsigned i=0; i < nRows; i++) {
		Array *row = Table_rowAt (self, i);

		if (printRowBars) {
			print_hbar (total_chars, output);
		}
		fputc ('|', output);

		for (unsigned j=0; j < nColumns; j++) {
			unsigned rowLength = $(row, count);
			unsigned cellLength = 0;
			Object *obj = j < rowLength ? $(row, at, j) : NULL;
			if (obj) {
				cellLength = Object_printingLength (obj);
				$(obj, print, output);
			}

			if (cellLength < maxColumnWidths[j]) {
				unsigned difference = maxColumnWidths[j] - cellLength;
				print_spaces (difference, output);
			}
			fputc ('|', output ?: stdout);

		} // for inner

		fputc ('\n', output);

	} // for output

	print_hbar (total_chars, output);
}

TableClass* TableClass_init (TableClass *class)
{
	SET_SUPERCLASS(Object);

	SET_OVERRIDDEN_METHOD_POINTER(Table,describe);
	SET_OVERRIDDEN_METHOD_POINTER(Table,print);

	SET_METHOD_POINTER(Table,at);
	SET_METHOD_POINTER(Table,rowAt);
	SET_METHOD_POINTER(Table,append);
	SET_METHOD_POINTER(Table,nRows);
	SET_METHOD_POINTER(Table,setColumnNames);
	SET_METHOD_POINTER(Table,columnName);
	SET_METHOD_POINTER(Table,nColumns);
	SET_METHOD_POINTER(Table,minimumRequiredColumns);
	SET_METHOD_POINTER(Table,maximumWidthOfColumn);
	SET_METHOD_POINTER(Table,count);
	SET_METHOD_POINTER(Table,parseFile);
	SET_METHOD_POINTER(Table,parseCSV);
	SET_METHOD_POINTER(Table,parseTSV);
	SET_METHOD_POINTER(Table,prettyPrint);
	//SET_METHOD_POINTER(Table,asCSV);
	//SET_METHOD_POINTER(Table,join);

        VALIDATE_CLASS_STRUCT(class);
	return class;
}

Table* Table_init (Table *self)
{
	ENSURE_CLASS_READY(Table);

	if (self) {
		Object_init ((Object*) self);
		self->is_a = _TableClass;

		self->columnNames = NULL;
		self->rows = retain(new(MutableArray));
	}

	return self;
}

Table* Table_fromFile (const char* path, bool readColumnNames, bool trim)
{
	if (!path) {
		return NULL;
	}
	if (!FileSystem_fileExistsWithCString (path)) {
		return NULL;
	}

	Char separator = ' ';
	if (has_suffix (path, ".csv")) {
		separator = ',';
	}
	else if (has_suffix (path, ".tsv")) {
		separator = '\t';
	}
	else if (has_suffix (path, ".md")) {
		separator = '|';
	}

	Table *self = new(Table);
	if (!self) {
		return NULL;
	}

	if (!Table_parseFile (self, path, readColumnNames, trim, separator)) {
		release(self);
		return NULL;
	}

	return self;
}

