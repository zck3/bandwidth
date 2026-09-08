/*============================================================================
  BenchmarkRISCV, an Object-Oriented C benchmark for riscv64.
  Copyright (C) 2023, 2026 by Zack T Smith.

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

#include "defs.h"
#include "ObjectOriented.h"
#include "Object.h"
#include "OOC/DateTime.h"
#include "BenchmarkRISCV.h"
#include "Console.h"
#include "routines.h"

BenchmarkRISCVClass* _BenchmarkRISCVClass = NULL;

extern Console* console;

static unsigned dummy = 0;

//============================================================================
// Tests.
//============================================================================

static void BenchmarkRISCV_destroy (Any* self)
{
	if (!self) {
		return;
	}
	verifyCorrectClass(self,BenchmarkRISCV);
}

static void BenchmarkRISCV_describe (BenchmarkRISCV* self, FILE *outputFile)
{
        if (!self) {
                return;
	}
        verifyCorrectClass(self,BenchmarkRISCV);

        if (!outputFile) {
                outputFile = stdout;
	}
        fprintf (outputFile, "%s", $(self,className));
}

//----------------------------------------------------------------------------
// Name:	BenchmarkRISCV_write
// Purpose:	Performs write on chunk of memory of specified size.
//----------------------------------------------------------------------------
static long BenchmarkRISCV_write (BenchmarkRISCV *self, unsigned long size, BenchmarkMode mode, bool random)
{
	if (size == CHECK_WHETHER_SUPPORTED) {
		switch (mode) {
		case SIZE_MAIN_REGISTER:
			return TEST_SUPPORTED;
		default:
			return TEST_UNSUPPORTED;
		}
	}

	//-------------------------------------------------
	unsigned char *chunk;
	unsigned char *chunk0;
	unsigned long loops;
	unsigned long total_count=0;
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

	chunk0 = malloc (size+256);
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

	unsigned long nChunks = size/256;

	//----------------------------------------
	// Set up random pointers to chunks.
	//
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
#ifdef DUMP_CHUNK_POINTERS
		for (i = 0; i < nChunks && i < 32; i++) {
			unsigned long *ptr = chunk_ptrs [i];
			printf ("chunk %2d = %lx\n",i,(unsigned long)ptr);
		}
#endif
	}

	//-------------------------------------------------
	if (random)
		$(console, printf, "Random write ");
	else
		$(console, printf, "Sequential write ");

	switch (mode) {
	case SIZE_MAIN_REGISTER:
#ifdef IS_64BIT
		$(console, printf, "(64-bit), size = ");
#else
		$(console, printf, "(32-bit), size = ");
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

	unsigned long total_time = 0;

	while (total_time < self->usec_per_test) {
		total_count += loops;

		t0 = DateTime_getMicrosecondTime ();

		switch (mode) {
		case SIZE_MAIN_REGISTER:
			if (random) {
				unsigned long nChunks = size / 256;
				RandomWriter (chunk_ptrs, nChunks, loops, value);
			}
			else {
				Writer (chunk, size, loops, value);
			}
			break;

		default:
			break;
		}

		diff = DateTime_getMicrosecondTime () - t0;
		total_time += diff;

		// This effectively flushes any write buffer.
		unsigned temp = 0;
		for (unsigned index = 0; index < size; index += 64) {
			temp += chunk[index];
		}
		dummy = temp;
	}
	$(console, printf, "loops = ");
	$(console, printUnsigned, total_count);
	$(console, printf, ", ");
	$(console, flush);

	int result = $(self, calculateResult, size, total_count, total_time);
	$(console, flush);

	$(self, deferFreeOfChunk, (void*)chunk0, size+256);

	if (chunk_ptrs) {
		$(self, deferFreeOfChunk, (void*)chunk_ptrs, sizeof (unsigned long*) * nChunks);
	}

	return result;
}

//----------------------------------------------------------------------------
// Name:	BenchmarkRISCV_read
// Purpose:	Performs sequential read on chunk of memory of specified size.
//----------------------------------------------------------------------------
static long BenchmarkRISCV_read (BenchmarkRISCV *self, unsigned long size, BenchmarkMode mode, bool random)
{
	if (size == CHECK_WHETHER_SUPPORTED) {
		switch (mode) {
		case SIZE_MAIN_REGISTER:
			return TEST_SUPPORTED;
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

	// Touch all memory blocks.
	unsigned long nChunks = size/256;
	char *touchPtr = (char*) chunk;
	for (unsigned long i=0; i < nChunks; i++) {
		*touchPtr = 0;
		touchPtr += 256;
	}

	//----------------------------------------
	// Set up random pointers to chunks.
	//
	if (random) {
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
	
	default:
		break;
	}

	$(self, printSize, size);
	$(console, printf, ", ");

	$(console, flush);

	uint64_t loops;
	uint64_t total_count = 0;
	uint64_t diff = 0;

	loops = (1 << 26) / size; 
	if (loops < 1) {
		loops = 1;
	}

	uint64_t t0 = DateTime_getMicrosecondTime ();

	while (diff < self->usec_per_test) {
		total_count += loops;

		switch (mode) {
		case SIZE_MAIN_REGISTER:
			if (random) {
				RandomReader (chunk_ptrs, size/256, loops);
			} else {
				Reader (chunk, size, loops);
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

	$(self, deferFreeOfChunk, (void*)chunk0, size+128);

	if (chunk_ptrs) {
		$(self, deferFreeOfChunk, (void*)chunk_ptrs, sizeof (unsigned long*) * nChunks);
	}

	return result;
}

//----------------------------------------------------------------------------
// Name:	BenchmarkRISCV_copy
// Purpose:	Performs sequential memory copy.
//----------------------------------------------------------------------------
static long BenchmarkRISCV_copy (BenchmarkRISCV *self, unsigned long size, BenchmarkMode mode)
{
	if (size == CHECK_WHETHER_SUPPORTED) {
		switch (mode) {
		case SIZE_MAIN_REGISTER:
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
		$(console, printf, "(64-bit), size = ");
	}

	$(self, printSize, size);
	$(console, printf, ", ");
	$(console, flush);

	loops = (1 << 26) / size;
	if (loops < 1) {
		loops = 1;
	}
	
	unsigned long total_time = 0;

	while (total_time < self->usec_per_test) {
		total_count += loops;

		t0 = DateTime_getMicrosecondTime ();

		if (mode == SIZE_MAIN_REGISTER ) {
			CopyWithMainRegisters (chunk_dest, chunk_src, size, loops);
		}

		diff = DateTime_getMicrosecondTime () - t0;
		total_time += diff;

		// This effectively flushes any write buffer.
		uint64_t temp = 0;
		for (unsigned index = 0; index < size; index += 64) {
			temp += chunk_dest[index];
		}
		dummy = temp;
	}

	$(console, printf, "loops = %llu, ", total_count);
	$(console, flush);

	int result = $(self, calculateResult, size, total_count, total_time);
	$(console, flush);

	$(self, deferFreeOfChunk, (void*)chunk_src0, size+64);
	$(self, deferFreeOfChunk, (void*)chunk_dest0, size+64);

	return result;
}

BenchmarkRISCVClass* BenchmarkRISCVClass_init (BenchmarkRISCVClass *class)
{
	SET_SUPERCLASS(Benchmark);

	SET_OVERRIDDEN_METHOD_POINTER(BenchmarkRISCV,describe);
	SET_OVERRIDDEN_METHOD_POINTER(BenchmarkRISCV,destroy);

	SET_METHOD_POINTER(BenchmarkRISCV,read);
	SET_METHOD_POINTER(BenchmarkRISCV,write);
	SET_METHOD_POINTER(BenchmarkRISCV,copy);

	SET_ABSTRACT_METHOD_POINTER(registerToVectorTest);
	SET_ABSTRACT_METHOD_POINTER(vectorToRegisterTest);
	SET_ABSTRACT_METHOD_POINTER(vectorToVectorTest128);
	SET_ABSTRACT_METHOD_POINTER(vectorToVectorTest256);
	SET_ABSTRACT_METHOD_POINTER(vectorToRegister8);
	SET_ABSTRACT_METHOD_POINTER(vectorToRegister16);
	SET_ABSTRACT_METHOD_POINTER(vectorToRegister32);
	SET_ABSTRACT_METHOD_POINTER(vectorToRegister64);
	SET_ABSTRACT_METHOD_POINTER(registerToVector8);
	SET_ABSTRACT_METHOD_POINTER(registerToVector16);
	SET_ABSTRACT_METHOD_POINTER(registerToVector32);
	SET_ABSTRACT_METHOD_POINTER(registerToVector64);
	
        VALIDATE_CLASS_STRUCT(_BenchmarkRISCVClass);
	return _BenchmarkRISCVClass;
}

BenchmarkRISCV *BenchmarkRISCV_init (BenchmarkRISCV *self)
{
	ENSURE_CLASS_READY(BenchmarkRISCV);

        Benchmark_init ((Benchmark*) self);

        self->is_a = _BenchmarkRISCVClass;

        return self;
}

