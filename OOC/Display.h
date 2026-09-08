/*============================================================================
  Display, an object-oriented C OpenGL display class.
  Copyright (C) 2019, 2024 by Zack T Smith.

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

#ifndef _OOC_DISPLAY_H
#define _OOC_DISPLAY_H

#include "Object.h"
#include "GraphicsTypes.h"
#include "Log.h"

#define kDisplayShiftFlag	(1)
#define kDisplayAltFlag 	(2)
#define kDisplayControlFlag 	(4)

#define DECLARE_DISPLAY_METHODS(TYPE_POINTER) \
	unsigned (*width) (TYPE_POINTER); \
	unsigned (*height) (TYPE_POINTER); \
	const char *(*name) (TYPE_POINTER); \
	unsigned (*flags) (TYPE_POINTER); \
	void (*setFlags) (TYPE_POINTER, unsigned); \
	void (*addWindow) (TYPE_POINTER, Any*); \
	bool (*canAddWindows) (TYPE_POINTER); \
	Any* (*lookupWindowByID) (TYPE_POINTER, long); \
	unsigned (*brightness) (TYPE_POINTER); \
	void (*mainLoop) (TYPE_POINTER); \
	void (*closeAllWindows) (TYPE_POINTER); \
	const char *(*driverType) (TYPE_POINTER); \
	const char *(*driverVersion) (TYPE_POINTER); \
	const char *(*driverVendor) (TYPE_POINTER); \
	const char *(*driverRenderer) (TYPE_POINTER);

struct display;

typedef struct displayclass {
	DECLARE_OBJECT_CLASS_VARS
	DECLARE_OBJECT_METHODS(struct display*)
	DECLARE_DISPLAY_METHODS(struct display*)
} DisplayClass;

extern DisplayClass *_DisplayClass;
extern DisplayClass* DisplayClass_init (DisplayClass*);

#define MAX_DISPLAYS (4U)
#define MAX_WINDOWS_PER_DISPLAY (32U)

#define WINDOW_FLAG_FULL_SCREEN (1U)

#define DECLARE_DISPLAY_INSTANCE_VARS(TYPE_POINTER) \
	unsigned short width, height; \
	unsigned char depth; \
	unsigned char flags; \
	unsigned char nWindows; \
	Any *windows[MAX_WINDOWS_PER_DISPLAY]; 

typedef struct display {
	DisplayClass *is_a;
	DECLARE_OBJECT_INSTANCE_VARS(struct display*)
	DECLARE_DISPLAY_INSTANCE_VARS(struct display*)
} Display;

extern Display* Display_init (Display*);
extern void Display_destroy (Any *);

#define CHECK_GL_ERROR(NAME) {\
        GLint err = glGetError(); \
        if (err) { \
                char temp [32]; \
                snprintf (temp, sizeof(temp)-1, "GL error %d", err); \
                Log_error (NAME, temp); \
        }}

// These keycodes are derived from FrugalWidgets, my user interface library.

enum {
	kKeycode_0='0',
	kKeycode_1,
	kKeycode_2,
	kKeycode_3,
	kKeycode_4,
	kKeycode_5,
	kKeycode_6,
	kKeycode_7,
	kKeycode_8,
	kKeycode_9,
	kKeycode_BackSpace = 8,
	kKeycode_Escape=27,
	kKeycode_Return=10,
	kKeycode_A='A',
	kKeycode_B,
	kKeycode_C,
	kKeycode_D,
	kKeycode_E,
	kKeycode_F,
	kKeycode_G,
	kKeycode_H,
	kKeycode_I,
	kKeycode_J,
	kKeycode_K,
	kKeycode_L,
	kKeycode_M,
	kKeycode_N,
	kKeycode_O,
	kKeycode_P,
	kKeycode_Q,
	kKeycode_R,
	kKeycode_S,
	kKeycode_T,
	kKeycode_U,
	kKeycode_V,
	kKeycode_W,
	kKeycode_X,
	kKeycode_Y,
	kKeycode_Z,
	kKeycode_a = 'a',
	kKeycode_b,
	kKeycode_c,
	kKeycode_d,
	kKeycode_e,
	kKeycode_f,
	kKeycode_g,
	kKeycode_h,
	kKeycode_i,
	kKeycode_j,
	kKeycode_k,
	kKeycode_l,
	kKeycode_m,
	kKeycode_n,
	kKeycode_o,
	kKeycode_p,
	kKeycode_q,
	kKeycode_r,
	kKeycode_s,
	kKeycode_t,
	kKeycode_u,
	kKeycode_v,
	kKeycode_w,
	kKeycode_x,
	kKeycode_y,
	kKeycode_z,
	kKeycode_slash = '/',
	kKeycode_period = '.',
	kKeycode_minus = '-',
	kKeycode_grave = '`',
	kKeycode_equal = '=',
	kKeycode_backslash = '\\',
	kKeycode_comma = ',',
	kKeycode_Tilde = '~',
	kKeycode_Delete = 127,
        kKeycode_Tab = 9,

	kKeycode_Shift_L = 2000,
	kKeycode_Shift_R,
	kKeycode_Control_L,
	kKeycode_Control_R,
        kKeycode_Down,
        kKeycode_Left,
        kKeycode_Right,
        kKeycode_Up,
	kKeycode_Alt_L,
	kKeycode_Meta_L,
	kKeycode_Alt_R,
	kKeycode_Meta_R,
	kKeycode_PageUp,
	kKeycode_PageDown,
	kKeycode_Home,
	kKeycode_Insert,
	kKeycode_End,
	kKeycode_F1,
	kKeycode_F2,
	kKeycode_F3,
	kKeycode_F4,
	kKeycode_F5,
	kKeycode_F6,
	kKeycode_F7,
	kKeycode_F8,
	kKeycode_F9,
	kKeycode_F10,
	kKeycode_F11,
	kKeycode_F12,
	kKeycode_ScrollLock,
	kKeycode_NumLock,
	kKeycode_CapsLock,
	kKeycode_PrintScreen,
};

#endif

