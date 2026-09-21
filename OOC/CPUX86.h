/*============================================================================
  CPUX86, an object-oriented C x86 (x64_64 and i386) CPU class.
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

#ifndef _OOC_CPUX86_H
#define _OOC_CPUX86_H

#include "CPU.h"

#define DECLARE_CPUX86_INSTANCE_VARS(FOO) \
	bool isIntel; \
	bool hasSSE; \
	bool hasSSE2; \
	bool hasSSE4; \
	bool hasAVX; \
	bool hasAVX512; 

#define DECLARE_CPUX86_METHODS(TYPE_POINTER) 

struct cpu_x86;

typedef struct cpu_x86class {
	DECLARE_OBJECT_CLASS_VARS
        DECLARE_OBJECT_METHODS(struct cpu_x86*)
        DECLARE_CPU_METHODS(struct cpu_x86*)
        DECLARE_CPUX86_METHODS(struct cpu_x86*)
} CPUX86Class;

extern CPUX86Class *_CPUX86Class;
extern CPUX86Class* CPUX86Class_init (CPUX86Class*);

typedef struct cpu_x86 {
        CPUX86Class *is_a;
	DECLARE_OBJECT_INSTANCE_VARS(struct cpu_x86*)
	DECLARE_CPU_INSTANCE_VARS(struct cpu_x86*)
	DECLARE_CPUX86_INSTANCE_VARS(struct cpu_x86*)
} CPUX86;

extern void CPUX86_destroy (Any *);
extern CPUX86 *CPUX86_init (CPUX86 *self);

#endif
