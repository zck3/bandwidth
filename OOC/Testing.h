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

#ifndef _OOC_TEST_H
#define _OOC_TEST_H

#ifdef TESTING

#include <stdbool.h>
#include "Object.h"
#include "MutableArray.h"

#define MAX_EXPECTATIONS (128)

typedef struct {
	bool failed;
	const char *message;
} ExpectationResult;

typedef struct {
	// NOTE: These are expected to be literal strings.
	const char *classUnderTest;
	const char *test;
	const char *message;
	unsigned short failed;
	unsigned short nExpectations;
	bool isClass;
	bool expectationFailed[MAX_EXPECTATIONS];
	const char *expectationMessages[MAX_EXPECTATIONS];
} TestSuiteResult;

void Testing_suiteEnd (); 
void Testing_suiteStart (const char *suite, const char *name, bool isClass); 
void Testing_logExpectationResult (const char *call, bool passed); 
void Testing_skip (const char *suite, const char *name, const char*); 
int Testing_result ();

#define TEST_SKIP(CLASS,NAME,REASON) {\
		CLASS *object = new(CLASS); \
		const char *className = $(object, className); \
		release(object); \
		Testing_skip (className, NAME, REASON); \
	}

extern void Testing_destroy (Any *);
extern void Testing_init ();

// Start a test with CLASS_NAME being a C string, and TEST_NAME is a C string.
#define TEST_NON_CLASS(MODULE_NAME, TEST_NAME) \
	Testing_suiteEnd ();\
	Testing_suiteStart (MODULE_NAME, TEST_NAME, false); \
	for(int T=0;T<1;T++) 

// Start a test with CLASS being a class name, and TEST_NAME is a C string.
// NOTE: This is better, because if the class doesn't exist, the test won't compile.
#define TEST(CLASS, TEST_NAME) {\
		CLASS *object = new(CLASS); \
		const char *className = $(object, className); \
		release(object); \
		Testing_suiteEnd ();\
		Testing_suiteStart (className, TEST_NAME, true);\
	} for(int T=0;T<1;T++) 

#define EXPECT_TRUE(SUBJECT, MESSAGE) {\
	const char *message = (MESSAGE) ?: STRINGIFY(SUBJECT); \
	if (SUBJECT) {\
		Testing_logExpectationResult (message, true);\
	} else {\
		Testing_logExpectationResult (message, false);\
	}\
}

#define EXPECT_FALSE(SUBJECT, MESSAGE) {\
	const char *message = (MESSAGE) ?: STRINGIFY(SUBJECT); \
	if (SUBJECT) {\
		Testing_logExpectationResult (message, false);\
	} else {\
		Testing_logExpectationResult (message, true);\
	}\
}

#define EXPECT_ZERO(SUBJECT, MESSAGE) {\
	const char *message = (MESSAGE) ?: STRINGIFY(SUBJECT); \
	if (0 == (SUBJECT)) {\
		Testing_logExpectationResult (message, true);\
	} else {\
		Testing_logExpectationResult (message, false);\
	}\
}

#define EXPECT_NONZERO(SUBJECT, MESSAGE) {\
	const char *message = (MESSAGE) ?: STRINGIFY(SUBJECT); \
	if (0 != (SUBJECT)) {\
		Testing_logExpectationResult (message, true);\
	} else {\
		Testing_logExpectationResult (message, false);\
	}\
}

#define EXPECT_EQUAL_INT(SUBJECT, RESULT, MESSAGE) {\
	const char *message = (MESSAGE) ?: STRINGIFY(SUBJECT); \
	if (RESULT == (SUBJECT)) {\
		Testing_logExpectationResult (message, true);\
	} else {\
		Testing_logExpectationResult (message, false);\
	}\
}

#define EXPECT_NONEQUAL_INT(SUBJECT, RESULT, MESSAGE) {\
	const char *message = (MESSAGE) ?: STRINGIFY(SUBJECT); \
	if (RESULT != (SUBJECT)) {\
		Testing_logExpectationResult (message, true);\
	} else {\
		Testing_logExpectationResult (message, false);\
	}\
}

#define EXPECT_NONEQUAL_PTR(SUBJECT, RESULT) {\
	if ((void*)RESULT != (void*)(SUBJECT)) {\
		Testing_logExpectationResult (STRINGIFY(SUBJECT), true);\
	} else {\
		Testing_logExpectationResult (STRINGIFY(SUBJECT), false);\
	}\
}

#define EXPECT_NULL(SUBJECT, MESSAGE) {\
	const char *message = (MESSAGE) ?: STRINGIFY(SUBJECT); \
	if (NULL == (void*)(SUBJECT)) {\
		Testing_logExpectationResult (message, true);\
	} else {\
		Testing_logExpectationResult (message, false);\
	}\
}

#define EXPECT_NONNULL(SUBJECT, MESSAGE) {\
	const char *message = (MESSAGE) ?: STRINGIFY(SUBJECT); \
	if (NULL != (void*)(SUBJECT)) {\
		Testing_logExpectationResult (message, true);\
	} else {\
		Testing_logExpectationResult (message, false);\
	}\
}
 
#define EXPECT_EQUAL_STR(SUBJECT, RESULT, MESSAGE) {\
	const char *message = (MESSAGE) ?: STRINGIFY(SUBJECT); \
	if (!strcmp(RESULT, (SUBJECT))) {\
		Testing_logExpectationResult (message, true);\
	} else {\
		Testing_logExpectationResult (message, false);\
	}\
}

#define EXPECT_CLASS(SUBJECT,CLASS,MESSAGE) {\
	const char *actualClass = $(SUBJECT, className);\
	const char *soughtClass = STRINGIFY(CLASS);\
	const char *message = (MESSAGE) ?: STRINGIFY(CLASS);\
	if (!strcmp(actualClass, soughtClass)) {\
		Testing_logExpectationResult (message, true);\
	} else {\
		Testing_logExpectationResult (message, false);\
	}\
}

#define EXPECT_NEVER(MESSAGE) {\
	Testing_logExpectationResult (MESSAGE, false);\
}

#define EXPECT_ALWAYS(MESSAGE) {\
	Testing_logExpectationResult (MESSAGE, true);\
}

#endif
#endif
