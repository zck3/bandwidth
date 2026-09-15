/*============================================================================
  BenchmarkLoong, an Object-Oriented C class for benchmarking.
  Copyright (C) 2026 by Zack T Smith.

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

#ifndef _OOC_BENCHMARK_LOONG_H
#define _OOC_BENCHMARK_LOONG_H

#include <stdint.h>
#include <stdbool.h>

#include "OOC/Object.h"
#include "Benchmark.h"

#define DECLARE_BENCHMARK_LOONG_INSTANCE_VARS(TYPE_POINTER) 

#define DECLARE_BENCHMARK_LOONG_METHODS(TYPE_POINTER) 

struct testing_loong;

typedef struct testing_loong_class {
	DECLARE_OBJECT_CLASS_VARS
        DECLARE_OBJECT_METHODS(struct testing_loong*)
        DECLARE_BENCHMARK_METHODS(struct testing_loong*)
        DECLARE_BENCHMARK_LOONG_METHODS(struct testing_loong*)
} BenchmarkLoongClass;

extern BenchmarkLoongClass *_BenchmarkLoongClass;
extern BenchmarkLoongClass *BenchmarkLoongClass_init (BenchmarkLoongClass*);

typedef struct testing_loong {
        BenchmarkLoongClass *is_a;
	DECLARE_OBJECT_INSTANCE_VARS(struct testing_loong*)
	DECLARE_BENCHMARK_INSTANCE_VARS(struct testing_loong*)
	DECLARE_BENCHMARK_LOONG_INSTANCE_VARS(struct testing_loong*)
} BenchmarkLoong;

extern BenchmarkLoong *BenchmarkLoong_new ();
extern BenchmarkLoong *BenchmarkLoong_init (BenchmarkLoong *self);

#endif
