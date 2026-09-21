/*============================================================================
  Display, an object-oriented C OpenGL display class.
  Copyright (C) 2019, 2024, 2026 by Zack T Smith.

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

// This code is derived from my FrugalWidgets project.

#include "DateTime.h"
#include "Display.h"
#include "Window.h"
#include "Log.h"
#include "FileSystem.h"

#include <ctype.h>
#include <sys/types.h>
#include <dirent.h>
#include <unistd.h> // usleep
#include <time.h> // time

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
     #ifndef FREEGLUT
      // Original GLUT allegedly called mouse function for scrollwheel events.
      #define GLUT_SCROLLWHEEL_UP 3 
      #define GLUT_SCROLLWHEEL_DOWN 4
     #endif
   #endif
 #else
   #include <OpenGL/gl.h>
   #include <OpenGL/glu.h>
   #include <OpenGL/glext.h>
   #include <GLUT/glut.h>
 #endif
#endif

static unsigned nDisplays = 0;
static Display *displays[MAX_DISPLAYS];

DisplayClass *_DisplayClass = NULL;

void Display_destroy (Any *self_)
{
	DEBUG_DESTROY;
	if (!self_) {
		return;
	}
	verifyCorrectClassOrSubclass(self_,Display);
}
	
static unsigned Display_width (Display* restrict self)
{
	if (!self) {
		return 0;
	}
	verifyCorrectClassOrSubclass(self,Display);
	return self->width;
}

static unsigned Display_height (Display* restrict self)
{
	if (!self) {
		return 0;
	}
	verifyCorrectClassOrSubclass(self,Display);
	return self->height;
}

unsigned Display_flags (Display* self)
{
	if (!self) {
		return 0;
	}
	verifyCorrectClassOrSubclass(self,Display);
	return self->flags;
}

static void Display_setFlags (Display* restrict self, unsigned value)
{
	if (!self) {
		return;
	}
	verifyCorrectClassOrSubclass(self,Display);
	self->flags = value;
}

static void Display_describe (Display* restrict self, FILE *file)
{
	if (!self) {
		return;
	}
	verifyCorrectClassOrSubclass(self,Display);
	
	fprintf (file ?: stdout, "%s\n", $(self, className));
}

static void Display_addWindow (Display* restrict self, Any *window)
{
	if (!self || !window) {
		return;
	}
	verifyCorrectClassOrSubclass(self,Display);
	verifyCorrectClassOrSubclass(window,Window);

	if (self->nWindows < MAX_WINDOWS_PER_DISPLAY) {
		self->windows[self->nWindows] = window;
		self->nWindows++;
	}
}

static void Display_closeAllWindows (Display* restrict self)
{
	if (!self) {
		return;
	}
	verifyCorrectClassOrSubclass(self,Display);

	Log_debug_printf (__FUNCTION__, "Closing all windows (%u)", 
		(unsigned) self->nWindows);

	for (unsigned char i=0; i < self->nWindows; i++) {
		Window *window = self->windows[i];
		if (window) {
			$(window, message, kQuittingNowMessage, NULL, 0, 0);
			self->windows[i] = NULL;
		}
	}
}

static Any* Display_lookupWindowByID (Display* restrict self, long soughtIdentifier)
{
	if (!self) {
		return NULL;
	}
	verifyCorrectClassOrSubclass(self,Display);

	if (soughtIdentifier == INVALID_WINDOW_ID) {
		return NULL;
	}

	for (unsigned short i=0; i < self->nWindows; i++) {
		Window *window = self->windows[i];
		long foundIdentifier = $(window, identifier);
		if (foundIdentifier == soughtIdentifier) {
			return window;
		}
	}

	return NULL;
}

static const char *Display_name (Display* restrict self)
{
	if (!self) {
		return NULL;
	}
	verifyCorrectClass(self,Display);

	return "X/11";
}

#if defined(HAVE_GLUT)
static bool lookup_display_and_window (Display **return_dpy, Window **return_win)
{
	int identifier = glutGetWindow();
	CHECK_GL_ERROR("glutGetWindow");
	for (unsigned i = 0; i < nDisplays; i++) {
		Display *display = displays[i];
		Window *window = Display_lookupWindowByID (display, identifier);
		if (window) {
			*return_dpy = display;
			*return_win = window;
			return true;
		}
	}
	*return_dpy = NULL;
	*return_win = NULL;
	return false;
}

//---------------------------------------------------------------------------
// Name:	draw_scene
// Purpose:	Draws the entire window contents.
//---------------------------------------------------------------------------
static time_t last_time = 0;
static unsigned framesPerSecond = 0;
static uint64_t frameStartTime = 0;

static void draw_scene ()
{
	frameStartTime = DateTime_getMicrosecondTime();

	Display *display;
	Window *window;
	if (!lookup_display_and_window (&display, &window)) {
		Log_warning (__FUNCTION__, "Lookup by display & window failed.");
		return;
	}
	//==========

	if ($(window, identifier) < 0) {
		return;
	}

	$(window, redraw);
	glutPostRedisplay ();

	time_t now = time(NULL);
	if (now != last_time) {
		if (framesPerSecond && last_time) {
			static char message[64];
			snprintf (message, sizeof(message), "Frames per Second = %u... Press F4 for full-screen.", framesPerSecond);
			$(window, setTopLeftMessage, message);
		}
		last_time = now;
		framesPerSecond = 0;
	}
	framesPerSecond++;
}

static void handle_keypress (unsigned char key, int x, int y)
{
	Display *display;
	Window *window;
	if (!lookup_display_and_window (&display, &window)) {
		Log_warning (__FUNCTION__, "Lookup by display & window failed.");
		return;
	}
	//==========

	// NOTE: SHIFT-A is automatically converted to 0x41 (A).
	// NOTE: CTRL-A is automatically converted to 0x1.
	// NOTE: CTRL-Q is automatically converted to 0x11.

	if (key >= 32 && key < 127) {
		Log_debug_printf (__FUNCTION__, "Key pressed %c (0x%02x)", key, key);
	} else {
		Log_debug_printf (__FUNCTION__, "Key pressed 0x%02x", key);
	}

	if (key == 0x11 /* CTRL-Q */) {
		Display_closeAllWindows (display);
		exit(0);
	} 

	$(window, message, kKeyDownMessage, NULL, key, 0);
	$(window, message, kKeyUpMessage, NULL, key, 0);
}

static void handle_resize (int w, int h) 
{
	Display *display;
	Window *window;
	if (!lookup_display_and_window (&display, &window)) {
		Log_warning (__FUNCTION__, "Lookup by display & window failed.");
		return;
	}
	//==========

	$(window, resize, w, h);
}

static void handle_idle () 
{
	uint64_t us = DateTime_getMicrosecondTime ();
	uint64_t maximumFPS = 120;
	uint64_t usPerFrame = 1000000 / maximumFPS;
	uint64_t elapsed = us - frameStartTime;
	if (elapsed < usPerFrame) {
		uint64_t delta = usPerFrame - elapsed;
		usleep (delta);
	}
}

#if defined(FREEGLUT)
static void handle_scroll_wheel (int button, int direction, int x, int y)
{
	Display *display;
	Window *window;
	if (!lookup_display_and_window (&display, &window)) {
		Log_warning (__FUNCTION__, "Lookup by display & window failed.");
		return;
	}
	//==========
	$(window, message, kMouseScrollWheelMessage, display, direction, 0);
}
#endif

static void handle_mouse (int button, int state, int x, int y) 
{
	Display *display;
	Window *window;
	if (!lookup_display_and_window (&display, &window)) {
		Log_warning (__FUNCTION__, "Lookup by display & window failed.");
		return;
	}
	//==========

	switch (button) {
	case GLUT_LEFT_BUTTON: 
		if (state == GLUT_DOWN)
			$(window, message, kPointerDownMessage, NULL, x, y);
		else
			$(window, message, kPointerUpMessage, NULL, x, y);
		break;
#ifdef NOT_YET
	case GLUT_RIGHT_BUTTON: 
		if (state == GLUT_DOWN)
			$(window, message, kPointerRightDownMessage, NULL, x, y);
		else
			$(window, message, kPointerRightUpMessage, NULL, x, y);
		break;
	case GLUT_MIDDLE_BUTTON: 
		if (state == GLUT_DOWN)
			$(window, message, kPointerMiddleDownMessage, NULL, x, y);
		else
			$(window, message, kPointerMiddleUpMessage, NULL, x, y);
		break;
#endif
#if !defined(FREEGLUT) && !defined(__APPLE__)
	case GLUT_SCROLLWHEEL_UP:
		$(window, message, kMouseScrollWheelMessage, display, -1, 0);
		break;
	case GLUT_SCROLLWHEEL_DOWN:
		$(window, message, kMouseScrollWheelMessage, display, 1, 0);
		break;
#endif
	default:
		Log_debug (__FUNCTION__, "OTHER MOUSE BUTTON PRESSED.");
		break;
	}
}

static void handle_motion (int x, int y) 
{
	Display *display;
	Window *window;
	if (!lookup_display_and_window (&display, &window)) {
		Log_warning (__FUNCTION__, "Lookup by display & window failed.");
		return;
	}
	//==========

	if (window) {
		$(window, message, kPointerMovedMessage, NULL, x, y);
	} 
}

static void handle_special_key (int incoming_key, int mouse_x, int mouse_y) 
{
	Display *display;
	Window *window;
	if (!lookup_display_and_window (&display, &window)) {
		Log_warning (__FUNCTION__, "Lookup by display & window failed.");
		return;
	}
	//==========

	int key = 0;
	switch (incoming_key) {
		case GLUT_KEY_LEFT:	key = kKeycode_Left; break;
		case GLUT_KEY_RIGHT:	key = kKeycode_Right; break;
		case GLUT_KEY_UP:	key = kKeycode_Up; break;
		case GLUT_KEY_DOWN:	key = kKeycode_Down; break;
		case GLUT_KEY_PAGE_UP:	key = kKeycode_PageUp; break;
		case GLUT_KEY_PAGE_DOWN:key = kKeycode_PageDown; break;
		case GLUT_KEY_HOME:	key = kKeycode_Home; break;
		case GLUT_KEY_END:	key = kKeycode_End; break;
		case GLUT_KEY_F1:	key = kKeycode_F1; break;
		case GLUT_KEY_F2:	key = kKeycode_F2; break;
		case GLUT_KEY_F3:	key = kKeycode_F3; break;
		case GLUT_KEY_F4:	key = kKeycode_F4; break;
		case GLUT_KEY_F5:	key = kKeycode_F5; break;
		case GLUT_KEY_F6:	key = kKeycode_F6; break;
		case GLUT_KEY_F7:	key = kKeycode_F7; break;
		case GLUT_KEY_F8:	key = kKeycode_F8; break;
		case GLUT_KEY_F9:	key = kKeycode_F9; break;
		case GLUT_KEY_F10:	key = kKeycode_F10; break;
		case GLUT_KEY_F11:	key = kKeycode_F11; break;
		case GLUT_KEY_F12:	key = kKeycode_F12; break;
		case GLUT_KEY_INSERT:	key = kKeycode_Insert; break;
#ifndef __APPLE__
		case GLUT_KEY_SUPER_L:	key = kKeycode_Meta_L; break;
		case GLUT_KEY_SUPER_R:	key = kKeycode_Meta_R; break;
		case GLUT_KEY_ALT_L:	key = kKeycode_Alt_L; break;
		case GLUT_KEY_ALT_R:	key = kKeycode_Alt_R; break;
		case GLUT_KEY_CTRL_L:	key = kKeycode_Control_L; break;
		case GLUT_KEY_CTRL_R:	key = kKeycode_Control_R; break;
		case GLUT_KEY_SHIFT_L:	key = kKeycode_Shift_L; break;
		case GLUT_KEY_SHIFT_R:	key = kKeycode_Shift_R; break;
#endif

		default:
			printf("Other key %d\n", incoming_key);
			break;
	}

	if (key) {
		$(window, message, kKeyDownMessage, NULL, key, 0);
		$(window, message, kKeyUpMessage, NULL, key, 0);
	}
}

static void handle_visibility (int value)
{
	Display *display;
	Window *window;
	if (!lookup_display_and_window (&display, &window)) {
		Log_warning (__FUNCTION__, "Lookup by display & window failed.");
		return;
	}
	//==========
	$(window, message, kWindowVisibilityMessage, NULL, value, 0);
}

static void handle_passivemotion (int x, int y)
{
	// Mouse motion when button not pressed.
}

static void handle_entry (int value)
{
	Display *display;
	Window *window;
	if (!lookup_display_and_window (&display, &window)) {
		Log_warning (__FUNCTION__, "Lookup by display & window failed.");
		return;
	}
	//==========
	$(window, message, kWindowPointerEntryMessage, NULL, value, 0);
}

static void handle_close ()
{
	Display *display;
	Window *window;
	if (!lookup_display_and_window (&display, &window)) {
		Log_warning (__FUNCTION__, "Lookup by display & window failed.");
		return;
	}
	//==========
	$(window, message, kQuittingNowMessage, NULL, 0, 0);
}
#endif

static unsigned Display_brightness (Display* restrict self)
{
#if defined(__linux__) 
	// For Intel CPU with built-in graphics: read /sys/class/backlight/acpi_video0/brightness
	unsigned value = 0;
	if (FileSystem_read_unsigned_from_file("/sys/class/backlight/acpi_video0/brightness", &value)) {
		return value;
	}
#endif
	return 0;
}

Display* Display_init (Display* restrict self)
{
	ENSURE_CLASS_READY(Display);

	if (nDisplays >= MAX_DISPLAYS) {
		release (self);
		return NULL;
	}

	if (self) {
		Object_init ((Object*)self);
		self->is_a = _DisplayClass;

		self->nWindows = 0;

#if defined(HAVE_GLUT)
		int argc = 0;
		static const char *argv[] = { 
			"abc123" 
		};
		glutInit (&argc, (char**) argv);
		glutInitDisplayMode (GLUT_DOUBLE | GLUT_RGBA | GLUT_DEPTH);
#endif

		Object_init ((Object*)self);
		self->is_a = _DisplayClass;
#if defined(HAVE_GLUT)
		self->width = glutGet (GLUT_SCREEN_WIDTH);
		self->height = glutGet (GLUT_SCREEN_HEIGHT);
		self->depth = 32;
#endif

        	Log_debug_printf (__FUNCTION__, "Display size=%ux%u", self->width, self->height);
	}

	displays[nDisplays++] = self;

	return self;
}

static bool Display_canAddWindows (Display* restrict self)
{
	if (!self) {
		return false;
	}
	verifyCorrectClass(self,Display);
	return self->nWindows < MAX_WINDOWS_PER_DISPLAY;
}

static const char *Display_driverType (Display* restrict self)
{
	if (!self) {
		return NULL;
	}
	verifyCorrectClass(self,Display);

#if defined(HAVE_OPENGL) 
	return "OpenGL";
#else
	// NOTE: If another display type will be supported,
	// the Display class will have to be subclassed
	// into GLDisplay (as I did in FrugalWidgets) 
	// and whatever other.
	return NULL;
#endif
}

static const char *Display_driverRenderer (Display* restrict self)
{
	if (!self) {
		return NULL;
	}
	verifyCorrectClass(self,Display);

#if defined(HAVE_OPENGL) && !defined(__APPLE__)
	return (const char*) glGetString(GL_RENDERER);
#else
	return NULL;
#endif
}

static const char *Display_driverVendor (Display* restrict self)
{
	if (!self) {
		return NULL;
	}
	verifyCorrectClass(self,Display);

#if defined(HAVE_OPENGL) && !defined(__APPLE__)
	return (const char*) glGetString(GL_VENDOR);
#else
	return NULL;
#endif
}

static const char *Display_driverVersion (Display* restrict self)
{
	if (!self) {
		return NULL;
	}
	verifyCorrectClass(self,Display);

#if defined(HAVE_OPENGL) && !defined(__APPLE__)
	return (const char*) glGetString(GL_VERSION);
#else
	return NULL;
#endif
}

static void Display_mainLoop (Display* restrict self)
{
	if (!self) {
		return;
	}
	verifyCorrectClass(self,Display);

#if defined(HAVE_GLUT)
        glutDisplayFunc (&draw_scene);
        glutKeyboardFunc (&handle_keypress);
        glutSpecialFunc (&handle_special_key);
        glutReshapeFunc (&handle_resize);
        glutIdleFunc (&handle_idle);
        glutMouseFunc (&handle_mouse);
        glutMotionFunc (&handle_motion);
	glutVisibilityFunc (&handle_visibility);
	glutPassiveMotionFunc (&handle_passivemotion);
	glutEntryFunc (&handle_entry);
#if defined(__APPLE__)
	glutWMCloseFunc (&handle_close);
#else
	glutCloseFunc (&handle_close);
#endif
#if defined(FREEGLUT)
	glutMouseWheelFunc (&handle_scroll_wheel);
#endif
        glutMainLoop ();
#endif
}

DisplayClass* DisplayClass_init (DisplayClass* restrict class)
{
	SET_SUPERCLASS(Object);

	SET_OVERRIDDEN_METHOD_POINTER(Display,describe);

        SET_METHOD_POINTER(Display,width);
        SET_METHOD_POINTER(Display,height);
	SET_METHOD_POINTER(Display,name);
	SET_METHOD_POINTER(Display,flags);
	SET_METHOD_POINTER(Display,setFlags);
	SET_METHOD_POINTER(Display,addWindow);
	SET_METHOD_POINTER(Display,brightness);
	SET_METHOD_POINTER(Display,lookupWindowByID);
	SET_METHOD_POINTER(Display,mainLoop);
	SET_METHOD_POINTER(Display,canAddWindows);
	SET_METHOD_POINTER(Display,closeAllWindows);
	SET_METHOD_POINTER(Display,driverType);
	SET_METHOD_POINTER(Display,driverVersion);
	SET_METHOD_POINTER(Display,driverVendor);
	SET_METHOD_POINTER(Display,driverRenderer);

	VALIDATE_CLASS_STRUCT(class);
	return class;
}

