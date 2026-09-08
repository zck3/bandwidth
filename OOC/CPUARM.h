/*============================================================================
  CPUARM, an object-oriented C ARM (aarch64 and aarch32) CPU class.
  Copyright (C) 2019, 2023, 2026 by Zack T Smith.

  Object-Oriented C is free software: you can redistribute it and/or modify
  it under the terms of the GNU Lesser General Public License as published
  by the Free Software Foundation, either version 3 of the License, or
  (at your option) any later version.
 
  Object-Oriented C is distributed in the hope that it will be useful,
  but WITHOUT ANY WARRANTY; without even the implied warranty of
  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
  GNU Lesser General Public License for more details.
 
  You should have received a copy of the GNU Lesser General Public License
  along with this software.  If not, see <http://www.gnu.org/licenses/>.

  The author may be reached at 3 at zs3 dot me.
 *===========================================================================*/

// This file is part of a refactoring to break down CPU.c into 
// subclasses for each CPU type.

#ifndef _OOC_CPUARM_H
#define _OOC_CPUARM_H

#include "CPU.h"

#define DECLARE_CPUARM_INSTANCE_VARS(FOO) \
	;

#define DECLARE_CPUARM_METHODS(TYPE_POCPUARMER) \
	;

struct cpu_arm;

typedef struct cpu_armclass {
	DECLARE_OBJECT_CLASS_VARS
        DECLARE_OBJECT_METHODS(struct cpu_arm*)
        DECLARE_CPU_METHODS(struct cpu_arm*)
        DECLARE_CPUARM_METHODS(struct cpu_arm*)
} CPUARMClass;

extern CPUARMClass *_CPUARMClass;
extern CPUARMClass* CPUARMClass_init (CPUARMClass*);

typedef struct cpu_arm {
        CPUARMClass *is_a;
	DECLARE_OBJECT_INSTANCE_VARS(struct cpu_arm*)
	DECLARE_CPU_INSTANCE_VARS(struct cpu_arm*)
	DECLARE_CPUARM_INSTANCE_VARS(struct cpu_arm*)
} CPUARM;

extern void CPUARM_destroy (Any *);
extern CPUARM *CPUARM_init (CPUARM *self);

#endif
