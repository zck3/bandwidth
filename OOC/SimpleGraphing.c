/*============================================================================
  SimpleGraphing, an object-oriented C class for graphing.
  Copyright (C) 2005-2019, 2021, 2025 by Zack T Smith.

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

#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include <limits.h>

#include "colors.h"
#include "SimpleGraphing.h"
#include "FontPK.h"
#include "FontFreetype.h"
#include "Int.h"
#include "Pair.h"

#define kDefaultMargin 40
#define kDefaultLeftMargin 80
#define kDefaultFontHeight (10)
#define kMaxTitleLength (200)
#define kMaxTickTextLength (40)
#define kDefaultWidth 1920
#define kDefaultHeight 1080

SimpleGraphingClass *_SimpleGraphingClass = NULL;

void SimpleGraphing_destroy (Any* self_)
{
        DEBUG_DESTROY;

	if (!self_) {
		return;
	}
	verifyCorrectClass(self_,SimpleGraphing);

	SimpleGraphing *self = self_;

	releaseAndClear (self->image);
	releaseAndClear (self->title);
	releaseAndClear (self->subtitle);
	releaseAndClear (self->font);
	releaseAndClear (self->titleFont);
	releaseAndClear (self->subtitleFont);
	releaseAndClear (self->linesPoints);
	releaseAndClear (self->linesInfo);

	Object_destroy((Object*)self);
}

//----------------------------------------------------------------------------
// Name:	SimpleGraphing_drawAxes 
// Purpose:	Draw the horizontal and vertical axes.
//----------------------------------------------------------------------------
static void SimpleGraphing_drawAxes (SimpleGraphing* restrict self)
{
	$(self->image, drawHorizontalLine, self->left_margin, self->width - self->margin, self->height - self->margin, RGB_BLACK);
	$(self->image, drawVerticalLine, self->left_margin, self->height - self->margin - self->y_span, self->height - self->margin, RGB_BLACK);
}

//----------------------------------------------------------------------------
// Name:	SimpleGraphing_drawLabelsLog2
// Purpose:	Draw the labels and ticks.
//----------------------------------------------------------------------------
static void SimpleGraphing_drawLabelsLog2 (SimpleGraphing* restrict self)
{
	if (!self) {
		return;
	}
	verifyCorrectClass(self,SimpleGraphing);
	if (!self->image) {
		return;
	}

	$(self, drawAxes);

	//----------------------------------------
	// Establish min & max x values.
	// Establish min & max y values.
	//
	long min_x = LONG_MAX;
	long max_x = LONG_MIN;
	long min_y = LONG_MAX;
	long max_y = LONG_MIN;

	size_t nLines = $(self->linesPoints, count);
	for (size_t i = 0; i < nLines; i++)
	{
		MutableArray *points = $(self->linesPoints, at, i);
		size_t nPoints = $(points, count);

		for (size_t j = 0; j < nPoints; j++) {
			Pair *pair = $(points, at, j);
	
			Int *xObj = $(pair, first);
			Int *yObj = $(pair, second);

			if (!xObj || !yObj) {
				break;
			}

			long x = $(xObj,asLong);
			long y = $(yObj,asLong);

			if (x < min_x) {
				min_x = x;
			}
			if (x > max_x) {
				max_x = x;
			}
			if (y < min_y) {
				min_y = y;
			}
			if (y > max_y) {
				max_y = y;
			}
		}
	}

	self->min_x = (long) log2 (min_x);
	self->max_x = (long) ceil (log2 (max_x));
	self->min_y = min_y;
	self->max_y = max_y;

	for (size_t i = self->min_x; i <= self->max_x; i++) {
		char str [kMaxTitleLength];
		int x = self->left_margin + ((i-self->min_x) * self->x_span) / (self->max_x - self->min_x);
		int y = self->height - self->margin + 10;

		unsigned long y2 = 1 << i;
		if (y2 < 1536) {
			snprintf (str, sizeof(str)-1, "%ld B", y2);
		}
		else if (y2 < (1<<20)) {
			snprintf (str, sizeof(str)-1, "%ld kB", y2 >> 10);
		}
		else {
			unsigned long j = y2 >> 20;
			switch ((y2 >> 18) & 3) {
			case 0: snprintf (str, sizeof(str)-1, "%lu MB", (unsigned long) j); break;
			case 1: snprintf (str, sizeof(str)-1, "%lu.25 MB", (unsigned long) j); break;
			case 2: snprintf (str, sizeof(str)-1, "%lu.5 MB", (unsigned long) j); break;
			case 3: snprintf (str, sizeof(str)-1, "%lu.75 MB", (unsigned long) j); break;
			}
		}

		$(self->image, drawVerticalLine, x, y, y - 10, RGB_BLACK);
		$(self->image, drawMiniString, str, x - 10, y + 8, RGB_BLACK);
	}

	int font_height = kDefaultFontHeight;
	int available_height = self->y_span;
	int max_labels = available_height / font_height;
	int preferred_n_labels = self->max_y/10000;
	int actual_n_labels;
	float multiplier = 1;
	if (preferred_n_labels < max_labels) {
		actual_n_labels = preferred_n_labels;
	} else {
		actual_n_labels = max_labels;
		multiplier = preferred_n_labels / (float) actual_n_labels;
	}

	for (size_t i = 0; i <= actual_n_labels; i++) {
		int x = self->left_margin - 10;
		int y = self->height - self->margin - (i * self->y_span) / (float)actual_n_labels;

		$(self->image, drawHorizontalLine, x, x+10, y, RGB_BLACK);

		int value = (int) (i * multiplier);

		char str [kMaxTickTextLength];
		snprintf (str, kMaxTickTextLength-1, "%d GB/s", value);

		$(self->image, drawMiniString, str, x - 40, y - MINIFONT_HEIGHT/2, RGB_BLACK);
	}
}

static void SimpleGraphing_setXAxisMode (SimpleGraphing* restrict self, int x_axis_mode)
{
	if (!self) {
		return;
	}
	verifyCorrectClass(self,SimpleGraphing);
	if (!self->image) {
		return;
	}

	if (self) {
		if (x_axis_mode != MODE_X_AXIS_LINEAR && x_axis_mode != MODE_X_AXIS_LOG2) {
			return;
		}
		self->x_axis_mode = x_axis_mode;
	}
}

static void SimpleGraphing_clear (SimpleGraphing* restrict self)
{
	if (!self) {
		return;
	}
	verifyCorrectClass(self,SimpleGraphing);
	if (!self->image) {
		return;
	}
	if (self) {
		$(self->image, clear);
		$(self->linesPoints, removeAll);
	}
}

static MutableImage *SimpleGraphing_image (SimpleGraphing* restrict self)
{       
	if (!self) {
		return NULL;
	}
	verifyCorrectClass(self,SimpleGraphing);
        if (self) 
		return self->image;
	
	return NULL;
}

SimpleGraphing* SimpleGraphing_init (SimpleGraphing*  restrict self)
{
	ENSURE_CLASS_READY(SimpleGraphing);

	if (!self) {
		return NULL;
	}
	
	Object_init ((Object*)self);
	self->is_a = _SimpleGraphingClass;

	self->x_axis_mode = MODE_X_AXIS_LINEAR; // Default value

	self->margin = kDefaultMargin;
	self->legendMargin = kDefaultMargin/3;
	self->left_margin = kDefaultLeftMargin;

	self->linesPoints = retain(new(MutableArray));
	self->linesInfo = retain(new(MutableArray));

	self->font = retain(FontPK_with ("cmr", 18));

	if (FontFreetype_isAvailable()) {
		self->subtitleFont = (Font*) FontFreetype_fromFile ("/usr/share/fonts/truetype/dejavu/DejaVuSerifCondensed-Italic.ttf", 14);
	}

	if (!self->subtitleFont) {
		self->subtitleFont = (Font*) FontPK_with ("cmr", 14);
	}
	if (self->subtitleFont) {
		retain(self->subtitleFont);
	}

	return self;
}

static SimpleGraphing *SimpleGraphing_initWithSize (SimpleGraphing* restrict self, int width, int height)
{
	ENSURE_CLASS_READY(SimpleGraphing);

	if (!self) {
		return NULL;
	}

	SimpleGraphing_init (self);

	if (width <= 0 || height <= 0) {
		width = kDefaultWidth;
		height = kDefaultHeight;
	}

	self->image = retain(MutableImage_withSize(width, height));
	if (!self->image) {
		release(self);
		return NULL;
	}

	self->width = width;
	self->height = height;

	$(self->image, clear);

	self->x_span = self->width - (self->margin + self->left_margin);
	self->y_span = self->height - 2 * self->margin;

	return self;
}

SimpleGraphing* SimpleGraphing_withSize (int width, int height)
{
	SimpleGraphing* graphing = allocate(SimpleGraphing);
	return SimpleGraphing_initWithSize (graphing, width, height);
}

static void SimpleGraphing_setTitle (SimpleGraphing* restrict self, Any *title_)
{
	if (!self) {
		return;
	}
	if (title_ == NULL) {
		error_null_parameter (__FUNCTION__);
	}
	verifyCorrectClass(self,SimpleGraphing);
	verifyCorrectClassOrSubclass(title_,String);
	String *title = title_;

	if (self->image == NULL) {
		return;
	}

	// RULE: We'll decide what font to use below
	// based on image size and text length,
	// to get the maximum size that will fit.

	if (title != self->title) {
		if (self->title) {
			releaseAndClear(self->title);
		}
		self->title = retain(title);
	}

	long x = 0;
	int y = self->margin/4;

	// Try title fonts from biggest to smallest until it fits.
	static int fontSizesToTry[] = { 35, 26, 18, 14 };
#define N_FONTS sizeof(fontSizesToTry)/sizeof(int)
	for (int fs = 0; fs < N_FONTS; fs++) {
		int sizeToTry = fontSizesToTry[fs];
		self->titleFont = retain(FontPK_with ("cmr", sizeToTry));
		if (self->titleFont) {
			int w = $(self->titleFont, stringWidth, title);
			x = (self->width - w) / 2;
			if (x >= 0) {
				break;
			} else {
				release(self->titleFont);
				self->titleFont = NULL;
			}
		}
	}

	// If the title didn't fit using any of the font sizes, use the smallest.
	if (!self->titleFont) {
		self->titleFont = retain(FontPK_with ("cmr", fontSizesToTry[N_FONTS-1]));
	}

	$(self->image, drawString, self->title, x, y, self->titleFont, RGB_BLACK);

	self->y_span -= self->titleFont->height;
}

static void SimpleGraphing_setSubtitle (SimpleGraphing* restrict self, Any *subtitle_)
{
	if (!self) {
		return;
	}
	if (!subtitle_) {
		error_null_parameter (__FUNCTION__);
	}
	verifyCorrectClass(self,SimpleGraphing);
	verifyCorrectClassOrSubclass(subtitle_,String);
	String *subtitle = subtitle_;
	if (!self->image) {
		return;
	}
	if (!self->subtitleFont) {
		return;
	}

	if (subtitle != self->subtitle) {
		if (self->subtitle) {
			release(self->subtitle);
		}
		self->subtitle = retain(subtitle);
	}

	int x = (self->width - $(self->subtitleFont, stringWidth, subtitle)) / 2;
	int y = self->margin/4 + self->titleFont->height + 3;
	$(self->image, drawString, self->subtitle, x, y, self->subtitleFont, RGB_BLACK);

	self->y_span -= self->subtitleFont->height;
}

static void SimpleGraphing_addLine (SimpleGraphing* restrict self, const char *str, RGB color)
{
	if (!self) {
		return;
	}
	verifyCorrectClass(self,SimpleGraphing);
	if (!str) {
		warning (__FUNCTION__, "Line name is null, therefore ignored");
		return;
	}

	// XX Refactor, needs a dictionary

	String *string = String_withCString (str);
	Int *colorObject = Int_withUnsignedLong (color);

	$(self->linesInfo, append, Pair_with(string, colorObject));
	$(self->linesPoints, append, new(MutableArray));
}

static void SimpleGraphing_drawLegend (SimpleGraphing* restrict self)
{
	if (!self) {
		return;
	}
	verifyCorrectClass(self,SimpleGraphing);
	if (!self->image) {
		return;
	}

	// RULE: Find out what the maximum width is for all the strings in the legend
	//	so that they can be put in a box that just fits them.

	int nLines = $(self->linesInfo, count);
	int maxWidth = 0;
	for (size_t i=0; i < nLines; i++) {
		Pair *lineInfo = $(self->linesInfo, at, i);

		String *string = $(lineInfo, first);
		int width = $(self->font, stringWidth, string);
		if (width > maxWidth)
			maxWidth = width;
	}

	int legend_x0 = self->width - maxWidth - self->margin - 2*self->legendMargin;
	int legend_y0 = self->margin;
	int legend_width = maxWidth + 2*self->legendMargin;

	int fontHeight = $(self->font, height);
	int legend_height = fontHeight * nLines + 2*self->legendMargin;

	if (self->title && self->titleFont) {
		legend_y0 += self->titleFont->height;
	}
	if (self->subtitle && self->subtitleFont) {
		legend_y0 += self->subtitleFont->height;
	}

	$(self->image, drawRoundedRectangle, legend_x0, legend_y0, legend_width, legend_height, 10, RGB_GRAY8);

	int x = legend_x0 + self->legendMargin;
	int y = legend_y0 + self->legendMargin;

	for (size_t i=0; i < nLines; i++) {
		Pair *lineInfo = $(self->linesInfo, at, i);

		String *string = $(lineInfo, first);

		Int *color = $(lineInfo, second);
		RGB rgb = (RGB) $(color, asLong);

		$(self->image, drawString, string, x, y, self->font, rgb);
		y += fontHeight;
	}
	
	self->fg = 0;
	self->last_x = self->last_y = -1;
}

//----------------------------------------------------------------------------
// Name:	SimpleGraphing_addPoint
// Purpose:	Adds a point to self list to be drawn.
//----------------------------------------------------------------------------
static void SimpleGraphing_addPoint (SimpleGraphing* restrict self, long x, long y)
{
	if (!self) {
		return;
	}
	verifyCorrectClass(self,SimpleGraphing);
	if (!self->image) {
		return;
	}

	MutableArray *currentLinePoints = $(self->linesPoints, last);
	Pair *pair = Pair_with (Int_withLong(x), Int_withLong(y));
	$(currentLinePoints, append, pair);
}

//----------------------------------------------------------------------------
// Name:	SimpleGraphing_plotLog2
// Purpose:	Plots a point on the current graph.
//----------------------------------------------------------------------------
static void SimpleGraphing_plotLog2 (SimpleGraphing* restrict self, long x, long y)
{
	if (!self) {
		return;
	}
	verifyCorrectClass(self,SimpleGraphing);
	if (!self->image) {
		return;
	}

	//----------------------------------------
	// Plot the point. The x axis is 
	// logarithmic, base 2.
	//
	double tmp = log2 (x);
	tmp -= (double) self->min_x;
	tmp *= (double) self->x_span;
	tmp /= (double) (self->max_x - self->min_x);

	int x2 = self->left_margin + (int) tmp;
	int y2 = self->height - self->margin - (y * self->y_span) / self->max_y;

	if (self->last_x != -1 && self->last_y != -1) {
		if (self->fg & DASHED) {
			$(self->image, drawDashedLine, self->last_x, self->last_y, x2, y2, self->fg & 0xffffff);
		} else {
			$(self->image, drawLine, self->last_x, self->last_y, x2, y2, self->fg);
		}
	}

	self->last_x = x2;
	self->last_y = y2;
}

//----------------------------------------------------------------------------
// Name:	SimpleGraphing_plotLinear
// Purpose:	Plots a point on the current graph.
//----------------------------------------------------------------------------
static void SimpleGraphing_plotLinear (SimpleGraphing* restrict self, long x, long y, long max_y)
{
	if (!self) {
		return;
	}
	verifyCorrectClass(self,SimpleGraphing);
	if (!self->image) {
		return;
	}

	//----------------------------------------
	// Plot the point. The x axis is 
	// logarithmic, base 2. The units of the
	// y value is kB.
	//
	double tmp = 10. + log2 (x);
	tmp -= (double) XVALUE_MIN;
	tmp *= (double) self->x_span;
	tmp /= (double) (XVALUE_MAX - XVALUE_MIN);
	int x2 = self->left_margin + (int) tmp;
	int y2 = self->height - self->margin - (y * self->y_span) / max_y;

//printf ("\tx=%d, y=%d\n",x,y); fflush(stdout);

	if (self->last_x != -1 && self->last_y != -1) {
		if (self->fg & DASHED) {
			$(self->image, drawDashedLine, self->last_x, self->last_y, x2, y2, self->fg & 0xffffff);
		} else {
			$(self->image, drawLine, self->last_x, self->last_y, x2, y2, self->fg);
		}
	}

	self->last_x = x2;
	self->last_y = y2;
}

//----------------------------------------------------------------------------
// Name:	SimpleGraphing_makeLog2
// Purpose:	Plots all lines.
//----------------------------------------------------------------------------
static void SimpleGraphing_makeLog2 (SimpleGraphing* restrict self)
{
	if (!self) {
		return;
	}
	verifyCorrectClass(self,SimpleGraphing);
	if (!self->image) {
		return;
	}

	int nLines = $(self->linesPoints, count);
	if (!nLines) {
		return;
	}

	SimpleGraphing_drawLabelsLog2 (self);

	self->last_x = -1;
	self->last_y = -1;

	//----------------------------------------
	// Draw the data lines.
	//
	for (size_t i = 0; i < nLines; i++)
	{
		MutableArray *points = $(self->linesPoints, at, i);
		size_t nPoints = $(points, count);

		self->last_x = -1;
		self->last_y = -1;

		Pair *lineInfo = $(self->linesInfo, at, i);
		Int *color = $(lineInfo, second);
		self->fg = (unsigned long) $(color, asUnsignedLong);

		for (size_t j = 0; j < nPoints; j++) {
			Pair *pair = $(points, at, j);
		
			Int *xObject = $(pair, first);
			Int *yObject = $(pair, second);
			if (xObject && yObject) {
				long x = $(xObject, asLong);
				long y = $(yObject, asLong);

				$(self, plotLog2, x, y);
			}
		}
	}
}

//----------------------------------------------------------------------------
// Name:	SimpleGraphing_makeLinear
// Purpose:	Plots linear graph.
//----------------------------------------------------------------------------
static void SimpleGraphing_makeLinear (SimpleGraphing* restrict self)
{
	if (!self) {
		return;
	}
	verifyCorrectClass(self,SimpleGraphing);
	if (!self->image) {
		return;
	}

	int nLines = $(self->linesPoints, count);
	if (!nLines) {
		return;
	}

	//----------------------------------------
	// Get the maximum bandwidth in order to
	// properly scale the graph vertically.
	//
	long max_y = 0;
	for (size_t i = 0; i < nLines; i++)
	{
		MutableArray *points = $(self->linesPoints, at, i);
		size_t nPoints = $(points, count);

		for (size_t j = 0; j < nPoints; j++) {
			Pair *pair = $(points, at, j);
			Int *yObject = $(pair, second);
			if (yObject) {
				long y = $(yObject, asLong);
				if (y > max_y) {
					max_y = y;
				}
			}
		}
	}

	long range = max_y > 10000 ? 2 : (max_y > 1000 ? 1 : 0);
	long y_spacing = 1;
	switch (range) {
	case 2:
		// Round up to the next 100.00 MB/sec. (=10000).
		y_spacing = 10000;
		break;
	case 1:
		// Round up to the next 10.00 MB/sec. 
		y_spacing = 1000;
		break;
	case 0:
		// Round up to the next 1.00 MB/sec. 
		y_spacing = 100;
		break;
	} 
	max_y /= y_spacing;
	max_y *= y_spacing;
	max_y += y_spacing;

	//----------------------------------------
	// Draw the axes, ticks & labels.
	//
	$(self, drawAxes);

	//----------
	// X axis:
	//
	if (XVALUE_MIN < 10) {
		return;
	}

	for (int i = XVALUE_MIN; i <= XVALUE_MAX; i++) {
		char str[kMaxTickTextLength];
		unsigned long y2 = 1 << (i-10); // XX XVALUE_MIN>=10
		if (y2 < 1024) {
			snprintf (str, sizeof(str)-1, "%u kB", (unsigned int) y2);
		} else {
			snprintf (str, sizeof(str)-1, "%lu MB", (unsigned long) (y2 >> 10));
		}

		int x = self->left_margin + ((i - XVALUE_MIN) * self->x_span) / (XVALUE_MAX - XVALUE_MIN);
		int y = self->height - self->margin + 10;
		
		$(self->image, drawVerticalLine, x, y, y-10, RGB_BLACK);
		$(self->image, drawMiniString, str, x - 10, y+8, RGB_BLACK);
	}

	//----------
	// Y axis:
	//
	// Decide what the tick spacing will be.
	for (int i = 0; i <= max_y; i += y_spacing) {
		char str[kMaxTickTextLength];
		unsigned long whole = i / 100;
		unsigned long frac = i % 100;
		snprintf (str, sizeof(str)-1, "%lu.%02lu MB/s", whole, frac);

		int x = self->left_margin - 10;
		int y = self->height - self->margin - (i * self->y_span) / max_y;

		$(self->image, drawHorizontalLine, x, x+10, y, RGB_BLACK);
		$(self->image, drawMiniString, str, x - 60, y - MINIFONT_HEIGHT/2, RGB_BLACK);
	}

	//----------------------------------------
	// Draw the data lines.
	//
	for (size_t i = 0; i < nLines; i++)
	{
		MutableArray *points = $(self->linesPoints, at, i);
		size_t nPoints = $(points, count);

		self->last_x = -1;
		self->last_y = -1;

		for (size_t j = 0; j < nPoints; j++) {
			Pair *pair = $(points, at, j);
		
			Int *xObject = $(pair, first);
			Int *yObject = $(pair, second);
			if (xObject && yObject) {
				long x = $(xObject, asLong);
				long y = $(yObject, asLong);
				$(self, plotLinear, x, y, max_y);
			}
		}
	}
}

static void SimpleGraphing_make (SimpleGraphing* restrict self)
{
	if (!self) {
		return; 
	}
	verifyCorrectClass(self,SimpleGraphing);
	if (!self->image) {
		return;
	}

	switch (self->x_axis_mode) {
	case MODE_X_AXIS_LOG2:
		SimpleGraphing_makeLog2 (self);
		break;
	case MODE_X_AXIS_LINEAR:
		SimpleGraphing_makeLinear (self);
		break;
	default:
		fprintf (stderr, "Invalid graph mode %d.\n", self->x_axis_mode);
		break;
	}

	SimpleGraphing_drawLegend (self);
}

static void SimpleGraphing_describe (SimpleGraphing* restrict self, FILE *file)
{
	if (!self) {
		return;
	}
	verifyCorrectClass(self,SimpleGraphing);
	if (!self->image) {
		return;
	}
	if (!file) {
		file = stdout;
	}
	fprintf (file, "%s(%dx%d)", $(self, className), self->width, self->height);
}

SimpleGraphingClass* SimpleGraphingClass_init (SimpleGraphingClass *class)
{
        SET_SUPERCLASS(Object);

        SET_OVERRIDDEN_METHOD_POINTER(SimpleGraphing,describe);
        SET_OVERRIDDEN_METHOD_POINTER(SimpleGraphing,destroy);

	SET_METHOD_POINTER(SimpleGraphing,clear);
	SET_METHOD_POINTER(SimpleGraphing,addPoint);
	SET_METHOD_POINTER(SimpleGraphing,drawLabelsLog2);
	SET_METHOD_POINTER(SimpleGraphing,image);
	SET_METHOD_POINTER(SimpleGraphing,make);
	SET_METHOD_POINTER(SimpleGraphing,addLine);
	SET_METHOD_POINTER(SimpleGraphing,plotLinear);
	SET_METHOD_POINTER(SimpleGraphing,plotLog2);
	SET_METHOD_POINTER(SimpleGraphing,setTitle);
	SET_METHOD_POINTER(SimpleGraphing,setSubtitle);
	SET_METHOD_POINTER(SimpleGraphing,setXAxisMode);
	SET_METHOD_POINTER(SimpleGraphing,drawAxes);

	VALIDATE_CLASS_STRUCT(class);
        return class;
}

