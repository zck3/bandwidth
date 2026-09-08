/*============================================================================
  Char, a Unicode type (at least until I use ICU).
  Copyright (C) 2026 by Zack T Smith.

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

#ifndef _OOC_CHAR_H
#define _OOC_CHAR_H

#include <stdint.h>

#include "ObjectOriented.h"

typedef uint32_t Char;

enum {
	CharLessThan = -1,
	CharEqual = 0,
	CharGreaterThan = 1
};

extern size_t Char_strlen (const Char*);
extern size_t Char_fromUTF8 (Char*, const char *, size_t maxlen);
extern size_t Char_toUTF8 (char *, const Char *, size_t maxlen);
extern int Char_strcmp (const Char* a, const Char *b);
extern int Char_strcasecmp (const Char* a, const Char *b);
extern double Char_strtod (const Char *str);
extern long Char_strtol (const Char *str);
extern Char *Char_strdup (const Char *str);
extern Char *Char_strndup (const Char *str, int length);
extern void Char_puts (const Char *str);
extern Char *Char_trimString (Char *str, size_t *len_return);

#endif
