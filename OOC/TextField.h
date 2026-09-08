/*============================================================================
  TextField, an Object-Oriented C text field class.
  Copyright (C) 2008-2019, 2022, 2026 by Zack T Smith.

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

// This code is adapted from the class TextField.cpp of my FrugalWidgets project.

#ifndef _OOC_TEXTFIELD_H
#define _OOC_TEXTFIELD_H

#include "View.h"
#include "Font.h"

#define kTextfieldMaxLength (1023)
#define kTextfieldDefaultBorder (5)
#define kTextfieldDefaultPadding (1)

#define DECLARE_TEXTFIELD_INSTANCE_VARS(FOO) \
	RGB textColor; \
	RGB highlightColor; \
	RGB cursorColor; \
	short charPositions [kTextfieldMaxLength+1]; \
	Char line [kTextfieldMaxLength+1]; \
	Char placeholder [kTextfieldMaxLength+1]; \
	Font *font; \
	unsigned length; \
	int horizontalPadding; \
	int cursorPosition, cursorX; \
	bool editing, onlyShowBorderWhenEditing; \
	bool isFull; \
	bool doHaveFocus; \
	bool ctrl, shift; \
	bool selecting; \
	int dragStartX, dragStartY; \
	int dragStartChar, dragEndChar; \
	Object *listener; 

#define DECLARE_TEXTFIELD_METHODS(TYPE_POINTER) \
	void (*setListener) (TYPE_POINTER, Any*); \
	void (*erase) (TYPE_POINTER); \
	void (*insertChar) (TYPE_POINTER, Char ch); \
	void (*insertChars) (TYPE_POINTER, const Char *); \
	void (*insertInteger) (TYPE_POINTER, long integer); \
	void (*insertText) (TYPE_POINTER, const char *); \
	void (*setText) (TYPE_POINTER, const char*); \
	Char *(*chars) (TYPE_POINTER); \
	void (*setChars) (TYPE_POINTER, const Char*); \
	void (*setWideText) (TYPE_POINTER, const wchar_t*); \
	void (*setFont) (TYPE_POINTER, Font*); \
	Font* (*font) (TYPE_POINTER); 

struct textfield;

typedef struct textfieldclass {
	DECLARE_OBJECT_CLASS_VARS
        DECLARE_OBJECT_METHODS(struct textfield*)
        DECLARE_VIEW_METHODS(struct textfield*)
        DECLARE_TEXTFIELD_METHODS(struct textfield*)
} TextFieldClass;

extern TextFieldClass *_TextFieldClass;
extern TextFieldClass* TextFieldClass_init (TextFieldClass*);

typedef struct textfield {
        TextFieldClass *is_a;
	DECLARE_OBJECT_INSTANCE_VARS(struct textfield*)
	DECLARE_VIEW_INSTANCE_VARS(struct textfield*)
	DECLARE_TEXTFIELD_INSTANCE_VARS(struct textfield*)
} TextField;

extern void TextField_destroy (Any *);
extern TextField *TextField_init (TextField *self);

#endif
