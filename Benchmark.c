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
#include "routines.h"
#include "ObjectOriented.h"
#include "Object.h"
#include "OOC/DateTime.h"
#include "OOC/Log.h"
#include "OOC/OS.h"
#include "Benchmark.h"
#include "BenchmarkX86.h"
#include "Console.h"

#include <unistd.h> // usleep
#include <pthread.h> 

BenchmarkClass* _BenchmarkClass = NULL;

static void Benchmark_printSize (Benchmark *self, size_t size)
{
        if (!self) {
                return;
	}
        verifyCorrectClassOrSubclass(self,Benchmark);

	if (self->quietMode) {
		return;
	}

	if (size < 1536) {
		$(console, printInt, size);
		$(console, printf, " B");
	}
	else if (size < (1<<20)) {
		$(console, printInt, size >> 10);
		$(console, printf, " kB");
	} else {
		$(console, printInt, size >> 20);
		switch ((size >> 18) & 3) {
		case 1: $(console, printf, ".25"); break;
		case 2: $(console, printf, ".5"); break;
		case 3: $(console, printf, ".75"); break;
		}
		$(console, printf, " MB");
	}
}

static void Benchmark_destroy (Any* self_)
{
	if (!self_) {
		return;
	}
	verifyCorrectClassOrSubclass(self_,Benchmark);
	Benchmark *self = self_;

	$(self, freeDeferredChunks);

	Object_destroy((Object*)self);
}

static void Benchmark_describe (Benchmark* self, FILE *outputFile)
{
        if (!self) {
                return;
	}
        verifyCorrectClassOrSubclass(self,Benchmark);

        if (!outputFile) {
                outputFile = stdout;
	}
        fprintf (outputFile, "%s", $(self,className));
}

//----------------------------------------------------------------------------
// Name:	calculateResult
// Purpose:	Calculates and prints a result.
// Returns:	10 times the number of megabytes per second.
//----------------------------------------------------------------------------
static int Benchmark_calculateResult (Benchmark* self, uint64_t chunk_size, uint64_t total_loops, uint64_t diff)
{
        if (!self) {
                return 0;
	}
        verifyCorrectClassOrSubclass(self,Benchmark);

	if (!diff) {
		warning (__FUNCTION__, "Zero time difference... ignoring.");
		return 0;
	}

	long double result = (long double) chunk_size;
	result *= (long double) total_loops;
	result *= 1000000.;
	result /= 1048576.;
	result /= (long double) diff;

	if (!self->quietMode) {
		$(console, printf, "%.1Lf MB/s\n", result);
	}

	return (long) (10.0 * result);
}

//============================================================================

static long Benchmark_registerToRegisterTest (Benchmark *self)
{
        if (!self) {
                return 0;
	}
        verifyCorrectClassOrSubclass(self,Benchmark);

        time_t t0 = DateTime_getMicrosecondTime ();
	
#ifdef IS_64BIT
	$(console, printf, "64-bit main register to main register transfers: ");
#else
	$(console, printf, "32-bit main register to main register transfers: ");
#endif
	$(console, flush);

	for (int i=0; i < N_REG_TO_REG_LOOPS; i++) {
		RegisterToRegister (REGISTER_TRANSFERS_COUNT);
	}
	long diff = DateTime_getMicrosecondTime () - t0;

	long double d = N_REG_TO_REG_LOOPS;
	d *= REGISTER_TRANSFERS_COUNT;
	d *= N_REG_TO_REG_PER_LOOP;
	d /= diff;
	d *= 1000000; // usec->sec
	d /= 1000000000; // billions/sec
#ifdef IS_64BIT
	$(console, printf, "%.2Lf billion/second\n", d);
#else
	$(console, printf, "%.2Lf billion/second\n", d);
#endif
	$(console, flush);

	return 0;
}

static long Benchmark_stackRead (Benchmark *self)
{
        if (!self) {
                return 0;
	}
        verifyCorrectClassOrSubclass(self,Benchmark);

        time_t t0 = DateTime_getMicrosecondTime ();

	for (int i=0; i<STACK_OPERATION_LOOPS_OUTER; i++) {
		StackReader (STACK_OPERATION_LOOPS_INNER);
	}
	time_t diff = DateTime_getMicrosecondTime () - t0;
	if (diff > 0) {
		long double d = N_STACK_OPS_PER_LOOP;
		d *= STACK_OPERATION_LOOPS_OUTER;
		d *= STACK_OPERATION_LOOPS_INNER;
		d *= 1000000.; // usec->sec
		d /= diff;
		d /= 1000000000.; // billions/sec
#ifdef IS_64BIT
		$(console, printf, "64-bit stack-to-register transfers: %.2Lf billion/second\n", d);
#else
		$(console, printf, "32-bit stack-to-register transfers: %.2Lf billion/second\n", d);
#endif
	}
	$(console, flush);
	return 0;
}

static long Benchmark_stackWrite (Benchmark *self)
{
        if (!self) {
                return 0;
	}
        verifyCorrectClassOrSubclass(self,Benchmark);

        time_t t0 = DateTime_getMicrosecondTime ();

	for (int i=0; i<STACK_OPERATION_LOOPS_OUTER; i++) {
		StackWriter (STACK_OPERATION_LOOPS_INNER);
	}
	time_t diff = DateTime_getMicrosecondTime () - t0;
	if (diff > 0) {
		long double d = N_STACK_OPS_PER_LOOP;
		d *= STACK_OPERATION_LOOPS_OUTER;
		d *= STACK_OPERATION_LOOPS_INNER;
		d *= 1000000.; // usec->sec
		d /= diff;
		d /= 1000000000.; // billions/sec
#ifdef IS_64BIT
		$(console, printf, "64-bit register-to-stack transfers: %.2Lf billion/second\n", d);
#else
		$(console, printf, "32-bit register-to-stack transfers: %.2Lf billion/second\n", d);
#endif
	}
	$(console, flush);
	return 0;
}

static long Benchmark_incrementRegisters (Benchmark *self)
{
        if (!self) {
                return 0;
	}
        verifyCorrectClassOrSubclass(self,Benchmark);

        time_t t0 = DateTime_getMicrosecondTime ();

#ifdef IS_64BIT
	$(console, printf, "64-bit register increments: ");
#else
	$(console, printf, "32-bit register increments: ");
#endif
	$(console, flush);

	int i;
	for (i=0; i<N_INC_OUTER_LOOPS; i++) {
		IncrementRegisters (N_INC_INNER_LOOPS);
	}
	time_t diff = DateTime_getMicrosecondTime () - t0;
	if (diff > 0) {
		unsigned long tmp = N_INC_OUTER_LOOPS;
		tmp *= N_INC_INNER_LOOPS;
		tmp *= N_INC_PER_INNER;
		long double dt = diff;
		dt /= 1000000.;
		long double d = tmp;
		d /= dt;
		d /= 1000000000.; // billions/sec
#ifdef IS_64BIT
		$(console, printf, "%.2Lf billion/second\n", d);
#else
		$(console, printf, "%.2Lf billion/second\n", d);
#endif
	}
	$(console, flush);

	return 0;
}

static long Benchmark_incrementStack (Benchmark *self)
{
        if (!self) {
                return 0;
	}
        verifyCorrectClassOrSubclass(self,Benchmark);

        time_t t0 = DateTime_getMicrosecondTime ();
	
#ifdef IS_64BIT
	$(console, printf, "64-bit stack value increments: ");
#else
	$(console, printf, "32-bit stack value increments: ");
#endif
	$(console, flush);

	int i;
	for (i=0; i < N_INC_OUTER_LOOPS; i++) {
		IncrementStack (N_INC_INNER_LOOPS);
	}
	long diff = DateTime_getMicrosecondTime () - t0;
	
	if (diff > 0) {
		unsigned long tmp = N_INC_OUTER_LOOPS;
		tmp *= N_INC_INNER_LOOPS;
		tmp *= N_INC_PER_INNER;
		long double d = tmp;
		d *= 1000000; // usec->sec
		d /= diff;
		d /= 1000000000; // billions/sec
#ifdef IS_64BIT
		$(console, printf, "%.2Lf billion/second\n", d);
#else
		$(console, printf, "%.2Lf billion/second\n", d);
#endif
	}
	$(console, flush);
	
	return 0;
}

static long Benchmark_vectorToVectorTest128 (Benchmark *self)
{
        if (!self) {
                return 0;
	}
        verifyCorrectClassOrSubclass(self,Benchmark);

        time_t t0 = DateTime_getMicrosecondTime ();
	
	$(console, printf, "Vector register to vector register transfers (128-bit): ");
	$(console, flush);

	int i;
	for (i=0; i < N_VREG_TO_VREG_LOOPS; i++) {
		VectorToVector128 (VREGISTER_TRANSFERS_COUNT);
	}
	long diff = DateTime_getMicrosecondTime () - t0;
	
	long double d = N_VREG_TO_VREG_LOOPS;
	d *= VREGISTER_TRANSFERS_COUNT;
	d *= N_VREG_TO_VREG_PER_LOOP;
	d /= diff;
	d *= 1000000; // usec->sec
	d /= 1000000000; // billions/sec
	$(console, printf, "%.2Lf billion/second\n", d);
	$(console, flush);

	return 0;
}

static long Benchmark_vectorToVectorTest256 (Benchmark *self)
{
        if (!self) {
                return 0;
	}
        verifyCorrectClassOrSubclass(self,Benchmark);

        time_t t0 = DateTime_getMicrosecondTime ();
	
	$(console, printf, "Vector register to vector register transfers (256-bit): ");
	$(console, flush);

	int i;
	for (i=0; i < N_VREG_TO_VREG_LOOPS; i++) {
		VectorToVector256 (VREGISTER_TRANSFERS_COUNT);
	}
	long diff = DateTime_getMicrosecondTime () - t0;
	
	long double d = N_VREG_TO_VREG_LOOPS;
	d *= VREGISTER_TRANSFERS_COUNT;
	d *= N_VREG_TO_VREG_PER_LOOP;
	d /= diff;
	d *= 1000000; // usec->sec
	d /= 1000000000; // billions/sec
	$(console, printf, "%.2Lf billion/second\n", d);
	$(console, flush);

	return 0;
}

static long Benchmark_vectorToVectorTest512 (Benchmark *self)
{
        if (!self) {
                return 0;
	}
        verifyCorrectClassOrSubclass(self,Benchmark);

        time_t t0 = DateTime_getMicrosecondTime ();
	
	$(console, printf, "Vector register to vector register transfers (512-bit): ");
	$(console, flush);

	int i;
	for (i=0; i < N_VREG_TO_VREG_LOOPS; i++) {
		VectorToVector512 (VREGISTER_TRANSFERS_COUNT);
	}
	long diff = DateTime_getMicrosecondTime () - t0;
	
	long double d = N_VREG_TO_VREG_LOOPS;
	d *= VREGISTER_TRANSFERS_COUNT;
	d *= N_VREG_TO_VREG_PER_LOOP;
	d /= diff;
	d *= 1000000; // usec->sec
	d /= 1000000000; // billions/sec
	$(console, printf, "%.2Lf billion/second\n", d);
	$(console, flush);

	return 0;
}

static void Benchmark_freeDeferredChunks (Benchmark* self)
{
	if (!self) {
		return;
	}
	verifyCorrectClassOrSubclass(self,Benchmark);

	if (options.multithreaded) {
		return;
	}

	for (unsigned i=0; i < MAX_DEFERRED_FREE_CHUNKS; i++) {
		void *deferredChunk = self->deferredFreeChunks[i];
		if (deferredChunk) {
			free(deferredChunk);
			self->deferredFreeChunks[i] = NULL;
			self->deferredFreeChunkSizes[i] = 0;
		}
	}
}

static size_t totalFreeDeferred (Benchmark* self) 
{
	if (options.multithreaded) {
		return 0LU;
	}

	size_t totalMegabytesDeferred = 0;
	for (unsigned i=0; i < MAX_DEFERRED_FREE_CHUNKS; i++) {
		totalMegabytesDeferred += self->deferredFreeChunkSizes[i];
	}
	return totalMegabytesDeferred;
}

/* Defer freeing of a chunk, because premature freeing can create unrealistic results in Linux
 * in which queues writes never happen. In addition, we want to avoid inadvertent reuse
 * of memory chunks by the system because these can throw off results.
 */
static void Benchmark_deferFreeOfChunk (Benchmark* self, void* chunk, size_t incomingSize /*in bytes*/)
{
	if (!self || !chunk) {
		return;
	}
	verifyCorrectClassOrSubclass(self,Benchmark);

	// RULE: In multithreaded mode, because there's a real risk that numerous 
	// large memory chunks could lead to a malloc() failure, we should not 
	// defer freeing of chunks, at least for now.
	//
	if (options.multithreaded) {
		free(chunk);
		return;
	}

	size_t sizeInMegabytes = (incomingSize + (ONE_MEGABYTE-1)) / ONE_MEGABYTE;

	// Get the total RAM required by all chunks.
	size_t totalMegabytesDeferred = totalFreeDeferred(self);

	size_t freeRAMInMegabytes = OS_freeRAM(); // In megabytes

	Log_debug_printf (__FUNCTION__, "FreeRAM %lu DeferredFree %lu IncomingChunk %lu\n", freeRAMInMegabytes,totalMegabytesDeferred,sizeInMegabytes);

	size_t slot = self->deferredFreeChunkIndex;
	size_t nextSlot = (self->deferredFreeChunkIndex + 1) % MAX_DEFERRED_FREE_CHUNKS;
	self->deferredFreeChunkIndex = nextSlot;

	const size_t padding = sizeInMegabytes * 1.5;
	if (totalMegabytesDeferred + sizeInMegabytes + padding >= freeRAMInMegabytes) {
		// Free enough chunks to make space for the new chunk.
		
		size_t currentSlot = slot;
		do {
			void *deferredChunk = self->deferredFreeChunks[currentSlot];
			size_t chunkSize = self->deferredFreeChunkSizes[currentSlot];

			if (deferredChunk) {
				free(deferredChunk);
				self->deferredFreeChunks[currentSlot] = NULL;
				self->deferredFreeChunkSizes[currentSlot] = 0;
				totalMegabytesDeferred -= chunkSize;
				freeRAMInMegabytes += chunkSize;
				if (freeRAMInMegabytes > (sizeInMegabytes+padding)) {
					break;
				}
			}

			currentSlot = (currentSlot+1) % MAX_DEFERRED_FREE_CHUNKS;
		}
		while (currentSlot != slot);
	}

	if (self->deferredFreeChunks[slot]) {
		free(self->deferredFreeChunks[slot]);
		self->deferredFreeChunkSizes[slot] = 0;
	}

	self->deferredFreeChunks[slot] = chunk;
	self->deferredFreeChunkSizes[slot] = sizeInMegabytes;
}

static MemoryCorrectnessTestResult Benchmark_memoryStuckBitTest (Benchmark* self, size_t chunk_size)
{
	if (!self || chunk_size < sizeof(StuckBitStorageType)) {
		return false;
	}
	verifyCorrectClassOrSubclass(self,Benchmark);

	volatile StuckBitStorageType *chunk = malloc(chunk_size);
	if (!chunk) {
		Log_perror(__FUNCTION__, "malloc");
		return MemoryCorrectnessTestResultMallocFailed;
	}

	size_t count = chunk_size / sizeof(StuckBitStorageType);
	for (size_t i = 0; i < count; i += 2) {
		chunk[i] = 0x55555555;
		if (chunk[i] != 0x55555555) {
			return MemoryCorrectnessTestResultFailure;
		}
		chunk[i+1] = 0xAAAAAAAA;
		if (chunk[i+1] != 0xAAAAAAAA) {
			return MemoryCorrectnessTestResultFailure;
		}
		chunk[i] = 0x55555555;
		if (chunk[i] != 0x55555555) {
			return MemoryCorrectnessTestResultFailure;
		}
		chunk[i+1] = 0xAAAAAAAA;
		if (chunk[i+1] != 0xAAAAAAAA) {
			return MemoryCorrectnessTestResultFailure;
		}
	}

	for (size_t i = 0; i < count; i++) {
		chunk[i] = 0;
	}

	count = chunk_size / sizeof(StuckBitStorageType);
	StuckBitStorageType bit = 1;
	for (size_t i = 0; i < count; i++) {
		chunk[i] = bit;
		if (chunk[i] != bit) {
			return MemoryCorrectnessTestResultFailure;
		}
		StuckBitStorageType inverse = bit ^ 0xffffffff;
		chunk[i] = inverse;
		if (chunk[i] != inverse) {
			return MemoryCorrectnessTestResultFailure;
		}
		bit <<= 1;
		if (!bit) {
			bit = 1;
		}
	}

	free ((void*) chunk);
	return MemoryCorrectnessTestResultSuccess;
}

static MemoryCorrectnessTestResult Benchmark_memoryCorrectnessCheck (Benchmark* self, size_t chunk_size)
{
	if (!self || chunk_size < sizeof(RandomizedCorrectnessStorageType)) {
		return MemoryCorrectnessTestResultBadParameter;
	}
	verifyCorrectClassOrSubclass(self,Benchmark);

	RandomizedCorrectnessStorageType *chunk = malloc(chunk_size);
	if (!chunk) {
		Log_perror(__FUNCTION__, "malloc");
		return MemoryCorrectnessTestResultMallocFailed;
	}

	srand (54321);
	size_t count = chunk_size / sizeof(RandomizedCorrectnessStorageType);
	for (size_t i = 0; i < count; i++) {
		RandomizedCorrectnessStorageType value = (RandomizedCorrectnessStorageType) rand();
		chunk[i] = value;
	}

	// Pause.
	usleep(10000);
	srand (54321);
	for (size_t i = 0; i < count; i++) {
		RandomizedCorrectnessStorageType value = (RandomizedCorrectnessStorageType) rand();
		if (chunk[i] != value) {
			free (chunk);
			return MemoryCorrectnessTestResultFailure;
		}
	}
	
	free (chunk);
	return MemoryCorrectnessTestResultSuccess;
}

// Override in subclass.
static MemoryCorrectnessTestResult Benchmark_memoryRowHammerTest (Benchmark* self, unsigned long bufSize, unsigned long nCycles)
{
	return MemoryCorrectnessTestResultUnsupported;
}

static long Benchmark_vector128ToRegister8 (Benchmark *self)
{
#if defined(__x86_64__) || defined(__i386__)
	if (isMemberOfClass(self, BenchmarkX86)) {
		BenchmarkX86 *benchmarks = (BenchmarkX86*) self;
		if (!benchmarks->use_sse4) {
			return 0;
		}
	}
#endif

	$(console, printf, "Vector 8-bit datum to main register transfers: ");
	$(console, flush);

	long long total_count = 0;
	unsigned long diff = 0;
	unsigned long t0 = DateTime_getMicrosecondTime ();
	
	while (diff < self->usec_per_test)
	{
		Vector128ToRegister8 (VECTOR_TO_REGISTER_TRANSFERS_COUNT);
		total_count += VECTOR_TO_REGISTER_TRANSFERS_COUNT;

		diff = DateTime_getMicrosecondTime () - t0;
	}

	long double d = total_count;
	d *= N_VECTOR_INSERTS_EXTRACTS_PER_LOOP;
	d /= diff;
	d *= 1000000; // usec->sec
	d /= 1000000000; // billions/sec
	$(console, printf, "%.2Lf billion/second\n", d);
	$(console, flush);

	return 0;
}

static long Benchmark_vector128ToRegister16 (Benchmark *self)
{
	$(console, printf, "Vector 16-bit datum to main register transfers: ");
	$(console, flush);

	long long total_count = 0;
	unsigned long diff = 0;
	unsigned long t0 = DateTime_getMicrosecondTime ();
	
	while (diff < self->usec_per_test)
	{
		Vector128ToRegister16 (VECTOR_TO_REGISTER_TRANSFERS_COUNT);
		total_count += VECTOR_TO_REGISTER_TRANSFERS_COUNT;

		diff = DateTime_getMicrosecondTime () - t0;
	}

	long double d = total_count;
	d *= N_VECTOR_INSERTS_EXTRACTS_PER_LOOP;
	d /= diff;
	d *= 1000000; // usec->sec
	d /= 1000000000; // billions/sec
	$(console, printf, "%.2Lf billion/second\n", d);
	$(console, flush);
	return 0;
}

static long Benchmark_vector128ToRegister32 (Benchmark *self)
{
#if defined(__x86_64__) || defined(__i386__)
	if (isMemberOfClass(self, BenchmarkX86)) {
		BenchmarkX86 *benchmarks = (BenchmarkX86*) self;
		if (!benchmarks->use_sse4) {
			return 0;
		}
	}
#endif

	$(console, printf, "Vector 32-bit datum to main register transfers: ");
	$(console, flush);

	long long total_count = 0;
	unsigned long diff = 0;
	unsigned long t0 = DateTime_getMicrosecondTime ();
	
	while (diff < self->usec_per_test)
	{
		Vector128ToRegister32 (VECTOR_TO_REGISTER_TRANSFERS_COUNT);
		total_count += VECTOR_TO_REGISTER_TRANSFERS_COUNT;

		diff = DateTime_getMicrosecondTime () - t0;
	}

	long double d = total_count;
	d *= N_VECTOR_INSERTS_EXTRACTS_PER_LOOP;
	d /= diff;
	d *= 1000000; // usec->sec
	d /= 1000000000; // billions/sec
	$(console, printf, "%.2Lf billion/second\n", d);
	$(console, flush);

	return 0;
}

static long Benchmark_vector128ToRegister64 (Benchmark *self)
{
#if defined(__x86_64__) || defined(__i386__)
	if (isMemberOfClass(self, BenchmarkX86)) {
		BenchmarkX86 *benchmarks = (BenchmarkX86*) self;
		if (!benchmarks->use_sse4) {
			return 0;
		}
	}
#endif

	$(console, printf, "Vector 64-bit datum to main register transfers: ");
	$(console, flush);

	long long total_count = 0;
	unsigned long diff = 0;
	unsigned long t0 = DateTime_getMicrosecondTime ();
	
	while (diff < self->usec_per_test)
	{
		Vector128ToRegister64 (VECTOR_TO_REGISTER_TRANSFERS_COUNT);
		total_count += VECTOR_TO_REGISTER_TRANSFERS_COUNT;

		diff = DateTime_getMicrosecondTime () - t0;
	}

	long double d = total_count;
	d *= N_VECTOR_INSERTS_EXTRACTS_PER_LOOP;
	d /= diff;
	d *= 1000000; // usec->sec
	d /= 1000000000; // billions/sec
	$(console, printf, "%.2Lf billion/second\n", d);
	$(console, flush);

	return 0;
}

static long Benchmark_register8ToVector128 (Benchmark *self)
{
#if defined(__x86_64__) || defined(__i386__)
	if (isMemberOfClass(self, BenchmarkX86)) {
		BenchmarkX86 *benchmarks = (BenchmarkX86*) self;
		if (!benchmarks->use_sse4) {
			return 0;
		}
	}
#endif

	$(console, printf, "Main register 8-bit datum to vector register transfers: ");
	$(console, flush);

	long long total_count = 0;
	unsigned long diff = 0;
	unsigned long t0 = DateTime_getMicrosecondTime ();
	
	while (diff < self->usec_per_test)
	{
		Register8ToVector128 (VREGISTER_TRANSFERS_COUNT);
		total_count += VREGISTER_TRANSFERS_COUNT;

		diff = DateTime_getMicrosecondTime () - t0;
	}

	long double d = total_count;
	d *= N_VECTOR_INSERTS_EXTRACTS_PER_LOOP;
	d /= diff;
	d *= 1000000; // usec->sec
	d /= 1000000000; // billions/sec
	$(console, printf, "%.2Lf billion/second\n", d);
	$(console, flush);

	return 0;
}

static long Benchmark_register16ToVector128 (Benchmark *self)
{
	$(console, printf, "Main register 16-bit datum to vector register transfers: ");
	$(console, flush);

	long long total_count = 0;
	unsigned long diff = 0;
	unsigned long t0 = DateTime_getMicrosecondTime ();
	
	while (diff < self->usec_per_test)
	{
		Register16ToVector128 (VREGISTER_TRANSFERS_COUNT);
		total_count += VREGISTER_TRANSFERS_COUNT;

		diff = DateTime_getMicrosecondTime () - t0;
	}

	long double d = total_count;
	d *= N_VECTOR_INSERTS_EXTRACTS_PER_LOOP;
	d /= diff;
	d *= 1000000; // usec->sec
	d /= 1000000000; // billions/sec
	$(console, printf, "%.2Lf billion/second\n", d);
	$(console, flush);
	return 0;
}

static long Benchmark_register32ToVector128 (Benchmark *self)
{
#if defined(__x86_64__) || defined(__i386__)
	if (isMemberOfClass(self, BenchmarkX86)) {
		BenchmarkX86 *benchmarks = (BenchmarkX86*) self;
		if (!benchmarks->use_sse4) {
			return 0;
		}
	}
#endif

	$(console, printf, "Main register 32-bit datum to vector register transfers: ");
	$(console, flush);

	long long total_count = 0;
	unsigned long diff = 0;
	unsigned long t0 = DateTime_getMicrosecondTime ();
	
	while (diff < self->usec_per_test)
	{
		Register32ToVector128 (VREGISTER_TRANSFERS_COUNT);
		total_count += VREGISTER_TRANSFERS_COUNT;

		diff = DateTime_getMicrosecondTime () - t0;
	}

	long double d = total_count;
	d *= N_VECTOR_INSERTS_EXTRACTS_PER_LOOP;
	d /= diff;
	d *= 1000000; // usec->sec
	d /= 1000000000; // billions/sec
	$(console, printf, "%.2Lf billion/second\n", d);
	$(console, flush);
	return 0;
}

static long Benchmark_register64ToVector128 (Benchmark *self)
{
#if defined(__x86_64__) || defined(__i386__)
	if (isMemberOfClass(self, BenchmarkX86)) {
		BenchmarkX86 *benchmarks = (BenchmarkX86*) self;
		if (!benchmarks->use_sse4) {
			return 0;
		}
	}
#endif

	$(console, printf, "Main register 64-bit datum to vector register transfers: ");
	$(console, flush);

	long long total_count = 0;
	unsigned long diff = 0;
	unsigned long t0 = DateTime_getMicrosecondTime ();
	
	while (diff < self->usec_per_test)
	{
		Register64ToVector128 (VREGISTER_TRANSFERS_COUNT);
		total_count += VREGISTER_TRANSFERS_COUNT;

		diff = DateTime_getMicrosecondTime () - t0;
	}

	long double d = total_count;
	d *= N_VECTOR_INSERTS_EXTRACTS_PER_LOOP;
	d /= diff;
	d *= 1000000; // usec->sec
	d /= 1000000000; // billions/sec
	$(console, printf, "%.2Lf billion/second\n", d);
	$(console, flush);

	return 0;
}

BenchmarkClass* BenchmarkClass_init (BenchmarkClass* class)
{
	SET_SUPERCLASS(Object);

	SET_OVERRIDDEN_METHOD_POINTER(Benchmark,describe);
	SET_OVERRIDDEN_METHOD_POINTER(Benchmark,destroy);

	SET_METHOD_POINTER(Benchmark,calculateResult);
	SET_METHOD_POINTER(Benchmark,printSize);
	SET_METHOD_POINTER(Benchmark,memoryCorrectnessCheck);
	SET_METHOD_POINTER(Benchmark,memoryStuckBitTest);
	SET_METHOD_POINTER(Benchmark,memoryRowHammerTest);
	SET_METHOD_POINTER(Benchmark,deferFreeOfChunk);
	SET_METHOD_POINTER(Benchmark,freeDeferredChunks);
	SET_METHOD_POINTER(Benchmark,stackRead);
	SET_METHOD_POINTER(Benchmark,stackWrite);
	SET_METHOD_POINTER(Benchmark,incrementRegisters);
	SET_METHOD_POINTER(Benchmark,incrementStack);
	SET_METHOD_POINTER(Benchmark,registerToRegisterTest);
	SET_METHOD_POINTER(Benchmark,vectorToVectorTest128);
	SET_METHOD_POINTER(Benchmark,vectorToVectorTest256);
	SET_METHOD_POINTER(Benchmark,vectorToVectorTest512);
	SET_METHOD_POINTER(Benchmark,vector128ToRegister8);
	SET_METHOD_POINTER(Benchmark,vector128ToRegister16);
	SET_METHOD_POINTER(Benchmark,vector128ToRegister32);
	SET_METHOD_POINTER(Benchmark,vector128ToRegister64);
	SET_METHOD_POINTER(Benchmark,register8ToVector128);
	SET_METHOD_POINTER(Benchmark,register16ToVector128);
	SET_METHOD_POINTER(Benchmark,register32ToVector128);
	SET_METHOD_POINTER(Benchmark,register64ToVector128);

	SET_ABSTRACT_METHOD_POINTER(read);
	SET_ABSTRACT_METHOD_POINTER(write);
	SET_ABSTRACT_METHOD_POINTER(copy);
	SET_ABSTRACT_METHOD_POINTER(registerToVectorMove);
	SET_ABSTRACT_METHOD_POINTER(vectorToRegisterMove);

	VALIDATE_CLASS_STRUCT(_BenchmarkClass);

	return _BenchmarkClass;
}

Benchmark *Benchmark_init (Benchmark *self)
{
	ENSURE_CLASS_READY(Benchmark);

        Object_init ((Object*) self);

        self->is_a = _BenchmarkClass;

	self->deferredFreeChunkIndex = 0;
	self->vectorToFromRegisterRoutinesAvailable = false;
	self->quietMode = false;
	ooc_bzero (self->deferredFreeChunks, sizeof(self->deferredFreeChunks));
	ooc_bzero (self->deferredFreeChunkSizes, sizeof(self->deferredFreeChunkSizes));

        return self;
}

