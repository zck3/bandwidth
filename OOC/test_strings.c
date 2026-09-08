/*============================================================================
  OOC, a library for object-oriented programming using C.
  Copyright (C) 2023-2024, 2026 by Zack T Smith.

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

#ifdef TESTING

#include <stdio.h>
#include <string.h>
#include <time.h>

#include "String.h"
#include "MutableString.h"
#include "Testing.h"

int main()
{
	// Empty string
	TEST(String,"empty") {
		// Empty string
		String *str = new(String);
		size_t len = $(str, length);
		EXPECT_ZERO(len, "New string should have 0 length");

		release (str);
		EXPECT_NULL(str, "Released string's pointer should be null");

		str = new(String);
		EXPECT_TRUE($(str, equalsCString, ""), "Empty string should be equal to literal 0-length string");
		EXPECT_FALSE($(str, equalsCString, "asdfasdf"), "Empty string should not be equal to non-empty C string");
		release(str);
	}

	// Empty MutableString
	TEST(MutableString,"empty") {
		MutableString *mut = new(MutableString);
		size_t len = $(mut, length);
		EXPECT_ZERO(len, "New mutable string should have 0 length");
		release (mut);
		EXPECT_NULL(mut, "Released mutable string's pointer should be null");

		mut = new(MutableString);
		EXPECT_TRUE($(mut, equalsCString, ""), "Empty string should be equal to literal 0-length string");
		EXPECT_FALSE($(mut, equalsCString, "asdfasdf"), "Empty string should not be equal to non-empty C string");
		release(mut);
	}

	// Literal wide string
	TEST(String,"literal wide") {
		// Empty string
		String *str = String_withWide (L"ай у фæлварæн");
		size_t len = $(str, length);
		EXPECT_EQUAL_INT(len,13, "New string with Unicode characters should have a length of 13");

		Char ch = $(str, at, 5);
		EXPECT_EQUAL_INT(ch, L'ф', "Check Unicode char at index 5");
		ch = $(str, at, 4);
		EXPECT_NONEQUAL_INT(ch, L'X', "Check Unicode char at index 4");

		ch = $(str, at, -1);
		EXPECT_ZERO(ch, "Char at negative index should have 0 value");

		ch = $(str, at, 13);
		EXPECT_ZERO(ch, "Char at location beyond end of string should have 0 value");

		release (str);
		EXPECT_NULL(str, "Released string's pointer should be null");
	}

	// Literal string
	TEST(String,"literal cstring") {
		String *str = _String("This is a test\n"); // 15 chars
		size_t len = $(str, length);
		EXPECT_EQUAL_INT(len, 15, "New string from C-String should have same length a C-String");

		Char ch = $(str, at, 0);
		EXPECT_EQUAL_INT(ch, 'T', "Check char at index 0");

		EXPECT_TRUE($(str, hasCStringPrefix, "This "), "String has correct prefix");
		EXPECT_TRUE($(str, hasCStringSuffix, "t\n"), "String has correct suffix");

		ch = $(str, at, -1); // XX Add end-of-string indexing?
		EXPECT_ZERO(ch, "Char at negative index should have 0 value");
		
		ch = $(str, at, 100);
		EXPECT_ZERO(ch, "Char at location beyond end of string should have 0 value");
		
		ch = $(str, at, 14);
		EXPECT_EQUAL_INT(ch, '\n', "Check char at index 14");
	
		release(str);
		EXPECT_NULL(str, "Released string's pointer should be null");
	}

	// MutableString literal C string
	TEST(MutableString,"reverse") {
		MutableString *mut = _MutableString("This is a test"); // 14 chars
		$(mut, reverse);
		EXPECT_TRUE($(mut, equalsCString, "tset a si sihT"), "String correctly reversed");
		$(mut, reverse);

		$(mut, truncateAt, 1);
		size_t len = $(mut, length);
		EXPECT_EQUAL_INT(len, 1, "After truncating mutable string at index 1, length should be 1");

		$(mut, reverse);
		EXPECT_TRUE($(mut, equalsCString, "T"), "Single-char string correctly reversed"); // Last char.

		release(mut);
		EXPECT_NULL(mut, "Released mutable string's pointer should be null");
	}

	// MutableString literal C string
	TEST(MutableString,"literal cstring") {
		MutableString *mut = MutableString_withCString("this is a test\n"); // 15 chars
		size_t len = $(mut, length);
		EXPECT_EQUAL_INT(len, 15, "New mutable string from C-String should have correct length");

		String *second = _String("this is a test\n"); 
		EXPECT_TRUE($(second, equals, mut), "Mutable string matches non-mutable string that has same chars");
		release(second);
		EXPECT_NULL(second, "Released second string's pointer should be null");

		// Character at MutableString location
		Char ch = $(mut, at, 0);
		EXPECT_EQUAL_INT(ch, 't', "Check char at index 0");

		ch = $(mut, at, 100);
		EXPECT_ZERO(ch, "Char at location beyond end of string should have 0 value");

		ch = $(mut, at, 14);
		EXPECT_EQUAL_INT(ch, '\n', "Check char at index 14");

		// Insert character at location
		$(mut, insertCharacterAt, 'A', 0);
		ch = $(mut, at, 0);
		EXPECT_EQUAL_INT(ch, 'A', "After inserting char, verify it's present in string");
		ch = $(mut, at, 1);
		EXPECT_EQUAL_INT(ch, 't', "After inserting char, verify next char is present");

		release (mut);
		EXPECT_NULL(mut, "Released mutable string's pointer should be null");
	}

	// Explode string -> array.
	TEST(String,"simple explode") {
		String *str = _String("1,2,3");
		Array *ary = $(str, explode, ',', false, false, false);
		size_t count = $(ary, count);
		$(str, print, NULL); printf (" explodes into %zu objects.\n", count);
		EXPECT_EQUAL_INT(count,3, "String explode should result in array of 3 items");
		release (ary);
		EXPECT_NULL(ary, "Released array's pointer should be null");

		release (str);
		EXPECT_NULL(str, "Released string's pointer should be null");
	}

	// Explode with separator at end(s).
	TEST(String,"explode") {
		String *str = _String("1,2,");
		Array *ary = $(str, explode, ',', true, false, false);
		size_t count = $(ary, count);
		$(str, print, NULL); printf (" explodes into %zu objects.\n", count);
		EXPECT_EQUAL_INT(count,3, "String explode with empty last string should result in array of 3 items");
		release (ary);
		EXPECT_NULL(ary, "Released array's pointer should be null");

		//-------------
		// Explode test
		str = _String(",fubar");
		ary = $(str, explode, ',', true, false, false);
		count = $(ary, count);
		EXPECT_EQUAL_INT(count,2, "String explode with empty 1st string should result in array of 2 items");
		release (ary);
		EXPECT_NULL(ary, "Released array's pointer should be null");

		//-------------
		// Explode test
		str = _String(",2,");
		ary = $(str, explode, ',', true, false, false);
		count = $(ary, count);
		$(str, print, NULL); printf (" explodes into %zu objects.\n", count);
		EXPECT_EQUAL_INT(count,3, "String explode with empty 1st and last strings should result in array with 3 items");

		release (ary);
		release (str);
		EXPECT_NULL(str, "Released string's pointer should be null");
		EXPECT_NULL(ary, "Released array's pointer should be null");
	}

	// Explode with only separator.
	TEST(String,"explode where string consists only of the separator") {
		// Explode test
		String *str = _String(",");
		Array *ary = $(str, explode, ',', true, false, false);
		size_t count = $(ary, count);
		
		EXPECT_EQUAL_INT(count,2, "String explode where string consists only of the separator should give array of 2 items");

		release (ary);
		release (str);
		EXPECT_NULL(str, "Released string's pointer should be null");
		EXPECT_NULL(ary, "Released array's pointer should be null");
	}

	TEST(String,"prefix suffix") {
		String *str = _String(",fubar");

		EXPECT_TRUE($(str, hasPrefix, _String(",fu")),
			"String has correct prefix");
		EXPECT_TRUE($(str, hasSuffix, _String("r")),
			"String has correct suffix");
		EXPECT_TRUE($(str, hasCStringSuffix, "bar"),
			"String has correct suffix (C String)");
		EXPECT_TRUE($(str, hasCStringPrefix, ",f"),
			"String has correct prefix (C String)");
		EXPECT_TRUE($(str, containsChar, 'f'),
			"String contains specific present char");
		EXPECT_FALSE($(str, containsChar, 'X'),
			"String doesn't contain specific missing char");

		release (str);
		EXPECT_NULL(str, "Released string's pointer should be null");
	}

	TEST(String,"explode respecting quoted text") {
		// Explode test with quotes.
		String *str = _String("\"hello\"\"hello\",2,\"< hello >\"");
		Array *ary = $(str, explode, ',', true, false, false);
		size_t count = $(ary, count);
		
		EXPECT_EQUAL_INT(count,3, "String explode that respects quotes should give an array fo 3 items");

		release (ary);
		release (str);
		EXPECT_NULL(str, "Released string's pointer should be null");
		EXPECT_NULL(ary, "Released array's pointer should be null");
	}

	TEST(String,"explode respecting quoted text") {
		// Explode test with quotes.
		String *str = _String("\",\"\",\",2,\", hello ,\"");
		Array *ary = $(str, explode, ',', true, false, false);
		size_t count = $(ary, count);
		
		EXPECT_EQUAL_INT(count,3, "String explode that respects quotes should give an array of 3 items");

		release (ary);
		release (str);
		EXPECT_NULL(ary, "Released array's pointer should be null");
		EXPECT_NULL(str, "Released string's pointer should be null");
	}

	// Explode test with trimming.
	TEST(String,"explode with trimming") {
		String *str = _String("   asdf	, zxcv\r	, eee \f");
		Array *ary = $(str, explode, ',', true, true, false);
		size_t count = $(ary, count);
		
		EXPECT_EQUAL_INT(count,3, "String explode that trims whitespace should give an array of 3 items");
		String *first = $(ary, first);
		String *second = $(ary, at, 1);
		String *third = $(ary, last);
		EXPECT_TRUE($(first, equalsCString, "asdf"), "First trimmed string should not have any whitespace");
		EXPECT_TRUE($(second, equalsCString, "zxcv"), "Second trimmed string should not have any whitespace");
		EXPECT_TRUE($(third, equalsCString, "eee"), "Third trimmed string should not have any whitespace");

		release (ary);
		release (str);
		EXPECT_NULL(ary, "Released array's pointer should be null");
		EXPECT_NULL(str, "Released string's pointer should be null");
	}

	TEST(String,"explode not respecting quoted text") {
		// Explode test without respecting quotes.
		String *str = _String("\",\"\",\"\"\"");
		Array *ary = $(str, explode, ',', false, false, false);
		size_t count = $(ary, count);
		// $(str, print, NULL); printf (" explodes into %lu objects.\n", count);

		EXPECT_EQUAL_INT(count,3, "String explode that trims whitespace should give an array of 3 items");

		release (ary);
		release (str);
		EXPECT_NULL(ary, "Released array's pointer should be null");
		EXPECT_NULL(str, "Released string's pointer should be null");
	}

	TEST(String,"extract prefix") {
		String *str = _String("This is a test of the substrings functions.");

		String *prefix = $(str, prefixOfLength, 5);
		EXPECT_TRUE($(prefix, equalsCString, "This "), "Prefix should be equal to literal string");
		release(prefix);

		prefix = $(str, prefixOfLength, 1);
		EXPECT_TRUE($(prefix, equalsCString, "T"), "Prefix should be equal to literal 1-char string");
		release(prefix);

		prefix = $(str, prefixOfLength, 0);
		EXPECT_TRUE($(prefix, equalsCString, ""), "Zero-length prefix should be equal to literal 0-length string");
		release(prefix);

		prefix = $(str, prefixOfLength, -20);
		EXPECT_TRUE($(prefix, equalsCString, ""), "Negative-length prefix should be equal to literal 0-length string");
		release(prefix);

		release (str);
		EXPECT_NULL(str, "Released string's pointer should be null");
	}

	TEST(String,"extract suffix") {
		String *str = _String("This is a test of the substrings functions.");

		String *suffix = $(str, suffixOfLength, 5);
		EXPECT_TRUE($(suffix, equalsCString, "ions."), "Suffix should be equal to literal string");
		release(suffix);

		suffix = $(str, suffixOfLength, 1);
		EXPECT_TRUE($(suffix, equalsCString, "."), "Suffix should be equal to literal 1-char string");
		release(suffix);

		suffix = $(str, suffixOfLength, 0);
		EXPECT_TRUE($(suffix, equalsCString, ""), "Zero-length suffix should be equal to literal 0-length string");
		release(suffix);

		suffix = $(str, suffixOfLength, -20);
		EXPECT_TRUE($(suffix, equalsCString, ""), "Negative-length suffix should be equal to literal 0-length string");
		release(suffix);

		release (str);
		EXPECT_NULL(str, "Released string's pointer should be null");
	}

	TEST(String,"extract substring") {
		const char *original = "This is a test of the substrings functions."; // Length 43 chars
		String *str = _String(original);

		String *substring = $(str, substring, 5, 10);
		EXPECT_TRUE($(substring, equalsCString, "is a test "), "Substring should be equal to literal string");
		release(substring);

		substring = $(str, substring, -5, 10);
		EXPECT_TRUE($(substring, equalsCString, "This "), "Negative-index substring should be equal to literal string");
		release(substring);

		substring = $(str, substring, 40, 10);
		EXPECT_TRUE($(substring, equalsCString, "ns."), "Large-index substring should be equal to literal string");
		release(substring);

		substring = $(str, substring, 50, 10);
		EXPECT_TRUE($(substring, equalsCString, ""), "Large-index substring should be equal to literal 0-length string");
		release(substring);

		substring = $(str, substring, -50, 10);
		EXPECT_TRUE($(substring, equalsCString, ""), "Negative-index substring should equal literal 0-length string");
		release(substring);

		substring = $(str, substring, -5, 100);
		EXPECT_TRUE($(substring, equalsCString, original), "Negative-index substring should equal entire literal string");
		release(substring);

		release (str);
		EXPECT_NULL(str, "Released string's pointer should be null");
	}

	int retval = Testing_result();
	deallocateClasses();
	return retval;
}

#endif // TESTING
