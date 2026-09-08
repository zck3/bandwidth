/*============================================================================
  BenchmarkRISCV, an Object-Oriented C class for benchmarking.
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

#ifndef _OOC_BENCHMARK_RISCV_H
#define _OOC_BENCHMARK_RISCV_H

#include <stdint.h>
#include <stdbool.h>
#include <math.h>

#include "OOC/Object.h"
#include "Benchmark.h"

#define DECLARE_BENCHMARK_RISCV_INSTANCE_VARS(TYPE_POINTER) 

#define DECLARE_BENCHMARK_RISCV_METHODS(TYPE_POINTER) 

struct testing_riscv;

typedef struct testing_riscv_class {
	DECLARE_OBJECT_CLASS_VARS
        DECLARE_OBJECT_METHODS(struct testing_riscv*)
        DECLARE_BENCHMARK_METHODS(struct testing_riscv*)
        DECLARE_BENCHMARK_RISCV_METHODS(struct testing_riscv*)
} BenchmarkRISCVClass;

extern BenchmarkRISCVClass *_BenchmarkRISCVClass;
extern BenchmarkRISCVClass *BenchmarkRISCVClass_init (BenchmarkRISCVClass*);

typedef struct testing_riscv {
        BenchmarkRISCVClass *is_a;
	DECLARE_OBJECT_INSTANCE_VARS(struct testing_riscv*)
	DECLARE_BENCHMARK_INSTANCE_VARS(struct testing_riscv*)
	DECLARE_BENCHMARK_RISCV_INSTANCE_VARS(struct testing_riscv*)
} BenchmarkRISCV;

extern BenchmarkRISCV *BenchmarkRISCV_new ();
extern BenchmarkRISCV *BenchmarkRISCV_init (BenchmarkRISCV *self);

#endif
