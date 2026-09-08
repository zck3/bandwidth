/*============================================================================
  ImageView, a class for displaying an image within a view.
  This file is derived from my FrugalWidgets and TouchWidgets projects.
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

#include "ImageView.h"
#include "Window.h"
#include "Log.h"

ImageViewClass *_ImageViewClass = NULL;

static void ImageView_describe (ImageView* restrict self, FILE *outputFile) 
{ 
	if (!self) {
		return;
	}
	verifyCorrectClass(self,ImageView);

	if (!outputFile) {
		outputFile = stdout;
	}

	fprintf (outputFile, "%s", $(self, className));
	if (self->image) {
		$(self->image, describe, outputFile);
	}
	Rect_print (self->rect, outputFile);
}

static void ImageView_regenerateStretchedImage (ImageView* restrict self)
{
	if (self->stretchedImage) {
		release(self->stretchedImage);
		self->stretchedImage = NULL;
	}

	if (!self->image) {
		return;
	}
	unsigned imageWidth = $(self->image, width);
	unsigned imageHeight = $(self->image, height);
	if (!imageWidth || !imageHeight) {
		return;
	}
	unsigned width = self->rect.size.width;
	unsigned height = self->rect.size.height;
	if (!width || !height) {
		return;
	}

	switch (self->stretch) {
		default:
		case ImageStretchNone:
			break;

		case ImageStretchIgnoreAspectRatio:
			// TODO Implement image expand.
			break;

		case ImageStretchRetainAspectRatio: {
			float aspect = (float)imageWidth / (float)imageHeight;
			int putativeWidth = aspect * (float)height;
			int desiredWidth;
			int desiredHeight;
			if (putativeWidth > width) {
				desiredWidth = width;
				desiredHeight = height / aspect;
			}
			else {
				desiredWidth = width * aspect;
				desiredHeight = height;
			}
			self->stretchedImage = Image_shrink (self->image, desiredWidth, desiredHeight);
			break;
		}
	};
}

static void ImageView_redraw (ImageView* restrict self)
{
	self->needsRedraw = false;

	MutableImage *drawable = self->drawable;
	if (!drawable) {
		return;
	}

	if (self->hidden) {
		return;
	}

	if (!self->image) {
		Log_warning (__FUNCTION__, "ImageView has no image.");
		return;
	}

	int x = 0, y = 0;
	int w = $(self->image, width);
	int h = $(self->image, height);
	if (w < 1 || h < 1) {
		return;
	}
	//==========

	if (self->stretch != ImageStretchNone && !self->stretchedImage) {
		ImageView_regenerateStretchedImage (self);
	}

	$(self, clear);

	if (self->stretch != ImageStretchNone && self->stretchedImage) {
		w = $(self->stretchedImage, width);
		h = $(self->stretchedImage, height);
	}

	int viewWidth = self->rect.size.width;
	int viewHeight = self->rect.size.height;

	switch (self->alignment) {
		case ImageAlignmentTopLeft:
			x = y = 0;
			break;
		case ImageAlignmentMiddleLeft:
			y = (viewHeight - h) / 2;
			break;
		case ImageAlignmentBottomLeft:
			y = viewHeight - h;
			break;

		case ImageAlignmentTopCenter:
			x = (viewWidth - w) / 2;
			break;
		case ImageAlignmentCenter: 
			x = (viewWidth - w) / 2;
			y = (viewHeight - h) / 2;
			break;
		case ImageAlignmentBottomCenter:
			x = (viewWidth - w) / 2;
			y = viewHeight - h;
			break;

		case ImageAlignmentTopRight:
			x = viewWidth - w;
			break;
		case ImageAlignmentMiddleRight:
			x = viewWidth - w;
			y = (viewHeight - h) / 2;
			break;
		case ImageAlignmentBottomRight:
			x = viewWidth - w;
			y = viewHeight - h;
			break;
	}

	if (x > 0) {
		int x0 = 0;
		$(drawable, fillRect, Rect_new(x+x0, 0, x, viewHeight), self->backgroundColor);
		$(drawable, fillRect, Rect_new(x0+viewWidth-x, 0, x, viewHeight), self->backgroundColor);
	}
	if (y > 0) {
		int y0 = 0;
		$(drawable, fillRect, Rect_new(0, y0, viewWidth, y), self->backgroundColor);
		$(drawable, fillRect, Rect_new(0, y0+viewHeight-y, viewWidth, y), self->backgroundColor);
	}

	if (self->stretch == ImageStretchNone || !self->stretchedImage) {
		$(drawable, putImageAt, (Image*) self->image, Point_new(x, y));
	} else {
		$(drawable, putImageAt, self->stretchedImage, Point_new(x, y));
	}
}

ImageView *ImageView_init (ImageView* restrict self)
{
	ENSURE_CLASS_READY(ImageView);

	if (self) {
		View_init ((View*) self);

		self->is_a = _ImageViewClass;

		self->image = NULL;
		self->stretchedImage = NULL;
		self->stretch = ImageStretchNone;
		self->resizeToFit = false;
		self->alignment = ImageAlignmentCenter;
		self->backgroundColor = RGB_GRAY;
		self->needsRedraw = true;
	}

	return self;
}

static void ImageView_destroy (Any* restrict self_)
{
	ImageView *self = self_;

	releaseAndClear (self->image);
	releaseAndClear (self->stretchedImage);
}

static void ImageView_setImage (ImageView* restrict self, Image* image)
{
	if (!self) {
		return;
	}

	if (image != NULL && image != self->image) {
		release(self->image);
		self->image = NULL;
	}
	
	if (self->stretchedImage) {
		release(self->stretchedImage);
		self->stretchedImage = NULL;
	}

	if (image) {
		self->image = retain(image);
	}
	$(self, setNeedsRedraw);
}

static Image* ImageView_image (ImageView* restrict self)
{
	return self->image;
}

ImageView* ImageView_newWithImage (Image* image)
{
	ImageView* self = new(ImageView);
	$(self, setImage, image);
	retain(image);
	return self;
}

static void ImageView_setImageAlignment (ImageView* restrict self, ImageAlignment alignment)
{ 
	if (!self) {
		return;
	}
	verifyCorrectClass(self,ImageView);

	switch (alignment) {
		case ImageAlignmentTopLeft:
		case ImageAlignmentMiddleLeft:
		case ImageAlignmentBottomLeft:
		case ImageAlignmentTopCenter:
		case ImageAlignmentCenter: 
		case ImageAlignmentBottomCenter:
		case ImageAlignmentTopRight:
		case ImageAlignmentMiddleRight:
		case ImageAlignmentBottomRight:
			self->alignment = alignment; 
			$(self, setNeedsRedraw);
			break;
		default:
			Log_warning (__FUNCTION__, "Invalid image alignment.");
			break;
	}
}

static void ImageView_setImageStretch (ImageView* restrict self, ImageStretch stretch)
{ 
	if (!self) {
		return;
	}
	verifyCorrectClass(self,ImageView);

	switch (stretch) {
		case ImageStretchNone:
		case ImageStretchRetainAspectRatio:
		case ImageStretchIgnoreAspectRatio:
			self->stretch = stretch; 
			$(self, setNeedsRedraw);
			break;
		default:
			Log_warning (__FUNCTION__, "Invalid image stretch.");
			break;
	}
}

ImageViewClass *ImageViewClass_init (ImageViewClass* class)
{
	SET_SUPERCLASS(View);
	
	SET_OVERRIDDEN_METHOD_POINTER(ImageView,describe);
        SET_OVERRIDDEN_METHOD_POINTER(ImageView,destroy);
        SET_OVERRIDDEN_METHOD_POINTER(ImageView,redraw);

	SET_METHOD_POINTER(ImageView,setImageStretch);
	SET_METHOD_POINTER(ImageView,setImageAlignment);
	SET_METHOD_POINTER(ImageView,setImage);
	SET_METHOD_POINTER(ImageView,image);

	VALIDATE_CLASS_STRUCT(class);
	return class;
}

