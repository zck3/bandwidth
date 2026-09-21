/*============================================================================
  bandwidth, a benchmark to measure memory transfer bandwidth.
  Copyright (C) 2005-2023, 2026 by Zack T Smith.

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

#include <stdio.h>
#include <stdint.h>
#include <stdlib.h>

#if defined(__linux__)
#include <malloc.h>
#endif

#include "defs.h"
#include "ObjectOriented.h"
#include "Object.h"
#include "OOC/DateTime.h"
#include "BenchmarkARM.h"
#include "Console.h"
#include "Log.h"
#include "routines.h"

BenchmarkARMClass* _BenchmarkARMClass = NULL;

extern Console* console;

//============================================================================
// Tests.
//============================================================================

static void BenchmarkARM_destroy (Any* self)
{
	if (!self) {
		return;
	}
	verifyCorrectClass(self,BenchmarkARM);
}

static void BenchmarkARM_describe (BenchmarkARM* self, FILE *outputFile)
{
        if (!self) {
                return;
	}
        verifyCorrectClass(self,BenchmarkARM);

        if (!outputFile) {
                outputFile = stdout;
	}
        fprintf (outputFile, "%s", $(self,className));
}

//----------------------------------------------------------------------------
// Name:	BenchmarkARM_write
// Purpose:	Performs write on chunk of memory of specified size.
//----------------------------------------------------------------------------
static long BenchmarkARM_write (BenchmarkARM *self, unsigned long size, BenchmarkMode mode, bool random)
{
	if (size == CHECK_WHETHER_SUPPORTED) {
		switch (mode) {
		case SIZE_MAIN_REGISTER:
		case SIZE_VECTOR_128:
			return TEST_SUPPORTED;

		case SIZE_MAIN_REGISTER_NONTEMPORAL:
#ifdef IS_64BIT
			return !random ? TEST_SUPPORTED : TEST_UNSUPPORTED;
#else
			return TEST_UNSUPPORTED;
#endif

		default:
			return TEST_UNSUPPORTED;
		}
	}

	unsigned char *chunk;
	unsigned char *chunk0;
	unsigned long loops;
	unsigned long long total_count=0;
#ifdef IS_64BIT
	unsigned long value = 0x1234567689abcdef;
#else
	unsigned long value = 0x12345678;
#endif
	unsigned long diff=0, t0;
	unsigned long **chunk_ptrs = NULL;

	if (size & 255) {
		error (__FUNCTION__, "Chunk size is not multiple of 256.");
	}

	//-------------------------------------------------
	chunk0 = malloc (size+128);
	if (!chunk0) {
		error (__FUNCTION__, "Out of memory");
	}
	
	chunk = chunk0;
	unsigned long tmp = (unsigned long) chunk;
	if (tmp & 31) {
		tmp -= (tmp & 31);
		tmp += 32;
		chunk = (unsigned char*) tmp;
	}

	//----------------------------------------
	// Set up random pointers to chunks.
	//
	unsigned long nChunks = size/256;
	if (random) {
		chunk_ptrs = (unsigned long**) malloc (sizeof (unsigned long*) * nChunks);
		if (!chunk_ptrs) {
			error (__FUNCTION__, "Out of memory.");
		}

		//-----------------------------------------
		// Store pointers to all chunks in an array.
		//
		int i;
		for (i = 0; i < nChunks; i++) {
			chunk_ptrs [i] = (unsigned long*) (((char*)chunk) + 256 * i);
		}

		//----------------------------------------
		// Randomize the array of chunk pointers.
		//
		int k = N_RANDOMIZATION_LOOPS;
		while (k--) {
			for (i = 0; i < nChunks; i++) {
				int j = rand() % nChunks;
				if (i != j) {
					unsigned long *ptr = chunk_ptrs [i];
					chunk_ptrs [i] = chunk_ptrs [j];
					chunk_ptrs [j] = ptr;
				}
			}
		}
	}

	//-------------------------------------------------
	if (random) {
		$(console, printf, "Random write ");
	} else {
		$(console, printf, "Sequential write ");
	}

	switch (mode) {
	case SIZE_MAIN_REGISTER:
#ifdef IS_64BIT
		$(console, printf, "(64-bit), size = ");
#else
		$(console, printf, "(32-bit), size = ");
#endif
		break;
	case SIZE_VECTOR_128:
		$(console, printf, "(128-bit), size = ");
		break;

	case SIZE_MAIN_REGISTER_NONTEMPORAL:
#ifdef IS_64BIT
		$(console, printf, "nontemporal (64-bit), size = ");
#else
		$(console, printf, "nontemporal (32-bit), size = ");
#endif
		break;

	default:
		break;
	}

	$(self, printSize, size);
	$(console, printf, ", ");

	loops = (1 << 26) / size;
	if (loops < 1) {
		loops = 1;
	}

	t0 = DateTime_getMicrosecondTime ();

	while (diff < self->usec_per_test) {
		total_count += loops;

		switch (mode) {
		case SIZE_MAIN_REGISTER:
			if (random)
				RandomWriter ((void**) chunk_ptrs, size/256, loops, value);
			else {
				Writer (chunk, size, loops, value);
			}
			break;

		case SIZE_VECTOR_128:
			if (random)
				RandomWriterVector128 ((void**) chunk_ptrs, size/256, loops, value);
			else
                        	WriterVector128 (chunk, size, loops, value);
			break;

		case SIZE_MAIN_REGISTER_NONTEMPORAL:
			if (!random) {
				Writer_nontemporal (chunk, size, loops, value);
			}
			break;

		default:
			break;
		}

		diff = DateTime_getMicrosecondTime () - t0;
	}

	$(console, printf, "loops = ");
	$(console, printUnsigned, total_count);
	$(console, printf, ", ");

	$(console, flush);

	int result = $(self, calculateResult, size, total_count, diff);
	$(console, flush);

	$(self, deferFreeOfChunk, chunk0, size+128);

	if (chunk_ptrs) {
		$(self, deferFreeOfChunk, chunk_ptrs, sizeof (unsigned long*) * nChunks);
	}

	return result;
}

//----------------------------------------------------------------------------
// Name:	BenchmarkARM_read
// Purpose:	Performs sequential read on chunk of memory of specified size.
//----------------------------------------------------------------------------
static long BenchmarkARM_read (BenchmarkARM *self, unsigned long size, BenchmarkMode mode, bool random)
{
	if (size == CHECK_WHETHER_SUPPORTED) {
		switch (mode) {
		case SIZE_MAIN_REGISTER:
		case SIZE_VECTOR_128:
			return TEST_SUPPORTED;

		case SIZE_MAIN_REGISTER_NONTEMPORAL:
#ifdef IS_64BIT
			return !random ? TEST_SUPPORTED : TEST_UNSUPPORTED;
#else
			return TEST_UNSUPPORTED;
#endif

		default:
			return TEST_UNSUPPORTED;
		}
	}

	unsigned long *chunk;
	unsigned long *chunk0;
	unsigned long **chunk_ptrs = NULL;

	if (size & 255) {
		error (__FUNCTION__, "Chunk size is not multiple of 256.");
	}

	//-------------------------------------------------
	chunk0 = malloc (size+128);
	if (!chunk0) {
		error (__FUNCTION__, "Out of memory");
	}

	chunk = chunk0;
	unsigned long tmp = (unsigned long) chunk;
	if (tmp & 31) {
		tmp -= (tmp & 31);
		tmp += 32;
		chunk = (unsigned long*) tmp;
	}

	// Touch all memory blocks, in case a read from an unwritten block
	// is a no-op for the CPU.
	unsigned long nChunks = size/256;
	char *touchPtr = (char*) chunk;
	for (unsigned long i=0; i < nChunks; i += 16) {
		*touchPtr = 0;
		touchPtr += 4096;
	}

	//----------------------------------------
	// Set up random pointers to chunks.
	//
	if (random) {
		int nChunks = size/256;
		chunk_ptrs = (unsigned long**) malloc (sizeof (unsigned long*) * nChunks);
		if (!chunk_ptrs)
			error (__FUNCTION__, "Out of memory.");

		//----------------------------------------
		// Store pointers to all chunks into array.
		//
		int i;
		for (i = 0; i < nChunks; i++) {
			chunk_ptrs [i] = (unsigned long*) (((char*)chunk) + 256 * i);
		}

		//----------------------------------------
		// Randomize the array of chunk pointers.
		//
		int k = N_RANDOMIZATION_LOOPS;
		while (k--) {
			for (i = 0; i < nChunks; i++) {
				int j = rand() % nChunks;
				if (i != j) {
					unsigned long *ptr = chunk_ptrs [i];
					chunk_ptrs [i] = chunk_ptrs [j];
					chunk_ptrs [j] = ptr;
				}
			}
		}
	}

	//-------------------------------------------------
	if (random)
		$(console, printf, "Random read ");
	else
		$(console, printf, "Sequential read ");

	switch (mode) {
	case SIZE_MAIN_REGISTER:
#ifdef IS_64BIT
		$(console, printf, "(64-bit), size = ");
#else
		$(console, printf, "(32-bit), size = ");
#endif
		break;
	case SIZE_VECTOR_128:
		$(console, printf, "(128-bit), size = ");
		break;

	case SIZE_MAIN_REGISTER_NONTEMPORAL:
#ifdef IS_64BIT
		$(console, printf, "nontemporal (64-bit), size = ");
#else
		$(console, printf, "nontemporal (32-bit), size = ");
#endif
		break;

	default:
		break;
	}

	$(self, printSize, size);
	$(console, printf, ", ");

	$(console, flush);

	uint64_t loops;
	uint64_t total_count = 0;
	uint64_t t0 = DateTime_getMicrosecondTime ();
	uint64_t diff = 0;

	loops = (1 << 26) / size; 
	if (loops < 1)
		loops = 1;

	while (diff < self->usec_per_test) {
		total_count += loops;

		switch (mode) {
		case SIZE_MAIN_REGISTER:
			if (random) {
				RandomReader ((void**) chunk_ptrs, size/256, loops);
			} else {
				Reader (chunk, size, loops);
			}
			break;

		case SIZE_VECTOR_128:
			if (random)
				RandomReaderVector128 ((void**) chunk_ptrs, size/256, loops);
			else
				ReaderVector128 (chunk, size, loops);
			break;
		
		case SIZE_MAIN_REGISTER_NONTEMPORAL:
			if (!random) {
				Reader_nontemporal (chunk, size, loops);
			}
			break;

		default:
			break;
		}

		diff = DateTime_getMicrosecondTime () - t0;
	}

	$(console, printf, "loops = ");
	$(console, printUnsigned, total_count);
	$(console, printf, ", ");

	int result = $(self, calculateResult, size, total_count, diff);
	$(console, flush);

	$(self, deferFreeOfChunk, chunk0, size+128);

	if (chunk_ptrs) {
		$(self, deferFreeOfChunk, chunk_ptrs, sizeof (unsigned long*) * nChunks);
	}

	return result;
}

//----------------------------------------------------------------------------
// Name:	BenchmarkARM_copy
// Purpose:	Performs sequential memory copy.
//----------------------------------------------------------------------------
static long BenchmarkARM_copy (BenchmarkARM *self, unsigned long size, BenchmarkMode mode)
{
	if (size == CHECK_WHETHER_SUPPORTED) {
		switch (mode) {
		case SIZE_MAIN_REGISTER:
		case SIZE_VECTOR_128:
			return TEST_SUPPORTED;
		default:
			return TEST_UNSUPPORTED;
		}
	}

	//-------------------------------------------------
	unsigned long loops;
	unsigned long long total_count = 0;
	unsigned long t0, diff=0;
	unsigned char *chunk_src;
	unsigned char *chunk_dest;
	unsigned char *chunk_src0;
	unsigned char *chunk_dest0;

	chunk_src0 = malloc (size+64);
	if (!chunk_src0) {
		error (__FUNCTION__, "Out of memory");
	}
	chunk_dest0 = malloc (size+64);
	if (!chunk_dest0) {
		error (__FUNCTION__, "Out of memory");
	}

	chunk_src = chunk_src0;
	chunk_dest = chunk_dest0;
	
	unsigned long tmp = (unsigned long) chunk_src;
	if (tmp & 31) {
		tmp -= (tmp & 31);
		tmp += 32;
		chunk_src = (unsigned char*) tmp;
	}
	tmp = (unsigned long) chunk_dest;
	if (tmp & 31) {
		tmp -= (tmp & 31);
		tmp += 32;
		chunk_dest = (unsigned char*) tmp;
	}

	ooc_bzero (chunk_src, size);
	ooc_bzero (chunk_dest, size);

	//-------------------------------------------------
	$(console, printf, "Sequential copy ");

	if (mode == SIZE_MAIN_REGISTER) {
#ifdef IS_64BIT
		$(console, printf, "(64-bit), size = ");
#else
		$(console, printf, "(32-bit), size = ");
#endif
	}
	else if (mode == SIZE_VECTOR_128) {
		$(console, printf, "(128-bit), size = ");
	}

	$(self, printSize, size);
	$(console, printf, ", ");
	$(console, flush);

	loops = (1 << 26) / size; 
	if (loops < 1) {
		loops = 1;
	}
	
	t0 = DateTime_getMicrosecondTime ();

	while (diff < self->usec_per_test) {
		total_count += loops;

		if (mode == SIZE_MAIN_REGISTER ) {
			CopyWithMainRegisters (chunk_dest, chunk_src, size, loops);
		}
		else if (mode == SIZE_VECTOR_128) {
			CopyVector128 (chunk_dest, chunk_src, size, loops);
		}

		diff = DateTime_getMicrosecondTime () - t0;
	}

	$(console, printf, "loops = %llu, ", total_count);
	$(console, flush);

	int result = $(self, calculateResult, size, total_count, diff);
	$(console, flush);

	$(self, deferFreeOfChunk, chunk_src0, size+64);
	$(self, deferFreeOfChunk, chunk_dest0, size+64);

	return result;
}

static MemoryCorrectnessTestResult BenchmarkARM_memoryRowHammerTest (BenchmarkARM* self, unsigned long bufferSize, unsigned long nCycles)
{
	if (!self) {
		return MemoryCorrectnessTestResultUnsupported;
	}
	verifyCorrectClass(self,BenchmarkARM);

#if !defined(IS_64BIT) || !defined(__GNUC__)
	return MemoryCorrectnessTestResultUnsupported;
#else

	// 0 cycles means the caller is checking whether the test is supported.
	if (!nCycles) {
		return MemoryCorrectnessTestResultSupported;
	}

	if (!bufferSize) {
		bufferSize = 12288;
	}

#if defined(__linux__)
	unsigned long *buffer = memalign (4096, bufferSize);
	if (!buffer) {
		Log_perror(__FUNCTION__, "memalign");
		return MemoryCorrectnessTestResultMallocFailed;
	}
#else
	void *buffer = NULL;
	if (posix_memalign (&buffer, 4096, bufferSize)) {
		Log_perror(__FUNCTION__, "posix_memalign");
		return MemoryCorrectnessTestResultMallocFailed;
	}
#endif

	register uint64_t value = ~(0LU);
	register const size_t n = bufferSize / 32;
	register const unsigned char *start = (const unsigned char*) buffer;
	register const unsigned char *end = &start[bufferSize];

	repeat(nCycles) {
		register uint64_t *ptr = (uint64_t*) start;
		for (register int i=0; i < n; i++) {
			ptr[i] = value;
			ptr[i+1] = value;
			ptr[i+2] = value;
			ptr[i+3] = value;
			ptr += 4;
		}
		__clear_cache ((void*)start, (void*)end);
	}

	// Check for cleared bits.
	uint64_t *ptr = (uint64_t*) start;
	for (int i=0; i < n; i++) {
		value &= ptr[i];
		value &= ptr[i+1];
		value &= ptr[i+2];
		value &= ptr[i+3];
		ptr += 4;
	}
	bool bitsCleared = 0LU != ~value;
	free (buffer);
	return !bitsCleared ? MemoryCorrectnessTestResultSuccess : MemoryCorrectnessTestResultFailure;
#endif
}

BenchmarkARMClass* BenchmarkARMClass_init (BenchmarkARMClass *class)
{
	SET_SUPERCLASS(Benchmark);

	SET_OVERRIDDEN_METHOD_POINTER(BenchmarkARM,describe);
	SET_OVERRIDDEN_METHOD_POINTER(BenchmarkARM,destroy);

	SET_METHOD_POINTER(BenchmarkARM,read);
	SET_METHOD_POINTER(BenchmarkARM,write);
	SET_METHOD_POINTER(BenchmarkARM,copy);
	SET_METHOD_POINTER(BenchmarkARM,memoryRowHammerTest);

	SET_ABSTRACT_METHOD_POINTER(registerToVectorMove);
	SET_ABSTRACT_METHOD_POINTER(vectorToRegisterMove);
	SET_ABSTRACT_METHOD_POINTER(vectorToVectorTest256);
	
        VALIDATE_CLASS_STRUCT(_BenchmarkARMClass);
	return _BenchmarkARMClass;
}

BenchmarkARM *BenchmarkARM_init (BenchmarkARM *self)
{
	ENSURE_CLASS_READY(BenchmarkARM);

        Benchmark_init ((Benchmark*) self);

        self->is_a = _BenchmarkARMClass;
	self->vectorToFromRegisterRoutinesAvailable = true;

        return self;
}

