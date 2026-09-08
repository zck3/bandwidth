/*============================================================================
  TextView, an object-oriented C textview class.
  Copyright (C) 2018, 2019, 2026 by Zack T Smith.

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

// This is based on my FrugalWidgets project's TextEditor class.

#include "TextView.h"
#include "String.h"
#include "FontPK.h"

TextViewClass *_TextViewClass = NULL;

static TextViewWord *Word_new ()
{
	TextViewWord *word = calloc(sizeof(TextViewWord), 1);
	return word;
}

static void TextView_clearWords (TextView* restrict self)
{
	if (!self->words) {
		return;
	}

	TextViewWord *word = self->words;
	while (word) {
		TextViewWord *next = word->next;
		if (word->word) {
			free (word->word);
		}
		free (word);
		word = next;
	}
	self->words = NULL;
	self->last = NULL;
}

void TextView_destroy (Any *self)
{
        DEBUG_DESTROY;

	if (!self) {
		return;
	}
	verifyCorrectClass(self,TextView);

	TextView_clearWords (self);
	Label_destroy(self);
}

static void TextView_determineWordSizes (TextView* restrict self)
{
	if (!self) {
		return;
	}

	Font *font = self->font;
	if (!font) {
		// puts("CANNOT DETERMINE WORD SIZE");
		return;
	}

	TextViewWord *word = self->words;
	while (word) {
		Char *string = word->word;
		if (string) {
			int len = Char_strlen(string);
			int width = 0;
			int height = 0;
			int ascent = 0;
			int descent = 0;
			$(font, sizeOfWideString, string, len, &width, &ascent, &descent);
			word->width = width;
#ifdef TEXTVIEW_VERBOSE
			printf("WID OF %ls IS %d\n",string,width);
#endif
			word->height = height;
			word->ascent = ascent;
			word->descent = descent;
		}

		word = word->next;
	}

	self->needToDetermineWordSizes = false;
}

static void TextView_appendWord (TextView* restrict self, TextViewWord *word)
{
	if (!self) {
		return;
	}

	if (!self->words) {
		self->words = word;
	} else {
		self->last->next = word;
	}
	self->last = word;
}

static void TextView_parseWords (TextView* restrict self)
{
	if (!self) {
		return;
	}

	TextView_clearWords (self);

	if (!self->string) {
		return;
	}

#define MAXWORDLEN 1024
	Char word[MAXWORDLEN];
	int word_ix = 0;

	Char *chars = $(self->string, characters);
	int len = Char_strlen(chars);

	int ix = 0;
	while (ix < len) {
		while ((chars[ix]==' ' || chars[ix]=='\t') && ix < len) {
			ix++;
		}
		if (ix >= len) {
			break;
		}

		int newlineCount = 0;
		while (chars[ix]=='\n' && ix < len) {
			ix++;
			newlineCount++;
		}

		if (newlineCount) {
			// Create an empty word with newline(s), and append it.
			TextViewWord *newlineWord = Word_new();
			newlineWord->newlineCount = newlineCount;
			TextView_appendWord (self, newlineWord);

			// Log_debug_printf (__FUNCTION__, "GOT NEWLINE(s) = %d", newlineCount);
			continue;
		}

		bool encounteredNewline = false;
		while (ix < len && word_ix < MAXWORDLEN-1) {
			Char ch = chars[ix++];
			//printf ("CHAR %C %u\n",ch,(unsigned)ch);
			if (ch == ' ' || ch == '\t' || ch == '\n') {
				encounteredNewline = ch == '\n';
				break;
			}
			word[word_ix++] = ch;
		}
		word[word_ix] = 0;
		// If we encountered a newline after a word, save it to become a new word.
		if (encounteredNewline) {
			ix--;
		}

		// Log_debug_printf (__FUNCTION__, "GOT WORD \"%ls\" (len %lu)", word, Char_strlen(word));

		TextViewWord *textWord = Word_new();
		textWord->word = Char_strdup (word);
		TextView_appendWord (self, textWord);
		word_ix = 0;
	}

	self->needToDetermineWordSizes = true;
}

static void TextView_setFont (TextView* restrict self, Any *font_)
{ 
	if (!self) {
		return;
	}
	verifyCorrectClassOrSubclass(self,TextView);
	if (font_) {
		verifyCorrectClassOrSubclass(font_,Font);
	}
	if (self->font) {
		release(self->font);
	}
	if (font_) {
		Font *font = font_;
		self->font = retain(font);
	}
	self->needsRedraw = true;
	self->needToDetermineWordSizes = true;
	self->renderedHeight = 0;
}

static int TextView_renderedHeight (TextView* restrict self)
{
	if (!self) {
		return 0;
	}
	verifyCorrectClassOrSubclass(self,TextView);
	return self->renderedHeight;
}

static int TextView_yOffset (TextView* restrict self)
{
	if (!self) {
		return 0;
	}
	verifyCorrectClassOrSubclass(self,TextView);
	return self->yOffset;
}

static void TextView_clampYOffset (TextView * restrict self)
{
	int offset = self->yOffset;

	if (self->renderedHeight > 0) {
		if (offset < 0) {
			self->yOffset = 0;
		}
		else {
			int limit = self->renderedHeight - self->rect.size.height;
			if (limit > 0) {
				if (offset > limit) {
					self->yOffset = limit;
				}
			} else {
				self->yOffset = 0;
			}
		}
	} else {
		self->yOffset = 0;
	}
}

static void TextView_setYOffset (TextView* restrict self, int yoffset)
{
	if (!self) {
		return;
	}
	verifyCorrectClassOrSubclass(self,TextView);

	// RULE: If the text is too short vertically to accept 
	// a yOffset other than 0, and the current yOffset is 0,
	// just return and DON'T redraw.
	//
	if (!self->yOffset && self->renderedHeight < self->rect.size.height) {
		return;
	}

	if (yoffset != self->yOffset) {
		self->yOffset = yoffset;
		TextView_clampYOffset (self);
		$(self, setNeedsRedraw);
	}
}

static long TextView_message (TextView * restrict self, long message, Any *sender, long key, long second)
{
	if (message == kKeyDownMessage) {
		if (key == kKeycode_Home) {
			self->yOffset = 0;
		}
		else if (key == kKeycode_End) {
			if (self->renderedHeight > 0) {
				self->yOffset = self->renderedHeight - self->rect.size.height;
			} else {
				self->yOffset = 0;
			}
		}
		else if (key == kKeycode_PageUp || key == 0x2 /* CTRL-B */ ) {
			self->yOffset += 20;
		}
		else if (key == kKeycode_PageDown || key == 0x6 /* CTRL-F */) {
			self->yOffset -= 20;
		}
	}
	else if (message == kMouseScrollWheelMessage) {
		self->yOffset += key > 0 ? -20 : 20;
	}

	TextView_clampYOffset (self);
	$(self, setNeedsRedraw);

	return true;
}

static void TextView_redraw (TextView* restrict self)
{
	if (!self) {
		return;
	}
	verifyCorrectClassOrSubclass(self,TextView);

	if (!self->needsRedraw) {
		return;
	}

	MutableImage *drawable = self->drawable;
	if (!drawable) {
		return;
	}

	$(self, clear);

	String *string = self->string;
	if (!string) {
		return;
	}

	if (!self->words) {
		return;
	}

	bool didAllocateFont = false;
	Font *font = self->font;
	if (!font) {
		didAllocateFont = true;
		font = (Font*) FontPK_with ("cmr", 26);
		if (!font) {
			return;
		}
		self->needToDetermineWordSizes = true;
	}

	TextViewWord *words = self->words;

	int width = self->rect.size.width;
	int height = self->rect.size.height;
	if (width < 1) {
		return;
	}

	if (self->needToDetermineWordSizes) {
		TextView_determineWordSizes (self);
	}

	const int spaceSize = 10;
	const int interlineSpacing = 3;
	const int fontHeight = $(font, height);

	self->needsRedraw = false;

	// The full rendered height of the text only needs to be calculated once
	// after the text is changed and/or the font is changed.
	bool needRenderedHeight = self->renderedHeight == 0;

	int x = 0;
	int y = -self->yOffset;
	while (words) {
		// Scan forward to assess how many words will fit on a line.
		int accumulatedWidthOfWords = 0;
		TextViewWord *start = words;
		TextViewWord *word = words;
		bool endsWithSpace = true;
		while (word != NULL && accumulatedWidthOfWords < width) {
			int wordWidth = word->width;
			if (accumulatedWidthOfWords + wordWidth < width) {
				accumulatedWidthOfWords += wordWidth;
				word = word->next;
				if (accumulatedWidthOfWords + spaceSize >= width) {
					endsWithSpace = false;
					break;
				}
				accumulatedWidthOfWords += spaceSize;
			} else {
				break;
			}
		}
		if (word == start) {
			// Overly wide word won't fit on a line.
			word = word->next;
		}
		TextViewWord *end = word;

		if (endsWithSpace) {
			accumulatedWidthOfWords -= spaceSize;
		}

		int xOffsetForAlignment = 0;
		if (self->horizontalAlignment == HorizontalAlignmentRight) {
			xOffsetForAlignment = width - accumulatedWidthOfWords;
		}
		else if (self->horizontalAlignment == HorizontalAlignmentCenter) {
			xOffsetForAlignment = (width - accumulatedWidthOfWords) / 2;
		}

		x = 0;
		while (start != NULL && start != end) {
			if (y >= height && !needRenderedHeight) {
				break;
			}

			Char *string = start->word;
			if (string) {
				int len = Char_strlen(string);
				if (y + fontHeight >= 0) {
					$(drawable, drawChars, string, len, xOffsetForAlignment + x, y, font, self->foregroundColor);
				}
				x += start->width + spaceSize;
			} else {
				if (start->newlineCount) {
					x = 0;
					y += (fontHeight + interlineSpacing) * start->newlineCount;
					if (y >= height && !needRenderedHeight) {
						break;
					}
				}
			}
			start = start->next;
		}

		words = end;

		y += fontHeight + interlineSpacing;
		if (y >= height && !needRenderedHeight) {
			break;
		}
	}

	if (needRenderedHeight) {
		if (x > 0) {
			// Need a newline.
			y += fontHeight;
		}
		self->renderedHeight = y;
	}

	if (didAllocateFont) {
		release(font);
	}
}

static void TextView_setString (TextView* restrict self, Any *string_)
{
	if (!self) {
		return;
	}
	verifyCorrectClassOrSubclass(self,TextView);
	verifyCorrectClassOrSubclass(string_,String);

	if (self->string) {
		release(self->string);
	}
	String *string = string_;
	self->string = string ? retain(string) : NULL;

	TextView_parseWords (self);

	self->needToDetermineWordSizes = true;
	self->needsRedraw = false;
	self->renderedHeight = 0;
}

TextView* TextView_init (TextView* restrict self)
{
	ENSURE_CLASS_READY(TextView);
	if (!self) {
		return NULL;
	}

	Label_init ((Label*) self);

	self->is_a = _TextViewClass;
	self->horizontalAlignment = HorizontalAlignmentLeft;

	return self;
}

TextViewClass* TextViewClass_init (TextViewClass* restrict class)
{
	SET_SUPERCLASS(Label);

	SET_OVERRIDDEN_METHOD_POINTER(TextView,destroy);
	SET_OVERRIDDEN_METHOD_POINTER(TextView,redraw);
	SET_OVERRIDDEN_METHOD_POINTER(TextView,setString);
	SET_OVERRIDDEN_METHOD_POINTER(TextView,setFont);
	SET_OVERRIDDEN_METHOD_POINTER(TextView,message);

	SET_METHOD_POINTER(TextView,yOffset);
	SET_METHOD_POINTER(TextView,setYOffset);
	SET_METHOD_POINTER(TextView,renderedHeight);

	VALIDATE_CLASS_STRUCT(class);
	return class;
}

