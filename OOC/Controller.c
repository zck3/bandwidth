/*============================================================================
  Controller, an object-oriented C user interface controller class.
  Copyright (C) 2024 by Zack T Smith.

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

// This class is based on my FrugalWidgets project's Controller.cpp.

#include "Controller.h"
#include "View.h"

#include <stdlib.h>

ControllerClass *_ControllerClass = NULL;

void Controller_destroy (Any *self_)
{
	DEBUG_DESTROY;
	if (!self_) {
		return;
	}
	verifyCorrectClassOrSubclass(self_,Controller);

	Controller *self = self_;
	releaseAndClear (self->tabTraversalArray);
	releaseAndClear (self->viewsArray);
	releaseAndClear (self->modelsArray);

	Object_destroy(self);
}

static void Controller_print (Controller* restrict self, FILE *outputFile) 
{ 
	if (!self) {
		return;
	}
	verifyCorrectClass(self,Controller);

	if (!outputFile) {
		outputFile = stdout;
	}
}

static void Controller_describe (Controller* restrict self, FILE *outputFile) 
{ 
	if (!self) {
		return;
	}
	verifyCorrectClass(self,Controller);

	if (!outputFile) {
		outputFile = stdout;
	}

	fprintf (outputFile, "%s", $(self, className));
}

static bool Controller_construct (Controller* restrict self)
{ 
	return false;
}

static void Controller_layout (Controller* restrict self)
{
}

static bool Controller_update (Controller* restrict self)
{ 
	return false;
}

static void Controller_dismantle (Controller* restrict self)
{ 
}

static MutableArray *Controller_views (Controller* restrict self)
{ 
	if (!self) {
		return NULL;
	}
	verifyCorrectClassOrSubclass(self,Controller);
	return self->viewsArray;
}

static MutableArray *Controller_models3D (Controller* restrict self)
{ 
	if (!self) {
		return NULL;
	}
	verifyCorrectClassOrSubclass(self,Controller);
	return self->modelsArray;
}

static long Controller_message (Controller* restrict self, long message, Any *sender, long first, long second)
{ 
	switch (message) {
	case kQuittingNowMessage:
		Log_debug_printf (__FUNCTION__, "Controller told program is exiting.");
		break;
	case kEditingDidEndMessage:
		if (self->window) {
			$(self->window, setKeyboardFocusView, NULL);
		} 
		break;
	}
	return 0;
}

static void Controller_setWindow (Controller* restrict self, Any* window)
{
	if (!self) {
		return;
	}
	verifyCorrectClassOrSubclass(self,Controller);
	if (window) {
		verifyCorrectClassOrSubclass(window,Window);
	}
	self->window = window; // Don't retain.
}

static Window *Controller_window (Controller* restrict self)
{
	if (!self) {
		return NULL;
	}
	verifyCorrectClassOrSubclass(self,Controller);
	return self->window;
}

static void Controller_removeModel3D (Controller* restrict self, Model3D *model)
{
	if (!self) {
		return;
	}
	verifyCorrectClassOrSubclass(self,Controller);
	$(self->modelsArray, remove, model);
}

static void Controller_clear3DModels (Controller* restrict self)
{
	if (!self) {
		return;
	}
	verifyCorrectClassOrSubclass(self,Controller);
	$(self->modelsArray, removeAll);
}

/* addModel3D
 * Add a 3D model to be rendered using OpenGL.
 */
static void Controller_addModel3D (Controller* restrict self, Model3D *model)
{
	if (!self || !model) {
		return;
	}
	verifyCorrectClassOrSubclass(self,Controller);
	verifyCorrectClassOrSubclass(model,Model3D);
	$(self->modelsArray, append, model);
}

/* addView
 * Note, there's isn't just one root contentView like in FrugalWidgets.
 */
static void Controller_addView (Controller* restrict self, Any* view)
{
	if (!self || !view) {
		return;
	}
	verifyCorrectClassOrSubclass(self,Controller);
	verifyCorrectClassOrSubclass(view,View);
	$(self->viewsArray, append, view);
	$(((View*)view), setController, self);
}

static void Controller_tabTraversal (Controller* restrict self, Any* sender)
{
	if (!self || !sender) {
		return;
	}
	verifyCorrectClassOrSubclass(self,Controller);
	verifyCorrectClassOrSubclass(sender,View);

	// XX Need a mutable array of the various views that 
	// will be reachable via hitting Tab, which 
	// the application should be able to set.

	View *view = sender;
	$(self->window, setKeyboardFocusView, NULL);
	$(view, message, kViewLostKeyboardFocus, self, 0, 0);
}

ControllerClass* ControllerClass_init (ControllerClass* restrict class)
{
	SET_SUPERCLASS(Object);

	SET_OVERRIDDEN_METHOD_POINTER(Controller,describe);
	SET_OVERRIDDEN_METHOD_POINTER(Controller,print);
	SET_OVERRIDDEN_METHOD_POINTER(Controller,message);

	SET_METHOD_POINTER(Controller,construct);
	SET_METHOD_POINTER(Controller,update);
	SET_METHOD_POINTER(Controller,layout);
	SET_METHOD_POINTER(Controller,dismantle);
	SET_METHOD_POINTER(Controller,setWindow);
	SET_METHOD_POINTER(Controller,window);
	SET_METHOD_POINTER(Controller,addView);
	SET_METHOD_POINTER(Controller,views);
	SET_METHOD_POINTER(Controller,addModel3D);
	SET_METHOD_POINTER(Controller,removeModel3D);
	SET_METHOD_POINTER(Controller,models3D);
	SET_METHOD_POINTER(Controller,clear3DModels);
	SET_METHOD_POINTER(Controller,tabTraversal);
	
        VALIDATE_CLASS_STRUCT(class);
	return class;
}

Controller* Controller_init (Controller* restrict self)
{
	ENSURE_CLASS_READY(Controller);

	if (self) {
		Object_init ((Object*) self);
		self->is_a = _ControllerClass;
		self->modelsArray = retain(new(MutableArray));
		self->viewsArray = retain(new(MutableArray));
	}

	return self;
}

