/*============================================================================
  Testing, a mechanism for unit testing object-oriented C code.
  (Not yet a class).
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

#ifdef TESTING

#include "Testing.h"
#include "Log.h"
#include "Console.h"

#include <stdlib.h>
#include <string.h>
#include <ctype.h>

#if defined(__ANDROID__) && defined(__TERMUX__)
#define N_DOTS 56
#else
#define N_DOTS 79
#endif

static int Test_total = 0;	
static int Expect_total = 0;	
static int Test_failures = 0;	
static int Expect_failures = 0;	
static int Test_skips = 0;	
static bool runningTest = 0;	
static size_t arraySize = 0;	
static size_t resultCount = 0;	
static TestSuiteResult *testResults = NULL;

static void printWithDots (const char *string, unsigned lineLength)
{
	if (!string || !lineLength) {
		return;
	}

	int stringLengthWithoutEscapeSequences = 0;
	char *s = (char*) string;
	while (*s) {
		// Ignore escape sequences, format is "\033[#m" or "\033[##m",
		// as they have zero width on-screen.
		if (*s == 27) { // Escape
			int lengthToSkip = 1;
			if (s[1] == '[') {
				++lengthToSkip;
				if (isdigit(s[2])) {
					++lengthToSkip;
					if (isdigit(s[3])) {
						++lengthToSkip;
						if (s[4] == 'm') {
							++lengthToSkip;
						}
					} else {
						if (s[3] == 'm') {
							++lengthToSkip;
						}
					}
				}
			}
			if (lengthToSkip != 4 && lengthToSkip != 5) {
				Log_error(__FUNCTION__, "Invalid escape sequence");
				exit(2);
			}
			s += lengthToSkip;
			continue;
		}
		stringLengthWithoutEscapeSequences++;
		s++;
	}

	printf ("%s", string);

	int n_dots_needed = lineLength - stringLengthWithoutEscapeSequences;
	for (int i=0; i < n_dots_needed; i++) {
		putchar ('.');
	}
}

void Testing_suiteEnd ()
{
	if (!runningTest) {
		Test_total = 0;
		Test_failures = 0;
		Expect_total = 0;
		Expect_failures = 0;
		runningTest = false;
	}
}

int Testing_result () 
{
	if (!Test_failures) {
		printf("Test Result: %sALL %d TESTS PASSED%s\n", 
			ANSIForegroundGreen,
			Test_total,
			ANSIForegroundPlain);
		printf("Expectation Result: %sALL %d EXPECTATIONS MET%s\n", 
			ANSIForegroundGreen,
			Expect_total,
			ANSIForegroundPlain);
	} else {
		printf("Test Result: %s%s%d TESTS PASSED, %s%d FAILED%s\n", 
			ANSIForegroundBold,
			ANSIForegroundGreen,
			Test_total - Test_failures, 
			ANSIForegroundRed,
			Test_failures,
			ANSIForegroundPlain);
		printf("Expectation Result: %s%s%d EXPECTATIONS MET, %s%d UNMET%s\n",
			ANSIForegroundBold,
			ANSIForegroundGreen,
			Expect_total, 
			ANSIForegroundRed,
			Expect_failures,
			ANSIForegroundPlain);
	}

#ifdef TESTING_VERBOSE
	printf ("Test Result: %d run, of which %d failed\n", Test_total, Test_failures);
	printf ("Expectation Result: %d EXPECTATIONS MET, %d UNMET\n", Expect_total, Expect_failures);
#endif
	fflush(NULL);

	return Test_failures == 0 ? 0 : -1;
}

void Testing_skip (const char *class, const char *name, const char*reason)
{
	if (!testResults) {
		Testing_init();
	}

	Testing_suiteEnd ();

	++Test_total;
	++Test_skips;

	// TODO: save name of skipped test

	char buffer[100];
	snprintf (buffer, sizeof(buffer), "%s%sClass %s%s%s, %s%sTest %s%s %sSKIPPED%s",
		ANSIForegroundBlue,
		ANSIForegroundBold,
		ANSIForegroundMagenta,
		class, 
		ANSIForegroundPlain,
		ANSIForegroundBold,
		ANSIForegroundBlue,
		ANSIForegroundMagenta,
		name,
		ANSIForegroundRed,
		ANSIForegroundYellow);

	printWithDots (buffer, N_DOTS);
	puts (ANSIForegroundPlain);

	if (reason) {
		puts (reason);
	}
}

void Testing_suiteStart (const char *class, const char *name, bool isClass)
{
	if (!testResults) {
		Testing_init ();
	}

	Testing_suiteEnd ();

	++Test_total;

	// Begin new suite by storing class/test names.
	testResults[resultCount].classUnderTest = class;
	testResults[resultCount].test = name;
	testResults[resultCount].isClass = isClass;
	runningTest = true;

	// Print something to indicate what test has begun.
	char buffer[100];
	snprintf (buffer, sizeof(buffer), "%s%s%s %s%s%s, %s%sTest %s%s%s",
		ANSIForegroundBlue,
		ANSIForegroundBold,
		isClass ? "Class" : "Module",
		ANSIForegroundMagenta,
		testResults[resultCount].classUnderTest,
		ANSIForegroundPlain,
		ANSIForegroundBold,
		ANSIForegroundBlue,
		ANSIForegroundMagenta,
		testResults[resultCount].test,
		ANSIForegroundYellow);

	printWithDots (buffer, N_DOTS);
	puts (ANSIForegroundPlain);
}

void Testing_logExpectationResult (const char *message, bool passed)
{
	if (!testResults) {
		Log_error (__FUNCTION__, "Missing test results array.");
		return;
	}

	if (!runningTest) {
		Log_error(__FUNCTION__, "Expectation logged outside of test suite.");
		exit(3);
	}

	if (!passed) {
		Test_failures++;
	}

	testResults[resultCount].failed++;

	int n = testResults[resultCount].nExpectations;
	testResults[resultCount].expectationFailed[n] = true;
	testResults[resultCount].expectationMessages[n] = message;
	testResults[resultCount].nExpectations = n+1;

	Expect_total++;
	if (!passed) {
		Expect_failures++;
	}

	printf ("%s (%s): %s%s%s%s%s\n", 
		testResults[resultCount].classUnderTest,
		testResults[resultCount].test,
		passed ?  ANSIForegroundGreen: ANSIForegroundRed,
		passed ? "Passed" : "Failed",
		"\033[0m",
		!passed? " -> " : "",
		!passed? message : ""
		);
	fflush(NULL);
}

void Testing_init ()
{
	Test_total = 0;
	Test_failures = 0;
	Expect_total = 0;
	Expect_failures = 0;

	runningTest = false;

#define DEFAULT_N_TESTS 128
	resultCount = 0;
	arraySize = DEFAULT_N_TESTS;
	testResults = calloc(arraySize, sizeof(TestSuiteResult));
	if (!testResults) {
		Log_error(__FUNCTION__, "Unable to allocate RAM for test results.");
	}
}

#endif
