/*============================================================================
  TextField, an Object-Oriented C text field class.
  Copyright (C) 2004-2019, 2022, 2026 by Zack T Smith.

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

// This code is adapted from the class TextField.cpp of my FrugalWidgets project.

#include <stdlib.h>
#include <string.h>
#include <ctype.h>

#include "Window.h"
#include "Controller.h"
#include "TextField.h"
#include "FontPK.h"
#include "colors.h"

TextFieldClass *_TextFieldClass = NULL;

void TextField_destroy (Any *self)
{
	DEBUG_DESTROY;

	if (!self) {
		return;
	}
	verifyCorrectClassOrSubclass(self,TextField);

	View_destroy (self);
}

static void TextField_print (TextField* restrict self, FILE *outputFile) 
{ 
	if (!self) {
		return;
	}
	verifyCorrectClass(self,TextField);

	if (!outputFile) {
		outputFile = stdout;
	}

	size_t len = Char_strlen(self->line);
	for (size_t i=0; i < len; i++) {
		Char ch = self->line[i];
		if (ch > 0 && ch < 128) {
			fputc (ch, outputFile);
		}
	}
}

static void TextField_describe (TextField* restrict self, FILE *outputFile) 
{ 
	if (!self) {
		return;
	}
	verifyCorrectClass(self,TextField);

	if (!outputFile) {
		outputFile = stdout;
	}

	fprintf (outputFile, "%s", $(self, className));
}

static void TextField_insertChars (TextField* restrict self, const Char *text)
{ 
	if (!self || !text || !*text) {
		return;
	}
	verifyCorrectClassOrSubclass(self,TextField);

	while (*text) {
		Char ch = *text++;

		self->isFull = self->length >= kTextfieldMaxLength;

		if (self->isFull) {
			return;
		}

		if (ch < ' ' || ch >= 127) {
			// RULE: Skip control codes and non-ASCII for now.
			continue;
		}

		if (self->cursorPosition == self->length) {
			self->line [self->length] = ch;
			self->cursorPosition = self->length + 1;
		} 
		else {
			int i = self->length;
			while (i > 0 && i >= self->cursorPosition) {
				self->line[i] = self->line[i-1];
				i--;
			}
			self->line [self->cursorPosition] = ch;
			self->cursorPosition++;
		}

		self->length++;
		self->line [self->length] = 0;
	}

	$(self, setNeedsRedraw);
}

static void TextField_insertText (TextField* restrict self, const char *text)
{
	if (!self || !text || !*text) {
		return;
	}
	verifyCorrectClassOrSubclass(self,TextField);
	
	// RULE: Allow multibyte (Unicode) sequences.
	size_t len = strlen (text);
	Char string[len+1];
	(void)Char_fromUTF8 (string, text, len);
	TextField_insertChars (self, string);
}

static void TextField_insertChar (TextField* restrict self, const Char ch)
{
	if (!self || !ch) {
		return;
	}
	verifyCorrectClassOrSubclass(self,TextField);

	Char string[2] = {ch, 0};
	TextField_insertChars (self, string);
}

static void TextField_insertInteger (TextField* restrict self, long value)
{
	if (!self) {
		return;
	}
	verifyCorrectClassOrSubclass(self,TextField);

	char string[32];
	snprintf (string, sizeof(string), "%ld", value);
	TextField_insertText (self, string);
}

static void TextField_setText (TextField* restrict self, const char *text)
{ 
	if (!self) {
		return;
	}
	verifyCorrectClassOrSubclass(self,TextField);

	$(self, erase);
	if (text && *text) {
		$(self, insertText, text);
	}

	$(self, setNeedsRedraw);
}

static Char *TextField_chars (TextField* restrict self)
{
	if (!self) {
		return NULL;
	}
	verifyCorrectClassOrSubclass(self,TextField);
	return self->line;
}

static void TextField_setChars (TextField* restrict self, const Char *text)
{ 
	if (!self) {
		return;
	}
	verifyCorrectClassOrSubclass(self,TextField);

	$(self, erase);
	if (text && *text) {
		$(self, insertChars, text);
	}

	$(self, setNeedsRedraw);
}

static void TextField_setWideText (TextField* restrict self, const wchar_t *text)
{ 
	if (!self) {
		return;
	}
	verifyCorrectClassOrSubclass(self,TextField);

	$(self, erase);
	if (text && *text) {
		String *string = String_withWide (text);
		Char *chars = $(string, characters);
		$(self, insertChars, chars);
		release(string);
	}

	$(self, setNeedsRedraw);
}

static void TextField_drawCursor (TextField* restrict self)
{
	MutableImage *drawable = self->drawable;
	if (!drawable) {
		return;
	}
	if (self->selecting) {
		return;
	}

	self->cursorX = self->charPositions[self->cursorPosition];
#ifdef TEXTFIELD_VERBOSE
	printf ("POSN %d CURSORX %d\n", self->cursorPosition, self->cursorX);
#endif
	int height = self->rect.size.height;
	RGB color = self->cursorColor;
	int inset = kTextfieldDefaultBorder;
	int cursorHeight = height - 2*inset;
	$(drawable, drawVerticalLine, self->cursorX-1, inset, cursorHeight, color);
	$(drawable, drawVerticalLine, self->cursorX, inset, cursorHeight, color);
}

static int TextField_convertCharacterIndexToCoordinate (TextField* restrict self, int index)
{
	if (index < 0) {
		return 0;
	}
	if (index >= self->length) {
		return self->charPositions[self->length];
	}
	return self->charPositions[index];
}

static int TextField_convertCoordinateToCharacterIndex (TextField* restrict self, int pointerX)
{
	int position = 0;

	if (position > self->length) {
		position = self->length;
	}
	else {
		if (self->length) {
			int i=0;
			while (i < self->length && pointerX >= (self->charPositions[i] + self->charPositions[i+1])/2) 
			{
				i++;
			}
			position = i;
		}
		else {
			position = 0;
		}
	}

	Log_debug_printf (__FUNCTION__, "Cursor position = %d (length = %u)", position, self->length);

	return position;
}

static void TextField_redraw (TextField* restrict self)
{
	if (!self) {
		return;
	}
	verifyCorrectClassOrSubclass(self,TextField);

	self->needsRedraw = false;

	if (self->hidden) {
		return;
	}
	MutableImage *drawable = self->drawable;
	if (!drawable) {
		return;
	}

	// Even if the border is being hidden, continue using internal padding.
	int inset = kTextfieldDefaultBorder;

	if (self->onlyShowBorderWhenEditing) {
		self->borderWidth = self->editing ? kTextfieldDefaultBorder : 0;
	} else {
		self->borderWidth = kTextfieldDefaultBorder;
	}

	int width = self->rect.size.width;
	int height = self->rect.size.height;

	// Clear any cursor that was drawn.
	int cursorHeight = height - 2*inset;
	$(drawable, drawVerticalLine, self->cursorX-1, inset, cursorHeight, self->backgroundColor);
	$(drawable, drawVerticalLine, self->cursorX, inset, cursorHeight, self->backgroundColor);

	if (self->selecting && self->dragStartChar >= 0 && self->dragEndChar >= 0) {
		int start = self->dragStartChar;
		int end = self->dragEndChar;
		if (end < start) {
			int temp = start;
			start = end;
			end = temp;
		}
		int startX = TextField_convertCharacterIndexToCoordinate(self, start);
		int endX = TextField_convertCharacterIndexToCoordinate(self, end);
		if (startX > 0) {
			int x0 = self->horizontalPadding + inset;
			$(drawable, fillRectangle, 
				x0,
				inset, 
				startX - x0 + 1,
				height - 2*inset, 
				self->backgroundColor);
		}

		$(drawable, fillRectangle, 
				startX, 
				inset, 
				endX - startX + 1,
				height - 2*inset, 
				self->highlightColor);

		if (endX < width-2*inset) {
			int x1 = width - self->horizontalPadding - inset;
			$(drawable, fillRectangle, 
				endX, 
				inset, 
				x1 - endX + 1,
				height - 2*inset, 
				self->backgroundColor);
		}
	} else {
		// No selection is happening, so just clear the entire background.
		$(self, clear);
	}

	if (!self->font) {
		return;
	}

	int x, y, x2;

	int ascent = $(self->font, ascent);
	x = inset + self->horizontalPadding;
	y = (height - ascent)/2;

	//------------------------------------------------------------
	// Draw each character individually so that we can record 
	// individual character positions, limit self->length and 
	// control spacing.
	//
	Char tmp[2];
	tmp[1] = 0;

	self->cursorX = -1;
	int i;
	self->isFull = false;

	// Draw all chars, not just the visible ones, because we need all of their X values.
	for (i = 0; i < self->length; i++) {
		tmp[0] = self->line[i];
		tmp[1] = 0;

		self->charPositions [i] = x;

		int finalX = $(drawable, drawChars, tmp, 1U, x, y, self->font, self->textColor);
		int charWidth = finalX - x;

		x2 = x + charWidth;

		if (i == self->cursorPosition) {
			self->cursorX = x;
		}
		x = x2 + 1;

		int maxX = width - inset - self->horizontalPadding;
		if (x >= maxX) {
			self->isFull = true;
		} else {
			self->isFull = false;
		}
	}

	self->charPositions[self->length] = x; // The position after the last char.

	if (self->cursorX < 0) {
		self->cursorX = x;
	}

	if (self->doHaveFocus && !self->selecting) {
		TextField_drawCursor (self);
	}
}

//--------------------------------------------------------------------
// Name:	erase
// Purpose:	Erase the text.
//--------------------------------------------------------------------
static void TextField_erase (TextField* restrict self)
{
	if (!self) {
		return;
	}
	verifyCorrectClassOrSubclass(self,TextField);

        self->line [0] = 0;
        self->length = 0;
        self->cursorPosition = 0;
        $(self, setNeedsRedraw);
}

TextField* TextField_init (TextField* restrict self)
{
	ENSURE_CLASS_READY(TextField);

	if (self) {
		View_init ((View*) self);
		self->is_a = _TextFieldClass;

		self->listener = NULL;
		self->cursorPosition = 0;
		self->length = 0;
		self->isFull = false;
		self->doHaveFocus = false;
		self->ctrl = false;
		self->shift = false;
		self->hidden = false;
		self->font = NULL;
		self->editing = false;
		self->onlyShowBorderWhenEditing = false;

		self->cursorColor = kColorRed;
		self->foregroundColor = kColorBlack;
		self->backgroundColor = kColorWhite;
		self->highlightColor = RGB_PASTEL_YELLOW;

		self->borderWidth = kTextfieldDefaultBorder;
		self->horizontalPadding = kTextfieldDefaultPadding;
		self->borderType = ViewBorderType3DIn;
		self->borderColor = kColorDarkGray;

		self->selecting = false;
		self->dragStartChar = -1;
		self->dragEndChar = -1;

		self->rect = Rect_zero ();

		self->font = retain(FontPK_with ("cmr", 18));

#if FROM_FRUGAL_WIDGETS
		self->font = Fonts_fetch ("courier", 14, false, false);
		if (!self->font) {
			self->font = Fonts_fetch ("cmr", 18, false, false);
		}
		if (self->font) {
			retain(self->font);
		}
#endif

		TextField_erase (self);
	}

	return self;
}

static void TextField_keyRelease (TextField * restrict self, Char key)
{
	switch (key) {
	case kKeycode_Shift_L:
	case kKeycode_Shift_R:
		self->shift = false;
		return;

	case kKeycode_Control_L:
	case kKeycode_Control_R:
		self->ctrl = false;
		return;

	case kKeycode_Meta_L:
	case kKeycode_Meta_R:
		return;

	case kKeycode_Alt_L:
	case kKeycode_Alt_R:
		return;
	}
}

// Private method
static void TextField_deleteSelection (TextField * restrict self)
{
	int newEnd = self->length;
	if (self->dragEndChar >= self->length) {
		// Truncate
		newEnd = self->dragStartChar;
		self->cursorPosition = newEnd;
	}
	else {
		int nCharsAtEnd = self->length - self->dragEndChar;
		int source = self->dragEndChar;
		int dest = self->cursorPosition;
		for (int i = 0; i < nCharsAtEnd; i++) {
			self->line[dest++] = self->line[source++];
		}
		newEnd = self->cursorPosition + nCharsAtEnd;
	}
	self->line[newEnd] = 0;
	self->length = newEnd;
	self->selecting = false;
	self->dragStartChar = -1;
	self->dragEndChar = -1;
}

//--------------------------------------------------------------------
// Name:	keyPress
// Purpose:	Key press handler.
//--------------------------------------------------------------------
static void TextField_keyPress (TextField * restrict self, Char key)
{
	if (!self->doHaveFocus) {
		return;
	}

	switch (key) {
	case kKeycode_Shift_L:
	case kKeycode_Shift_R:
		self->shift = true;
		return;

	case kKeycode_Control_L:
	case kKeycode_Control_R:
		self->ctrl = true;
		return;

	case kKeycode_Meta_L:
	case kKeycode_Meta_R:
		return;

	case kKeycode_Alt_L:
	case kKeycode_Alt_R:
		return;

	case kKeycode_Escape:
		TextField_erase(self);
		break;

	case kKeycode_Up:
	case kKeycode_Down:
		// XX Access history
		return;

	case kKeycode_Tab:
		if (self->controller) {
			//self->doHaveFocus = false;
			//self->editing = false;
			//self->selecting = false;
			Controller *controller = self->controller;
			$(controller, tabTraversal, self);
		}
		break;

	case kKeycode_Home:
		self->cursorPosition = 0;
		break;

	case 1:	// CTRL-A = Select All.
		self->selecting = true;
		self->dragStartChar = 0;
		self->dragEndChar = self->length;
		self->cursorPosition = self->length;
		break;

	case 2: // CTRL-B
	case kKeycode_Left:
		if (self->selecting) {
			// Move cursor to start of selected area.
			self->selecting = false;
			self->cursorPosition = self->dragStartChar;
		}
		else {
			if (self->cursorPosition > 0) {
				self->cursorPosition--;
			}
		}
		self->dragStartChar = -1;
		self->dragEndChar = -1;
		break;

	case 4: // CTRL-D = Delete char to right of cursor.
	case kKeycode_Delete: 
		if (self->cursorPosition < self->length) {
			int i = self->cursorPosition;
			while (i < self->length-1) {
				self->line [i] = self->line [i+1];
				i++;
			}
			self->length--;
			self->line [self->length] = 0;
		} 
		break;

	case kKeycode_End:
	case 5:	// CTRL-E = Move to end.
		self->cursorPosition = self->length;
		break;

	case 6: // CTRL-F
	case kKeycode_Right:
		if (self->selecting) {
			// Move cursor to end of selected area.
			self->selecting = false;
			self->cursorPosition = self->dragEndChar;
		}
		else {
			if (self->cursorPosition < self->length) {
				self->cursorPosition++;
			}
		}
		self->dragStartChar = -1;
		self->dragEndChar = -1;
		break;

	case 11: // CTRL-K = Clear to end.
		self->length = self->cursorPosition;
		self->line [self->length] = 0;
		break;

	case 21: // CTRL-U = Clear to start.
		self->length = self->cursorPosition;
		self->line [self->length] = 0;
		break;

	case 22: // CTRL-V
		// XX Paste
		return;
		
	case 24: // CTRL-X
		// XX Cut
		return;

	case kKeycode_BackSpace:
		if (!self->length) {
			return;
		}
		if (self->selecting) {
			TextField_deleteSelection (self);
		}
		else {
			if (self->cursorPosition >= self->length) {
				self->line[--self->length] = 0;
				self->cursorPosition = self->length;
			} 
			else if (self->cursorPosition > 0) {
				self->cursorPosition--;
				for (int i = self->cursorPosition; i < self->length-1; i++) {
					self->line[i] = self->line[i+1];
				}
				self->line[--self->length] = 0;
			}
		} 
		break;

	case 13: // Linux on Macbook produces carriage return.
	case 10: // Perhaps other Unixes use a newline.
		self->doHaveFocus = false;
		self->editing = false;
		self->selecting = false;

		// Report that editing's finished.
		if (self->listener) {
			Object *recipient = self->listener;
			$(recipient, message, kEditingDidEndMessage, self, 0, 0);
		}

		// Tell window to disable keyboard and release key focus.
		if (self->controller) {
			// XX Temporary kludge
			Controller *myController = (Controller*) self->controller;
			Window *win = $(myController, window);
			if (win) {
				$(win, setKeyboardFocusView, NULL);
			}
		}
		break;

	default:
		if (key >= 0x20 && key < 0x7f) {
			if (self->selecting) {
				// Newly typed character will replace any selected text.
				TextField_deleteSelection (self);
			}

			if (self->length < kTextfieldMaxLength) {
				TextField_insertChar (self, key);
			} else {
				//Sound::singleton()->beepWithPitch (440 /* Hz */, 250 /* milliseconds */);
			}
		}
		else {
			Log_debug_printf (__FUNCTION__, "Unknown keysym %d", key);
		}
		break;

	}

	$(self, setNeedsRedraw);
}

static long TextField_message (TextField * restrict self, long message, Any *sender, long first, long second)
{
	switch (message) {
	case kKeyDownMessage:
		TextField_keyPress (self, first);
		break;
	case kKeyUpMessage:
		TextField_keyRelease (self, first);
		break;
	case kViewGainedPointerFocus:
		if (self->listener) {
			Object *recipient = self->listener;
			$(recipient, message, kEditingWillBeginMessage, self, 0, 0);
		}
		self->shift = false;
		self->ctrl = false;
		puts("TextField kViewGainedPointerFocus");
		break;
	case kViewLostPointerFocus:
		if (self->listener) {
			Object *recipient = self->listener;
			$(recipient, message, kEditingDidEndMessage, self, 0, 0);
		}
		self->shift = false;
		self->ctrl = false;
		puts("TextField kViewLostPointerFocus");
		break;
	case kViewGainedKeyboardFocus:
		puts("TextField kViewGainedKeyboardFocus");
		if (!self->doHaveFocus) {
			self->doHaveFocus = true;
			self->shift = false;
			self->ctrl = false;
			$(self, setNeedsRedraw); 
		}
		break;
	case kViewLostKeyboardFocus:
		puts("TextField kViewLostKeyboardFocus");
		if (self->doHaveFocus) {
			self->doHaveFocus = false;
			$(self, setNeedsRedraw); 
		}
		break;
	case kPointerMovedMessage:
		puts("TextField kPointerMovedMessage");
		int x = first;
		int distance = abs(self->dragStartX - x);
		if (!self->selecting && distance >= 5) {
			self->selecting = true;
		}
		if (self->selecting) {
			int position = TextField_convertCoordinateToCharacterIndex (self, x);
			self->dragEndChar = position;
			$(self, setNeedsRedraw); 
		}
		break;
	case kPointerUpMessage: {
		puts("TextField kPointerUpMessage");
		int x = first;
		if (!self->selecting) {
			self->dragStartChar = -1;
			self->dragEndChar = -1;

			if (!self->doHaveFocus) {
				self->doHaveFocus = true;
			} 

			// If the user has clicked in the view during
			// data entry, it's to move the cursor.
			// So let's locate where the cursor should go.
			//
		} else {
			// Selection of text
			int position = TextField_convertCoordinateToCharacterIndex (self, x);
			self->dragEndChar = position;
			Log_debug_printf (__FUNCTION__, "Dragged from char %d to %d", self->dragStartChar, self->dragEndChar);
		}

		self->editing = true;
		$(self, setNeedsRedraw);
		break;
	 }
	case kPointerDownMessage: {
		puts("kPointerDownMessage");
		int x = first;
		self->cursorPosition = TextField_convertCoordinateToCharacterIndex (self, x);
		self->dragStartChar = self->cursorPosition;
		self->dragEndChar = -1;
		self->selecting = false;
		self->editing = true;
		TextField_drawCursor (self);
		$(self, setNeedsRedraw);
		self->dragStartX = x;
		self->dragStartY = second;
		break;
	 }
	}

	return true;
}

static void TextField_setListener (TextField* restrict self, Any* object)
{
	if (!self) {
		return;
	}
	verifyCorrectClassOrSubclass(self,TextField);
	if (object && !isObject(object)) {
		return;
	}
	self->listener = object;
}

static void TextField_setFont (TextField* restrict self, Font* font)
{
	if (!self) {
		return;
	}
	verifyCorrectClassOrSubclass(self,TextField);

	if (self->font) {
		release(self->font);
	}

	if (font) {
		self->font = retain(font);
	} else {
		self->font = NULL;
	}
}

static Font* TextField_font (TextField* restrict self)
{
	if (!self) {
		return NULL;
	}
	verifyCorrectClassOrSubclass(self,TextField);
	return self->font;
}

TextFieldClass* TextFieldClass_init (TextFieldClass *class)
{
	SET_SUPERCLASS(View);

	SET_OVERRIDDEN_METHOD_POINTER(TextField,describe);
	SET_OVERRIDDEN_METHOD_POINTER(TextField,print);
	SET_OVERRIDDEN_METHOD_POINTER(TextField,redraw);
	SET_OVERRIDDEN_METHOD_POINTER(TextField,message);

	SET_METHOD_POINTER(TextField,setListener);
	SET_METHOD_POINTER(TextField,erase);
	SET_METHOD_POINTER(TextField,setText);
	SET_METHOD_POINTER(TextField,setChars);
	SET_METHOD_POINTER(TextField,chars);
	SET_METHOD_POINTER(TextField,setWideText);
	SET_METHOD_POINTER(TextField,setFont);
	SET_METHOD_POINTER(TextField,font);
	SET_METHOD_POINTER(TextField,insertText);
	SET_METHOD_POINTER(TextField,insertChars);
	SET_METHOD_POINTER(TextField,insertChar);
	SET_METHOD_POINTER(TextField,insertInteger);
	
        VALIDATE_CLASS_STRUCT(class);
	return class;
}

