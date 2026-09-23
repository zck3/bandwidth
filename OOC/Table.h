/*============================================================================
  Table, an object-oriented C immutable table class.
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

#ifndef _OOC_TABLE_H
#define _OOC_TABLE_H

#include "MutableString.h"
#include "MutableArray.h"

#include <stdbool.h>
#include <math.h>

typedef Object *ObjectPointer;

#define DECLARE_TABLE_INSTANCE_VARS(FOO) \
	MutableArray *columnNames; \
	MutableArray *rows; \
	unsigned nColumns;

	// TODO
	// String *title;

#define DECLARE_TABLE_METHODS(TYPE_POINTER) \
	Any *(*at) (TYPE_POINTER, unsigned row, unsigned column); \
	unsigned (*nRows) (TYPE_POINTER); \
	void (*append) (TYPE_POINTER, Array*); \
	Array *(*rowAt) (TYPE_POINTER, unsigned row); \
	unsigned (*nColumns) (TYPE_POINTER); \
	unsigned (*minimumRequiredColumns) (TYPE_POINTER); \
	unsigned (*count) (TYPE_POINTER); \
	String *(*columnName) (TYPE_POINTER, unsigned column); \
	void (*setColumnNames) (TYPE_POINTER, Array*); \
	bool (*parseFile) (TYPE_POINTER, const char *path, bool readColumnNames, bool doTrimming, Char separator); \
	bool (*parseCSV) (TYPE_POINTER, const char *path, bool readColumnNames, bool doTrimming); \
	bool (*parseTSV) (TYPE_POINTER, const char *path, bool readColumnNames, bool doTrimming); \
	unsigned (*maximumWidthOfColumn) (TYPE_POINTER, unsigned column); \
	void (*prettyPrint) (TYPE_POINTER, FILE*, bool printRowBars);

	// TODO
	// void (*appendCStrings) (TYPE_POINTER, const char**);
	// void (*appendInts) (TYPE_POINTER, int*, int count);
	// void (*appendDoubles) (TYPE_POINTER, double*, int count);
	// MutableString* (*asCSV) (TYPE_POINTER); 

struct table;

typedef struct tableclass {
	DECLARE_OBJECT_CLASS_VARS
        DECLARE_OBJECT_METHODS(struct table*)
        DECLARE_TABLE_METHODS(struct table*)
} TableClass;

extern TableClass *_TableClass;
extern TableClass* TableClass_init (TableClass*);

typedef struct table {
        TableClass *is_a;
	DECLARE_OBJECT_INSTANCE_VARS(struct table*)
	DECLARE_TABLE_INSTANCE_VARS(struct table*)
} Table;

extern void Table_destroy (Any *);
extern Table *Table_init (Table *self);
extern Table* Table_fromFile (const char* path, bool readColumnNames, bool trim);

#endif
