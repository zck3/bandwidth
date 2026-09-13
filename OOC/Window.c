/*============================================================================
  Window, an object-oriented C window class.
  Copyright (C) 2019, 2022, 2024 by Zack T Smith.

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

// This class is inspired by my FrugalWidgets project's Window.cpp class.

#include "Window.h"
#include "Controller.h"
#include "View.h"
#include "Log.h"

#include <string.h>

#if defined(HAVE_OPENGL) && defined(HAVE_GLUT)
 #define GL_SILENCE_DEPRECATION
 #define GL_GLEXT_PROTOTYPES
 #ifndef __APPLE__
   #include <GL/gl.h>
   #include <GL/glu.h>
   #include <GL/glext.h>
   #include <GL/glut.h>
   #include <GL/freeglut_ext.h>
   #ifndef FREEGLUT
    // Original GLUT allegedly called mouse function for scrollwheel events.
    #define GLUT_SCROLLWHEEL_UP 3 
    #define GLUT_SCROLLWHEEL_DOWN 4
   #endif
 #else
   #include <OpenGL/gl.h>
   #include <OpenGL/glu.h>
   #include <OpenGL/glext.h>
   #include <GLUT/glut.h>
   //#include <GLUT/glut_ext.h>
 #endif

static GLint lightNames[MAX_LIGHTS] = {
	GL_LIGHT0,
	GL_LIGHT1,
	GL_LIGHT2,
	GL_LIGHT3,
	GL_LIGHT4,
	GL_LIGHT5,
	GL_LIGHT6,
	GL_LIGHT7,
};
#endif

WindowClass *_WindowClass = NULL;

Window* Window_init (Window* restrict self)
{
	ENSURE_CLASS_READY(Window);

	if (self) {
		Object_init ((Object*) self);
		self->is_a = _WindowClass;
		self->identifier = INVALID_WINDOW_ID;
		self->cameraVector.x = 0.f;
		self->cameraVector.y = 0.f;
		self->cameraVector.z = 2.f;
		self->focusVector.x = 0.f;
		self->focusVector.y = 0.f;
		self->focusVector.z = 0.f;
		self->upVector.x = 0.f;
		self->upVector.y = 1.f;
		self->upVector.z = 0.f;

		self->useLight[0] = true;
		self->lightAmbientColors[0] = 0x808080;
		self->lightDiffuseColors[0] = RGB_WHITE;
		self->lightSpecularColors[0] = RGB_WHITE;
		self->lightPositions[0] = Vector_new(0,0,0);
	}

	return self;
}

void Window_destroy (Any *self_)
{
        DEBUG_DESTROY;

	if (!self_) {
		return;
	}
	verifyCorrectClass(self_,Window);

	Window *self = self_;

	releaseAndClear (self->controller);

	if (self->identifier >= 0) {
#if defined(HAVE_OPENGL) && defined(HAVE_GLUT)
		glutDestroyWindow (self->identifier);
#endif
	}

	Object_destroy (self);
}

static void Window_print (Window* restrict self, FILE *outputFile) 
{ 
	if (!self) {
		return;
	}
	verifyCorrectClass(self,Window);

	if (!outputFile) {
		outputFile = stdout;
	}

	fprintf (outputFile, "%s(#%ld %ux%u %d,%d; title \"%s\")", $(self, className), self->identifier, 
		self->width, 
		self->height, 
		self->x, 
		self->y,
		self->title);
}

static void Window_describe (Window* restrict self, FILE *outputFile) 
{ 
	if (!self) {
		return;
	}
	verifyCorrectClass(self,Window);

	if (!outputFile) {
		outputFile = stdout;
	}

	fprintf (outputFile, "%s", $(self, className));
}

static long Window_identifier (Window* restrict self)
{ 
	if (!self) {
		return INVALID_WINDOW_ID;
	}
	verifyCorrectClass(self,Window);
	return self->identifier;
}

static void Window_setIdentifier (Window* restrict self, long identifier)
{ 
	if (!self) {
		return;
	}
	verifyCorrectClass(self,Window);
	self->identifier = identifier;
}

static void Window_move (Window* restrict self, int x, int y)
{
	if (!self) {
		return;
	}
	verifyCorrectClass(self,Window);

	self->needsRedraw = true;
	self->x = x;
	self->y = y;
#if defined(HAVE_OPENGL) && defined(HAVE_GLUT)
	glutSetWindow (self->identifier);
	glutPositionWindow (x, y);
#endif
}

static void Window_resize (Window* restrict self, unsigned width, unsigned height)
{ 
	if (!self) {
		return;
	}
	verifyCorrectClass(self,Window);

	if (!width || !height) {
		return;
	}

	self->needsRedraw = true;
	self->width = width;
	self->height = height;

	if (self->controller) {
		$(((Controller*)self->controller), layout);
	}
}

#if defined(HAVE_OPENGL) && defined(HAVE_GLUT)
static void Window_initGLForModel (Window* restrict self)
{
	int width = self->width;
	int height = self->height;

	glViewport (0, 0, width, height);
	glMatrixMode (GL_PROJECTION); 
	glLoadIdentity (); 
	double minZPlane = 0.01;
	double maxZPlane = 1000.0;
	double aspectRatio = (float) width / (float) height;
	gluPerspective (45.0f, aspectRatio, minZPlane, maxZPlane);

	glMatrixMode (GL_MODELVIEW);
	glLoadIdentity ();

	gluLookAt (self->cameraVector.x,
		self->cameraVector.y,
		self->cameraVector.z,
		self->focusVector.x,
		self->focusVector.y,
		self->focusVector.z,
		self->upVector.x,
		self->upVector.y,
		self->upVector.z);
}
#endif

static void Window_moveCamera (Window* restrict self, Vector vector)
{
	if (!self) {
		return;
	}
	verifyCorrectClass(self,Window);

	self->cameraVector.x = vector.x;
	self->cameraVector.y = vector.y;
	self->cameraVector.z = vector.z;
}

static unsigned Window_width (Window* restrict self)
{
	if (!self) {
		return 0;
	}
	verifyCorrectClass(self,Window);

	return self->width;
}

static unsigned Window_height (Window* restrict self)
{
	if (!self) {
		return 0;
	}
	verifyCorrectClass(self,Window);

	return self->height;
}

static Rect Window_redraw (Window* restrict self)
{
	Rect zeroRect = Rect_zero();

	if (!self || self->identifier < 0) { 
		return zeroRect;
	}
	verifyCorrectClass(self,Window);

	if (self->controller) {
		Controller *controller = self->controller;
		$(controller, layout); // Layout 2D widgets, if applicable.
		$(controller, update); // Revise 3D scene, if applicable.
	}

#if defined(HAVE_OPENGL) && defined(HAVE_GLUT)
	long width = self->width;
	long height = self->height;

	glutSetWindow (self->identifier);

	Window_initGLForModel (self);

	glEnable (GL_NORMALIZE);
	glEnable (GL_LIGHTING);
	glEnable (GL_DEPTH_TEST);
	glLightModeli (GL_LIGHT_MODEL_LOCAL_VIEWER, 1);
	glLightModeli (GL_LIGHT_MODEL_TWO_SIDE, GL_FALSE);
	float overallAmbient[] = { 0.15, 0.15, 0.15, 1 };
	glLightModelfv (GL_LIGHT_MODEL_AMBIENT, overallAmbient);

	RGBA background_red = (self->backgroundColor >> 16) & 0xff;
	RGBA background_green = (self->backgroundColor >> 8) & 0xff;
	RGBA background_blue = self->backgroundColor & 0xff;
	glClearColor (background_red / 255.f, background_green / 255.f, background_blue / 255.f, 1);
	glClear (GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

	GLfloat zero[] = { 0, 0, 0, 0 };

	for (int i=0; i < MAX_LIGHTS; i++) {
		float values[4];
		RGBA color = self->lightDiffuseColors[i];
		values[0] = ((color >> 16) & 0xff) / 255.f;
		values[1] = ((color >> 8) & 0xff) / 255.f;
		values[2] = (color & 0xff) / 255.f;
		values[3] = 1.f - (((color >> 24) & 0xff) / 255.f);
		glLightfv (lightNames[i], GL_DIFFUSE, values); 

		color = self->lightAmbientColors[i];
		values[0] = ((color >> 16) & 0xff) / 255.f;
		values[1] = ((color >> 8) & 0xff) / 255.f;
		values[2] = (color & 0xff) / 255.f;
		values[3] = 1.f - (((color >> 24) & 0xff) / 255.f);
		glLightfv (lightNames[i], GL_AMBIENT, values); 

		if (self->useLight[i]) {
			glEnable (lightNames[i]);
		} else {
			glDisable (lightNames[i]);
		}
		
		glPushMatrix();
		glTranslated(self->lightPositions[i].x, self->lightPositions[i].y, self->lightPositions[i].z);
		glLightfv(GL_LIGHT1, GL_POSITION, zero);
		glPopMatrix();
	}

	Controller *controller = self->controller;
	if (controller) {
		MutableArray *modelsArray = $(controller, models3D);
		if (modelsArray) {
			unsigned nModels = $(modelsArray, count);
			if (nModels) {
				glViewport (0, 0, width, height); 

				glPushMatrix ();
				for (unsigned i=0; i < nModels; i++) {
					Model3D *model = $(modelsArray, at, i);
					RGBA color = model->color;

					for (unsigned j=0; j < model->nTriangles; j++) {
						if (model->color == RGB_TEST_PATTERN) {
							color = j & 1 ? RGB_GREEN: RGB_RED;
						}

						RGBA red = (color >> 16) & 0xff;
						RGBA green = (color >> 8) & 0xff;
						RGBA blue = color & 0xff;
						GLfloat colorvalues[] = { red / 255.f, green / 255.f, blue / 255.f, 1.f};

						glBegin (GL_TRIANGLES);

						Triangle triangle = model->triangles[j];

						color = triangle.color_a;
						red = (color >> 16) & 0xff;
						green = (color >> 8) & 0xff;
						blue = color & 0xff;
						colorvalues[0] = red / 255.f;
						colorvalues[1] = green / 255.f;
						colorvalues[2] = blue / 255.f;
						colorvalues[3] = 1.f;
						glMaterialfv (GL_FRONT, GL_AMBIENT_AND_DIFFUSE, colorvalues); 

						glNormal3f (triangle.normal.x, triangle.normal.y, triangle.normal.z);
						glVertex3f (triangle.a.x, triangle.a.y, triangle.a.z);

						color = triangle.color_b;
						red = (color >> 16) & 0xff;
						green = (color >> 8) & 0xff;
						blue = color & 0xff;
						colorvalues[0] = red / 255.f;
						colorvalues[1] = green / 255.f;
						colorvalues[2] = blue / 255.f;
						colorvalues[3] = 1.f;
						glMaterialfv (GL_FRONT, GL_AMBIENT_AND_DIFFUSE, colorvalues); 

						glVertex3f (triangle.b.x, triangle.b.y, triangle.b.z);

						color = triangle.color_c;
						red = (color >> 16) & 0xff;
						green = (color >> 8) & 0xff;
						blue = color & 0xff;
						colorvalues[0] = red / 255.f;
						colorvalues[1] = green / 255.f;
						colorvalues[2] = blue / 255.f;
						colorvalues[3] = 1.f;
						glMaterialfv (GL_FRONT, GL_AMBIENT_AND_DIFFUSE, colorvalues); 

						glVertex3f (triangle.c.x, triangle.c.y, triangle.c.z);
						glEnd ();
					}
				}
				glPopMatrix ();
			}

			glDisable(GL_DEPTH_TEST);
			glDisable(GL_CULL_FACE);
			glDisable(GL_BLEND);
			glDisable(GL_LIGHTING);
			glEnable (GL_TEXTURE_2D);

			glMatrixMode (GL_PROJECTION);
			glLoadIdentity ();
			glOrtho (0, width, 0, height, -1, 1);
			glMatrixMode (GL_MODELVIEW);
			glLoadIdentity ();
		}

		MutableArray *viewsArray = $(controller, views);
		if (viewsArray) {
			unsigned nViews = $(viewsArray, count);
			if (nViews) {
				glPushMatrix ();

				float white[4] = {1,1,1,1};
				glColor3fv (white);

				for (unsigned i=0; i < nViews; i++) {
					View *view = $(viewsArray, at, i);
					if ($(view, hidden)) {
						continue;
					}

					unsigned textureID = view->textureID;
					MutableImage *drawable = $(view, drawable);
					if (!drawable) {
						continue;
					}
					RGBA *pixels = (RGBA*) $(drawable, pixels);
					if (!pixels || textureID == INVALID_TEXTURE_ID) {
						continue;
					}
					if ($(view, needsRedraw)) {
						//printf ("NEEDS REDRAW: %s\n", $(view, className));
						$(view, redraw);
					}

					GLint x = view->rect.origin.x;
					GLint y = view->rect.origin.y;
					GLint w = view->rect.size.width;
					GLint h = view->rect.size.height;

					glBindTexture (GL_TEXTURE_2D, textureID);
					CHECK_GL_ERROR("glBindTexture");

					glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
					glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
					glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP);	
					glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP);

					glTexImage2D (GL_TEXTURE_2D, 0, GL_RGBA, w, h, 0, GL_BGRA, GL_UNSIGNED_BYTE, pixels);
					glBegin (GL_QUADS);
					float invertedY = height - h - y;
					glNormal3f (0,0,1);
					glTexCoord2d (0.0, 1.0); 
					glVertex3d (x, invertedY, 0);
					glNormal3f (0,0,1);
					glTexCoord2d (1.0, 1.0); 
					glVertex3d (x+w, invertedY, 0);
					glNormal3f (0,0,1);
					glTexCoord2d (1.0, 0.0); 
					glVertex3d (x+w, invertedY+h, 0);
					glNormal3f (0,0,1);
					glTexCoord2d (0.0, 0.0);
					glVertex3d (x, invertedY+h, 0);
					glEnd ();

					// RULE: Since I'm using glScissor, there'll be only 2 levels of Views,
					// i.e. subviews cannot have their own subviews.
					// RULE: Subviews' coordinates are relative to the parent Views'.
					// This policy is a diversion from FrugalWidgets which allowed unlimited nested subviews,
					// and I may reverse it. TODO
					// 
					if ($(view, hasSubviews)) {
						glEnable (GL_SCISSOR_TEST);
						glScissor (x, invertedY, w, h);
						//Log_debug_printf (__FUNCTION__, "OBJECT OF TYPE %s HAS SUBVIEWS", $(view, className));
						MutableArray *subviews = view->subviews;
						int total = $(subviews, count);
						for (int j=0; j < total; j++) {
							View *subview = $(subviews, at, j);
							GLint sx = x + subview->rect.origin.x;
							GLint sy = y + subview->rect.origin.y;
							GLint sw = subview->rect.size.width;
							GLint sh = subview->rect.size.height;
							GLint subviewInvertedY = height - sh - sy;
							/*
							   Inverted OpenGL coordinates are tedious.
							   +-Win------------+ 0 9
							   | +-View-----+   | 1 8
							   | |  +-Sub-+ |   | 2 7
							   | |  |     | |   | 4 6
							   | |  +-----+ |   | 5 4
							   | |          |   | 6 3
							   | +----------+   | 7 2
							   |                | 8 1
							   +----------------+ 9 0
							 */
							unsigned subviewTextureID = subview->textureID;
							MutableImage *drawable = $(subview, drawable);
							if (!drawable) {
								continue;
							}
							RGBA *subviewPixels = (RGBA*) $(drawable, pixels);
							if (!subviewPixels || subviewTextureID == INVALID_TEXTURE_ID) {
								continue;
							}
							if ($(subview, needsRedraw)) {
								Log_debug_printf (__FUNCTION__, "SUBVIEW NEEDS REDRAW: %s", $(subview, className));
								$(subview, redraw);
							}

							glBindTexture (GL_TEXTURE_2D, subviewTextureID);
							CHECK_GL_ERROR("glBindTexture");

							glTexImage2D (GL_TEXTURE_2D, 0, GL_RGBA, sw, sh, 0, GL_BGRA, GL_UNSIGNED_BYTE, subviewPixels);
							glBegin (GL_QUADS);
							glNormal3f (0,0,1);
							glTexCoord2d (0.0, 1.0); 
							glVertex3d (sx, subviewInvertedY, 0);
							glNormal3f (0,0,1);
							glTexCoord2d (1.0, 1.0); 
							glVertex3d (sx+sw, subviewInvertedY, 0);
							glNormal3f (0,0,1);
							glTexCoord2d (1.0, 0.0); 
							glVertex3d (sx+sw, subviewInvertedY+sh, 0);
							glNormal3f (0,0,1);
							glTexCoord2d (0.0, 0.0);
							glVertex3d (sx, subviewInvertedY+sh, 0);
							glEnd ();
							glBindTexture (GL_TEXTURE_2D, 0);
						}

						glDisable (GL_SCISSOR_TEST);
					}

					glBindTexture (GL_TEXTURE_2D, 0);
				}

				glPopMatrix ();
			}
		}
	}

	char *message = (char*) self->topLeftMessage;
	if (message) {
		glDisable (GL_LIGHTING);
		glViewport (0, 0, width, height); 
		glMatrixMode (GL_PROJECTION);
		glLoadIdentity ();
		glOrtho (0, width, 0, height, -1, 1);
		glMatrixMode (GL_MODELVIEW);
		glLoadIdentity ();
		glColor3f (1, 1, 1);
		glRasterPos2f(5,height-20);
		while (*message) {
			glutBitmapCharacter(GLUT_BITMAP_HELVETICA_18, *message);
			message++;
		}
	}

	glFlush ();
	CHECK_GL_ERROR("glFlush");

	glutSwapBuffers ();
	CHECK_GL_ERROR("glutSwapBuffers");
#endif

	self->needsRedraw = false;

	return zeroRect;
}

#ifdef UNUSED
static void Window_selfWasShown (Window* restrict self) {
}

static void Window_selfWasHidden (Window* restrict self) {
}

static void Window_didResize (Window* restrict self) {
}

static void Window_pointerDidEnter (Window* restrict self) {
}

static void Window_pointerDidLeave (Window* restrict self) {
}
#endif

static void Window_setKeyboardFocusView (Window* restrict self, Any* view_)
{
	if (!self) {
		return;
	}
	verifyCorrectClass(self,Window);
	if (view_) {
		verifyCorrectClassOrSubclass(view_,View);
	}
	View *view = view_;
	View *previousView = self->keyboardFocusView;
	if (previousView) {
		release(previousView);
	}
	if (view) {
		self->keyboardFocusView = retain(view);
	} else {
		self->keyboardFocusView = NULL;
	}
}

static Any *Window_keyboardFocusView (Window* restrict self)
{
	if (!self) {
		return NULL;
	}
	verifyCorrectClass(self,Window);
	return self->keyboardFocusView;
}

static bool Window_keyDown (Window* restrict self, unsigned keycode) 
{
	View *view = self->keyboardFocusView;
	if (view) {
		$(view, message, kKeyDownMessage, self, keycode, 0);
	}
	return false;
}

static void Window_setFullScreen (Window* restrict self, bool fullscreen)
{
	if (!self) {
		return;
	}
	verifyCorrectClass(self,Window);

#if defined(HAVE_GLUT)
	int winID = self->identifier;
	glutSetWindow (winID);
	if (fullscreen) {
		// Save the window location & size to allow it to be
		// restored later.
		//
		self->x = glutGet (GLUT_WINDOW_X);
		self->y = glutGet (GLUT_WINDOW_Y);
		self->width = glutGet (GLUT_WINDOW_WIDTH);
		self->height = glutGet (GLUT_WINDOW_HEIGHT);
		glutFullScreen ();
		self->isFullScreen = true;
		Log_debug(__FUNCTION__, "Window made full-screen.");
	} else {
#if defined(FREEGLUT)
		glutLeaveFullScreen ();
#else
		glutReshapeWindow (640, 480);
		glutPositionWindow (100, 100);
#endif
		self->isFullScreen = false;
		Log_debug(__FUNCTION__, "Window is no longer full-screen.");
	}
#endif
}

static bool Window_keyUp (Window* restrict self, unsigned keycode) 
{
	// Toggle full-screen with F4.
	if (keycode == kKeycode_F4) {
		Window_setFullScreen (self, !self->isFullScreen);
		return true;
	}

	View *view = self->keyboardFocusView;
	if (view) {
		return $(view, message, kKeyUpMessage, self, keycode, 0);
	}
	return false;
}

static View *Window_viewAtLocation (Window* restrict self, int x, int y, int *returnX, int *returnY)
{
	if (!self) {
		return NULL;
	}
	verifyCorrectClass(self,Window);

	Controller *controller = self->controller;
	if (controller) {
		MutableArray *viewsArray = $(controller, views);
		if (viewsArray) {
			Point point = Point_new(x,y);
			unsigned nViews = $(viewsArray, count);
			for (unsigned i=0; i < nViews; i++) {
				View *view = $(viewsArray, at, i);
				if (!$(view, hidden)) {
					if (Rect_containsPoint (view->rect, point)) {
						if (returnX && returnY) {
							*returnX = x - view->rect.origin.x;
							*returnY = y - view->rect.origin.y;
						}
						return view;
					}
				}
			}
		}
	}

	return NULL;
}

static long Window_message (Window* restrict self, long message, Any *sender, long first, long second)
{
	if (!self) {
		return -1;
	}
	verifyCorrectClass(self,Window);
	if (sender) {
		verifyCorrectClassOrSubclass(sender,Object); // XX inefficient
	}

	switch (message) {
	case kWindowPointerEntryMessage: {
		bool entered = first != 0;
		if (entered != self->pointerInside) {
			self->pointerInside = entered;
			Log_debug_printf (__FUNCTION__, "Pointer has %s", entered ? "entered" : "exited");
		}
	 } break;
	case kWindowVisibilityMessage: {
		//Log_debug_printf (__FUNCTION__, "Window is %s", first ? "visible" : "hidden");
	 } break;
	case kKeyDownMessage: 
		Window_keyDown (self, first);
		break;
	case kKeyUpMessage: 
		Window_keyUp (self, first);
		break;
	case kQuittingNowMessage:
		Log_debug_printf (__FUNCTION__, "Window told that the program is exiting.");
		if (self->controller) {
			Controller *controllerToRemove = self->controller;
			$(controllerToRemove, dismantle);
			$(controllerToRemove, setWindow, NULL);
			release(controllerToRemove);
			self->controller = NULL;
		}
		break;
	case kPointerDownMessage: {
		self->pointerX = first;
		self->pointerY = second;

		int relativeX = 0, relativeY = 0;
		View *view = Window_viewAtLocation (self, first, second, &relativeX, &relativeY);

		// Current focused view, if different, loses pointer focus.
		if (view != self->pointerFocusView) {
			View *previousView = self->pointerFocusView;
			if (previousView) {
				$(previousView, message, kViewLostPointerFocus, self, 0, 0);
			}
			self->pointerFocusView = NULL;
		}

		// Current focused view, if different, loses keyboard focus.
		if (view != self->keyboardFocusView) {
			View *previousView = self->keyboardFocusView;
			if (previousView) {
				$(previousView, message, kViewLostKeyboardFocus, self, 0, 0);
				release(previousView);
			}
			self->keyboardFocusView = NULL;
// HERE Keyboard focus is unfinished.
		}

		if (view) {
			$(view, message, kPointerDownMessage, self, relativeX, relativeY);
			self->pointerFocusView = retain(view);
			self->keyboardFocusView = retain(view);
			$(view, message, kViewGainedPointerFocus, self, 0, 0);
			$(view, message, kViewGainedKeyboardFocus, self, 0, 0);
		} else {
			// Click within the 3D model...
			// 
			// TODO: Identify clicked 3D object.
		}
		} break;
	case kPointerUpMessage: {
		View *view = self->pointerFocusView;
		if (view) {
			int relativeX = first - view->rect.origin.x;
			int relativeY = second - view->rect.origin.y;
			$(view, message, kPointerUpMessage, self, relativeX, relativeY);
		
			int dx = first - self->pointerX;
			int dy = second - self->pointerY;
			float distance = sqrtf (dx * dx + dy * dy);
			if (distance < 5) {
				$(view, message, kClickedMessage, self, 0, 0);
			}
		}
		} break;
	case kPointerMovedMessage: {
		View *view = self->pointerFocusView;
		if (view) {
			int relativeX = first - view->rect.origin.x;
			int relativeY = second - view->rect.origin.y;
			$(view, message, kPointerMovedMessage, self, relativeX, relativeY);
		}
		} break;
	case kMouseScrollWheelMessage: {
		Log_debug_printf (__FUNCTION__, "SCROLL WHEEL direction=%ld", first);
		View *view = self->pointerFocusView;
		if (view) {
			$(view, message, kMouseScrollWheelMessage, self, first, 0);
		}
		break;
	}
	}

	return 0;
}

static void Window_setLightEnabled (Window* restrict self, unsigned which, bool enabled)
{
	if (!self || which >= MAX_LIGHTS) {
		return;
	}
	verifyCorrectClass(self,Window);

	self->useLight[which] = enabled;
	self->needsRedraw = true;
}

static void Window_setLightColors (Window* restrict self, unsigned which, RGBA ambient, RGBA diffuse, RGBA specular)
{
	if (!self || which >= MAX_LIGHTS) {
		return;
	}
	verifyCorrectClass(self,Window);

	self->lightAmbientColors[which] = ambient;
	self->lightDiffuseColors[which] = diffuse;
	self->lightSpecularColors[which] = specular;
	self->needsRedraw = true;
}

static void Window_setLightPosition (Window* restrict self, unsigned which, Vector position)
{
	if (!self || which >= MAX_LIGHTS) {
		return;
	}
	verifyCorrectClass(self,Window);

	self->lightPositions[which] = position;
	self->needsRedraw = true;
}

static void Window_setTopLeftMessage (Window* restrict self, const char* message)
{
	if (!self) {
		return;
	}
	verifyCorrectClass(self,Window);

	self->topLeftMessage = message; // Can be NULL.
	self->needsRedraw = true;
}

static void Window_setController (Window* restrict self, Any* controller_)
{
	if (!self) {
		return;
	}
	verifyCorrectClass(self,Window);

	if (self->controller) {
		Controller *controllerToRemove = self->controller;
		$(controllerToRemove, dismantle);
		$(controllerToRemove, setWindow, NULL);
		release(controllerToRemove);
	}

	Controller *controller = controller_;
	if (controller) {
		verifyCorrectClassOrSubclass(controller_,Controller);
		self->controller = retain(controller);
		$(controller, setWindow, self);
		$(controller, construct);
	} 
}

WindowClass* WindowClass_init (WindowClass *class)
{
	SET_SUPERCLASS(Object);

	SET_OVERRIDDEN_METHOD_POINTER(Window,describe);
	SET_OVERRIDDEN_METHOD_POINTER(Window,destroy);
	SET_OVERRIDDEN_METHOD_POINTER(Window,print);
	SET_OVERRIDDEN_METHOD_POINTER(Window,message);

	SET_METHOD_POINTER(Window,identifier);
	SET_METHOD_POINTER(Window,setIdentifier);
	SET_METHOD_POINTER(Window,resize);
	SET_METHOD_POINTER(Window,move);
	SET_METHOD_POINTER(Window,width);
	SET_METHOD_POINTER(Window,height);
	SET_METHOD_POINTER(Window,redraw);
	SET_METHOD_POINTER(Window,setTopLeftMessage);
	SET_METHOD_POINTER(Window,moveCamera);
	SET_METHOD_POINTER(Window,setLightEnabled);
	SET_METHOD_POINTER(Window,setLightColors);
	SET_METHOD_POINTER(Window,setLightPosition);
	SET_METHOD_POINTER(Window,setController);
	SET_METHOD_POINTER(Window,setKeyboardFocusView);
	SET_METHOD_POINTER(Window,keyboardFocusView);
	SET_METHOD_POINTER(Window,setFullScreen);

        VALIDATE_CLASS_STRUCT(class);
	return class;
}

static Window *Window_initWith (Window* restrict self,
			Display *display,
			const char *title,
			unsigned width,
			unsigned height,
			int x,
			int y)
{
	if (!self) { 
		return NULL;
	}
	if (!display || !width || !height) {
		release (self);
		return NULL;
	}
	verifyCorrectClassOrSubclass(display,Display);
	if (!$(display, canAddWindows)) {
		release (self);
		return NULL;
	}

	Window_init (self);

	self->x = x;
	self->y = y;
	self->width = width;
	self->height = height;
	self->title = strdup(title ?: ""); // XX use String
	self->backgroundColor = RGB_STEELBLUE;

	// RULE: Don't retain because Display retains Window.
	self->display = display; 

	char temp [256];
	snprintf (temp, sizeof(temp)-1, "Window (title \"%s\") is %ux%u @ %d,%d", self->title, width, height, x, y);
	Log_debug (__FUNCTION__, temp);

#if defined(HAVE_OPENGL) && defined(HAVE_GLUT)
	glutInitWindowSize (width, height);
	glutInitWindowPosition (x, y);
	int glWindow = glutCreateWindow (self->title);

        int actualWidth = glutGet (GLUT_WINDOW_WIDTH);
        int actualHeight = glutGet (GLUT_WINDOW_HEIGHT);
        
	if (actualWidth > 0 && actualHeight > 0 && (actualWidth != width || actualHeight != height)) {
		// NOTE: In X Windows, GLUT has a bug that prevents a window 
		// which the window manager forced to be smaller to display correctly 
		// until after the idle call.
		x = glutGet (GLUT_WINDOW_X);
		y = glutGet (GLUT_WINDOW_Y);
		width = actualWidth;
		height = actualHeight;

		Log_debug_printf (__FUNCTION__, "Window manager resized window to %ux%u", width, height);

        	//wasResizedAtOutset = true;
	}
	self->identifier = glWindow;
#endif

	$(display, addWindow, self);

	Window_resize (self, width, height);

	return self;
}

Window *Window_newWith (Display *display, const char *title, unsigned width, unsigned height, int x, int y)
{
	ENSURE_CLASS_READY(Window);

	if (!display || !width || !height) {
		return NULL;
	}

	Window *self = allocate(Window);
	return Window_initWith (self, display, title, width, height, x, y);
}

