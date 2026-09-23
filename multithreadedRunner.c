/*============================================================================
  bandwidth, a benchmark to measure memory transfer bandwidth.
  This file has copyright (C) 2026 by Zack T Smith.

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

#ifdef __linux__
#define _GNU_SOURCE
#include <sched.h>
#endif

#include "defs.h"
#include "multithreadedRunner.h"
#include "OOC/Log.h"
#include "OOC/Utility.h" // cpu_current_core

#include <unistd.h>
#include <pthread.h>

#if defined(__APPLE__) && defined(__aarch64__)
#include <mach/mach.h>
#include <mach/thread_policy.h>
#endif 

#if defined(__WIN32__) || defined(__WIN64__)
#include <w32api/windows.h> // Cygwin
#include <inttypes.h>
#else
#include <time.h>
#endif

static size_t chunk_sizes[] = {
	160 * ONE_MEGABYTE,
	192 * ONE_MEGABYTE,
	224 * ONE_MEGABYTE,
};
#define N_CHUNK_SIZES sizeof(chunk_sizes)/sizeof(unsigned long)
#define	MIDPOINT (N_CHUNK_SIZES/2)

enum {
	MethodReading = 0,
	MethodWriting,
	MethodCopying
};

struct thread_data {
	Benchmark *benchmarks;
	unsigned threadNumber;
	unsigned actualCoreAtStart;
	unsigned actualCoreAtEnd;
	long *results;
	BenchmarkMode mode;
	bool random;
	short method;
};

#if defined(__APPLE__) && defined(__aarch64__)

// CAVEAT: This is not very effective.
static kern_return_t set_thread_affinity (int core)
{
	thread_affinity_policy_data_t policy = { core };

	return thread_policy_set (mach_thread_self(), 
		THREAD_AFFINITY_POLICY, 
		(thread_policy_t) &policy, 
		THREAD_AFFINITY_POLICY_COUNT);
}
#endif

static void *thread_routine (void* args)
{
	if (!args) {
		error(__FUNCTION__, "Reader is missing its args.");
	}
	struct thread_data *data = args;
        
#if defined(__APPLE__) && defined(__aarch64__)
	set_thread_affinity (data->threadNumber);
#endif

	data->actualCoreAtStart = cpu_current_core();

	for (unsigned i=0; i < N_CHUNK_SIZES; i++) {
		size_t chunk_size = chunk_sizes[i];
		long amount = -1;
		switch (data->method) {
		case MethodReading:
			amount = $(data->benchmarks, read, chunk_size, data->mode, data->random);
			break;
		case MethodWriting:
			amount = $(data->benchmarks, write, chunk_size, data->mode, data->random);
			break;
		case MethodCopying:
			amount = $(data->benchmarks, copy, chunk_size, data->mode);
			break;
		default:
			exit(99);
		}
		if (i == MIDPOINT) {
			data->results[data->threadNumber] = amount;
		}
	}

	// Just in case the thread is migrated to a different core during execution.
	data->actualCoreAtEnd = cpu_current_core();

	return NULL;
}

struct thread_data *make_thread_data (Benchmark *benchmarks, unsigned threadNumber, long* results, BenchmarkMode mode, bool random, short method)
{
	struct thread_data *args = malloc(sizeof(struct thread_data));
	if (!args) {
		Log_perror (__FUNCTION__, "malloc");
		exit(1);
	}
	args->benchmarks = benchmarks;
	args->threadNumber = threadNumber;
	args->results = results;
	args->mode = mode;
	args->random = random;
	args->method = method;
	args->actualCoreAtStart = -1;
	args->actualCoreAtEnd = -1;
	return args;
}

void runMultithreadedTests (Benchmark *benchmarks, CPU *cpu)
{
	if (!benchmarks || !cpu) {
		return;
	}

	benchmarks->quietMode = true;

	unsigned nCores = $(cpu, nCores);
	if (!nCores) {
		error(__FUNCTION__, "Unable to determine number of CPU cores.");
	}

	pthread_t threads[nCores];
	struct thread_data *data[nCores];
	long results[nCores];

	// Before we run the multithreaded tests, let's plan out what we want to run
	// based on command-line options, taking into account the limitations of this CPU.

	typedef struct run_information {
		BenchmarkMode mode;
		const char *run_name;
		bool random;
		short method;
	} RunInfo;
	RunInfo run_info[64];
	unsigned n_planned_runs = 0;
	
	bool supported = TEST_SUPPORTED == $(benchmarks, read, CHECK_WHETHER_SUPPORTED, SIZE_MAIN_REGISTER, false);
	if (supported && options.perform_read_tests) {
		run_info[n_planned_runs].mode = SIZE_MAIN_REGISTER;
		run_info[n_planned_runs].random = false;
		run_info[n_planned_runs].run_name = "Sequential 64-bit reads";
		run_info[n_planned_runs].method = MethodReading;
		n_planned_runs++;
	}

	supported = TEST_SUPPORTED == $(benchmarks, read, CHECK_WHETHER_SUPPORTED, SIZE_VECTOR_128, false)
		&& $(cpu, has128bitVectors);
	if (supported && options.perform_read_tests && options.perform_128bit_tests) {
		run_info[n_planned_runs].mode = SIZE_VECTOR_128;
		run_info[n_planned_runs].random = false;
		run_info[n_planned_runs].run_name = "Sequential 128-bit reads";
		run_info[n_planned_runs].method = MethodReading;
		n_planned_runs++;
	}

	supported = TEST_SUPPORTED == $(benchmarks, read, CHECK_WHETHER_SUPPORTED, SIZE_VECTOR_256, false)
		&& $(cpu, has256bitVectors);
	if (supported && options.perform_read_tests && options.perform_256bit_tests) {
		run_info[n_planned_runs].mode = SIZE_VECTOR_256;
		run_info[n_planned_runs].random = false;
		run_info[n_planned_runs].run_name = "Sequential 256-bit reads";
		run_info[n_planned_runs].method = MethodReading;
		n_planned_runs++;
	}

	supported = TEST_SUPPORTED == $(benchmarks, read, CHECK_WHETHER_SUPPORTED, SIZE_VECTOR_512, false)
		&& $(cpu, has512bitVectors);
	if (supported && options.perform_read_tests && options.perform_512bit_tests) {
		run_info[n_planned_runs].mode = SIZE_VECTOR_512;
		run_info[n_planned_runs].random = false;
		run_info[n_planned_runs].run_name = "Sequential 512-bit reads";
		run_info[n_planned_runs].method = MethodReading;
		n_planned_runs++;
	}

	supported = TEST_SUPPORTED == $(benchmarks, read, CHECK_WHETHER_SUPPORTED, SIZE_MAIN_REGISTER_NONTEMPORAL, false);
	if (supported && options.perform_read_tests) {
		run_info[n_planned_runs].mode = SIZE_MAIN_REGISTER_NONTEMPORAL;
		run_info[n_planned_runs].random = false;
		run_info[n_planned_runs].run_name = 
#ifdef IS_64BIT
			"Sequential 64-bit nontemporal reads";
#else
			"Sequential 32-bit nontemporal reads";
#endif
		run_info[n_planned_runs].method = MethodReading;
		n_planned_runs++;
	}

	supported = TEST_SUPPORTED == $(benchmarks, read, CHECK_WHETHER_SUPPORTED, SIZE_VECTOR_128_NONTEMPORAL, false)
		&& $(cpu, has128bitVectors);
	if (supported && options.perform_read_tests && options.perform_128bit_nontemporal_tests) {
		run_info[n_planned_runs].mode = SIZE_VECTOR_128_NONTEMPORAL;
		run_info[n_planned_runs].random = false;
		run_info[n_planned_runs].run_name = "Sequential 128-bit nontemporal reads"; 
		run_info[n_planned_runs].method = MethodReading;
		n_planned_runs++;
	}

	supported = TEST_SUPPORTED == $(benchmarks, read, CHECK_WHETHER_SUPPORTED, SIZE_VECTOR_256_NONTEMPORAL, false)
		&& $(cpu, has256bitVectors);
	if (supported && options.perform_read_tests && options.perform_256bit_nontemporal_tests) {
		run_info[n_planned_runs].mode = SIZE_VECTOR_256_NONTEMPORAL;
		run_info[n_planned_runs].random = false;
		run_info[n_planned_runs].run_name = "Sequential 256-bit nontemporal reads"; 
		run_info[n_planned_runs].method = MethodReading;
		n_planned_runs++;
	}

	supported = TEST_SUPPORTED == $(benchmarks, read, CHECK_WHETHER_SUPPORTED, SIZE_VECTOR_512_NONTEMPORAL, false)
		&& $(cpu, has512bitVectors);
	if (supported && options.perform_read_tests && options.perform_512bit_nontemporal_tests) {
		run_info[n_planned_runs].mode = SIZE_VECTOR_512_NONTEMPORAL;
		run_info[n_planned_runs].random = false;
		run_info[n_planned_runs].run_name = "Sequential 512-bit nontemporal reads"; 
		run_info[n_planned_runs].method = MethodReading;
		n_planned_runs++;
	}

	supported = TEST_SUPPORTED == $(benchmarks, read, CHECK_WHETHER_SUPPORTED, SIZE_MAIN_REGISTER, true);
	if (supported && options.perform_read_tests) {
		run_info[n_planned_runs].mode = SIZE_MAIN_REGISTER;
		run_info[n_planned_runs].random = true;
		run_info[n_planned_runs].run_name = "Random 64-bit reads";
		run_info[n_planned_runs].method = MethodReading;
		n_planned_runs++;
	}

	supported = TEST_SUPPORTED == $(benchmarks, read, CHECK_WHETHER_SUPPORTED, SIZE_VECTOR_128, true)
		&& $(cpu, has128bitVectors);
	if (supported && options.perform_read_tests && options.perform_random_tests && options.perform_128bit_tests) {
		run_info[n_planned_runs].mode = SIZE_VECTOR_128;
		run_info[n_planned_runs].random = true;
		run_info[n_planned_runs].run_name = "Random 128-bit reads";
		run_info[n_planned_runs].method = MethodReading;
		n_planned_runs++;
	}

	supported = TEST_SUPPORTED == $(benchmarks, read, CHECK_WHETHER_SUPPORTED, SIZE_VECTOR_256, true)
		&& $(cpu, has256bitVectors);
	if (supported && options.perform_read_tests && options.perform_random_tests && options.perform_256bit_tests) {
		run_info[n_planned_runs].mode = SIZE_VECTOR_256;
		run_info[n_planned_runs].random = true;
		run_info[n_planned_runs].run_name = "Random 256-bit reads";
		run_info[n_planned_runs].method = MethodReading;
		n_planned_runs++;
	}

	supported = TEST_SUPPORTED == $(benchmarks, read, CHECK_WHETHER_SUPPORTED, SIZE_VECTOR_128_NONTEMPORAL, true)
		&& $(cpu, has128bitVectors);
	if (supported && options.perform_read_tests && options.perform_random_tests && options.perform_128bit_nontemporal_tests) {
		run_info[n_planned_runs].mode = SIZE_VECTOR_128_NONTEMPORAL;
		run_info[n_planned_runs].random = true;
		run_info[n_planned_runs].run_name = "Random 128-bit nontemporal reads";
		run_info[n_planned_runs].method = MethodReading;
		n_planned_runs++;
	}

	supported = TEST_SUPPORTED == $(benchmarks, read, CHECK_WHETHER_SUPPORTED, SIZE_VECTOR_256_NONTEMPORAL, true)
		&& $(cpu, has256bitVectors);
	if (supported && options.perform_read_tests && options.perform_256bit_nontemporal_tests) {
		run_info[n_planned_runs].mode = SIZE_VECTOR_256_NONTEMPORAL;
		run_info[n_planned_runs].random = true;
		run_info[n_planned_runs].run_name = "Random 256-bit nontemporal reads"; 
		run_info[n_planned_runs].method = MethodReading;
		n_planned_runs++;
	}

	supported = TEST_SUPPORTED == $(benchmarks, write, CHECK_WHETHER_SUPPORTED, SIZE_MAIN_REGISTER, false);
	if (supported && options.perform_write_tests) {
		run_info[n_planned_runs].mode = SIZE_MAIN_REGISTER;
		run_info[n_planned_runs].random = false;
		run_info[n_planned_runs].method = MethodWriting;
		run_info[n_planned_runs].run_name = 
#ifdef IS_64BIT
			"Sequential 64-bit writes";
#else
			"Sequential 32-bit writes";
#endif
		n_planned_runs++;
	}

	supported = TEST_SUPPORTED == $(benchmarks, write, CHECK_WHETHER_SUPPORTED, SIZE_VECTOR_128, false)
		&& $(cpu, has128bitVectors);
	if (supported && options.perform_write_tests && options.perform_128bit_tests) {
		run_info[n_planned_runs].mode = SIZE_MAIN_REGISTER;
		run_info[n_planned_runs].random = false;
		run_info[n_planned_runs].method = MethodWriting;
		run_info[n_planned_runs].run_name = "Sequential 128-bit writes";
		n_planned_runs++;
	}

	supported = TEST_SUPPORTED == $(benchmarks, write, CHECK_WHETHER_SUPPORTED, SIZE_VECTOR_256, false)
		&& $(cpu, has256bitVectors);
	if (supported && options.perform_write_tests && options.perform_256bit_tests) {
		run_info[n_planned_runs].mode = SIZE_MAIN_REGISTER;
		run_info[n_planned_runs].random = false;
		run_info[n_planned_runs].method = MethodWriting;
		run_info[n_planned_runs].run_name = "Sequential 256-bit writes";
		n_planned_runs++;
	}

	supported = TEST_SUPPORTED == $(benchmarks, write, CHECK_WHETHER_SUPPORTED, SIZE_VECTOR_512, false)
		&& $(cpu, has512bitVectors);
	if (supported && options.perform_write_tests && options.perform_512bit_tests) {
		run_info[n_planned_runs].mode = SIZE_MAIN_REGISTER;
		run_info[n_planned_runs].random = false;
		run_info[n_planned_runs].method = MethodWriting;
		run_info[n_planned_runs].run_name = "Sequential 512-bit writes";
		n_planned_runs++;
	}

	supported = TEST_SUPPORTED == $(benchmarks, write, CHECK_WHETHER_SUPPORTED, SIZE_MAIN_REGISTER_NONTEMPORAL, false);
	if (supported && options.perform_write_tests && options.perform_mainregister_nontemporal_tests) {
		run_info[n_planned_runs].mode = SIZE_MAIN_REGISTER_NONTEMPORAL;
		run_info[n_planned_runs].random = false;
		run_info[n_planned_runs].method = MethodWriting;
		run_info[n_planned_runs].run_name = 
#ifdef IS_64BIT
		"Sequential 64-bit nontemporal writes";
#else
		"Sequential 32-bit nontemporal writes";
#endif
		n_planned_runs++;
	}

	supported = TEST_SUPPORTED == $(benchmarks, write, CHECK_WHETHER_SUPPORTED, SIZE_VECTOR_128_NONTEMPORAL, false)
		&& $(cpu, has128bitVectors);
	if (supported && options.perform_write_tests && options.perform_128bit_nontemporal_tests) {
		run_info[n_planned_runs].mode = SIZE_VECTOR_128_NONTEMPORAL;
		run_info[n_planned_runs].random = false;
		run_info[n_planned_runs].method = MethodWriting;
		run_info[n_planned_runs].run_name = "Sequential 128-bit nontemporal writes";
		n_planned_runs++;
	}

	supported = TEST_SUPPORTED == $(benchmarks, write, CHECK_WHETHER_SUPPORTED, SIZE_VECTOR_256_NONTEMPORAL, false)
		&& $(cpu, has256bitVectors);
	if (supported && options.perform_write_tests && options.perform_256bit_nontemporal_tests) {
		run_info[n_planned_runs].mode = SIZE_VECTOR_256_NONTEMPORAL;
		run_info[n_planned_runs].random = false;
		run_info[n_planned_runs].method = MethodWriting;
		run_info[n_planned_runs].run_name = "Sequential 256-bit nontemporal writes";
		n_planned_runs++;
	}

	supported = TEST_SUPPORTED == $(benchmarks, write, CHECK_WHETHER_SUPPORTED, SIZE_VECTOR_512_NONTEMPORAL, false)
		&& $(cpu, has512bitVectors);
	if (supported && options.perform_write_tests && options.perform_512bit_nontemporal_tests) {
		run_info[n_planned_runs].mode = SIZE_VECTOR_512_NONTEMPORAL;
		run_info[n_planned_runs].random = false;
		run_info[n_planned_runs].method = MethodWriting;
		run_info[n_planned_runs].run_name = "Sequential 512-bit nontemporal writes";
		n_planned_runs++;
	}

	supported = TEST_SUPPORTED == $(benchmarks, write, CHECK_WHETHER_SUPPORTED, SIZE_MAIN_REGISTER, true);
	if (supported && options.perform_write_tests) {
		run_info[n_planned_runs].mode = SIZE_MAIN_REGISTER;
		run_info[n_planned_runs].random = true;
		run_info[n_planned_runs].method = MethodWriting;
		run_info[n_planned_runs].run_name = 
#ifdef IS_64BIT
			"Random 64-bit writes";
#else
			"Random 32-bit writes";
#endif
		n_planned_runs++;
	}

	supported = TEST_SUPPORTED == $(benchmarks, write, CHECK_WHETHER_SUPPORTED, SIZE_VECTOR_128, true)
		&& $(cpu, has128bitVectors);
	if (supported && options.perform_write_tests && options.perform_128bit_tests) {
		run_info[n_planned_runs].mode = SIZE_MAIN_REGISTER;
		run_info[n_planned_runs].random = true;
		run_info[n_planned_runs].method = MethodWriting;
		run_info[n_planned_runs].run_name = "Random 128-bit writes";
		n_planned_runs++;
	}

	supported = TEST_SUPPORTED == $(benchmarks, write, CHECK_WHETHER_SUPPORTED, SIZE_VECTOR_256, true)
		&& $(cpu, has256bitVectors);
	if (supported && options.perform_write_tests && options.perform_256bit_tests) {
		run_info[n_planned_runs].mode = SIZE_MAIN_REGISTER;
		run_info[n_planned_runs].random = true;
		run_info[n_planned_runs].method = MethodWriting;
		run_info[n_planned_runs].run_name = "Random 256-bit writes";
		n_planned_runs++;
	}

	supported = TEST_SUPPORTED == $(benchmarks, write, CHECK_WHETHER_SUPPORTED, SIZE_VECTOR_128_NONTEMPORAL, true)
		&& $(cpu, has128bitVectors);
	if (supported && options.perform_write_tests && options.perform_random_tests && options.perform_128bit_nontemporal_tests) {
		run_info[n_planned_runs].mode = SIZE_MAIN_REGISTER;
		run_info[n_planned_runs].random = true;
		run_info[n_planned_runs].method = MethodWriting;
		run_info[n_planned_runs].run_name = "Random 128-bit nontemporal writes";
		n_planned_runs++;
	}

	supported = TEST_SUPPORTED == $(benchmarks, write, CHECK_WHETHER_SUPPORTED, SIZE_VECTOR_256_NONTEMPORAL, true)
		&& $(cpu, has256bitVectors);
	if (supported && options.perform_write_tests && options.perform_random_tests && options.perform_256bit_nontemporal_tests) {
		run_info[n_planned_runs].mode = SIZE_MAIN_REGISTER;
		run_info[n_planned_runs].random = true;
		run_info[n_planned_runs].method = MethodWriting;
		run_info[n_planned_runs].run_name = "Random 256-bit nontemporal writes";
		n_planned_runs++;
	}

	supported = TEST_SUPPORTED == $(benchmarks, copy, CHECK_WHETHER_SUPPORTED, SIZE_MAIN_REGISTER);
	if (supported && options.perform_copy_tests) {
		run_info[n_planned_runs].mode = SIZE_MAIN_REGISTER;
		run_info[n_planned_runs].random = false;
		run_info[n_planned_runs].method = MethodCopying;
		run_info[n_planned_runs].run_name = 
#ifdef IS_64BIT
		"Sequential 64-bit copy";
#else
		"Sequential 32-bit copy";
#endif
		n_planned_runs++;
	}

	supported = TEST_SUPPORTED == $(benchmarks, copy, CHECK_WHETHER_SUPPORTED, SIZE_VECTOR_128)
		&& $(cpu, has128bitVectors);
	if (supported && options.perform_copy_tests && options.perform_128bit_tests && $(cpu, has128bitVectors))
	{
		run_info[n_planned_runs].mode = SIZE_VECTOR_128;
		run_info[n_planned_runs].random = false;
		run_info[n_planned_runs].method = MethodCopying;
		run_info[n_planned_runs].run_name = "Sequential 128-bit copy";
		n_planned_runs++;
	}

	supported = TEST_SUPPORTED == $(benchmarks, copy, CHECK_WHETHER_SUPPORTED, SIZE_VECTOR_256)
		&& $(cpu, has256bitVectors);
	if (supported && options.perform_copy_tests && options.perform_256bit_tests && $(cpu, has256bitVectors))
	{
		run_info[n_planned_runs].mode = SIZE_VECTOR_256;
		run_info[n_planned_runs].random = false;
		run_info[n_planned_runs].method = MethodCopying;
		run_info[n_planned_runs].run_name = "Sequential 256-bit copy";
		n_planned_runs++;
	}

	supported = TEST_SUPPORTED == $(benchmarks, copy, CHECK_WHETHER_SUPPORTED, SIZE_VECTOR_512)
		&& $(cpu, has512bitVectors);
	if (supported && options.perform_copy_tests && options.perform_512bit_tests && $(cpu, has512bitVectors))
	{
		run_info[n_planned_runs].mode = SIZE_VECTOR_512;
		run_info[n_planned_runs].random = false;
		run_info[n_planned_runs].method = MethodCopying;
		run_info[n_planned_runs].run_name = "Sequential 512-bit copy";
		n_planned_runs++;
	}

	for (unsigned r=0; r < n_planned_runs; r++) {
		$(console, newline);
		$(console, printf, "%s:\n", run_info[r].run_name);
		$(console, flush);

		ooc_bzero (threads, sizeof(threads));
		ooc_bzero (data, sizeof(data));
		ooc_bzero (results, sizeof(results));

		// Launch one thread per core.
		//
		for (unsigned core=0; core < nCores; core++) {
			struct thread_data *args = make_thread_data (benchmarks, core, results, 
				run_info[r].mode, 
				run_info[r].random,
				run_info[r].method);
			data[core] = args;
			pthread_create (&threads[core], NULL, &thread_routine, args);
#ifdef __linux__
			cpu_set_t cpuset;
			CPU_ZERO (&cpuset);
			CPU_SET (core, &cpuset);
			if (pthread_setaffinity_np (threads[core], sizeof(cpu_set_t), &cpuset) != 0) {
				// Not the end of the world, but good to report it to the user.
				perror("pthread_setaffinity_np");
			}
#endif
		}

		// Wait for each to finish, the sum their results.
		//
		long sum = 0;
		for (unsigned core=0; core < nCores; core++) {
			pthread_join (threads[core], NULL);
			long amount = results[core];
			float rate = (float)amount / 10.f;
			int startCore = data[core]->actualCoreAtStart;
			int endCore = data[core]->actualCoreAtEnd;
			if (startCore < 0) {
				$(console, printf, "Thread %u: %.1f MB/s (unknown core number)\n", core, rate);
			}
			else if (startCore == endCore) {
				$(console, printf, "Thread %u: %.1f MB/s (ran on core %d)\n", core, rate, startCore);
			}
			else {
				$(console, printf, "Thread %u: %.1f MB/s (ran on cores %d & %d)\n", core, rate, startCore, endCore);
			}
			sum += amount;
			free (data[core]);
		}
		float rate = (float)sum / 10.f;
		$(console, printf, "Sum: %.1f MB/s\n", rate);
	}

	$(console, newline);
}
