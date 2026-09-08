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

#include "URL.h"
#include "Testing.h"
#include "Utility.h"

int main()
{
	TEST(URL,"empty") {
		URL *url;
		url = new(URL);
		//$(url, describe, NULL); puts("");
		EXPECT_EQUAL_INT($(url, scheme), URLSchemeNone, 
			"New URL object's scheme should be None");

		String *str = $(url, string);
		EXPECT_ZERO($(str, length), "New URL has empty URL string");

		release(url);
		EXPECT_NULL(url, "Released URL object's pointer should be null");
	}

	TEST(URL,"schemes") {
		typedef struct {
			const char *url;
			URLScheme scheme;
		} SchemeData;
		SchemeData testData [] = {
			{ "tel:123456", URLSchemeTelephone },
			{ "fiLe:///etc/hosts", URLSchemeFile},
			{ "https://xyz.be", URLSchemeHTTPS },
			{ "httP://fubar.foo?nu", URLSchemeHTTP },
			{ "FTP://fubar.foo?nu", URLSchemeFTP },
			{ "Mailto:fubar@foo.nu", URLSchemeMailto },
			{ "irc://fubar@foo.nu", URLSchemeIRC },
			{ "javascript:asldkfjlkads.nu", URLSchemeJavascript },
			{ "javascript//asldkfjlkads.nu", URLSchemeNone },
			{ "sms:171717", URLSchemeSMS },
			{ "sms:", URLSchemeNone },
			{ "file:", URLSchemeNone },
			{ "file://", URLSchemeNone },
			{ "file:///", URLSchemeFile },
			{ "file://../../etc/hosts", URLSchemeFile },
			{ "file:///etc/hosts", URLSchemeFile },
			{ "", URLSchemeNone },
			{ "://", URLSchemeNone },
			{ "abc://asdlfjalds", URLSchemeNone },
			{ "ftp:/asdlfjalds", URLSchemeNone },
			{ "ftp//asdlfjalds", URLSchemeNone },
			{ "http:asdlfjalds", URLSchemeNone },
			{ "http:/asdlfjalds", URLSchemeNone },
			{ "http//asdlfjalds", URLSchemeNone },
			{ "https:/asdlfjalds", URLSchemeNone },
		};

		URL *url = NULL;
		int bad_schemes = 0;
		for (unsigned i=0; i < sizeof(testData)/sizeof(SchemeData); i++) {
			url = _URL(testData[i].url);
			// $(url, describe, NULL); puts(""); fflush(NULL);
			if (testData[i].scheme != $(url, scheme)) {
				bad_schemes++;
			}
		}
		EXPECT_ZERO(bad_schemes, "No bad URL schemes found");

		release(url);
		release(url);
		release(url);
		release(url);
		EXPECT_NULL(url, "Released URL object's pointer should be null");
	}

	TEST(URL,"string") {
		URL *url;
		url = _URL("http://yahoo.com");
		EXPECT_EQUAL_INT($(url, scheme), URLSchemeHTTP, 
			"New URL object's scheme should be HTTP");
			
		String *str = $(url, string);
		String *str2 = _String("http://yahoo.com");
		EXPECT_TRUE($(str, equals, str2), "String provided during URL initialization is correct");

		release(str);
		release(str2);
		release(url);
		EXPECT_NULL(str, "Released URL string's pointer should be null");
		EXPECT_NULL(str2, "Released literal string's pointer should be null");
		EXPECT_NULL(url, "Released URL object's pointer should be null");
	}

#ifdef __linux__
	if (Network_isReachable ()) {
#endif
	TEST(URL,"HTTP GET") {
		URL *url = _URL("https://google.com");
		unsigned statusCode = 0;

		Data *data = $(url, synchronousGet, &statusCode);
		EXPECT_NONNULL(data, "Synchronous HTTP fetch should return Data object");

		printf ("HTTP status code %u, got %lu bytes\n", statusCode, (unsigned long) $(data, length));

		release(data);
		release(url);
		EXPECT_NULL(url, "Released URL object's pointer should be null");
		EXPECT_NULL(data, "Released Data object's pointer should be null");
	}
#ifdef __linux__
	} // Network_isReachable 
	else {
		TEST_SKIP(URL,"HTTP GET", "Network wasn't reachable");
	}
#endif

#ifdef __linux__
	if (Network_isReachable ()) {
#endif
	TEST(URL,"HTTPS GET") {
		URL *url = _URL("https://yahoo.com");

		EXPECT_EQUAL_INT($(url, scheme), URLSchemeHTTPS,
			"New URL object's scheme should be HTTPS");
			
		unsigned statusCode = 0;
		Data *data = $(url, synchronousGet, &statusCode);
		EXPECT_NONNULL(data, "Synchronous HTTPS fetch should return Data object");
		
		char *string = $(data, asCString);
		printf ("HTTP status code %u, got %lu bytes\n", statusCode, (unsigned long) $(data, length));
		free (string);

		release(url);
		release(data);
		EXPECT_NULL(url, "Released URL object's pointer should be null");
		EXPECT_NULL(data, "Released Data object's pointer should be null");
	}
#ifdef __linux__
	} // Network_isReachable 
	else {
		TEST_SKIP(URL,"HTTPS GET", "Network wasn't reachable");
	}
#endif

	TEST(URL,"withString") {
		MutableString *string1 = new(MutableString);
		$(string1, appendCString, "https://");
		$(string1, appendCString, "google");
		$(string1, appendCString, ".com");
		URL *url = URL_withString(string1);
		String *string2 = $(url, string);
		EXPECT_TRUE($(string2, equals, string1), "String from URL should match string used to create URL");
		release(string1);
		release(string2);
		release(url);
	}

	int retval = Testing_result();
	deallocateClasses();
	return retval;
}

#endif // TESTING
