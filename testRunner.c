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

#include "defs.h"
#include "OOC/MutableImage.h"
#include "OOC/SimpleGraphing.h"
#include "testRunner.h"

#include <unistd.h>

#define TITLE_MEMORY_GRAPH "Results from ''bandwidth'' " RELEASE " by Zack T Smith, https://zs3.me"

#if defined(__WIN32__) || defined(__WIN64__)
#include <w32api/windows.h> // Cygwin
#include <inttypes.h>
#else
#include <time.h>
#endif

static SimpleGraphing *graph = NULL;
FILE *csv_output_file = NULL;

static void randomize() {
#if defined(__WIN32__) || defined(__WIN64__)
	srand (42); // Cygwin doesn't support time.h any longer.
#else
	srand (time(NULL));
#endif
}

static size_t chunk_sizes[] = {
	256,
	512,
	768,
	1024,
	1280,
	2048,
	3072,
	4096,
	6144,
	8192,	// Some processors' L1 data caches are only 8kB.
	12288,
	16384,
	20480,
	24576,
	28672,
	32768,	// Common L1 data cache size.
	34*1024,
	36*1024,
	40960,
	49152,
	65536,
	72*1024,
	80*1024,
	96*1024,
	131072,	// Tiny L2 cache size from yesteryear.
	192 * 1024,
	256 * 1024,	// Old L2 cache size.
	320 * 1024,
	384 * 1024,
	480 * 1024,
	512 * 1024,	// Old L2 cache size.
	768 * 1024,
	ONE_MEGABYTE,	// 1 MB = common L2 cache size.
	(1024 + 256) * 1024,	// 1.25
	(1024 + 512) * 1024,	// 1.5
	(1024 + 768) * 1024,	// 1.75
	2 * ONE_MEGABYTE,	// 2 MB = common L2 cache size.
	(2048 + 256) * 1024,	// 2.25
	(2048 + 512) * 1024,	// 2.5
	(2048 + 768) * 1024,	// 2.75
	3 * ONE_MEGABYTE ,	// 3 MB = common L2 cache size. 
	3407872, // 3.25 MB
	3 * ONE_MEGABYTE + 1024 * 512,	// 3.5 MB
	4 * ONE_MEGABYTE,	// 4 MB
	5 * ONE_MEGABYTE,	// 5 megs (Core i7-11xxxH has 5 MB L2)
	6 * ONE_MEGABYTE,	// 6 megs (common L2 cache size)
	7 * ONE_MEGABYTE,
	8 * ONE_MEGABYTE, // Xeon E3's often has 8MB L3
	9 * ONE_MEGABYTE,
	10 * ONE_MEGABYTE, // Xeon E5-2609 has 10MB L3
	11 * ONE_MEGABYTE,
	12 * ONE_MEGABYTE, // Core i7-11xx[x] often have 12MB L3
	13 * ONE_MEGABYTE,
	14 * ONE_MEGABYTE,
	15 * ONE_MEGABYTE, // Xeon E6-2630 has 15MB L3
	16 * ONE_MEGABYTE,
	20 * ONE_MEGABYTE, // Xeon E5-2690 has 20MB L3
	21 * ONE_MEGABYTE,
	32 * ONE_MEGABYTE,
	48 * ONE_MEGABYTE,
	64 * ONE_MEGABYTE,
	72 * ONE_MEGABYTE,
	96 * ONE_MEGABYTE,
	128 * ONE_MEGABYTE,
	160 * ONE_MEGABYTE,
	192 * ONE_MEGABYTE,
	224 * ONE_MEGABYTE,
	256 * ONE_MEGABYTE,
	320 * ONE_MEGABYTE,
	384 * ONE_MEGABYTE,
	480 * ONE_MEGABYTE,
	512 * ONE_MEGABYTE,
	640 * ONE_MEGABYTE,
	768 * ONE_MEGABYTE,
	1024 * ONE_MEGABYTE,
};
#define N_CHUNK_SIZES sizeof(chunk_sizes)/sizeof(unsigned long)

static double chunk_sizes_log2 [N_CHUNK_SIZES];

//============================================================================
// Output multiplexor. 
//============================================================================

void dataBegins (MutableString *title, String *subtitle, int graphWidth, int graphHeight)
{
	if (options.outputMode == OUTPUT_MODE_NONE) {
		if (title) release(title);
		if (subtitle) release(subtitle);
		return; 
	}

	if (!title) {
		title = _MutableString(TITLE_MEMORY_GRAPH);
		subtitle = NULL;
	}
	else if (!subtitle) {
		subtitle = _String(TITLE_MEMORY_GRAPH);
	}

	if (options.outputMode & OUTPUT_MODE_GRAPH) {
		if (graph) {
			error (__FUNCTION__, "Graphing already initialized.");
		}

		graph = SimpleGraphing_withSize (options.graphWidth, options.graphHeight);
		if (!graph) {
			error (__FUNCTION__, "Can't allocate graph.");
		}

		$(graph, setXAxisMode, MODE_X_AXIS_LOG2);
		if (title) {
			$(graph, setTitle, title);
		}
		if (subtitle) {
			$(graph, setSubtitle, subtitle);
		}	
	}

	if (options.outputMode & OUTPUT_MODE_CSV) {
		if (csv_output_file) {
			error (__FUNCTION__, "CSV file already initialized.");
		}
		csv_output_file = fopen (options.csv_file_path, "wb");
		if (!csv_output_file) {
			error (__FUNCTION__, "Cannot open CSV output file.");
		}
		if (title) {
			fputc ('"', csv_output_file);
			$(title, print, csv_output_file);
			fputc ('"', csv_output_file);
			fputc ('\n', csv_output_file);
		}
	}
}

void dataBeginSection (const char *name, uint32_t parameter)
{
	if (!name) { 
		error_null_parameter (__FUNCTION__); 
	}
	//==========

	if (options.outputMode & OUTPUT_MODE_GRAPH) {
		if (!graph) {
			error (__FUNCTION__, "Graphing not initialized.");
		}

		$(graph, addLine, name, parameter);
	}

	if (options.outputMode & OUTPUT_MODE_CSV) {
		if (!csv_output_file) 
			error (__FUNCTION__, "CSV output not initialized.");

		fprintf (csv_output_file, "%s\n", name);
	}
}

void dataEnds (const char *path)
{
	if (!path) { 
		error_null_parameter (__FUNCTION__); 
	}
	//==========

	if (options.outputMode & OUTPUT_MODE_GRAPH) {
		if (!graph) {
			error (__FUNCTION__, "Graphing not initialized.");
		}

		$(graph, make);

		MutableImage *image = $(graph, image);
		if (options.do_invert_graph) {
			$(image, invert);
		}

		//if ($(image, writeTIFF, path)) {
		if ($(image, writeBMP, path)) {
			$(console, printf, "Wrote graph to %s\n", path);
		} else {
			$(console, printf, "Failed to write graph to %s\n", path);
		}
	}

	if (options.outputMode & OUTPUT_MODE_CSV) {
		if (!csv_output_file) {
			error (__FUNCTION__, "CSV output not initialized.");
		}
		fclose (csv_output_file);
		$(console, puts, "Wrote CSV file.");
	}

	if (options.outputMode) {
		$(console, newline);
	}
}

void dataAddDatum (long x, long y)
{
	if (options.outputMode == OUTPUT_MODE_NONE) {
		return; 
	}

	if (options.outputMode & OUTPUT_MODE_GRAPH) {
		if (!graph) {
			error (__FUNCTION__, "Graphing not initialized.");
		}

		$(graph, addPoint, x, y);
	}

	if (options.outputMode & OUTPUT_MODE_CSV) {
		if (!csv_output_file) 
			error (__FUNCTION__, "CSV output not initialized.");

		fprintf (csv_output_file, "%lld, %.1Lf\n", (long long)x, (long double)y/10.);
		fflush (csv_output_file);
	}
}

void enforce_nice_mode (CPU *cpu)
{
	if (!options.nice_mode) {
		return;
	}

#define TARGET_TEMP (70.f)
	// If we can read the core temperature, let's sleep until it goes below 70C.
	int core = $(cpu, currentCore);
	if (core >= 0) {
		float temp = $(cpu, temperature, core);
		if (temp > 0.f) {
			$(console, printf, "\nCurrent core %d temperature: %.2fC\n", core, temp);
#define MAX_SLEEPS (20)
			unsigned n_sleeps = 0;
			while (temp > TARGET_TEMP) {
				sleep (2);
				n_sleeps++;
				if (n_sleeps >= MAX_SLEEPS) {
					$(console, printf, "Unable to reach target CPU temp of %.2fC\n", TARGET_TEMP);
					break;
				}
				$(console, printf, "Current core %d temperature: %.2fC\n", core, temp);
			}
		}
	}
}

void runTests (Benchmark *benchmarks, CPU *cpu, MutableString *title)
{
	if (!benchmarks || !cpu || !title) {
		return;
	}

	String *subtitle = _String(TITLE_MEMORY_GRAPH);

	$(console, newline);
	$(console, printf, "Title: ");
	$(title, print, NULL);
	$(console, newline);

	if (options.outputMode & OUTPUT_MODE_GRAPH) {
		$(console, printf, "Graph size: %dx%d\n", options.graphWidth, options.graphHeight);
	}

	dataBegins (title, subtitle, options.graphWidth, options.graphHeight);

	size_t chunkMinimumSize = 1<<8;
	size_t chunkMaximumSize = 0;
	if (options.limit_at_128MB) {
		chunkMaximumSize = 1<<27;
	} else {
		chunkMaximumSize = 1<<30;
	}

	if (options.reverse_chunk_size_order) {
		int last = 0;
		while (last < N_CHUNK_SIZES && chunk_sizes[last] != chunkMaximumSize) {
			last++;
		}

		for (int i = 0; i < last/2; i++) {
			unsigned long tmp = chunk_sizes[i];
			chunk_sizes[i] = chunk_sizes[last-i];
			chunk_sizes[last-i] = tmp;
		}
	}

	for (unsigned i=0; chunk_sizes[i] && i < N_CHUNK_SIZES; i++) {
		chunk_sizes_log2[i] = log2 (chunk_sizes[i]);
	}

	if (options.only_main_memory) {
		chunkMinimumSize = 320 * ONE_MEGABYTE;
		chunkMaximumSize = 384 * ONE_MEGABYTE;
	}

	if (options.diagnostic_mode) {
		// Limit to small chunk sizes.
                unsigned L1i = $(cpu, levelNCacheSize, 0, 1, false);
		chunkMaximumSize = L1i * 1024 ?: 16384;
	}

	size_t chunk_size;

	//------------------------------------------------------------
	// Sequential non-vector register reads.
	//
	bool supported = TEST_SUPPORTED == $(benchmarks, read, CHECK_WHETHER_SUPPORTED, SIZE_MAIN_REGISTER, false);
	if (supported && options.perform_read_tests) {
		enforce_nice_mode (cpu);
		$(console, newline);
#ifdef IS_64BIT
		dataBeginSection ("Sequential 64-bit reads", RGB_BLUE);
#else
		dataBeginSection ("Sequential 32-bit reads", RGB_BLUE);
#endif

		unsigned i = 0;
		while ((chunk_size = chunk_sizes [i++]) && i < N_CHUNK_SIZES) {
			if (chunk_size >= chunkMinimumSize && chunk_size <= chunkMaximumSize) {
				long amount = $(benchmarks, read, chunk_size, SIZE_MAIN_REGISTER, false);
				dataAddDatum (chunk_size, amount);
			}
		}
	}

	//------------------------------------------------------------
	// 128-bit sequential reads using e.g. Intel SSE2 or ARM NEON.
	//
	supported = TEST_SUPPORTED == $(benchmarks, read, CHECK_WHETHER_SUPPORTED, SIZE_VECTOR_128, false)
		&& $(cpu, has128bitVectors);
	if (supported && options.perform_read_tests && options.perform_128bit_tests) {
		enforce_nice_mode (cpu);
		dataBeginSection ("Sequential 128-bit reads", RGB_LIGHTBLUE);

		$(console, newline);

		unsigned i = 0;
		while ((chunk_size = chunk_sizes [i++]) && i < N_CHUNK_SIZES) {
			if (chunk_size >= chunkMinimumSize && chunk_size <= chunkMaximumSize) {
				long amount = $(benchmarks, read, chunk_size, SIZE_VECTOR_128, false);
				dataAddDatum (chunk_size, amount);
			}
		}
	}

	//------------------------------------------------------------
	// 256-bit sequential reads using e.g. AVX.
	//
	supported = TEST_SUPPORTED == $(benchmarks, read, CHECK_WHETHER_SUPPORTED, SIZE_VECTOR_256, false)
		&& $(cpu, has256bitVectors);
	if (supported && options.perform_read_tests && options.perform_256bit_tests) {
		enforce_nice_mode (cpu);
		dataBeginSection ("Sequential 256-bit reads", RGB_NAVYBLUE);

		$(console, newline);

		unsigned i = 0;
		while ((chunk_size = chunk_sizes [i++]) && i < N_CHUNK_SIZES) {
			if (chunk_size >= chunkMinimumSize && chunk_size <= chunkMaximumSize) {
				long amount = $(benchmarks, read, chunk_size, SIZE_VECTOR_256, false);
				dataAddDatum (chunk_size, amount);
			}
		}
	}

	//------------------------------------------------------------
	// 512-bit sequential reads using AVX512.
	//
	supported = TEST_SUPPORTED == $(benchmarks, read, CHECK_WHETHER_SUPPORTED, SIZE_VECTOR_512, false)
		&& $(cpu, has512bitVectors);
	if (supported && options.perform_read_tests && options.perform_512bit_tests) {
		enforce_nice_mode (cpu);
		dataBeginSection ("Sequential 512-bit reads (dashed)", RGB_NAVYBLUE | DASHED);

		$(console, newline);

		unsigned i = 0;
		while ((chunk_size = chunk_sizes [i++]) && i < N_CHUNK_SIZES) {
			if (chunk_size >= chunkMinimumSize && chunk_size <= chunkMaximumSize) {
				long amount = $(benchmarks, read, chunk_size, SIZE_VECTOR_512, false);
				dataAddDatum (chunk_size, amount);
			}
		}
	}

	//------------------------------------------------------------
	// Sequential non-vector 32/64-bit sequential nontemporal reads
	// e.g. aarch64 LDNP instruction.
	//
	supported = TEST_SUPPORTED == $(benchmarks, read, CHECK_WHETHER_SUPPORTED, SIZE_MAIN_REGISTER_NONTEMPORAL, false);
	if (supported && options.perform_read_tests && options.perform_mainregister_nontemporal_tests) {
		enforce_nice_mode (cpu);
#ifdef IS_64BIT
		dataBeginSection ("Sequential 64-bit nontemporal reads", RGB_STEELBLUE);
#else
		dataBeginSection ("Sequential 32-bit nontemporal reads", RGB_STEELBLUE);
#endif

		$(console, newline);

		unsigned i = 0;
		while ((chunk_size = chunk_sizes [i++]) && i < N_CHUNK_SIZES) {
			if (chunk_size >= chunkMinimumSize && chunk_size <= chunkMaximumSize) {
				long amount = $(benchmarks, read, chunk_size, SIZE_MAIN_REGISTER_NONTEMPORAL, false);
				dataAddDatum (chunk_size, amount);
			}
		}
	}

	//------------------------------------------------------------
	// 128-bit nontemporal sequential reads e.g. using SSE4.
	//
	supported = TEST_SUPPORTED == $(benchmarks, read, CHECK_WHETHER_SUPPORTED, SIZE_VECTOR_128_NONTEMPORAL, false)
		&& $(cpu, has128bitVectors);
	if (supported && options.perform_read_tests && options.perform_128bit_nontemporal_tests) {
		enforce_nice_mode (cpu);
		dataBeginSection ("Sequential 128-bit nontemporal reads", RGB_LIGHTTEAL); 

		$(console, newline);

		unsigned i = 0;
		while ((chunk_size = chunk_sizes [i++]) && i < N_CHUNK_SIZES) {
			if (chunk_size >= chunkMinimumSize && chunk_size <= chunkMaximumSize) {
				long amount = $(benchmarks, read, chunk_size, SIZE_VECTOR_128_NONTEMPORAL, false);
				dataAddDatum (chunk_size, amount);
			}
		}
	}

	//------------------------------------------------------------
	// 256-bit nontemporal sequential reads e.g. using AVX512.
	//
	supported = TEST_SUPPORTED == $(benchmarks, read, CHECK_WHETHER_SUPPORTED, SIZE_VECTOR_256_NONTEMPORAL, false)
		&& $(cpu, has256bitVectors);
	if (supported && options.perform_read_tests && options.perform_256bit_nontemporal_tests) {
		enforce_nice_mode (cpu);
		dataBeginSection ("Sequential 256-bit nontemporal reads", RGB_DARKCYAN); 

		$(console, newline);

		unsigned i = 0;
		while ((chunk_size = chunk_sizes [i++]) && i < N_CHUNK_SIZES) {
			if (chunk_size >= chunkMinimumSize && chunk_size <= chunkMaximumSize) {
				long amount = $(benchmarks, read, chunk_size, SIZE_VECTOR_256_NONTEMPORAL, false);
				dataAddDatum (chunk_size, amount);
			}
		}
	}

	//------------------------------------------------------------
	// 512-bit nontemporal sequential reads e.g. using AVX512.
	//
	supported = TEST_SUPPORTED == $(benchmarks, read, CHECK_WHETHER_SUPPORTED, SIZE_VECTOR_512_NONTEMPORAL, false)
		&& $(cpu, has512bitVectors);
	if (supported && options.perform_read_tests && options.perform_512bit_nontemporal_tests) {
		enforce_nice_mode (cpu);
		dataBeginSection ("Sequential 512-bit nontemporal reads (dashed)", RGB_DARKCYAN | DASHED); 

		$(console, newline);

		unsigned i = 0;
		while ((chunk_size = chunk_sizes [i++]) && i < N_CHUNK_SIZES) {
			if (chunk_size >= chunkMinimumSize && chunk_size <= chunkMaximumSize) {
				long amount = $(benchmarks, read, chunk_size, SIZE_VECTOR_512_NONTEMPORAL, false);
				dataAddDatum (chunk_size, amount);
			}
		}
	}

	//------------------------------------------------------------
	// Sequential non-vector register writes.
	//
	supported = TEST_SUPPORTED == $(benchmarks, write, CHECK_WHETHER_SUPPORTED, SIZE_MAIN_REGISTER, false);
	if (supported && options.perform_write_tests) {
		enforce_nice_mode (cpu);
#ifdef IS_64BIT
		dataBeginSection ("Sequential 64-bit writes", RGB_MEDIUMGREEN);
#else
		dataBeginSection ("Sequential 32-bit writes", RGB_MEDIUMGREEN);
#endif

		$(console, newline);

		unsigned i = 0;
		while ((chunk_size = chunk_sizes [i++]) && i < N_CHUNK_SIZES) {
			if (chunk_size >= chunkMinimumSize && chunk_size <= chunkMaximumSize) {
				long amount = $(benchmarks, write, chunk_size, SIZE_MAIN_REGISTER, false);
				dataAddDatum (chunk_size, amount);
			}
		}
	}

	//------------------------------------------------------------
	// 128-bit sequential writes using SSE2 or NEON 128.
	//
	supported = TEST_SUPPORTED == $(benchmarks, write, CHECK_WHETHER_SUPPORTED, SIZE_VECTOR_128, false)
		&& $(cpu, has128bitVectors);
	if (supported && options.perform_write_tests && options.perform_128bit_tests) {
		enforce_nice_mode (cpu);
		dataBeginSection ("Sequential 128-bit writes", RGB_LIGHTGREEN);

		$(console, newline);

		unsigned i = 0;
		while ((chunk_size = chunk_sizes [i++]) && i < N_CHUNK_SIZES) {
			if (chunk_size >= chunkMinimumSize && chunk_size <= chunkMaximumSize) {
				long amount = $(benchmarks, write, chunk_size, SIZE_VECTOR_128, false);
				dataAddDatum (chunk_size, amount);
			}
		}
	}

	//------------------------------------------------------------
	// 256-bit sequential writes using AVX.
	//
	supported = TEST_SUPPORTED == $(benchmarks, write, CHECK_WHETHER_SUPPORTED, SIZE_VECTOR_256, false)
		&& $(cpu, has256bitVectors);
	if (supported && options.perform_write_tests && options.perform_256bit_tests) {
		enforce_nice_mode (cpu);
		dataBeginSection ("Sequential 256-bit writes", RGB_DARKGREEN);

		$(console, newline);

		unsigned i = 0;
		while ((chunk_size = chunk_sizes [i++]) && i < N_CHUNK_SIZES) {
			if (chunk_size >= chunkMinimumSize && chunk_size <= chunkMaximumSize) {
				long amount = $(benchmarks, write, chunk_size, SIZE_VECTOR_256, false);
				dataAddDatum (chunk_size, amount);
			}
		}
	}

	//------------------------------------------------------------
	// 512-bit sequential writes using AVX512.
	//
	supported = TEST_SUPPORTED == $(benchmarks, write, CHECK_WHETHER_SUPPORTED, SIZE_VECTOR_512, false)
		&& $(cpu, has512bitVectors);
	if (supported && options.perform_write_tests && options.perform_512bit_tests) {
		enforce_nice_mode (cpu);
		if ($(cpu, has512bitVectors)) {
			dataBeginSection ("Sequential 512-bit writes (dashed)", RGB_DARKGREEN | DASHED);

			$(console, newline);

			unsigned i = 0;
			while ((chunk_size = chunk_sizes [i++]) && i < N_CHUNK_SIZES) {
				if (chunk_size >= chunkMinimumSize && chunk_size <= chunkMaximumSize) {
					long amount = $(benchmarks, write, chunk_size, SIZE_VECTOR_512, false);
					dataAddDatum (chunk_size, amount);
				}
			}
		}
	}

	//------------------------------------------------------------
	// 32/64-bit sequential nontemporal writes.
	//
	supported = TEST_SUPPORTED == $(benchmarks, write, CHECK_WHETHER_SUPPORTED, SIZE_MAIN_REGISTER_NONTEMPORAL, false);
	if (supported && options.perform_write_tests && options.perform_mainregister_nontemporal_tests) {
		enforce_nice_mode (cpu);
#ifdef IS_64BIT
		dataBeginSection ("Sequential 64-bit nontemporal writes", RGB_BROWN);
#else
		dataBeginSection ("Sequential 32-bit nontemporal writes", RGB_BROWN);
#endif

		$(console, newline);

		unsigned i = 0;
		while ((chunk_size = chunk_sizes [i++]) && i < N_CHUNK_SIZES) {
			if (chunk_size >= chunkMinimumSize && chunk_size <= chunkMaximumSize) {
				long amount = $(benchmarks, write, chunk_size, SIZE_MAIN_REGISTER_NONTEMPORAL, false);
				dataAddDatum (chunk_size, amount);
			}
		}
	}

	//------------------------------------------------------------
	// 128-bit sequential nontemporal writes using SSE4.
	//
	supported = TEST_SUPPORTED == $(benchmarks, write, CHECK_WHETHER_SUPPORTED, SIZE_VECTOR_128_NONTEMPORAL, false)
		&& $(cpu, has128bitVectors);
	if (supported && options.perform_write_tests && options.perform_128bit_nontemporal_tests) {
		enforce_nice_mode (cpu);
		dataBeginSection ("Sequential 128-bit nontemporal writes", RGB_LIGHTBROWN);

		$(console, newline);

		unsigned i = 0;
		while ((chunk_size = chunk_sizes [i++]) && i < N_CHUNK_SIZES) {
			if (chunk_size >= chunkMinimumSize && chunk_size <= chunkMaximumSize) {
				long amount = $(benchmarks, write, chunk_size, SIZE_VECTOR_128_NONTEMPORAL, false);
				dataAddDatum (chunk_size, amount);
			}
		}
	}

	//------------------------------------------------------------
	// 256-bit sequential writes with nontemporal hint.
	//
	supported = TEST_SUPPORTED == $(benchmarks, write, CHECK_WHETHER_SUPPORTED, SIZE_VECTOR_256_NONTEMPORAL, false)
		&& $(cpu, has256bitVectors);
	if (supported && options.perform_write_tests && options.perform_256bit_nontemporal_tests) {
		enforce_nice_mode (cpu);
		dataBeginSection ("Sequential 256-bit nontemporal writes", RGB_DARKBROWN);

		$(console, newline);

		unsigned i = 0;
		while ((chunk_size = chunk_sizes [i++]) && i < N_CHUNK_SIZES) {
			if (chunk_size >= chunkMinimumSize && chunk_size <= chunkMaximumSize) {
				long amount = $(benchmarks, write, chunk_size, SIZE_VECTOR_256_NONTEMPORAL, false);
				dataAddDatum (chunk_size, amount);
			}
		}
	}

	//------------------------------------------------------------
	// 512-bit sequential nontemporal writes e.g. using AVX-512.
	//
	supported = TEST_SUPPORTED == $(benchmarks, write, CHECK_WHETHER_SUPPORTED, SIZE_VECTOR_512_NONTEMPORAL, false)
		&& $(cpu, has512bitVectors);
	if (supported && options.perform_write_tests && options.perform_512bit_nontemporal_tests) {
		enforce_nice_mode (cpu);
		dataBeginSection ("Sequential 512-bit nontemporal writes (dashed)", RGB_DARKBROWN | DASHED);

		$(console, newline);

		unsigned i = 0;
		while ((chunk_size = chunk_sizes [i++]) && i < N_CHUNK_SIZES) {
			if (chunk_size >= chunkMinimumSize && chunk_size <= chunkMaximumSize) {
				long amount = $(benchmarks, write, chunk_size, SIZE_VECTOR_512_NONTEMPORAL, false);
				dataAddDatum (chunk_size, amount);
			}
		}
	}

	//------------------------------------------------------------
	// Random non-vector register reads.
	//
	supported = TEST_SUPPORTED == $(benchmarks, read, CHECK_WHETHER_SUPPORTED, SIZE_MAIN_REGISTER, true);
	if (supported && options.perform_read_tests && options.perform_random_tests) {
		enforce_nice_mode (cpu);
		$(console, newline);
#ifdef IS_64BIT
		dataBeginSection ("Random 64-bit reads", RGB_RED);
#else
		dataBeginSection ("Random 32-bit reads", RGB_RED);
#endif
		randomize();
		
		unsigned i = 0;
		while ((chunk_size = chunk_sizes [i++]) && i < N_CHUNK_SIZES) {
			if (chunk_size >= chunkMinimumSize && chunk_size <= chunkMaximumSize) {
				long amount = $(benchmarks, read, chunk_size, SIZE_MAIN_REGISTER, true);
				dataAddDatum (chunk_size, amount);
			}
		}
	}

	//------------------------------------------------------------
	// 128-bit random reads using NEON or SSE2.
	//
	supported = TEST_SUPPORTED == $(benchmarks, read, CHECK_WHETHER_SUPPORTED, SIZE_VECTOR_128, true)
		&& $(cpu, has128bitVectors);
	if (supported && options.perform_read_tests && options.perform_random_tests && options.perform_128bit_tests) {
		enforce_nice_mode (cpu);
		dataBeginSection ("Random 128-bit reads", RGB_LIGHTRED);

		$(console, newline);
		randomize();

		unsigned i = 0;
		while ((chunk_size = chunk_sizes [i++]) && i < N_CHUNK_SIZES) {
			if (chunk_size >= chunkMinimumSize && chunk_size <= chunkMaximumSize) {
				long amount = $(benchmarks, read, chunk_size, SIZE_VECTOR_128, true);
				dataAddDatum (chunk_size, amount);
			}
		}
	}

	//------------------------------------------------------------
	// 256-bit random reads using AVX.
	//
	supported = TEST_SUPPORTED == $(benchmarks, read, CHECK_WHETHER_SUPPORTED, SIZE_VECTOR_256, true)
		&& $(cpu, has256bitVectors);
	if (supported && options.perform_read_tests && options.perform_random_tests && options.perform_256bit_tests) {
		enforce_nice_mode (cpu);
		dataBeginSection ("Random 256-bit reads", 0xc00000);

		$(console, newline);
		randomize();

		unsigned i = 0;
		while ((chunk_size = chunk_sizes [i++]) && i < N_CHUNK_SIZES) {
			if (chunk_size >= chunkMinimumSize && chunk_size <= chunkMaximumSize) {
				long amount = $(benchmarks, read, chunk_size, SIZE_VECTOR_256, true);
				dataAddDatum (chunk_size, amount);
			}
		}
	}

	//------------------------------------------------------------
	// 128-bit random nontemporal reads, using SSE4.
	//
	supported = TEST_SUPPORTED == $(benchmarks, read, CHECK_WHETHER_SUPPORTED, SIZE_VECTOR_128_NONTEMPORAL, true)
		&& $(cpu, has128bitVectors);
	if (supported && options.perform_read_tests && options.perform_random_tests && options.perform_128bit_nontemporal_tests) {
		enforce_nice_mode (cpu);
		dataBeginSection ("Random 128-bit nontemporal reads", RGB_PURPLE);

		$(console, newline);
		randomize();

		unsigned i = 0;
		while ((chunk_size = chunk_sizes [i++]) && i < N_CHUNK_SIZES) {
			if (chunk_size >= chunkMinimumSize && chunk_size <= chunkMaximumSize) {
				long amount = $(benchmarks, read, chunk_size, SIZE_VECTOR_128_NONTEMPORAL, true);
				dataAddDatum (chunk_size, amount);
			}
		}
	}

	//------------------------------------------------------------
	// 256-bit random nontemporal reads, using AVX.
	//
	supported = TEST_SUPPORTED == $(benchmarks, read, CHECK_WHETHER_SUPPORTED, SIZE_VECTOR_256_NONTEMPORAL, true) 
		&& $(cpu, has256bitVectors);
	if (supported && options.perform_read_tests && options.perform_random_tests && options.perform_256bit_nontemporal_tests) {
		enforce_nice_mode (cpu);
		dataBeginSection ("Random 256-bit nontemporal reads", RGB_LIGHTPURPLE);

		$(console, newline);
		randomize();

		unsigned i = 0;
		while ((chunk_size = chunk_sizes [i++]) && i < N_CHUNK_SIZES) {
			if (chunk_size >= chunkMinimumSize && chunk_size <= chunkMaximumSize) {
				long amount = $(benchmarks, read, chunk_size, SIZE_VECTOR_256_NONTEMPORAL, true);
				dataAddDatum (chunk_size, amount);
			}
		}
	}

	//------------------------------------------------------------
	// Random non-vector register writes.
	//
	supported = TEST_SUPPORTED == $(benchmarks, write, CHECK_WHETHER_SUPPORTED, SIZE_MAIN_REGISTER, true);
	if (supported && options.perform_write_tests && options.perform_random_tests) {
		enforce_nice_mode (cpu);
#ifdef IS_64BIT
		dataBeginSection ("Random 64-bit writes", RGB_ORANGE);
#else
		dataBeginSection ("Random 32-bit writes", RGB_ORANGE);
#endif

		$(console, newline);
		randomize();

		unsigned i = 0;
		while ((chunk_size = chunk_sizes [i++]) && i < N_CHUNK_SIZES) {
			if (chunk_size >= chunkMinimumSize && chunk_size <= chunkMaximumSize) {
				long amount = $(benchmarks, write, chunk_size, SIZE_MAIN_REGISTER, true);
				dataAddDatum (chunk_size, amount);
			}
		}
	}

	//------------------------------------------------------------
	// 128-bit random writes using SSE2 or NEON 128-bit.
	//
	supported = TEST_SUPPORTED == $(benchmarks, write, CHECK_WHETHER_SUPPORTED, SIZE_VECTOR_128, true)
		&& $(cpu, has128bitVectors);
	if (supported && options.perform_write_tests && options.perform_random_tests && options.perform_128bit_tests) {
		enforce_nice_mode (cpu);
		dataBeginSection ("Random 128-bit writes", RGB_LIGHTORANGE);

		$(console, newline);
		randomize();

		unsigned i = 0;
		while ((chunk_size = chunk_sizes [i++]) && i < N_CHUNK_SIZES) {
			if (chunk_size >= chunkMinimumSize && chunk_size <= chunkMaximumSize) {
				long amount = $(benchmarks, write, chunk_size, SIZE_VECTOR_128, true);
				dataAddDatum (chunk_size, amount);
			}
		}
	}

	//------------------------------------------------------------
	// 256-byte randomized writes using e.g. AVX or Loong64 LASX.
	//
	supported = TEST_SUPPORTED == $(benchmarks, write, CHECK_WHETHER_SUPPORTED, SIZE_VECTOR_256, true)
		&& $(cpu, has256bitVectors);
	if (supported && options.perform_write_tests && options.perform_random_tests && options.perform_256bit_tests) {
		enforce_nice_mode (cpu);
		dataBeginSection ("Random 256-bit writes", RGB_DARKORANGE);

		$(console, newline);
		randomize();

		unsigned i = 0;
		while ((chunk_size = chunk_sizes [i++]) && i < N_CHUNK_SIZES) {
			if (chunk_size >= chunkMinimumSize && chunk_size <= chunkMaximumSize) {
				long amount = $(benchmarks, write, chunk_size, SIZE_VECTOR_256, true);
				dataAddDatum (chunk_size, amount);
			}
		}
	}

	//------------------------------------------------------------
	// 128-bit random nontemporal writes, using SSE4.
	//
	supported = TEST_SUPPORTED == $(benchmarks, write, CHECK_WHETHER_SUPPORTED, SIZE_VECTOR_128_NONTEMPORAL, true)
		&& $(cpu, has128bitVectors);
	if (supported && options.perform_write_tests && options.perform_random_tests && options.perform_128bit_nontemporal_tests) {
		enforce_nice_mode (cpu);
		dataBeginSection ("Random 128-bit nontemporal writes", RGB_DARKPURPLE);

		$(console, newline);
		randomize();

		unsigned i = 0;
		while ((chunk_size = chunk_sizes [i++]) && i < N_CHUNK_SIZES) {
			if (chunk_size >= chunkMinimumSize && chunk_size <= chunkMaximumSize) {
				long amount = $(benchmarks, write, chunk_size, SIZE_VECTOR_128_NONTEMPORAL, true);
				dataAddDatum (chunk_size, amount);
			}
		}
	}

	//------------------------------------------------------------
	// 256-bit random nontemporal writes, using SSE4.
	//
	supported = TEST_SUPPORTED == $(benchmarks, write, CHECK_WHETHER_SUPPORTED, SIZE_VECTOR_256_NONTEMPORAL, true) 
		&& $(cpu, has256bitVectors);
	if (supported && options.perform_write_tests && options.perform_random_tests && options.perform_256bit_nontemporal_tests) {
		enforce_nice_mode (cpu);
		dataBeginSection ("Random 256-bit nontemporal writes", RGB_PINK);

		$(console, newline);
		randomize();

		unsigned i = 0;
		while ((chunk_size = chunk_sizes [i++]) && i < N_CHUNK_SIZES) {
			if (chunk_size >= chunkMinimumSize && chunk_size <= chunkMaximumSize) {
				long amount = $(benchmarks, write, chunk_size, SIZE_VECTOR_256_NONTEMPORAL, true);
				dataAddDatum (chunk_size, amount);
			}
		}
	}

	//------------------------------------------------------------
	// 32/64-bit sequential copy using main registers.
	//
	supported = TEST_SUPPORTED == $(benchmarks, copy, CHECK_WHETHER_SUPPORTED, SIZE_MAIN_REGISTER);
	if (supported && options.perform_copy_tests) {
		enforce_nice_mode (cpu);
#ifdef IS_64BIT
		dataBeginSection ("Sequential 64-bit copy", RGB_BLACK);
#else
		dataBeginSection ("Sequential 32-bit copy", RGB_BLACK);
#endif

		$(console, newline);

		unsigned i = 0;
		while ((chunk_size = chunk_sizes [i++]) && i < N_CHUNK_SIZES) {
			if (chunk_size >= chunkMinimumSize && chunk_size <= chunkMaximumSize) {
				long amount = $(benchmarks, copy, chunk_size, SIZE_MAIN_REGISTER);
				dataAddDatum (chunk_size, amount);
			}
		}
	}

	//------------------------------------------------------------
	// 128-bit sequential copy using SSE2 or NEON.
	//
	supported = TEST_SUPPORTED == $(benchmarks, copy, CHECK_WHETHER_SUPPORTED, SIZE_VECTOR_128)
		&& $(cpu, has128bitVectors);
	if (supported && options.perform_copy_tests && options.perform_128bit_tests && $(cpu, has128bitVectors))
	{
		enforce_nice_mode (cpu);
		dataBeginSection ("Sequential 128-bit copy", RGB_DARKGRAY);

		$(console, newline);

		unsigned i = 0;
		while ((chunk_size = chunk_sizes [i++]) && i < N_CHUNK_SIZES) {
			if (chunk_size >= chunkMinimumSize && chunk_size <= chunkMaximumSize) {
				long amount = $(benchmarks, copy, chunk_size, SIZE_VECTOR_128);
				dataAddDatum (chunk_size, amount);
			}
		}
	}

	//------------------------------------------------------------
	// 256-bit sequential copy using AVX.
	//
	supported = TEST_SUPPORTED == $(benchmarks, copy, CHECK_WHETHER_SUPPORTED, SIZE_VECTOR_256) 
		&& $(cpu, has256bitVectors);
	if (supported && options.perform_copy_tests && options.perform_256bit_tests) {
		enforce_nice_mode (cpu);
		dataBeginSection ("Sequential 256-bit copy", RGB_GRAY);

		$(console, newline);

		unsigned i = 0;
		while ((chunk_size = chunk_sizes [i++]) && i < N_CHUNK_SIZES) {
			if (chunk_size >= chunkMinimumSize && chunk_size <= chunkMaximumSize) {
				long amount = $(benchmarks, copy, chunk_size, SIZE_VECTOR_256);
				dataAddDatum (chunk_size, amount);
			}
		}
	}

	//------------------------------------------------------------
	// 512-bit sequential copy using AVX512.
	//
	supported = TEST_SUPPORTED == $(benchmarks, copy, CHECK_WHETHER_SUPPORTED, SIZE_VECTOR_512) && $(cpu, has512bitVectors)
		&& $(cpu, has512bitVectors);
	if (supported && options.perform_copy_tests && options.perform_512bit_tests) {
		enforce_nice_mode (cpu);
		dataBeginSection ("Sequential 512-bit copy (dashed)", RGB_GRAY | DASHED);

		$(console, newline);

		unsigned i = 0;
		while ((chunk_size = chunk_sizes [i++]) && i < N_CHUNK_SIZES) {
			if (chunk_size >= chunkMinimumSize && chunk_size <= chunkMaximumSize) {
				long amount = $(benchmarks, copy, chunk_size, SIZE_VECTOR_512);
				dataAddDatum (chunk_size, amount);
			}
		}
	}

	//------------------------------------------------------------
	// Perform register and stack memory copy/increment tests.
	//
	if (options.perform_register_and_stack_tests) {
		enforce_nice_mode (cpu);

		//------------------------------------------------------------
		// Register to register.
		//
		$(console, newline);
		$(benchmarks, registerToRegisterTest);

		if (options.perform_128bit_tests) {
			if (benchmarks->vectorToFromRegisterRoutinesAvailable) {
#if defined(__arm__) && !defined(__aarch64__)
				// Special case for arm32 due to its limitations.
				$(benchmarks, vector128ToRegister32);
				$(benchmarks, register8ToVector128);
				$(benchmarks, register16ToVector128);
				$(benchmarks, register32ToVector128);
#else
				$(benchmarks, vector128ToRegister8);
				$(benchmarks, vector128ToRegister16);
				$(benchmarks, vector128ToRegister32);
#ifdef IS_64BIT
				$(benchmarks, vector128ToRegister64);
#endif
				$(benchmarks, register8ToVector128);
				$(benchmarks, register16ToVector128);
				$(benchmarks, register32ToVector128);
#ifdef IS_64BIT
				$(benchmarks, register64ToVector128);
#endif
#endif
			}
			$(benchmarks, registerToVectorMove);
			$(benchmarks, vectorToRegisterMove);
			$(benchmarks, vectorToVectorTest128);
		}
		if (options.perform_256bit_tests) {
			$(benchmarks, vectorToVectorTest256);
		}
		if (options.perform_512bit_tests) {
			$(benchmarks, vectorToVectorTest512);
		}

		//------------------------------------------------------------
		// Stack to/from register.
		//
		$(benchmarks, stackRead);
		$(benchmarks, stackWrite);

		//------------------------------------------------------------
		// Register vs stack.
		//
		$(benchmarks, incrementRegisters);
		$(benchmarks, incrementStack);
	}

	$(benchmarks, freeDeferredChunks);

	$(console, flush);
	$(console, newline);
	dataEnds (RESULTS_IMAGE_FILENAME);

	releaseAndClear (graph);
}
