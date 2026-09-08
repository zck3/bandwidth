/*============================================================================
  CPURISCV, an object-oriented C riscv64 CPU class.
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

#ifndef _OOC_CPURISCV_H
#define _OOC_CPURISCV_H

#include "CPU.h"

#define DECLARE_CPURISCV_INSTANCE_VARS(FOO) \
	;

#define DECLARE_CPURISCV_METHODS(TYPE_POCPURISCVER) \
	;

struct cpu_riscv64;

typedef struct cpu_riscv64class {
	DECLARE_OBJECT_CLASS_VARS
        DECLARE_OBJECT_METHODS(struct cpu_riscv64*)
        DECLARE_CPU_METHODS(struct cpu_riscv64*)
        DECLARE_CPURISCV_METHODS(struct cpu_riscv64*)
} CPURISCVClass;

extern CPURISCVClass *_CPURISCVClass;
extern CPURISCVClass* CPURISCVClass_init (CPURISCVClass*);

typedef struct cpu_riscv64 {
        CPURISCVClass *is_a;
	DECLARE_OBJECT_INSTANCE_VARS(struct cpu_riscv64*)
	DECLARE_CPU_INSTANCE_VARS(struct cpu_riscv64*)
	DECLARE_CPURISCV_INSTANCE_VARS(struct cpu_riscv64*)
} CPURISCV;

extern void CPURISCV_destroy (Any *);
extern CPURISCV *CPURISCV_init (CPURISCV *self);

#endif
