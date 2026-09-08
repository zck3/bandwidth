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
#include <stdbool.h>
#include <time.h>
#include <signal.h>
#include <errno.h>

#include "Double.h"
#include "Testing.h"

//==================================================
// NOTE: Currently mprotect is only called on Linux.
//==================================================

static struct sigaction old_action;

static void signal_handler(int sig, siginfo_t *si, void *unused)
{
	printf("Successfully caused SIGSEGV by trying to write into the Double virtual table at address: 0x%lx\n", (unsigned long) si->si_addr);
	fflush(NULL);

	deallocateClasses();

	// Successful test.
	exit(0);
}

int main()
{
	TEST_NON_CLASS("MemoryProtection","overwrite method pointer") {
		Double *object;

		object = new(Double);
		EXPECT_NONNULL(object, "Double object successfully created");

		// Set up a signal handler to catch our illegal attempt to overwrite a method pointer.
		struct sigaction sa;
		sa.sa_flags = SA_SIGINFO;
		sigemptyset (&sa.sa_mask);
		sa.sa_sigaction = signal_handler;
		EXPECT_NONEQUAL_INT(-1, sigaction(SIGSEGV, &sa, &old_action), "The sigaction call should not return -1");

		// Overwrite a method pointer, which should cause signal_handler to be called.
		DoubleClass *class = object->is_a;
		printf("The Double virtual table address is 0x%lx\n", (unsigned long) class);
		EXPECT_NONNULL(class, "Double object's is_a pointer is non-null");
		class->destroy = NULL;

		// We shouldn't reach here.
		// THIS TEST ONLY WORKS ON LINUX AT PRESENT.
		EXPECT_FALSE(true, "We shouldn't reach here");

		// Attempt to modify the class size, which should cause signal_handler to be called.
		class->classSize++;

		// Won't reach here.
		release(object);
	}

	int retval = Testing_result();
	deallocateClasses();

	// If no SIGSEGV occurs that means the test failed.
	return retval;
}

#endif // TESTING
