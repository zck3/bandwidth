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

//-----------------------------------------------------------------------------
// Change log
// 0.18	Grand unified release supports x86/intel64/arm, Linux/Win32/WinMobile.
// 0.19	Now have 128-bit writer that goes to cache AND one that uses nontemporal writes.
// 0.20	Added my bmplib and graphing of output. Also added --slow option.
// 0.21	Adds random testing. Min chunk size = 256 B. Allows non-2^n chunks.
// 0.22	Adds register-to-register and register-to/from-stack transfers.
// 0.23	Adds vector-to-vector and register-to-vector transfers, & Mac support.
// 0.24	Adds network bandwidth tests from this PC to specified others.
// 0.25	Made network tests bidirectional to test asymmetric networks.
// 0.26	Fixes to prevent certain vector instructions being used w/AMD chips.
// 0.27 Added 128-byte tests for greater precision.
// 0.28	Added use of CPUID.
// 0.29 Added more 128-byte tests.
// 0.30 Adds cache identification for Intel CPUs in 64-bit mode.
// 0.31 Adds cache identification for Intel CPUs in 32-bit mode.
// 0.32 Added AVX support.
// 1.0	Moved graphing logic into BMPGraphing. Added string-instruction test.
// 1.1	Switched to larger font in graphing module.
// 1.2	Re-added ARM 32 support.
// 1.3	Added CSV output support. Added 32-bit Raspberry π 3 support.
// 1.4	Added 256-bit routines RandomReaderVector256, RandomWriterAVX.
// 1.4.1 Added --limit parameter.
// 1.4.2 Fixed compiler warnings.
// 1.5	Fixed AVX writer bug that gave inaccurate results. Added nice mode.
// 1.5.1 Fixed crashing bug.
// 1.5.2 Disabled AVX tests if AMD CPU.
// 1.6	Converted the code to object-oriented C.
// 1.6.1 Moved CPUID calls into utility*asm. Detection of SHA, SGX, CET.
// 1.6.2 Converted some code to Object-Oriented C.
// 1.6.3 Removed string instruction tests. Improved Object-Oriented C.
// 1.6.4 Code cleanup.
// 1.6.5 Fixed Linux issues.
// 1.7	Separated object-oriented C (OOC) from bandwidth.
// 1.8	OO improvements. Makefile improvements. Added Win64 support.
// 1.9	OO improvements. Win64. Removed Linux fbdev.
// 1.10 ARM64 support and improved ARM32. Apple M1 support. AVX512 detection.
// 1.11	AVX-512 support. PK fonts.
// 1.11.2 Fixes for Windows 64-bit Cygwin.
// 1.11.3 Subclassed tests per CPU architecture.
// 1.12.0 RISC-V support. OO class hierarchy improvements. Changed line colors.
// 1.12.1 Apple M2 support.
// 1.12.3 Fixed Windows bug. Added --reverse option. Added x86 mfence.
// 1.12.4 Added: OS CPU Hardware Set Dictionary URL FontFreetype.
// 1.12.5 Fixes for ARM CPUs. Fixed i386 issue.
// 1.13	Added more AVX-512 support e.g. non-temporal transfers, 512-bit copy.
// 1.13.1 Changed "cache-bypassing" to nontemporal. Added option -nonontemporal.
// 1.13.2 Added aarch64 nontemporal transfers.
// 1.13.3 Added aarch64 memory barriers.
// 1.14.0 Added aarch64 Android support which requires using Termux app.
// 1.14.1 Fixed Windows/Cygwin issue. Fixed Darwin x86 issue.
// 1.14.2 Improved MacOS use of sysctl in OOC.
// 1.14.3 Fix for aarch64 vector-to-vector xfer.
// 1.14.4 Improved graph title construction. 
// 1.14.5 Fixed issues with Raspberry Pi Desktop PC (=32+64-bit x86 Debian).
// 1.14.6 Improved temperature sensing.
// 1.14.7 Reinstated x86_64 assembly code language for reading cache info.
// 1.14.8 Makefile renovation.
// 1.14.9 Android makefile fix.
// 1.14.10 Small aarch32 fix.
// 1.15.0 
// 	- New option --main-memory-only => a small set of large chunk sizes.
//	- Deferred freeing of chunks for more accurate results.
// 	- Small RISC-V fix. Small i386 fix. 
// 	- Better CPU feature extraction.
// 	- MacOS release name support.
// 	- Fixes for pecularities of MacOS.
// 	- Compilation fixes for aarch32 that I discovered using Qemu/podman.
// 	- Initial preparations for PowerPC support.
// 1.15.1
//	- Fixed memory leaks. Refined testRunner.
// 1.15.2
//	- Array bounds issue in testRunner.
// 1.15.3
//	- Minor fixes for Windows.
//	- Added Dockerfiles & scripts for running in i386 & arm32 containers.
//	- Removed x86 cache asm code in favor of Linux sysfs.
// 1.15.4
// 	- Initial PowerPC support (64-bit little endian), tested w/Podman.
// 1.16.0
// 	- Added big-endian PowerPC 64-bit support, testing in Qemu/Debian13.
// 1.16.1
// 	- Now reporting PowerPC hardware capabilities.
// 1.16.2
// 	- Now reporting RISC-V vector size, tested in Podman container.
//	- Initial LoongArch64 support, tested in Qemu/Debian.
// 1.16.3
//	- Now reporting arm32 hardware capabilities on Linux.
//	- Added vector-to/from-register routines for aarch32.
//	- Added vector-to/from-register routines for aarch64.
//	- Regularized method naming to be non-X86-specific.
//	- Regularized command-line parameters to be non-X86-specific.
//	- Incorporated KatyushaScarlet's LoongArch64 vector routines.
//	- Improved program usage information.
//	- Refactoring and pruning.
//	- OOC improvements.
// 1.16.4
//	- Various fixes for macOS.
// 1.17.0
//	- Added multithreaded test runner, invoked with the -M option.
//-----------------------------------------------------------------------------

#ifndef _DEFS_H
#define _DEFS_H

#include "OOC/Console.h"
extern Console *console;

#define RELEASE "1.17.0"
#define RESULTS_IMAGE_FILENAME "bandwidth.bmp"

#define DEFAULT_GRAPH_WIDTH 1440
#define DEFAULT_GRAPH_HEIGHT 900

typedef enum {
	OUTPUT_MODE_NONE=0,
	OUTPUT_MODE_GRAPH=1,
	OUTPUT_MODE_CSV=2,
} OutputMode;

typedef struct {
	bool perform_mainregister_nontemporal_tests;
	bool perform_128bit_tests;
	bool perform_128bit_nontemporal_tests;
	bool perform_256bit_tests;
	bool perform_256bit_nontemporal_tests;
	bool perform_512bit_tests;
	bool perform_512bit_nontemporal_tests;
	bool perform_read_tests;
	bool perform_write_tests;
	bool perform_copy_tests;
	bool perform_register_and_stack_tests;
	bool perform_random_tests;
	bool perform_direct_tests;
	bool nice_mode;
	bool reverse_chunk_size_order;
	bool limit_at_128MB;
	bool launch_viewer;
	bool do_invert_graph;
	bool diagnostic_mode;
	bool only_main_memory;
	bool multithreaded;
	OutputMode outputMode;
	unsigned long usec_per_test;
	char *csv_file_path;
	int graphWidth;
	int graphHeight;
} ProgramOptions;

extern ProgramOptions options;

#endif
