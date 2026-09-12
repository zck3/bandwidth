/*============================================================================
  CPUPowerPC, an object-oriented C PowerPC 64-bit CPU class.
  Copyright (C) 2026 by Zack T Smith.

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

#ifndef _OOC_CPUPowerPC_H
#define _OOC_CPUPowerPC_H

#include "CPU.h"

#define DECLARE_CPUPowerPC_INSTANCE_VARS(FOO) \
	;

#define DECLARE_CPUPowerPC_METHODS(TYPE_POCPUPowerPCER) \
	;

struct cpu_powerpc64;

typedef struct cpu_powerpc64class {
	DECLARE_OBJECT_CLASS_VARS
        DECLARE_OBJECT_METHODS(struct cpu_powerpc64*)
        DECLARE_CPU_METHODS(struct cpu_powerpc64*)
        DECLARE_CPUPowerPC_METHODS(struct cpu_powerpc64*)
} CPUPowerPCClass;

extern CPUPowerPCClass *_CPUPowerPCClass;
extern CPUPowerPCClass* CPUPowerPCClass_init (CPUPowerPCClass*);

typedef struct cpu_powerpc64 {
        CPUPowerPCClass *is_a;
	DECLARE_OBJECT_INSTANCE_VARS(struct cpu_powerpc64*)
	DECLARE_CPU_INSTANCE_VARS(struct cpu_powerpc64*)
	DECLARE_CPUPowerPC_INSTANCE_VARS(struct cpu_powerpc64*)
} CPUPowerPC;

extern void CPUPowerPC_destroy (Any *);
extern CPUPowerPC *CPUPowerPC_init (CPUPowerPC *self);

#endif
