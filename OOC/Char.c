/*============================================================================
  Char, a Unicode type.
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

#include "Log.h"
#include "Char.h"

#include <wctype.h> // towlower
#include <ctype.h>  // isdigit
#include <string.h> // memcpy
#include <limits.h> // LONG_MAX

size_t Char_strlen (const Char* str)
{
	if (!str) {
		return 0;
	}
	register size_t len = 0;
	register Char *ptr = (Char*)str;
	while (*ptr++) {
		len++;
	}
	return len;
}

int Char_strcmp (const Char* a, const Char *b)
{
	if (!a && !b) {
		return CharEqual;
	}
	if (!a && b) {
		return CharLessThan;
	}
	if (a && !b) {
		return CharGreaterThan;
	}
	register size_t index = 0;
	while (true) {
		Char ca = a[index];
		Char cb = b[index];
		if (!ca && !cb) {
			return CharEqual;
		}
		else if (ca < cb) {
			return CharLessThan;
		}
		else if (ca > cb) {
			return CharGreaterThan;
		}
		index++;
	}
	return CharEqual;
}

long Char_strtol (const Char *str)
{
	if (!str || !*str) {
		return 0;
	}
	long value = 0;
	long sign = 1;
	int index = 0;
	if (str[0] == '-') {
		sign = -1;
		index++;
	}
	long digit;
	while ((digit = str[index])) {
		if (isdigit(digit)) {
			long prev_value = value;
			value *= 10;
			value += digit - '0';
			if (prev_value > value || value < 0) {
				// Overflow
				return sign == -1? LONG_MIN : LONG_MAX;
			}
		} else {
			break;
		}
		index++;
	}
	return value * sign;
}

double Char_strtod (const Char *str)
{
	if (!str || !*str) {
		return 0.;
	}
	char copy[32];
	int i=0;
	while (i < sizeof(copy)-1 && str[i]) {
		Char ch = str[i];
		if (isdigit(ch) || ch == '.' || ch == '-') {
			copy[i] = str[i];
		} else {
			break;
		}
		i++;
	}
	copy[i] = 0;
	return strtod(copy, NULL);
}

int Char_strcasecmp (const Char* a, const Char *b)
{
	if (!a && !b) {
		return CharEqual;
	}
	if (!a && b) {
		return CharLessThan;
	}
	if (a && !b) {
		return CharGreaterThan;
	}
	register size_t index = 0;
	while (true) {
		Char ca = towlower(a[index]);
		Char cb = towlower(b[index]);
		if (!ca && !cb) {
			return CharEqual;
		}
		else if (ca < cb) {
			return CharLessThan;
		}
		else if (ca > cb) {
			return CharGreaterThan;
		}
		index++;
	}
	return CharEqual;
}

Char *Char_strdup (const Char *str)
{
	if (!str) {
		return NULL;
	}
	size_t len = Char_strlen(str);
	size_t nBytes = sizeof(Char) * (len+1);
	Char *chars = malloc(nBytes);
	if (!chars) {
		Log_perror (__FUNCTION__, "malloc");
		return NULL;
	}
	memcpy (chars, str, nBytes);
	return chars;
}

Char *Char_strndup (const Char *str, int maxLength)
{
	if (!str) {
		return NULL;
	}
	size_t len = Char_strlen(str);
	if (len > maxLength) {
		len = maxLength;
	}
	size_t nBytes = sizeof(Char) * (len+1);
	Char *chars = malloc(nBytes);
	if (!chars) {
		Log_perror (__FUNCTION__, "malloc");
		return NULL;
	}

	memcpy (chars, str, sizeof(Char) * len);
	chars[len] = 0; // end of string

	return chars;
}

size_t Char_fromUTF8 (Char* dest, const char *src_, size_t maxlen)
{
	if (!dest || !src_ || !maxlen) {
		return 0;
	}
	register size_t dest_index = 0;
	register size_t src_index = 0;
	register uint8_t *src = (uint8_t*) src_;
	size_t src_len = strlen((const char*)src);

	while (dest_index < maxlen-1 && src_index < src_len && src[src_index]) {

		// XX For now, ignore anything but ASCII.
		unsigned ch = src[src_index++];

		// ASCII
		if (ch >= 0 && ch <= 127) {
			dest[dest_index++] = ch;
		}
		else { 
			// 2-byte Unicode = 11 bits = 5 + 6 bits
			if (src_index < src_len && (ch & 0b11100000) == 0b11000000) {
				unsigned byte2 = src[src_index++];
				if ((byte2 & 0b11000000) == 0b10000000) {
					unsigned value = ((ch & 0b11111) << 6) | (byte2 & 0b111111);
					if (value) {
						dest[dest_index++] = value;
					}
				}
				else {
					Log_debug(__FUNCTION__, "Invalid subsequent byte in UTF-8");
				}
			}
			// 3-byte Unicode = 16 bits = 4 + 6 bits + 6 bits
			else if (src_index < src_len-1 && (ch & 0b11110000) == 0b11100000) {
				unsigned byte2 = src[src_index++];
				unsigned byte3 = src[src_index++];
				if ((byte2 & 0b11000000) == 0b10000000 && (byte3 & 0b11000000) == 0b10000000) {
					unsigned value = ((ch & 0b1111) << 12) 
						| ((byte2 & 0b111111) << 6) 
						| (byte3 & 0b111111);
					if (value) {
						dest[dest_index++] = value;
					}
				}
				else {
					Log_debug(__FUNCTION__, "Invalid subsequent byte in UTF-8");
				}
			}
			// 4-byte Unicode = 21 bits = 3 + 6 bits + 6 bits + 6 bits
			else if (src_index < src_len-2 && (ch & 0b11111000) == 0b11110000) {
				unsigned byte2 = src[src_index++];
				unsigned byte3 = src[src_index++];
				unsigned byte4 = src[src_index++];
				if ((byte2 & 0b11000000) == 0b10000000
				 && (byte3 & 0b11000000) == 0b10000000
				 && (byte4 & 0b11000000) == 0b10000000)
				{
					unsigned value = ((ch & 0b111) << 18) 
						| ((byte2 & 0b111111) << 12) 
						| ((byte3 & 0b111111) << 6)
						| (byte4 & 0b111111);
					if (value) {
						dest[dest_index++] = value;
					}
				}
				else {
					Log_debug(__FUNCTION__, "Invalid subsequent byte in UTF-8");
				}
			}
			else {
				Log_debug(__FUNCTION__, "Invalid Unicode");
			}

		}
	}
	dest[dest_index] = 0;
	return dest_index;
}

size_t Char_toUTF8 (char *dest, const Char *src, size_t maxlen)
{
	if (!dest || !src || !maxlen) {
		return 0;
	}

	register size_t dest_index = 0;
	register size_t src_index = 0;

	while (dest_index < maxlen-1 && src[src_index]) {

		register Char ch = src[src_index++];

		// 1-byte Unicode (7 bits)
		if (ch >= 0 && ch <= 127) {
			dest[dest_index++] = ch;
		}
		// Too-large Unicode (over 21 bits)
		else if (ch >= (1 << 21)) {
			Log_debug (__FUNCTION__, "Char requiring more than 21 bits not supported.");
		}
		// 4-byte Unicode (21 bits)
		else if (ch >= (1 << 16)) {
			if (dest_index < maxlen-4) {
				unsigned byte1 = (ch >> 18) & 7;
				unsigned byte2 = (ch >> 12) & 63;
				unsigned byte3 = (ch >> 6) & 63;
				unsigned byte4 = ch & 63;
				byte1 |= 0b11110000;
				byte2 |= 0b10000000;
				byte3 |= 0b10000000;
				byte4 |= 0b10000000;
				dest[dest_index++] = byte1;
				dest[dest_index++] = byte2;
				dest[dest_index++] = byte3;
				dest[dest_index++] = byte4;
			} else {
				Log_debug (__FUNCTION__, "UTF-8 won't fit in destination");
			}
		}
		// 3-byte Unicode (16 bits)
		else if (ch >= (1 << 11)) {
			if (dest_index < maxlen-3) {
				unsigned byte1 = (ch >> 12) & 15;
				unsigned byte2 = (ch >> 6) & 63;
				unsigned byte3 = ch & 63;
				byte1 |= 0b11100000;
				byte2 |= 0b10000000;
				byte3 |= 0b10000000;
				dest[dest_index++] = byte1;
				dest[dest_index++] = byte2;
				dest[dest_index++] = byte3;
			} else {
				Log_debug (__FUNCTION__, "UTF-8 won't fit in destination");
			}
		}
		// 2-byte Unicode (11 bits)
		else if (ch >= 128) {
			if (dest_index < maxlen-2) {
				unsigned byte1 = (ch >> 6) & 31;
				unsigned byte2 = ch & 63;
				byte1 |= 0b11000000;
				byte2 |= 0b10000000;
				dest[dest_index++] = byte1;
				dest[dest_index++] = byte2;
			} else {
				Log_debug (__FUNCTION__, "UTF-8 won't fit in destination");
			}
		}
	}
	dest[dest_index] = 0;
	return dest_index;
}

void Char_puts (const Char *str)
{
	if (!str) {
		return;
	}
	size_t len = Char_strlen(str);
	if (!len) {
		return;
	}
	size_t maxlen = len * 4 + 1;
	char buffer[maxlen];
	Char_toUTF8 (buffer, str, maxlen);
	puts(buffer);
}

/* Trim a Char string of whitespace at start & end, return new string & new length.
 */
Char *Char_trimString (Char *str, size_t *len_return)
{
	if (!str) {
		return NULL;
	}
	if (!len_return) {
		return str;
	}
	size_t len = *len_return;
	Char *start = str;
	Char *end = str + len - 1;

	while (*start && isspace(*start)) {
		start++;
		len--;
	}

	while (end >= str && isspace(*end)) {
		*end-- = 0;
		len--;
	}
	
	*len_return = len;
	return start;
}

