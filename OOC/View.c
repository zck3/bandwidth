/*============================================================================
  View, an object-oriented C view class.
  Copyright (C) 2019, 2023, 2024 by Zack T Smith.

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

#include "Window.h"
#include "Controller.h"
#include "View.h"

#include <stdlib.h>

#if defined(HAVE_OPENGL) 
 #define GL_SILENCE_DEPRECATION
 #define GL_GLEXT_PROTOTYPES
 #ifndef __APPLE__
   #include <GL/gl.h>
   #include <GL/glu.h>
   #include <GL/glext.h>
   #if defined(HAVE_GLUT)
     #include <GL/glut.h>
     #include <GL/freeglut_ext.h>
   #endif
 #else
   #include <OpenGL/gl.h>
   #include <OpenGL/glu.h>
   #include <OpenGL/glext.h>
   #include <GLUT/glut.h>
 #endif
#endif

static unsigned createTexture ()
{
#ifdef HAVE_OPENGL
	GLuint textureID;
	glGenTextures(1, &textureID);
	glBindTexture(GL_TEXTURE_2D, textureID);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_BASE_LEVEL, 0);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAX_LEVEL, 0);
	return textureID;
#else
	return INVALID_TEXTURE_ID;
#endif
}

ViewClass *_ViewClass = NULL;

void View_destroy (Any *self_)
{
	DEBUG_DESTROY;
	if (!self_) {
		return;
	}
	verifyCorrectClassOrSubclass(self_,View);

	View *self = self_;

	if (self->textureID != INVALID_TEXTURE_ID) {
#ifdef HAVE_OPENGL
		GLuint oldTextureID = self->textureID;
		self->textureID = INVALID_TEXTURE_ID;
		glDeleteTextures (1, (const GLuint*) &oldTextureID);
		CHECK_GL_ERROR("glDeleteTextures");
#endif
	}

	releaseAndClear (self->drawable);

	Object_destroy ((Object*) self);
}

static long View_message (View* restrict self, long message, Any *sender, long first, long second)
{
	if (!self) {
		return -1;
	}
	verifyCorrectClassOrSubclass(self,View);

	switch (message) {
		case kClickedMessage: {
			Log_debug_printf (__FUNCTION__, "Pointer click in View");
			break;
	 	} 
		case kPointerMovedMessage: {
			Log_debug_printf (__FUNCTION__, "Pointer moved (%d, %d) relative to focused View",
				first, second);
			break;
		}
		default:
			break;
	}

	return 0;
}

static void View_print (View* restrict self, FILE *outputFile) 
{ 
	if (!self) {
		return;
	}
	verifyCorrectClassOrSubclass(self,View);

	if (!outputFile) {
		outputFile = stdout;
	}
}

static void View_describe (View* restrict self, FILE *outputFile) 
{ 
	if (!self) {
		return;
	}
	verifyCorrectClassOrSubclass(self,View);

	if (!outputFile) {
		outputFile = stdout;
	}

	fprintf (outputFile, "%s", $(self, className));
}

static void View_drawBorder (View* restrict self)
{
	if (!self
	  || (self->borderColor >> 24) == 0xff
	  || self->borderWidth <= 0 
	  || self->borderType == ViewBorderTypeNone) {
		return;
	}
	verifyCorrectClassOrSubclass(self,View);
	MutableImage *drawable = self->drawable;
	if (!drawable) {
		return;
	}
	
	int width = self->rect.size.width;
	int height = self->rect.size.height;
	int x1 = width - 1;
	int y1 = height - 1;

	ViewBorderType borderType = self->borderType;
	if (borderType == ViewBorderTypeNone && self->borderWidth > 0 && self->borderColor != RGB_CLEAR) {
		borderType = ViewBorderTypeSolid;
	}

	switch (borderType) {
	case ViewBorderTypeNone:
		break;

	case ViewBorderTypeRounded: 
		if (self->cornerRadius > 0) {
			$(drawable, drawRoundedRectangle, 0, 0, width, height, self->cornerRadius, self->borderColor);
			break;
		}
		// If cornerRadius is 0, just draw a rectangle below.

	default:
	case ViewBorderTypeSolid: {
		for (int i=0; i < self->borderWidth; i++) {
			int y = i;
			$(drawable, drawLine, i, y, x1-i, y, self->borderColor);
			y = y1-i;
			$(drawable, drawLine, i, y, x1-i, y, self->borderColor);

			int x = i;
			$(drawable, drawLine, x, i, x, y1-i, self->borderColor);
			x = x1-i;
			$(drawable, drawLine, x, 1+i, x, y1-i-1, self->borderColor);
		}
	 } break;

	case ViewBorderType3DOut: 
	case ViewBorderType3DIn: {
		// RULE: 3D borders are based on the upper gradient color.

		RGB color = self->borderColor;
		RGB dark = RGBChangeIntensity (color, 0.15); 
		RGB light = RGBChangeIntensity (color, 0.3); 
		RGB light2 = RGBChangeIntensity (color, 0.2); 
		RGB medium = RGBChangeIntensity (color, 0.28); 

		bool outward = (self->borderType == ViewBorderType3DOut);

		for (int i=0; i < self->borderWidth; i++) {
			// Left
			int x = i;
			$(drawable, drawLine, x, i, x, y1-i, medium);

			// Top
			int y = i;
			$(drawable, drawLine, i, y, x1-i, y, outward ? light : dark);

			// Right
			x = x1-i;
			$(drawable, drawLine, x, 1+i, x, y1-i-1, outward ? light2 : dark);

			// Bottom
			y = y1-i;
			$(drawable, drawLine, i, y, x1-i, y, outward ? dark : light);
		}
	 } break;
	}
}

static RGBA View_backgroundColor (View* restrict self)
{
	if (!self) {
		return RGB_CLEAR;
	}
	verifyCorrectClassOrSubclass(self,View);
	return self->backgroundColor;
}

static void View_setBackgroundColor (View* restrict self, RGBA color)
{
	if (!self) {
		return;
	}
	verifyCorrectClassOrSubclass(self,View);
	self->backgroundColor = color;
	self->needsRedraw = true;
}

static RGBA View_foregroundColor (View* restrict self)
{
	if (!self) {
		return RGB_CLEAR;
	}
	verifyCorrectClassOrSubclass(self,View);
	return self->foregroundColor;
}

static void View_setForegroundColor (View* restrict self, RGBA color)
{
	if (!self) {
		return;
	}
	verifyCorrectClassOrSubclass(self,View);
	self->foregroundColor = color;
	self->needsRedraw = true;
}

static bool View_hidden (View* restrict self)
{
	if (!self) {
		return false;
	}
	verifyCorrectClassOrSubclass(self,View);
	return self->hidden;
}

static void View_setHidden (View* restrict self, bool value)
{
	if (!self) {
		return;
	}
	verifyCorrectClassOrSubclass(self,View);
	self->hidden = value;
	self->needsRedraw = true;
}

static void View_defineBorder (View* restrict self, ViewBorderType type, RGB color, int width, int radius)
{
	if (!self) {
		return;
	}
	verifyCorrectClassOrSubclass(self,View);
	if (type < ViewBorderTypeNone || type > ViewBorderType_last) {
		return;
	}
	self->borderType = type;
	self->borderColor = color;
	self->borderWidth = width;
	self->cornerRadius = radius;
	self->needsRedraw = true;
}

static void View_clear (View* restrict self)
{
	if (!self) {
		return;
	}
	verifyCorrectClassOrSubclass(self,View);
	if (self->drawable) {
		$(self->drawable, fillWithColor, self->backgroundColor);
		View_drawBorder (self);
	}
}

static unsigned View_width (View* restrict self)
{
	if (!self) {
		return 0;
	}
	verifyCorrectClassOrSubclass(self,View);
	return self->rect.size.width;
}

static unsigned View_height (View* restrict self)
{
	if (!self) {
		return 0;
	}
	verifyCorrectClassOrSubclass(self,View);
	return self->rect.size.height;
}

static void View_setRect (View* restrict self, Rect rect)
{
	if (!self) {
		return;
	}
	verifyCorrectClassOrSubclass(self,View);

	unsigned width = rect.size.width;
	unsigned height = rect.size.height;

	bool needsDraw = false;
	if (width > 0 && height > 0 && !self->drawable) {
		needsDraw = true;
	}
	else if (width != self->rect.size.width || height != self->rect.size.height) {
		needsDraw = true;
	}

	self->rect = rect;

	if (needsDraw) {
		self->needsRedraw = true;

		if (self->drawable) {
			release(self->drawable);
			self->drawable = NULL;
		}

		if (width > 0 && height > 0) {
			self->drawable = retain(MutableImage_withSize (width, height));
		}
	}
}

static void View_redraw (View* restrict self)
{
	if (!self) {
		return;
	}
	verifyCorrectClassOrSubclass(self,View);
	self->needsRedraw = false;
}

static bool View_needsRedraw (View* restrict self)
{
	if (!self) {
		return false;
	}
	verifyCorrectClassOrSubclass(self,View);
	return self->needsRedraw;
}

static void View_setNeedsRedraw (View* restrict self)
{
	if (!self) {
		return;
	}
	verifyCorrectClassOrSubclass(self,View);
	self->needsRedraw = true;
}

static void View_setController (View* restrict self, Any *controller)
{
	if (!self) {
		return;
	}
	verifyCorrectClassOrSubclass(self,View);
	verifyCorrectClassOrSubclass(controller,Controller);
	self->controller = controller; // Don't retain.
}

static MutableImage* View_drawable (View* restrict self)
{
	if (!self) {
		return (MutableImage*)NULL;
	}
	verifyCorrectClassOrSubclass(self,View);
	return self->drawable;
}

static void View_addSubview (View* restrict self, Any* subview)
{
	if (!self) {
		return;
	}
	verifyCorrectClassOrSubclass(self,View);
	verifyCorrectClassOrSubclass(subview,View);

	if (!$(self->subviews, contains, subview)) {
		$(self->subviews, append, subview);
		$(self, setNeedsRedraw);
		$(((View*)subview), setController, self->controller);
	}
}

static bool View_hasSubviews (View* restrict self)
{
	if (!self) {
		return false;
	}
	verifyCorrectClassOrSubclass(self,View);
	return self->subviews != NULL && $(self->subviews, count) > 0;
}

ViewClass* ViewClass_init (ViewClass *class)
{
	SET_SUPERCLASS(Object);

	SET_OVERRIDDEN_METHOD_POINTER(View,describe);
	SET_OVERRIDDEN_METHOD_POINTER(View,print);
	SET_OVERRIDDEN_METHOD_POINTER(View,message);

	SET_METHOD_POINTER(View,backgroundColor);
	SET_METHOD_POINTER(View,clear);
	SET_METHOD_POINTER(View,defineBorder);
	SET_METHOD_POINTER(View,drawBorder);
	SET_METHOD_POINTER(View,drawable);
	SET_METHOD_POINTER(View,foregroundColor);
	SET_METHOD_POINTER(View,hidden);
	SET_METHOD_POINTER(View,needsRedraw);
	SET_METHOD_POINTER(View,redraw);
	SET_METHOD_POINTER(View,setBackgroundColor);
	SET_METHOD_POINTER(View,setForegroundColor);
	SET_METHOD_POINTER(View,setHidden);
	SET_METHOD_POINTER(View,setNeedsRedraw);
	SET_METHOD_POINTER(View,setRect);
	SET_METHOD_POINTER(View,width);
	SET_METHOD_POINTER(View,height);
	SET_METHOD_POINTER(View,setController);
	SET_METHOD_POINTER(View,addSubview);
	SET_METHOD_POINTER(View,hasSubviews);

        VALIDATE_CLASS_STRUCT(class);
	return class;
}

View *View_with (Rect rect, RGB backgroundColor)
{
	View *self = new(View);
	self->backgroundColor = backgroundColor;
	View_setRect (self, rect);
	return self;
}

View* View_init (View* restrict self)
{
	ENSURE_CLASS_READY(View);

	if (self) {
		Object_init ((Object*) self);
		self->is_a = _ViewClass;

		self->drawable = NULL;
		self->rect = Rect_zero();
		self->textureID = createTexture();
		self->backgroundColor = RGB_WHITE;
		self->foregroundColor = RGB_BLACK;
		self->needsRedraw = true;
		self->subviews = retain(new(MutableArray));
	}

	return self;
}

