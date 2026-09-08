/*============================================================================
  ProgressBar, an object-oriented C progress bar class.
  Copyright (C) 2019, 2022, 2026 by Zack T Smith.

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

// This is based on my FrugalWidget project's ProgressBar class.

#include <stdlib.h>

#include "Window.h"
#include "ProgressBar.h"
#include "colors.h"

ProgressBarClass *_ProgressBarClass = NULL;

void ProgressBar_destroy (Any *self)
{
	DEBUG_DESTROY;

	if (!self) {
		return;
	}
	verifyCorrectClassOrSubclass(self,ProgressBar);

	View_destroy (self);
}

static void ProgressBar_print (ProgressBar* restrict self, FILE *outputFile) 
{ 
	if (!self) {
		return;
	}
	verifyCorrectClass(self,ProgressBar);

	if (!outputFile) {
		outputFile = stdout;
	}

	fprintf (outputFile, "ProgressBar(%.1f%%)", self->percentage);
}

static void ProgressBar_describe (ProgressBar* restrict self, FILE *outputFile) 
{ 
	if (!self) {
		return;
	}
	verifyCorrectClass(self,ProgressBar);

	if (!outputFile) {
		outputFile = stdout;
	}

	fprintf (outputFile, "%s(%.1g%%)", $(self, className), self->percentage);
}

static void ProgressBar_setGradientColors (ProgressBar* restrict self, RGB upper, RGB lower)
{ 
	if (!self) {
		return;
	}
	verifyCorrectClassOrSubclass(self,ProgressBar);

	self->upperGradientColor = upper;
	self->lowerGradientColor = lower;

	$(self, setNeedsRedraw);
}

MAKE_GETTER_SCALAR(ProgressBar,float,percentage)

static void ProgressBar_setPercentage (ProgressBar* restrict self, float value)
{ 
	if (!self) {
		return;
	}
	verifyCorrectClassOrSubclass(self,ProgressBar);

	// TODO
	// Special cases
	// If the value is increasing, it's possible to draw fewer pixels and limit to: 
	// x0 = the previous value 
	// x1 = the new value
	// If the value is decreasing, it's possible to draw fewer pixels and limit to: 
	// x0 = the new value 
	// x1 = the previous value

	if (value > 100.f) {
		value = 100.f;
	}
	else if (value < 0.f) {
		value = 0.f;
	}
	self->percentage = value;

	$(self, setNeedsRedraw);
}

static void ProgressBar_redraw (ProgressBar* restrict self)
{
	if (!self) {
		return;
	}
	verifyCorrectClassOrSubclass(self,ProgressBar);

	self->needsRedraw = false;

	if (self->hidden) {
		return;
	}
	MutableImage *drawable = self->drawable;
	if (!drawable) {
		return;
	}

	$(self, drawBorder);

	unsigned width = self->rect.size.width - 2*self->borderWidth;
	unsigned progressWidth = (self->percentage * (float)width) / 100U;
	if (progressWidth > width) {
		progressWidth = width;
	}
	Rect verticalGradientRect = self->rect;
	verticalGradientRect.origin.x = self->borderWidth;
	verticalGradientRect.origin.y = self->borderWidth;
	verticalGradientRect.size.width = progressWidth;
	verticalGradientRect.size.height -= 2*self->borderWidth;

	Rect areaToClear = self->rect;
	areaToClear.origin.x = progressWidth+1;
	areaToClear.origin.y = self->borderWidth;
	areaToClear.size.width = width - progressWidth;
	areaToClear.size.height -= 2*self->borderWidth;

	if (progressWidth < width) {
		$(drawable, fillRect, areaToClear, self->backgroundColor);
	}

	if (progressWidth > 0) {
		$(drawable, fillRectUsingVerticalGradient, verticalGradientRect, self->upperGradientColor, self->lowerGradientColor);
	}
}

ProgressBarClass* ProgressBarClass_init (ProgressBarClass* restrict class)
{
	SET_SUPERCLASS(View);

	SET_OVERRIDDEN_METHOD_POINTER(ProgressBar,describe);
	SET_OVERRIDDEN_METHOD_POINTER(ProgressBar,print);
	SET_OVERRIDDEN_METHOD_POINTER(ProgressBar,redraw);

	SET_METHOD_POINTER(ProgressBar,percentage);
	SET_METHOD_POINTER(ProgressBar,setPercentage);
	SET_METHOD_POINTER(ProgressBar,setGradientColors);
	
        VALIDATE_CLASS_STRUCT(class);
	return class;
}

ProgressBar* ProgressBar_init (ProgressBar* restrict self)
{
	ENSURE_CLASS_READY(ProgressBar);

	if (self) {
		View_init ((View*) self);
		self->is_a = _ProgressBarClass;

		self->percentage = 0.f;
		self->showingGradient = false;
		self->backgroundColor = RGB_LIGHTBLUE;
		self->upperGradientColor = RGB_DARKBLUE;
		self->lowerGradientColor = RGB_BLUE;
		self->borderColor = RGB_NAVYBLUE;
		self->borderWidth = 1;
		self->borderType = ViewBorderTypeSolid;
	}

	return self;
}

ProgressBar *ProgressBar_withPercentage (float value)
{
	ProgressBar *self = new(ProgressBar);
	ProgressBar_setPercentage (self, value);
	return self;
}

