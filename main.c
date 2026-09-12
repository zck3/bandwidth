/*============================================================================
  bandwidth, a benchmark to measure memory transfer bandwidth.
  Copyright (C) 2005-2024, 2026 by Zack T Smith.

  This program is free software; you can redistribute it and/or modify
  it under the terms of the GNU General Public License as published by
  the Free Software Foundation; either version 3 of the License, or
  (at your option) any later version.

  This program is distributed in the hope that it will be useful,
  but WITHOUT ANY WARRANTY; without even the implied warranty of
  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
  GNU General Public License for more details.

  You should have received a copy of the GNU General Public License
  along with this program; if not, write to the Free Software
  Foundation, Inc., 51 Franklin St, Fifth Floor, Boston, MA  02110-1301  USA

  The author may be reached at 3 at zs3 dot me.
 *===========================================================================*/

#include <string.h>
#include <unistd.h>
#include <getopt.h>

#define DEFAULT_GRAPH_WIDTH 1440
#define DEFAULT_GRAPH_HEIGHT 900

#include "defs.h"

#include "OOC/Console.h"
#include "OOC/OS.h"
#include "OOC/CPUX86.h"
#include "OOC/CPUARM.h"
#include "OOC/CPURISCV.h"
#include "OOC/CPUPowerPC.h"
#include "OOC/Hardware.h"
#include "OOC/colors.h"

#include "BenchmarkX86.h"
#include "BenchmarkARM.h"
#include "BenchmarkRISCV.h"
#include "BenchmarkPPC.h"
#include "testRunner.h"
#include "systemInfo.h"

ProgramOptions options;

#if defined(x86)
  BenchmarkX86 *benchmarks;
#elif defined(__arm__) || defined(__aarch64__)
  BenchmarkARM *benchmarks;
#elif defined(RISCV64) 
  BenchmarkRISCV *benchmarks;
#elif defined(__powerpc__) 
  BenchmarkPPC *benchmarks;
#else
  Benchmark *benchmarks;
#endif

#if defined(__WIN32__) || defined(__WIN64__)
#include <w32api/windows.h> // Cygwin
#include <inttypes.h>
#endif

Console *console = NULL;

static Hardware *hardware = NULL;
static CPU *cpu = NULL;

//----------------------------------------------------------------------------
// Name:	usage
//----------------------------------------------------------------------------
void usage ()
{
	puts ("Usage: bandwidth (options)");
	puts ("The output graph is in bandwidth.bmp.");
	puts ("Options include:");
	puts ("  --size WIDTHxHEIGHT");
	puts ("  --title string");
	puts ("  --csv file");
	puts ("  --slow / -s");
	puts ("  --fast / -f");
	puts ("  --faster / -r");
	puts ("  --fastest / -F");
	puts ("  --main-memory-only / -m");
#ifdef x86
	puts ("  --nosse2 / -2");
	puts ("  --nosse4 / -4");
	puts ("  --noavx / -X");
	puts ("  --noavx512 / -5");
#endif
	puts ("  --noread / -R");
	puts ("  --nowrite / -W");
	puts ("  --norandom / -D");
	puts ("  --nocopy / -C");
	puts ("  --nonontemporal / -T");
	puts ("  --nograph / -G");
	puts ("  --nomemtest");
	puts ("  --longmemtest");
	puts ("  --noregister");
	puts ("  --unlimited");
	puts ("  --reverse");
	puts ("  --invert");
	puts ("  --rotate");
	puts ("  --noviewer");
	puts ("  --nice");
	exit (-1);
}

//----------------------------------------------------------------------------
// Name:	main
//----------------------------------------------------------------------------
int
main (int argc, char **argv)
{
	if (argc < 1) { // Rare attack method.
		return -1;
	}

	console = new(Console);
	$(console, puts, "This is bandwidth version " RELEASE);
	$(console, puts, "Copyright (C) 2005-2024, 2026 by Zack T Smith.");
	$(console, newline);
	$(console, puts, "This software is covered by the GNU Public License.");
	$(console, puts, "It is provided AS-IS, use at your own risk.");
	$(console, puts, "See the file LICENSE for more information.");
	$(console, newline);
	$(console, flush);

	ooc_bzero (&options, sizeof(ProgramOptions));

// Mode to be nice and to keep CPU temperature low, since Mac x86 laptops tend to overheat.
#if defined(__APPLE__) && !defined(__aarch64__)
	options.nice_mode = true;
#endif

	options.graphWidth = DEFAULT_GRAPH_WIDTH;
	options.graphHeight = DEFAULT_GRAPH_HEIGHT;

	options.perform_mainregister_nontemporal_tests = true;
	options.perform_128bit_tests = true;
	options.perform_128bit_nontemporal_tests = true;
	options.perform_256bit_tests = true;
	options.perform_256bit_nontemporal_tests = true;
	options.perform_512bit_tests = true;
	options.perform_512bit_nontemporal_tests = true;
	options.perform_read_tests = true;
	options.perform_write_tests = true;
	options.perform_copy_tests = true;
	options.perform_register_and_stack_tests = true;
	options.perform_random_tests = true;
	options.perform_direct_tests = true;
	options.limit_at_128MB = true;
	options.launch_viewer = true;

	options.usec_per_test = 5000000;	// 5 seconds per test.
	options.outputMode = OUTPUT_MODE_GRAPH;

	hardware = new(Hardware);

#if defined(x86)
	cpu = (CPU*) new(CPUX86);
	benchmarks = new(BenchmarkX86);
#elif defined(__arm__) || defined(__aarch64__)
	cpu = (CPU*) new(CPUARM);
	benchmarks = new(BenchmarkARM);
#elif defined(RISCV64) 
	cpu = (CPU*) new(CPURISCV);
	benchmarks = new(BenchmarkRISCV);
#elif defined(__powerpc__) 
	cpu = (CPU*) new(CPUPowerPC);
	benchmarks = new(BenchmarkPPC);
#else
	fprintf(stderr, "Unsupported instruction set architecture.\n");
	exit(1);
#endif

	const char *userSpecifiedGraphTitle = NULL;

	struct option getopt_long_options[] = {
		{"title", required_argument, NULL, 't'},
		{"size", required_argument, NULL, 'z'},
		{"csv", required_argument, NULL, 'c'},

		{"nice", no_argument, NULL, 'n'},
		{"slow", no_argument, NULL, 's'},
		{"longmemtest", no_argument, NULL, 'l'},
		{"reverse", no_argument, NULL, 'e'},
		{"rotate", no_argument, NULL, 'o'},
		{"invert", no_argument, NULL, 'i'},
		{"unlimited", no_argument, NULL, 'u'},
		{"multithreaded", no_argument, NULL, 'M'},
		{"main-memory-only", no_argument, NULL, 'm'},
		{"fast", no_argument, NULL, 'f'},
		{"faster", no_argument, NULL, 'r'},
		{"fastest", no_argument, NULL, 'F'},
		{"diagnostic", no_argument, NULL, 'd'},
		{"noregister", no_argument, NULL, 'E'},
		{"noviewer", no_argument, NULL, 'V'},
		{"norandom", no_argument, NULL, 'D'},
		{"noread", no_argument, NULL, 'R'},
		{"nowrite", no_argument, NULL, 'W'},
		{"nocopy", no_argument, NULL, 'C'},
		{"nonontemporal", no_argument, NULL, 'T'},
		{"nograph", no_argument, NULL, 'G'},
		{"nomemtest", no_argument, NULL, 'N'},
		{"nosse2", no_argument, NULL, '2'},
		{"nosse4", no_argument, NULL, '4'},
		{"noavx", no_argument, NULL, 'X'},
		{"noavx512", no_argument, NULL, '5'},
		{"help", no_argument, NULL, 'h'},
		{NULL, 0, NULL, 0}
	};

	int opt;
	int width, height;
	while (-1 != (opt = getopt_long(argc, argv, "t:z:c:245ACDEFGMRTVWXdefhilmnorsu", getopt_long_options, NULL))) {
		switch (opt) {
			case 'c':
				options.outputMode |= OUTPUT_MODE_CSV;
				options.csv_file_path = optarg;
				break;
			case 't':
				userSpecifiedGraphTitle = optarg;
				break;
			case 'z': 
				if (2 == sscanf (optarg, "%dx%d", &width, &height)) {
					if (width >= 640 && width <= 16384 && height >= 480 && height <= 16384) {
						options.graphWidth = width;
						options.graphHeight = height;
					} else {
						fprintf (stderr, "Invalid graph dimensions %dx%d\n", width, height);
					}
				}
				break;
			case 'f': 
				options.usec_per_test = 1000000; // 1 second per test.
				break;
			case 'r': 
				options.usec_per_test = 500000; // 0.5 second per test.
				break;
			case 'F': 
				options.usec_per_test = 50000; // 0.05 second per test.
				break;
			case 'u':
				if (sizeof(long) == 8) {
					options.limit_at_128MB = false;
				}
				break;
			case 'M':
				options.multithreaded = true;
				break;
			case 'm':
				options.only_main_memory = true;
				break;
			case '2': // No SSE2
				options.perform_128bit_tests = false;
				options.perform_128bit_nontemporal_tests = false;
				options.perform_256bit_tests = false;
				options.perform_512bit_tests = false;
				break;
			case '4': // No SSE4
				options.perform_128bit_nontemporal_tests = false;
				options.perform_256bit_tests = false;
				options.perform_512bit_tests = false;
				break;
			case 'X': // No AVX
				options.perform_256bit_tests = false;
				options.perform_512bit_tests = false;
				break;
			case '5': // No AVX152
				options.perform_512bit_tests = false;
				break;
			case 'V': options.launch_viewer = false; break;
			case 'R': options.perform_read_tests = false; break;
			case 'W': options.perform_write_tests = false; break;
			case 'D': options.perform_random_tests = false; break;
			case 'C': options.perform_copy_tests = false; break;
			case 'T':
				options.perform_mainregister_nontemporal_tests = false;
				options.perform_128bit_nontemporal_tests = false;
				options.perform_256bit_nontemporal_tests = false;
				options.perform_512bit_nontemporal_tests = false;
				break;
			case 'E':
				options.perform_register_and_stack_tests = false;
				break;
			case 'n': options.nice_mode = true; break;
			case 'd': 
				options.usec_per_test = 10000;
				options.diagnostic_mode = true; 
				break;
			case 's': options.usec_per_test = 20000000; /* 20 seconds */ break;
			case 'i': options.do_invert_graph = true; break;
			case 'o': options.do_rotate_graph = true; break;
			case 'e': options.reverse_chunk_size_order = true; break;
			case 'h': usage (); break;
			case 'G': options.outputMode &= ~OUTPUT_MODE_GRAPH; break;
			default:
				usage();
				break;
		}
	}
	if (!$(cpu, has128bitVectors)) {
		options.perform_128bit_tests = false;
		options.perform_128bit_nontemporal_tests = false;
	}
	if (!$(cpu, has256bitVectors)) {
		options.perform_256bit_tests = false;
	}
	if (!$(cpu, has512bitVectors)) {
		options.perform_512bit_tests = false;
	}

	$(console, puts, "Notation: B = byte, kB = 1024 B, MB = 1048576 B.");
	$(console, newline);
	$(console, flush);

	MutableString *title = getSystemInfo(cpu, hardware);

        if ((options.outputMode & OUTPUT_MODE_GRAPH) && userSpecifiedGraphTitle) {
		$(title, setCString, userSpecifiedGraphTitle);
	} 

	benchmarks->usec_per_test = options.usec_per_test;
	runTests (cpu, title);

        if ((options.outputMode & OUTPUT_MODE_GRAPH) && options.launch_viewer) {
#ifdef __APPLE__
		system ("open "RESULTS_IMAGE_FILENAME);
#elif defined(__linux__) && !defined(__ANDROID__)
		static const char *linux_image_viewers[] = {
			"ffplay",
			"gthumb",
			"eog",
			"zInspect",
			"okular",
			"geeqie",
			"gwenview",
			"xv"
		};
		for (int i=0; i < sizeof(linux_image_viewers)/sizeof(char*); i++) {
			char command[256];
			snprintf (command, sizeof(command), "%s %s 2>/dev/null", linux_image_viewers[i], RESULTS_IMAGE_FILENAME);
			if (!system (command)) {
				break;
			}
		}
#elif defined(__WIN32__) || defined(__WIN64__)
		system ("explorer "RESULTS_IMAGE_FILENAME);
#endif
	}

	releaseAndClear (benchmarks);
	releaseAndClear (console);
	releaseAndClear (cpu);
	releaseAndClear (hardware);

	deallocateClasses();

	if (g_totalObjectAllocations == g_totalObjectDeallocations) {
		puts ("All objects that were allocated were deallocated.");
	} else {
		puts ("Not all objects that were allocated were deallocated.");
		printf ("Remaining = %ld.\n", (long)g_totalObjectAllocations-(long)g_totalObjectDeallocations);
	}

	return 0;
}

