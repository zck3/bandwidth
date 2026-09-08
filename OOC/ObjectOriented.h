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

#ifndef _OBJECTORIENTED_H
#define _OBJECTORIENTED_H

#include <stdlib.h>
#include <stdint.h>
#include <stdbool.h>

#include "config.h"
#include "Char.h"

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
// 0.39	Ported TextField from FrugalWidgets: highlight, select-all, insert/delete, Enter, listener, delete selection.
// 	Fixed normal logic in Model3D. 
//     	MutableArray: Added setAt method and arrayWithArray.
//     	Began porting TextView/TextEdit from FrugalWidgets.
//     	Added general scroll wheel support. TextView scroll-wheel support.
//     	Views with subviews, using GL scissor region to crop.
//     	Moved arrays of Model3D and View objects into Controller.
//     	Added support in Window for OpenGL full-screen mode.
//     	Added Display class methods for OpenGL version lookup.
//     	Added Char routines due to mbstowcs failing on basic UTF-8.
//     	Improved unit testing e.g. added EXPECT_CLASS, EXPECT_NEVER.
//	Improved OpenGL test.
//     	Added lookup of OS name on macOS.
//	Added macros MAKE_[GS]ETTER_SCALAR, MAKE_SETTER_OBJECT
//	Added ANSI text color logic to Console class.
//	Fixed header file & framework issues with OpenGL & GLUT on MacOS.
//	Added 'restrict' keyword for better compiler optimization.
//	Fixed memory leak discovered with Valgrind.
//-----------------------------------------------------------------------------
#define OOC_RELEASE "0.39"

#if defined(__x86_64__) || defined(_WIN64) || defined(__WIN64__) || defined(__aarch64__) || __WORDSIZE == 64 
 #define IS_64BIT
#endif

//#define GRATUITOUS_DEBUGGING

#define OBJECT_MAGIC_NUMBER (0x7ac4c0de)

#define kDefaultDPI 100

#define ONE_MEGABYTE (1LU << 20LU)
#define ONE_GIGABYTE (1LU << 30LU)
#define ONE_MILLION (1000000LU)

#ifndef MAXUNSIGNED
#define MAXUNSIGNED (~(0U))
#endif

typedef void Any;

struct returnable_pair {
	Any *first;
	Any *second;
};

struct returnable_triplet {
	Any *first;
	Any *second;
	Any *third;
};

typedef struct {
	int32_t start;
	int32_t length;
} Range;

inline Range RangeAt(int32_t start_, int32_t length_) {
	Range r;
	r.start = start_;
	r.length =length_;
	return r;
}
inline Range RangeFrom(int32_t start_, int32_t end_) {
	Range r;
	r.start = start_;
	r.length = end_ - start_ + 1;
	return r;
}

#define IS_FOUR_BYTE_ALIGNED(ADDR) ((3 & (unsigned long)ADDR) == 0)
#define IS_EIGHT_BYTE_ALIGNED(ADDR) ((7 & (unsigned long)ADDR) == 0)
#define IS_16_BYTE_ALIGNED(ADDR) ((15 & (unsigned long)ADDR) == 0)

extern void *ooc_bzero(const void *start, size_t length);
extern size_t ooc_strlen(const char *start);
extern void ooc_strncpy(const char *dest, const char *src, size_t length);
extern int ooc_abstract_method();
extern void *ooc_alloc_memory (size_t);
extern void *ooc_allocate_class (size_t);
extern void *ooc_retain (void* ptr, const char* funcName);
extern void *ooc_allocate (const char *objectType, size_t);
extern bool ooc_release (void *object_, size_t, const char*);
extern void ooc_free (void *object_);
extern bool ooc_isMemberOfClassOrSubclass(void *, void *, const char*);
extern bool isObject(void*);
extern bool areSameClass(void*,void*);

#ifndef __arm__
  #if defined(__i386__) || defined(__x86_64__) || defined(_WIN64) || defined(_WIN32) ||defined(__WIN32__) || defined(__WIN64__) || defined(_M_IX86) || defined(__MINGW32__) || defined(__i386) || defined(__CYGWIN__)
    #define x86
  #endif
#endif

#define STRINGIFY(a) ""#a""
#define STRINGIFY_EVALUATING(EXPR) STRINGIFY(EXPR)

#ifdef DEBUG
#define nullObjectPointerError (fprintf(stderr, "NULL OBJECT POINTER for %s IN %s\n",STRINGIFY(METHOD),__FUNCTION__) & 0)
#define nullMethodPointerError (fprintf(stderr, "NULL METHOD POINTER for %s IN %s\n",STRINGIFY(METHOD),__FUNCTION__) & 0)
#define nullClassStructError (fprintf(stderr, "MISSING IS_A POINTER for %s IN %s\n",STRINGIFY(METHOD),__FUNCTION__) & 0)
#endif

#define DECLARE_EMPTY_CLASS_VARS \
        void *superClass; \
        void *unused; \
        char *_className; \
	size_t classSize;
#define TOTAL_EMPTY_CLASS_ULONGS (4)

typedef struct {
	DECLARE_EMPTY_CLASS_VARS 
} EmptyClass;

#define warning(FUNC,MESSAGE) { fprintf(stderr, "Warning (%s): %s\n",FUNC, MESSAGE); }
#define error(FUNC,MESSAGE) { fprintf(stderr, "Error (%s): %s\n",FUNC, MESSAGE); exit(1); }
#define error_null_parameter(FUNC) { fprintf(stderr, "Error (%s): NULL parameter\n",FUNC); exit(2); }
#define error_bad_class(FUNC,SOUGHT) { fprintf(stderr, "Error (%s): Incorrect is_a pointer, expected %s\n",FUNC,STRINGIFY(SOUGHT)); exit(3); }
#define error_not_an_object(FUNC) { fprintf(stderr, "Error (%s): Parameter is not an object\n",FUNC); exit(4); }
#define error_double_release(FUNC) { fprintf(stderr, "Error (%s): Detected doulbe release\n",FUNC); exit(5); }

// Note, preprocessor concatenation does not evaluate the parameters!
#define CONCAT_SYMBOLS(a,b) a##b
#define CONCAT_SYMBOLS3(a,b,c) a##b##c
#define CONCAT_SYMBOLS4(a,b,c,d) a##b##c##d
#define CONCAT_SYMBOLS5(a,b,c,d,e) a##b##c##d##e

#define EVALUATING_CONCAT(a,b) CONCAT_SYMBOLS(a,b)
#define EVALUATING_CONCAT3(a,b,c) CONCAT_SYMBOLS3(a,b,c)

#define CONCAT_SYMBOLS_FOO(x, y) CONCAT_SYMBOLS(x, y)
#define REPEAT_COUNTER CONCAT_SYMBOLS_FOO(_repeat_counter, __LINE__)
#define repeat(NNN) for(unsigned REPEAT_COUNTER = (NNN); REPEAT_COUNTER-- ;)
#define repeat_as(NNN,MMM) for(unsigned MMM = 0; MMM < (NNN); MMM ++)

extern void deallocateClasses ();
extern void registerClass (const void*);

#define classNameOf(OBJ) ($(OBJ,className) ?: "")

/* Ensure that the current class exists. This will invoke its *Class_init routine.
 */
#define ENSURE_CLASS_READY(CLASS) do {\
  if (! CONCAT_SYMBOLS3(_,CLASS,Class)) {\
    size_t class_struct_size = sizeof(CONCAT_SYMBOLS(CLASS,Class)); \
    CONCAT_SYMBOLS3(_,CLASS,Class)=(CONCAT_SYMBOLS(CLASS,Class)*) ooc_allocate_class(class_struct_size);\
    CONCAT_SYMBOLS3(_,CLASS,Class)->_className = STRINGIFY(CLASS); \
    CONCAT_SYMBOLS3(_,CLASS,Class)->classSize = class_struct_size; \
    CONCAT_SYMBOLS3(CLASS,Class,_init) (CONCAT_SYMBOLS3(_,CLASS,Class));\
    registerClass (CONCAT_SYMBOLS3(_,CLASS,Class)); \
  }} while(false)

/* Ensure the superclass struct exists. Called from current class's *Class_init routine.
 */
#define SET_SUPERCLASS(PARENTCLASS) do {\
  ENSURE_CLASS_READY(PARENTCLASS); \
  CONCAT_SYMBOLS3(PARENTCLASS,Class,_init) ((void*)class); \
  class->superClass = CONCAT_SYMBOLS3(_,PARENTCLASS,Class); \
  } while(false)

#ifdef DEBUG
#define $(OBJ,METHOD,...) \
 ((OBJ)? \
    ((OBJ)->is_a ? \
        ((OBJ)->is_a->METHOD ? 	\
            ((OBJ)->is_a->METHOD(OBJ, ##__VA_ARGS__) \
	    ) \
            : (typeof(((OBJ)->is_a->METHOD(OBJ, ##__VA_ARGS__))))0  nullMethodPointerError \
         ) \
         : nullClassStructError \
     ) \
     : nullObjectPointerError)
#else 
#define $(OBJ,METHOD,...) (\
		(OBJ)? \
		(OBJ)->is_a->METHOD(OBJ, ##__VA_ARGS__)\
            : (typeof(((OBJ)->is_a->METHOD(OBJ, ##__VA_ARGS__))))0LU \
		)
#endif

// This is a simplified form of $ macro for use inside of Blocks i.e. closures.
#define $$(OBJ,METHOD,...) (OBJ->is_a->METHOD(OBJ, ##__VA_ARGS__))

#define VALIDATE_CLASS_STRUCT_(CLASS_PTR) {\
	void **p = (void**) CLASS_PTR; \
	size_t class_struct_size = sizeof(*CLASS_PTR); \
	int n = class_struct_size / sizeof(void*); \
	int totalNullMethodPointers = 0; \
	for (int i=4; i < n; i++) { \
		if (NULL == p[i]) { \
			totalNullMethodPointers++; \
			fprintf (stderr, "%s: Class struct has NULL method pointer at index %d.\n", __FUNCTION__, i-TOTAL_EMPTY_CLASS_ULONGS); \
		} \
	} \
	if (totalNullMethodPointers > 0) { \
		fprintf (stderr, "%s: A total of %d method pointer(s) are NULL\n", __FUNCTION__, totalNullMethodPointers); \
		exit (-1); \
	} \
}
#define VALIDATE_CLASS_STRUCT(CLASS_PTR) VALIDATE_CLASS_STRUCT_(CLASS_PTR)

// NOTE: Retain count is initially 0 i.e. not yet owned.
#ifdef GRATUITOUS_DEBUGGING
	#define allocate(CLASS) ((CLASS*)(printf("Alloc'd %s object in %s\n",STRINGIFY(CLASS), __FUNCTION__), g_totalObjectAllocations++, ooc_allocate (STRINGIFY_EVALUATING(CLASS),sizeof(CLASS)) ))
#else
	#define allocate(CLASS) ((CLASS*)(g_totalObjectAllocations++, ooc_allocate (STRINGIFY_EVALUATING(CLASS),sizeof(CLASS)) ))
#endif

#define new(CLASS) ((CLASS*) CONCAT_SYMBOLS(CLASS,_init) (allocate(CLASS)))
// NOT USED #define new(CLASS) ((Any*)new_(CLASS))

#define retain(OBJ) ooc_retain(OBJ, __FUNCTION__)

// Note, release() only zeroes out the pointer if the retainCount falls to zero, or was already zero.
#define release(OBJ) do { \
	if (!ooc_release((void*)OBJ, sizeof(__typeof(*OBJ)), __FUNCTION__)) {\
		OBJ = (__typeof(OBJ)) NULL; \
	} \
} while(false)

#define releaseAndClear(OBJ) if(OBJ) { release(OBJ); OBJ=NULL; }

#define isMemberOfClass(THIS,CLASS) ((THIS == NULL || \
				      THIS->is_a == NULL || \
				      THIS->magic != OBJECT_MAGIC_NUMBER || \
				      THIS->is_a != (void*) EVALUATING_CONCAT3(_,CLASS,Class)) ? false : true)

#define isMemberOfClassOrSubclass(THIS,CLASS) ooc_isMemberOfClassOrSubclass(THIS,CONCAT_SYMBOLS3(_,CLASS,Class),NULL)

#define verifyCorrectClassOrSubclass(THIS,CLASS) do { if (!ooc_isMemberOfClassOrSubclass(THIS,EVALUATING_CONCAT3(_,CLASS,Class),__FUNCTION__)) error_bad_class(__FUNCTION__,CLASS); } while(false)

#define verifyCorrectClass(THIS,CLASS) \
	if (!THIS || !((Object*)THIS)->is_a || ((Object*)THIS)->magic != OBJECT_MAGIC_NUMBER || ((Object*)THIS)->is_a != (void*) EVALUATING_CONCAT3(_,CLASS,Class)) {\
		error_bad_class (__FUNCTION__,CLASS);\
	}

#define describeObject(THIS) \
	do { \
		if (THIS) { \
			printf ("Object (%lx)'s class is %lx %s, ", (unsigned long)THIS,(unsigned long)THIS->is_a,THIS->is_a->_className); \
			printf ("superclass is %lx %s\n", (unsigned long)THIS->is_a->superClass, \
					THIS->is_a->superClass ?  ((EmptyClass*)THIS->is_a->superClass)->_className : 0); \
		} \
	while(false)

#define verifyCorrectClasses(THIS,CLASS,SUBCLASS) \
	if (!THIS->is_a || (THIS->is_a != (void*)CONCAT_SYMBOLS3(_,CLASS,Class) && THIS->is_a != (void*)CONCAT_SYMBOLS3(_,SUBCLASS,Class))) {\
		error_bad_class (__FUNCTION__,CLASS);\
	}

#define SET_ABSTRACT_METHOD_POINTER(NAME) class->NAME = (void*)ooc_abstract_method

#define SET_METHOD_POINTER_(CLASS,NAME) class->NAME = CLASS##_##NAME
#define SET_METHOD_POINTER(CLASS,NAME) SET_METHOD_POINTER_(CLASS,NAME)

#define SET_OVERRIDDEN_METHOD_POINTER_(CLASS,NAME) class->NAME = CLASS##_##NAME
#define SET_OVERRIDDEN_METHOD_POINTER(CLASS,NAME) SET_OVERRIDDEN_METHOD_POINTER_(CLASS,NAME)

#ifdef GRATUITOUS_DEBUGGING
	#define DEBUG_DESTROY puts(__FUNCTION__);fflush(0)
#else
	#define DEBUG_DESTROY 
#endif

// Example: MAKE_GETTER_SCALAR(Animal,bool,isFurry)
#define MAKE_GETTER_SCALAR(CLASS,TYPE,VARNAME) \
	static TYPE CONCAT_SYMBOLS3(CLASS,_,VARNAME) (CLASS *self) { \
		if (!self) { \
			return 0; \
		} \
		verifyCorrectClassOrSubclass(self,CLASS); \
		return self->VARNAME; \
	} 

// Example: MAKE_SETTER_SCALAR(Animal,bool,isFurry,setIsFurry)
#define MAKE_SETTER_SCALAR(CLASS,TYPE,VARNAME,SETTERNAME) \
	static void CONCAT_SYMBOLS3(CLASS,_,SETTERNAME) (CLASS *self, TYPE value) { \
		if (!self) { \
			return; \
		} \
		verifyCorrectClassOrSubclass(self,CLASS); \
		self->VARNAME = value; \
	}

// Example: MAKE_GETTER_SETTER_SCALAR(Animal,bool,isFurry,setIsFurry)
#define MAKE_GETTER_SETTER_SCALAR(CLASSNAME,SCALARTYPE,IVARNAME,SETTERNAME) \
	MAKE_GETTER_SCALAR(CLASSNAME,SCALARTYPE,IVARNAME) \
	MAKE_SETTER_SCALAR(CLASSNAME,SCALARTYPE,IVARNAME,SETTERNAME) 

// Example: MAKE_SETTER_OBJECT(Animal,View*,theView,setTheView)
#define MAKE_SETTER_OBJECT(CLASS,TYPE,VARNAME,SETTERNAME) \
	static void CONCAT_SYMBOLS3(CLASS,_,SETTERNAME) (CLASS *self, TYPE value) { \
		if (!self) { \
			return; \
		} \
		verifyCorrectClassOrSubclass(self,CLASS); \
		self->VARNAME = retain((Object*)value); \
	}

#endif

