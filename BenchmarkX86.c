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

#include "defs.h"
#include "ObjectOriented.h"
#include "Object.h"
#include "OOC/DateTime.h"
#include "BenchmarkX86.h"
#include "Console.h"
#include "Log.h"
#include "routines.h"

#include <sys/mman.h>
#include <malloc.h>

BenchmarkX86Class* _BenchmarkX86Class = NULL;

//============================================================================
// Tests.
//============================================================================

static void BenchmarkX86_destroy (Any* self)
{
	if (!self) {
		return;
	}
	verifyCorrectClass(self,BenchmarkX86);
}

static void BenchmarkX86_describe (BenchmarkX86* self, FILE *outputFile)
{
        if (!self) {
                return;
	}
        verifyCorrectClass(self,BenchmarkX86);

        if (!outputFile) {
                outputFile = stdout;
	}
        fprintf (outputFile, "%s", $(self,className));
}

//----------------------------------------------------------------------------
// Name:	BenchmarkX86_write
// Purpose:	Performs write on chunk of memory of specified size.
//----------------------------------------------------------------------------
static long BenchmarkX86_write (BenchmarkX86 *self, unsigned long size, BenchmarkMode mode, bool random)
{
	if (size == CHECK_WHETHER_SUPPORTED) {
#ifdef IS_64BIT
		// x64
		if (!random) {
			switch (mode) {
			case SIZE_MAIN_REGISTER: // Writer
				return TEST_SUPPORTED;

			case SIZE_MAIN_REGISTER_NONTEMPORAL: // Writer_nontemporal
				return (!self->use_sse2) ? TEST_UNSUPPORTED : TEST_SUPPORTED;

			case SIZE_VECTOR_128: 
			case SIZE_VECTOR_128_NONTEMPORAL: // WriterSSE2_nontemporal
				return (!self->use_sse2) ? TEST_UNSUPPORTED : TEST_SUPPORTED;

			case SIZE_VECTOR_256:
			case SIZE_VECTOR_256_NONTEMPORAL: // WriterVector256_nontemporal
				return (!self->use_avx) ? TEST_UNSUPPORTED : TEST_SUPPORTED;

			case SIZE_VECTOR_512:
			case SIZE_VECTOR_512_NONTEMPORAL: // WriterVector512_nontemporal
				return (!self->use_avx512) ? TEST_UNSUPPORTED : TEST_SUPPORTED;
			}
		} else {
			switch (mode) {
			case SIZE_MAIN_REGISTER: // RandomWriter
				return TEST_SUPPORTED;

			case SIZE_MAIN_REGISTER_NONTEMPORAL:
				return TEST_UNSUPPORTED;

			case SIZE_VECTOR_128: 
			case SIZE_VECTOR_128_NONTEMPORAL: 
				return (!self->use_sse2) ? TEST_UNSUPPORTED : TEST_SUPPORTED;

			case SIZE_VECTOR_256: 
			case SIZE_VECTOR_256_NONTEMPORAL: // RandomWriterVector256_nontemporal
				return (!self->use_avx) ? TEST_UNSUPPORTED : TEST_SUPPORTED;

			case SIZE_VECTOR_512:
			case SIZE_VECTOR_512_NONTEMPORAL:
				return TEST_UNSUPPORTED;
			}
		}
#else
		// i386
		if (!random) {
			switch (mode) {
			case SIZE_MAIN_REGISTER: // Writer
				return TEST_SUPPORTED;

			case SIZE_MAIN_REGISTER_NONTEMPORAL:
				return TEST_UNSUPPORTED;

			case SIZE_VECTOR_128:	
			case SIZE_VECTOR_128_NONTEMPORAL: // WriterSSE2_nontemporal
				return (!self->use_sse2) ? TEST_UNSUPPORTED : TEST_SUPPORTED;

			case SIZE_VECTOR_256:
			case SIZE_VECTOR_256_NONTEMPORAL: // WriterVector256_nontemporal
				return (!self->use_avx) ? TEST_UNSUPPORTED : TEST_SUPPORTED;

			case SIZE_VECTOR_512:
			case SIZE_VECTOR_512_NONTEMPORAL:
				return TEST_UNSUPPORTED;
			}
		} else {
			switch (mode) {
			case SIZE_MAIN_REGISTER:
			case SIZE_VECTOR_128:	
			case SIZE_VECTOR_128_NONTEMPORAL: 
				return TEST_SUPPORTED;

			case SIZE_MAIN_REGISTER_NONTEMPORAL:
			case SIZE_VECTOR_256:
			case SIZE_VECTOR_256_NONTEMPORAL:
			case SIZE_VECTOR_512:
			case SIZE_VECTOR_512_NONTEMPORAL:
				return TEST_UNSUPPORTED;
			default:
				break;
			}
		} 
#endif

		return TEST_SUPPORTED;
	}

	//-------------------------------------------------
	unsigned char *chunk;
	unsigned long loops;
	unsigned long long total_count=0;
#ifdef IS_64BIT
	unsigned long long value = 0x1234567689abcdef;
#else
	unsigned long value = 0x12345678;
#endif
	unsigned long diff=0, t0;
	unsigned long **chunk_ptrs = NULL;

	chunk = aligned_alloc (64, size);
	if (!chunk) {
		error (__FUNCTION__, "Out of memory");
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
	}

	//-------------------------------------------------
	if (!self->quietMode) {
		if (random)
			$(console, printf, "Random write ");
		else
			$(console, printf, "Sequential write ");

		switch (mode) {
		case SIZE_VECTOR_128:
			$(console, printf, "(128-bit), size = ");
			break;
		case SIZE_VECTOR_256:
			$(console, printf, "(256-bit), size = ");
			break;
		case SIZE_VECTOR_512:
			$(console, printf, "(512-bit), size = ");
			break;
		case SIZE_VECTOR_128_NONTEMPORAL:
			$(console, printf, "nontemporal (128-bit), size = ");
			break;
		case SIZE_VECTOR_256_NONTEMPORAL:
			$(console, printf, "nontemporal (256-bit), size = ");
			break;
		case SIZE_VECTOR_512_NONTEMPORAL:
			$(console, printf, "nontemporal (512-bit), size = ");
			break;
		case SIZE_MAIN_REGISTER:
#ifdef IS_64BIT
			$(console, printf, "(64-bit), size = ");
#else
			$(console, printf, "(32-bit), size = ");
#endif
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
	}

	loops = (1 << 26) / size; 
	if (loops < 1) {
		loops = 1;
	}

	t0 = DateTime_getMicrosecondTime ();

	while (diff < self->usec_per_test) {
		total_count += loops;

		switch (mode) {
		case SIZE_VECTOR_128:
			if (!random) {
				WriterVector128 (chunk, size, loops, value);
			} else {
				RandomWriterVector128 ((void**) chunk_ptrs, size/256, loops, value);
			}
			break;

		case SIZE_VECTOR_128_NONTEMPORAL:
			if (!random) {
				WriterVector128_nontemporal (chunk, size, loops, value);
			} else {
				RandomWriterVector128_nontemporal ((void**)chunk_ptrs, size/256, loops, value);
			}
			break;

		case SIZE_VECTOR_256:
			if (!random) {
				WriterVector256 (chunk, size, loops, value);
			} else {
				RandomWriterVector256 ((void**)chunk_ptrs, size/256, loops, value);
			}
			break;

		case SIZE_VECTOR_512:
			if (!random) {
				WriterVector512 (chunk, size, loops, value);
			}
			break;

		case SIZE_VECTOR_256_NONTEMPORAL:
			if (!random) {
				WriterVector256_nontemporal (chunk, size, loops, value);
			} else {
				RandomWriterVector256_nontemporal ((void**)chunk_ptrs, size/256, loops, value);
			}
			break;

		case SIZE_VECTOR_512_NONTEMPORAL:
			if (!random) {
				WriterVector512_nontemporal (chunk, size, loops, value);
			} else {
				return TEST_UNSUPPORTED;
			}
			break;
		
		case SIZE_MAIN_REGISTER:
			if (!random) {
				Writer (chunk, size, loops, value);
			} else {
				RandomWriter ((void**) chunk_ptrs, size/256, loops, value);
			}
			break;

		case SIZE_MAIN_REGISTER_NONTEMPORAL:
			if (!random) {
				Writer_nontemporal (chunk, size, loops, value);
				Reader_nontemporal (chunk, size, loops);
			}
			break;

		default:
			break;
		}

		diff = DateTime_getMicrosecondTime () - t0;
	}

	if (!self->quietMode) {
		$(console, printf, "loops = ");
		$(console, printUnsigned, total_count);
		$(console, printf, ", ");
		$(console, flush);
	}

	long result = $(self, calculateResult, size, total_count, diff);
	$(console, flush);

	$(self, deferFreeOfChunk, (void*)chunk, size);

	if (chunk_ptrs) {
		$(self, deferFreeOfChunk, (void*)chunk_ptrs, sizeof (unsigned long*) * nChunks);
	}

	return result;
}

//----------------------------------------------------------------------------
// Name:	BenchmarkX86_read
// Purpose:	Performs sequential read on chunk of memory of specified size.
// Note:	SSE4 is required for the nontemporal 128-bit vector reads.
//----------------------------------------------------------------------------
static long BenchmarkX86_read (BenchmarkX86 *self, unsigned long size, BenchmarkMode mode, bool random)
{
	if (!self) {
		error_null_parameter(__FUNCTION__);
		return 0;
	}

	if (size == CHECK_WHETHER_SUPPORTED) {
#ifdef IS_64BIT
		// x64
		if (!random) {
			switch (mode) {
			case SIZE_MAIN_REGISTER: // Reader
				return TEST_SUPPORTED;

			case SIZE_MAIN_REGISTER_NONTEMPORAL: // Reader_nontemporal
				return TEST_UNSUPPORTED;

			case SIZE_VECTOR_128: 
				return (!self->use_sse2) ? TEST_UNSUPPORTED : TEST_SUPPORTED;

			case SIZE_VECTOR_128_NONTEMPORAL: 
				return (!self->use_sse4) ? TEST_UNSUPPORTED : TEST_SUPPORTED;

			case SIZE_VECTOR_256: 
			case SIZE_VECTOR_256_NONTEMPORAL: // ReaderVector256_nontemporal
				return (!self->use_avx) ? TEST_UNSUPPORTED : TEST_SUPPORTED;

			case SIZE_VECTOR_512:
			case SIZE_VECTOR_512_NONTEMPORAL: // ReaderVector512_nontemporal
				return (!self->use_avx512) ? TEST_UNSUPPORTED : TEST_SUPPORTED;

			default:
				return TEST_UNSUPPORTED;
			}
		} else {
			switch (mode) {
			case SIZE_MAIN_REGISTER: 
				return TEST_SUPPORTED;

			case SIZE_MAIN_REGISTER_NONTEMPORAL:
				return TEST_UNSUPPORTED;

			case SIZE_VECTOR_128: 
				return (!self->use_sse2) ? TEST_UNSUPPORTED : TEST_SUPPORTED;

			case SIZE_VECTOR_128_NONTEMPORAL: // RandomReaderVector128_nontemporal
				return (!self->use_sse4) ? TEST_UNSUPPORTED : TEST_SUPPORTED;

			case SIZE_VECTOR_256:
				return (!self->use_avx) ? TEST_UNSUPPORTED : TEST_SUPPORTED;

			case SIZE_VECTOR_256_NONTEMPORAL: 
			case SIZE_VECTOR_512:
			case SIZE_VECTOR_512_NONTEMPORAL:
				return TEST_UNSUPPORTED;
			}
		}
#else
		// i386
		if (!random) {
			switch (mode) {
			case SIZE_MAIN_REGISTER:
				return TEST_SUPPORTED;

			case SIZE_MAIN_REGISTER_NONTEMPORAL:
				return TEST_UNSUPPORTED;

			case SIZE_VECTOR_128:	
				return (!self->use_sse2) ? TEST_UNSUPPORTED : TEST_SUPPORTED;

			case SIZE_VECTOR_128_NONTEMPORAL: // ReaderVector128_nontemporal
				return (!self->use_sse4) ? TEST_UNSUPPORTED : TEST_SUPPORTED;

			case SIZE_VECTOR_256:
				return (!self->use_avx) ? TEST_UNSUPPORTED : TEST_SUPPORTED;

			case SIZE_VECTOR_256_NONTEMPORAL: 
			case SIZE_VECTOR_512:
			case SIZE_VECTOR_512_NONTEMPORAL:
				return TEST_UNSUPPORTED;
			}
		} else {
			switch (mode) {
			case SIZE_MAIN_REGISTER:
				return TEST_SUPPORTED;

			case SIZE_MAIN_REGISTER_NONTEMPORAL:
				return TEST_UNSUPPORTED;

			case SIZE_VECTOR_128:	
				return (!self->use_sse2) ? TEST_UNSUPPORTED : TEST_SUPPORTED;

			case SIZE_VECTOR_128_NONTEMPORAL: 
				return (!self->use_sse4) ? TEST_UNSUPPORTED : TEST_SUPPORTED;

			case SIZE_VECTOR_256:
			case SIZE_VECTOR_256_NONTEMPORAL:
			case SIZE_VECTOR_512:
			case SIZE_VECTOR_512_NONTEMPORAL:
				return TEST_UNSUPPORTED;
			default:
				break;
			}
		} 
#endif

		return TEST_SUPPORTED;
	}

	//-------------------------------------------------
	unsigned long long loops = 0;
	unsigned long long total_count = 0;
	unsigned long t0, diff=0;
	unsigned long *chunk;
	unsigned long **chunk_ptrs = NULL;

	chunk = aligned_alloc (64, size);
	if (!chunk) {
		error (__FUNCTION__, "Out of memory");
	}

	// Touch all memory blocks, in case a read from an unwritten block
	// is a no-op for the CPU.
	//
	ooc_bzero (chunk, size);
        unsigned long nChunks = size/256;

	//----------------------------------------
	// Set up random pointers to chunks.
	//
	if (random) {
		chunk_ptrs = (unsigned long**) malloc (sizeof (unsigned long*) * nChunks);
		if (!chunk_ptrs) {
			error (__FUNCTION__, "Out of memory.");
		}

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
	if (!self->quietMode) {
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

		case SIZE_MAIN_REGISTER_NONTEMPORAL:
#ifdef IS_64BIT
			$(console, printf, "nontemporal (64-bit), size = ");
#else
			$(console, printf, "nontemporal (32-bit), size = ");
#endif
			break;
		case SIZE_VECTOR_128:
			$(console, printf, "(128-bit), size = ");
			break;
		case SIZE_VECTOR_128_NONTEMPORAL:
			$(console, printf, "nontemporal (128-bit), size = ");
			break;
		case SIZE_VECTOR_256:
			$(console, printf, "(256-bit), size = ");
			break;
		case SIZE_VECTOR_256_NONTEMPORAL:
			$(console, printf, "nontemporal (256-bit), size = ");
			break;
		case SIZE_VECTOR_512:
			$(console, printf, "(512-bit), size = ");
			break;
		case SIZE_VECTOR_512_NONTEMPORAL:
			$(console, printf, "nontemporal (512-bit), size = ");
			break;

		default:
			break;
		}

		$(self, printSize, size);
		$(console, printf, ", ");
		$(console, flush);
	}

	loops = (1LU << 29) / size; 
	if (loops < 1) {
		loops = 1;
	}
	
	t0 = DateTime_getMicrosecondTime ();

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

		case SIZE_MAIN_REGISTER_NONTEMPORAL:
			if (!random) {
				Reader_nontemporal (chunk, size, loops);
			}
			break;

		case SIZE_VECTOR_128:
			if (random)
				RandomReaderVector128 ((void**) chunk_ptrs, size/256, loops);
			else {
				ReaderVector128 (chunk, size, loops);
			}
			break;
		
		case SIZE_VECTOR_128_NONTEMPORAL:
			if (random) {
				RandomReaderVector128_nontemporal ((void**) chunk_ptrs, size/256, loops);
			}
			else {
				ReaderVector128_nontemporal (chunk, size, loops);
			}
			break;

		case SIZE_VECTOR_256:
			if (!random) {
				ReaderVector256 (chunk, size, loops);
			} else {
				RandomReaderVector256 ((void**) chunk_ptrs, size/256, loops);
			}
			break;

		case SIZE_VECTOR_256_NONTEMPORAL:
			if (!random) {
				ReaderVector256_nontemporal (chunk, size, loops);
			} else { 
				return TEST_UNSUPPORTED;
			}
			break;
		
		case SIZE_VECTOR_512:
			if (!random) {
				ReaderVector512 (chunk, size, loops);
			} else {
				return TEST_UNSUPPORTED;
			}
			break;

		case SIZE_VECTOR_512_NONTEMPORAL:
			if (!random) {
				ReaderVector512_nontemporal (chunk, size, loops);
			} else { 
				return TEST_UNSUPPORTED;
			}
			break;

		default:
			exit(-9);
			break;
		}

		diff = DateTime_getMicrosecondTime () - t0;
	}

	if (!self->quietMode) {
		$(console, printf, "loops = ");
		$(console, printUnsigned, total_count);
		$(console, printf, ", ");
		$(console, flush);
	}

	long result = $(self, calculateResult, size, total_count, diff);
	$(console, flush);

	$(self, deferFreeOfChunk, (void*)chunk, size);

	if (chunk_ptrs) {
		$(self, deferFreeOfChunk, (void*)chunk_ptrs, sizeof (unsigned long*) * nChunks);
	}

	return result;
}

//----------------------------------------------------------------------------
// Name:	BenchmarkX86_copy
// Purpose:	Performs sequential memory copy.
//----------------------------------------------------------------------------
static long BenchmarkX86_copy (BenchmarkX86 *self, unsigned long size, BenchmarkMode mode)
{
	if (size == CHECK_WHETHER_SUPPORTED) {
#ifdef IS_64BIT
		// x64
		switch (mode) {
		case SIZE_MAIN_REGISTER: // CopyWithMainRegisters
			return TEST_SUPPORTED;

		case SIZE_MAIN_REGISTER_NONTEMPORAL:
			return TEST_UNSUPPORTED;

		case SIZE_VECTOR_128: 
			return (!self->use_sse2) ? TEST_UNSUPPORTED : TEST_SUPPORTED;

		case SIZE_VECTOR_256:
			return (!self->use_avx) ? TEST_UNSUPPORTED : TEST_SUPPORTED;

		case SIZE_VECTOR_512:
			return (!self->use_avx512) ? TEST_UNSUPPORTED : TEST_SUPPORTED;

		case SIZE_VECTOR_128_NONTEMPORAL: 
		case SIZE_VECTOR_256_NONTEMPORAL: 
		case SIZE_VECTOR_512_NONTEMPORAL: 
			return TEST_UNSUPPORTED;
		}
#else
		// i386
		switch (mode) {
		case SIZE_MAIN_REGISTER: // CopyWithMainRegisters
			return TEST_SUPPORTED;

		case SIZE_MAIN_REGISTER_NONTEMPORAL:
			return TEST_UNSUPPORTED;

		case SIZE_VECTOR_128:
			return (!self->use_sse2) ? TEST_UNSUPPORTED : TEST_SUPPORTED;

		case SIZE_VECTOR_256:	
			return (!self->use_avx) ? TEST_UNSUPPORTED : TEST_SUPPORTED;

		case SIZE_VECTOR_128_NONTEMPORAL: 
		case SIZE_VECTOR_256_NONTEMPORAL: 
		case SIZE_VECTOR_512:
		case SIZE_VECTOR_512_NONTEMPORAL:
			return TEST_UNSUPPORTED;
		}
#endif

		return TEST_SUPPORTED;
	}

	unsigned long loops;
	unsigned long long total_count = 0;
	unsigned long t0, diff=0;
	unsigned char *chunk_src;
	unsigned char *chunk_dest;

	switch (mode) {
	case SIZE_VECTOR_128_NONTEMPORAL:
	case SIZE_VECTOR_256_NONTEMPORAL:
	case SIZE_VECTOR_512_NONTEMPORAL:
		return TEST_UNSUPPORTED;
	default:
		break;
	}

	if (size == CHECK_WHETHER_SUPPORTED) {
		return TEST_SUPPORTED;
	}

	//-------------------------------------------------
	chunk_src = aligned_alloc (64, size);
	if (!chunk_src) {
		error (__FUNCTION__, "Out of memory");
	}
	chunk_dest = aligned_alloc (64, size);
	if (!chunk_dest) {
		error (__FUNCTION__, "Out of memory");
	}
	ooc_bzero (chunk_src, size);
	ooc_bzero (chunk_dest, size);

	//-------------------------------------------------
	if (!self->quietMode) {
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
		else if (mode == SIZE_VECTOR_256) {
			$(console, printf, "(256-bit), size = ");
		}
		else if (mode == SIZE_VECTOR_512) {
			$(console, printf, "(512-bit), size = ");
		}

		$(self, printSize, size);
		$(console, printf, ", ");
		$(console, flush);
	}

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
		else if (mode == SIZE_VECTOR_256) {
			CopyVector256 (chunk_dest, chunk_src, size, loops);
		}
		else if (mode == SIZE_VECTOR_512) {
			CopyVector512 (chunk_dest, chunk_src, size, loops);
		}

		diff = DateTime_getMicrosecondTime () - t0;
	}

	if (!self->quietMode) {
		$(console, printf, "loops = %llu, ", total_count);
		$(console, flush);
	}

	long result = $(self, calculateResult, size, total_count, diff);
	$(console, flush);

	$(self, deferFreeOfChunk, (void*)chunk_src, size);
	$(self, deferFreeOfChunk, (void*)chunk_dest, size);

	return result;
}

static long BenchmarkX86_registerToVectorMove (BenchmarkX86 *self)
{
#ifdef IS_64BIT
	$(console, printf, "Main register to vector register moves (64-bit, MOVQ): ");
#else
	$(console, printf, "Main register to vector register moves (32-bit, MOVD): ");
#endif
	$(console, flush);

	long long total_count = 0;
	unsigned long diff = 0;
	unsigned long t0 = DateTime_getMicrosecondTime ();

	while (diff < self->usec_per_test)
	{
		RegisterToVectorMove (VREGISTER_TRANSFERS_COUNT);
		total_count += VREGISTER_TRANSFERS_COUNT;

		diff = DateTime_getMicrosecondTime () - t0;
	}
	
	long double d = total_count;
	d *= N_VREG_TO_VREG_PER_LOOP;
	d /= diff;
	d *= 1000000; // usec->sec
	d /= 1000000000; // billions/sec
	$(console, printf, "%.2Lf billion/second\n", d);
	$(console, flush);

	return 0;
}

static long BenchmarkX86_vectorToRegisterMove (BenchmarkX86 *self)
{
#ifdef IS_64BIT
	$(console, printf, "Vector register to main register moves (64-bit, MOVQ): ");
#else
	$(console, printf, "Vector register to main register moves (32-bit, MOVD): ");
#endif
	$(console, flush);

	unsigned long t0 = DateTime_getMicrosecondTime ();
	unsigned long diff = 0;
	long long total_count = 0;
	
	while (diff < self->usec_per_test)
	{
		VectorToRegisterMove (VREGISTER_TRANSFERS_COUNT);
		total_count += VREGISTER_TRANSFERS_COUNT;

		diff = DateTime_getMicrosecondTime () - t0;
	}

	long double d = total_count;
	d *= N_VREG_TO_VREG_PER_LOOP;
	d /= diff;
	d *= 1000000; // usec->sec
	d /= 1000000000; // billions/sec
	$(console, printf, "%.2Lf billion/second\n", d);
	$(console, flush);

	return 0;
}

static long BenchmarkX86_vectorToVectorTest256 (BenchmarkX86 *self)
{
	if (self->use_avx) {
		$(console, printf, "Vector register to vector register transfers (256-bit): ");
		$(console, flush);

		long long total_count = 0;
		unsigned long diff = 0;
		unsigned long t0 = DateTime_getMicrosecondTime ();

		while (diff < self->usec_per_test)
		{
			VectorToVector256 (VREGISTER_TRANSFERS_COUNT);
			total_count += VREGISTER_TRANSFERS_COUNT;

			diff = DateTime_getMicrosecondTime () - t0;
		}

		long double d = total_count;
		d *= N_VREG_TO_VREG_PER_LOOP;
		d /= diff;
		d *= 1000000; // usec->sec
		d /= 1000000000; // billions/sec
		$(console, printf, "%.2Lf billion/second\n", d);
		$(console, flush);
	}
	return 0;
}

static long BenchmarkX86_vectorToVectorTest512 (BenchmarkX86 *self)
{
	if (self->use_avx512) {
		$(console, printf, "Vector register to vector register transfers (512-bit): ");
		$(console, flush);

		long long total_count = 0;
		unsigned long diff = 0;
		unsigned long t0 = DateTime_getMicrosecondTime ();

		while (diff < self->usec_per_test)
		{
			VectorToVector512 (VREGISTER_TRANSFERS_COUNT);
			total_count += VREGISTER_TRANSFERS_COUNT;

			diff = DateTime_getMicrosecondTime () - t0;
		}

		long double d = total_count;
		d *= N_VREG_TO_VREG_PER_LOOP;
		d /= diff;
		d *= 1000000; // usec->sec
		d /= 1000000000; // billions/sec
		$(console, printf, "%.2Lf billion/second\n", d);
		$(console, flush);
	}
	return 0;
}

static MemoryCorrectnessTestResult BenchmarkX86_memoryRowHammerTest (BenchmarkX86* self, unsigned long bufferSize, unsigned long nCycles)
{
	if (!self) {
		return MemoryCorrectnessTestResultUnsupported;
	}
	verifyCorrectClass(self,BenchmarkX86);

#ifndef IS_64BIT
	return MemoryCorrectnessTestResultUnsupported;
#else 

	// 0 cycles means main() is checking whether the test is supported.
	if (!nCycles) {
		return MemoryCorrectnessTestResultSupported;
	}

	if (!bufferSize) {
		bufferSize = 12288;
	}

	unsigned long *buffer = memalign (4096, bufferSize);
	if (!buffer) {
		Log_perror(__FUNCTION__, "memalign");
		return MemoryCorrectnessTestResultMallocFailed;
	}
	long result = RowHammerTest (buffer, bufferSize, nCycles);
	free (buffer);
	
	return !result ? MemoryCorrectnessTestResultSuccess : MemoryCorrectnessTestResultFailure;
#endif
}

BenchmarkX86Class* BenchmarkX86Class_init (BenchmarkX86Class* class)
{
	SET_SUPERCLASS(Benchmark);

	SET_OVERRIDDEN_METHOD_POINTER(BenchmarkX86,describe);
	SET_OVERRIDDEN_METHOD_POINTER(BenchmarkX86,destroy);
	SET_OVERRIDDEN_METHOD_POINTER(BenchmarkX86,memoryRowHammerTest);

	SET_METHOD_POINTER(BenchmarkX86,read);
	SET_METHOD_POINTER(BenchmarkX86,write);
	SET_METHOD_POINTER(BenchmarkX86,copy);
	SET_METHOD_POINTER(BenchmarkX86,vectorToVectorTest256);
	SET_METHOD_POINTER(BenchmarkX86,vectorToVectorTest512);
	SET_METHOD_POINTER(BenchmarkX86,registerToVectorMove);
	SET_METHOD_POINTER(BenchmarkX86,vectorToRegisterMove);
	
        VALIDATE_CLASS_STRUCT(_BenchmarkX86Class);
	return _BenchmarkX86Class;
}

BenchmarkX86 *BenchmarkX86_init (BenchmarkX86 *self)
{
	ENSURE_CLASS_READY(BenchmarkX86);

        Benchmark_init ((Benchmark*) self);

        self->is_a = _BenchmarkX86Class;
	self->vectorToFromRegisterRoutinesAvailable = true;

        return self;
}

