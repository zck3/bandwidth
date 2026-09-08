/*============================================================================
  TestController, an object-oriented C view controller class.
  Copyright (C) 2024, 2026 by Zack T Smith.

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
#include <string.h>
#include <time.h>

#include "Display.h"
#include "Window.h"
#include "Controller.h"
#include "View.h"
#include "FontPK.h"
#include "Testing.h"
#include "test_gui.h"
#include "Log.h"

#if defined(HAVE_OPENGL) && defined(HAVE_GLUT)

#define _GNU_SOURCE
#include <stdio.h>
#include <stdarg.h>

#define CUSTOM_VIEW_SIZE 180
#define MARGIN 24

TestControllerClass *_TestControllerClass = NULL;

void TestController_destroy (Any* self)
{
        DEBUG_DESTROY;

	if (!self) {
		return;
	}
	verifyCorrectClass(self,TestController);
	Controller_destroy (self);
}

TestController* TestController_init (TestController* self)
{
	ENSURE_CLASS_READY(TestController);

	if (!self) {
		return NULL;
	}
	
	Controller_init ((Controller*)self);
	self->is_a = _TestControllerClass;

	self->progressValue = 10.f;
	self->progressDelta = 0.33f;

	return self;
}

bool TestController_construct (TestController *self)
{
	if (!self) { 
		return false;
	}
	verifyCorrectClass(self,TestController);
	Window *window = self->window;
	if (!window) {
		return false;
	}

	self->t0 = time(NULL);

	View *view = self->view = retain(new(View));
	$(view, defineBorder, ViewBorderTypeRounded, RGB_YELLOW, 1, 30);
	$(self, addView, view);

	self->label = retain(new(Label));
	$(self->label, setBackgroundColor, RGB_RED);
	$(self->label, setForegroundColor, RGB_YELLOW);
	$(self, addView, self->label);

	self->exitButton = retain(new(Button));
	$(self->exitButton, setCString, "Exit Button");
	$(self, addView, self->exitButton);
	self->exitButton->listener = self;

	Image *image = Image_fromFile("PIA25970.tif");
	if (!image) {
		MutableImage *mi = MutableImage_withSize(300,200);
		$(mi, drawTestPattern);
		image = (Image*) mi;
	}
	self->iv = retain(new(ImageView));
	$(self->iv, setImageAlignment, ImageAlignmentTopCenter);
	if (image) {
		$(self->iv, setImage, image);
	}
	$(self, addView, self->iv);

	self->tv = retain(new(TextView));
	$(self->tv, setString, _String("When in the Course of human events, it becomes necessary for one people to dissolve the political bands which have connected them with another, and to assume among the powers of the earth, the separate and equal station to which the Laws of Nature and of Nature's God entitle them, a decent respect to the opinions of mankind requires that they should declare the causes which impel them to the separation.\nWe hold these truths to be self-evident, that all men are created equal, that they are endowed by their Creator with certain unalienable Rights, that among these are Life, Liberty and the pursuit of Happiness."));
	Font *font = (Font*) FontPK_with ("cmr", 18);
	$(self->tv, setForegroundColor, RGB_RED);
	$(self->tv, setBackgroundColor, RGB_YELLOW);
	$(self->tv, setFont, font);
	release (font);
	$(self, addView, self->tv);

	self->progressBar = retain(new(ProgressBar));
	$(self->progressBar, setPercentage, 44);
	$(self, addView, self->progressBar);

	self->textfield = retain(new(TextField));
	//self->textfield->onlyShowBorderWhenEditing = true;
	$(self, addView, self->textfield);
	$(self->textfield, setWideText, L"This is a text field.");
	$(self->textfield, setListener, self);

	Label *subview = self->subview = retain(new(Label));
	$(subview, setCString, "Martian Helicopter");
	$(subview, setBackgroundColor, RGB_GREEN);
	$(self->iv, addSubview, subview);

	Label *subview2 = retain(new(Label));
	$(subview2, setCString, "Won't Draw");
	$(subview2, setBackgroundColor, RGB_PINK);
	$(subview, addSubview, subview2);

	return true;
}

static long TestController_message (TestController* self, long message, Any *sender, long first, long second)
{
	if (sender == self->exitButton && message == kClickedMessage) {
		// TODO Application_exit?
		exit (0);
	}
	if (message == kEditingDidEndMessage) {
		const Char *text = $(self->textfield, chars);
		printf ("TEXT FIELD EDITING ENDED, STRING=");
		Char_puts (text);
	}
	return 0;
}

static void TestController_layout (TestController* self)
{
	if (!self) {
		return;
	}
	verifyCorrectClass(self,TestController);
	if (!self->window || !self->label) {
		return;
	}
	
	unsigned width = $(self->window, width);
	unsigned height = $(self->window, height);
	int labelWidth = 200;
	int buttonWidth = 200;
	const int size = CUSTOM_VIEW_SIZE;

	int bottomRowHeight = 48;
	int bottomRowY = height - bottomRowHeight - MARGIN;

	int x = width - MARGIN - size;
	int y = MARGIN;
	$(self->view, setRect, Rect_new(x, y, size, size));
	y += size + MARGIN;
	int tvHeight = bottomRowY - MARGIN - y;
	$(self->tv, setRect, Rect_new(x, y, size, tvHeight));

	$(self->label, setRect, Rect_new(MARGIN, bottomRowY, labelWidth, bottomRowHeight));

	$(self->exitButton, setRect, Rect_new(width - buttonWidth - MARGIN, 
						bottomRowY,
						buttonWidth, bottomRowHeight));

	int progressBarWidth = width - 4*MARGIN - buttonWidth - labelWidth;
	$(self->progressBar, setRect, Rect_new(buttonWidth + 2*MARGIN, 
						bottomRowY,
						progressBarWidth, bottomRowHeight));

	$(self->textfield, setRect, Rect_new(buttonWidth + 2*MARGIN, 
						bottomRowY - MARGIN - bottomRowHeight,
						progressBarWidth, bottomRowHeight));

	$(self->iv, setRect, Rect_new (MARGIN, MARGIN, 2*size, 2*size));

	$(self->subview, setRect, Rect_new (MARGIN, 2*size-50-MARGIN, 2*size-2*MARGIN, 50));
}

bool TestController_update (TestController *self)
{
	if (!self) { 
		return false;
	}
	verifyCorrectClass(self,TestController);

	Window *window = self->window;
	if (!window) {
		return false;
	}

	static int textViewScrollDirection = 1;
	static int pauseDelay = 20;
	int yOffset = $(self->tv, yOffset);
	int renderedHeight = $(self->tv, renderedHeight);
	int tvHeight = $(self->tv, height);
	int maxOffset = renderedHeight - tvHeight;

	static HorizontalAlignment alignment = 0;

	if (pauseDelay > 0) {
		pauseDelay--;
	} else {
		if (yOffset >= maxOffset) {
			textViewScrollDirection = -1;
			alignment = (alignment % 3);
			$(self->tv, setHorizontalAlignment, alignment);
			alignment++;
			pauseDelay = 30;
		}
		else if (yOffset == 0) {
			textViewScrollDirection = 1;
			alignment = (alignment % 3);
			$(self->tv, setHorizontalAlignment, alignment);
			alignment++;
			pauseDelay = 40;
		}
		$(self->tv, setYOffset, yOffset + textViewScrollDirection);
	}

	static int hideCounter = 0;
	hideCounter = (hideCounter + 1) % 200;
	$(self->view, setHidden, hideCounter < 150 ? false : true);

#ifdef DEBUG
	printf ("Camera location: %s\n", Vector_toString(self->cameraPosition));
#endif

	View *view = self->view;
	$(view->drawable, fillWithColor, RGB_PURPLE);
	$(view, drawBorder);
	Font *font = (Font*) FontPK_with ("cmr", 18);
	const int size = CUSTOM_VIEW_SIZE;
	$(view->drawable, drawCString, "CUSTOM VIEW", 10, size/2 - 9, font, RGB_WHITE);
	$(view->drawable, drawLine, 0, 0, size-1, size-1, RGB_ORANGE);
	$(view->drawable, drawCircle, size/2,size/2, 70, 0xff, RGB_YELLOW);
	release(font);

	time_t now = time(NULL);
	time_t diff = now - self->t0;
	char timestring[32];
	snprintf (timestring, sizeof(timestring), "Label %02lu:%02lu", diff / 60, diff % 60);
	$(self->label, setCString, timestring);

	self->progressValue += self->progressDelta;
	if (self->progressValue < 0) {
		self->progressValue = 0;
		self->progressDelta *= -1.f;
	}
	else if (self->progressValue > 100) {
		self->progressValue = 100;
		self->progressDelta *= -1.f;
	}
	$(self->progressBar, setPercentage, (int) self->progressValue);

	return true;
}

static void TestController_dismantle (TestController *self)
{
	puts(__FUNCTION__); fflush(NULL);

	if (!self) { 
		return;
	}
	verifyCorrectClass(self,TestController);
}

TestControllerClass* TestControllerClass_init (TestControllerClass *class)
{
	SET_SUPERCLASS(Controller);

	// Overridden method
	SET_OVERRIDDEN_METHOD_POINTER(TestController,construct);
	SET_OVERRIDDEN_METHOD_POINTER(TestController,update);
	SET_OVERRIDDEN_METHOD_POINTER(TestController,layout);
	SET_OVERRIDDEN_METHOD_POINTER(TestController,dismantle);
	SET_OVERRIDDEN_METHOD_POINTER(TestController,message);

	VALIDATE_CLASS_STRUCT(class);
	return class;
}

int main()
{
	Display *dpy = new(Display);

	const int windowWidth = 800;
	const int windowHeight = 600;

	Log_show_debug_output = true;

	Window *window = Window_newWith (dpy, "Test", windowWidth, windowHeight, 100, 160);
	TestController *controller = new(TestController);
	$(window, setController, controller);

	// These have to be called after OpenGL is initialized.
	puts ("-----------------------------------");
	printf ("Driver: %s\n", $(dpy, driverType));
	printf ("Version: %s\n", $(dpy, driverVersion));
	printf ("Vendor: %s\n", $(dpy, driverVendor));
	printf ("Renderer: %s\n", $(dpy, driverRenderer));

	$(dpy, mainLoop);

	release(dpy);
	release(window);
	release(controller);

	deallocateClasses();
	return 0;
}

#else // HAVE_OPENGL && HAVE_GLUT
int main () { 
	return 0; 
}
#endif
