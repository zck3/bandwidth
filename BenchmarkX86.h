/*============================================================================
  BenchmarkX86, an Object-Oriented C class for benchmarking.
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

#ifndef _OOC_BENCHMARK_X86_H
#define _OOC_BENCHMARK_X86_H

#include <stdint.h>
#include <stdbool.h>
#include <math.h>

#include "OOC/Object.h"
#include "Benchmark.h"

#define DECLARE_BENCHMARK_X86_INSTANCE_VARS(TYPE_POINTER) \
	bool use_sse2; \
	bool use_sse4; \
	bool use_avx; \
	bool use_avx512; \
	bool use_direct_transfers; 

#define DECLARE_BENCHMARK_X86_METHODS(TYPE_POINTER) 

struct testing_x86;

typedef struct testing_x86_class {
	DECLARE_OBJECT_CLASS_VARS
        DECLARE_OBJECT_METHODS(struct testing_x86*)
        DECLARE_BENCHMARK_METHODS(struct testing_x86*)
        DECLARE_BENCHMARK_X86_METHODS(struct testing_x86*)
} BenchmarkX86Class;

extern BenchmarkX86Class *_BenchmarkX86Class;
extern BenchmarkX86Class *BenchmarkX86Class_init (BenchmarkX86Class*);

typedef struct testing_x86 {
        BenchmarkX86Class *is_a;
	DECLARE_OBJECT_INSTANCE_VARS(struct testing_x86*)
	DECLARE_BENCHMARK_INSTANCE_VARS(struct testing_x86*)
	DECLARE_BENCHMARK_X86_INSTANCE_VARS(struct testing_x86*)
} BenchmarkX86;

extern BenchmarkX86 *BenchmarkX86_new ();
extern BenchmarkX86 *BenchmarkX86_init (BenchmarkX86 *self);

#endif
