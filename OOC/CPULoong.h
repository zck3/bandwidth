/*============================================================================
  CPULoong, an object-oriented C LoongArch64 instruction architecture.
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

#ifndef _OOC_CPULoong_H
#define _OOC_CPULoong_H

#include "CPU.h"

#define DECLARE_CPULoong_INSTANCE_VARS(FOO) \
	;

#define DECLARE_CPULoong_METHODS(TYPE_POINTER) \
	;

struct cpu_loongarch64;

typedef struct cpu_loongarch64class {
	DECLARE_OBJECT_CLASS_VARS
        DECLARE_OBJECT_METHODS(struct cpu_loongarch64*)
        DECLARE_CPU_METHODS(struct cpu_loongarch64*)
        DECLARE_CPULoong_METHODS(struct cpu_loongarch64*)
} CPULoongClass;

extern CPULoongClass *_CPULoongClass;
extern CPULoongClass* CPULoongClass_init (CPULoongClass*);

typedef struct cpu_loongarch64 {
        CPULoongClass *is_a;
	DECLARE_OBJECT_INSTANCE_VARS(struct cpu_loongarch64*)
	DECLARE_CPU_INSTANCE_VARS(struct cpu_loongarch64*)
	DECLARE_CPULoong_INSTANCE_VARS(struct cpu_loongarch64*)
} CPULoong;

extern void CPULoong_destroy (Any *);
extern CPULoong *CPULoong_init (CPULoong *self);

#endif
