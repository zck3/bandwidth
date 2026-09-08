/*============================================================================
  TextView, an object-oriented C textview class.
  Copyright (C) 2018-2019, 2026 by Zack T Smith.

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

// This is based on my FrugalWidgets project's TextEditor class.

#ifndef _OOC_TEXTVIEW_H
#define _OOC_TEXTVIEW_H

#include "Label.h"

typedef struct textviewword {
	Char *word; // Will be NULL if newlineCount > 0.
	short width, height;
	short ascent, descent;
	int newlineCount;
	struct textviewword *next;
} TextViewWord;

#define DECLARE_TEXTVIEW_METHODS(TYPE_POINTER) \
	int (*yOffset) (TYPE_POINTER); \
	void (*setYOffset) (TYPE_POINTER, int yoffset); \
	int (*renderedHeight) (TYPE_POINTER); 

#define DECLARE_TEXTVIEW_INSTANCE_VARS(TYPE_POINTER) \
        TextViewWord *words; \
	TextViewWord *last; \
	int yOffset; \
	int renderedHeight; \
	bool needToDetermineWordSizes; 

// TODO: Allow multiple fonts.

struct textview;

typedef struct textviewclass {
	DECLARE_OBJECT_CLASS_VARS
        DECLARE_OBJECT_METHODS(struct textview*)
        DECLARE_VIEW_METHODS(struct textview*)
        DECLARE_LABEL_METHODS(struct textview*)
        DECLARE_TEXTVIEW_METHODS(struct textview*)
} TextViewClass;

extern TextViewClass *_TextViewClass;

typedef struct textview {
        TextViewClass *is_a;
	DECLARE_OBJECT_INSTANCE_VARS(struct textview*)
	DECLARE_VIEW_INSTANCE_VARS(struct textview*)
	DECLARE_LABEL_INSTANCE_VARS(struct textview*)
	DECLARE_TEXTVIEW_INSTANCE_VARS(struct textview*)
} TextView;

extern TextView *TextView_init (TextView *self);
extern void TextView_destroy (Any *self);
extern TextViewClass* TextViewClass_init (TextViewClass*);

#endif

