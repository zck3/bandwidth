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

#ifndef _MULTITHREADED_TESTRUNNER_H
#define _MULTITHREADED_TESTRUNNER_H

#include "OOC/CPU.h"
#include "Benchmark.h"

extern void runMultithreadedTests (Benchmark *benchmarks, CPU *cpu);

#endif
