/*============================================================================
  ImageView, a class for displaying an image within a view.
  Copyright (C) 2008-2016,2019,2023-2024,2026 Zack T Smith.

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

// This file is derived from my FrugalWidgets and TouchWidgets projects.

#ifndef OOC_IMAGEVIEW_H
#define OOC_IMAGEVIEW_H

#include <stdbool.h>

#include "Image.h"
#include "View.h"
#include "GraphicsTypes.h"

typedef enum {
	 ImageAlignmentCenter = 0, 
	 ImageAlignmentTopLeft = 1,
	 ImageAlignmentMiddleLeft = 2,
	 ImageAlignmentBottomLeft = 3,
	 ImageAlignmentTopCenter = 4,
	 ImageAlignmentBottomCenter = 5,
	 ImageAlignmentTopRight = 6,
	 ImageAlignmentMiddleRight = 7,
	 ImageAlignmentBottomRight = 8,
} ImageAlignment;

typedef enum {
	 ImageStretchNone = 0, 
	 ImageStretchRetainAspectRatio = 1, 
	 ImageStretchIgnoreAspectRatio = 2, 
} ImageStretch;

#define DECLARE_IMAGEVIEW_INSTANCE_VARS(TYPE_POINTER) \
	Image *image;\
	Image *stretchedImage;\
	bool resizeToFit; \
	ImageAlignment alignment;\
	ImageStretch stretch;

#define DECLARE_IMAGEVIEW_METHODS(TYPE_POINTER) \
	void (*setImageAlignment) (TYPE_POINTER, ImageAlignment);\
	void (*setImageStretch) (TYPE_POINTER, ImageStretch);\
	void (*setImage) (TYPE_POINTER, Image*);\
	Image *(*image) (TYPE_POINTER);

struct imageview;

typedef struct imageviewclass {
	DECLARE_OBJECT_CLASS_VARS
        DECLARE_OBJECT_METHODS(struct imageview*)
        DECLARE_VIEW_METHODS(struct imageview*)
        DECLARE_IMAGEVIEW_METHODS(struct imageview*)
} ImageViewClass;

extern ImageViewClass *_ImageViewClass;

typedef struct imageview {
        ImageViewClass *is_a;
	DECLARE_OBJECT_INSTANCE_VARS(struct imageview*)
	DECLARE_VIEW_INSTANCE_VARS(struct imageview*)
	DECLARE_IMAGEVIEW_INSTANCE_VARS(struct imageview*)
} ImageView;

extern ImageView *ImageView_new ();
extern ImageView *ImageView_init (ImageView *self);
extern ImageView *ImageView_newWithImage (Image* img);
extern ImageView *ImageView_newWith (Size size, RGB backgroundColor);

extern ImageViewClass *ImageViewClass_init (ImageViewClass* );

#endif
