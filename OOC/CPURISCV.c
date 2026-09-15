/*============================================================================
  CPURISCV, an object-oriented C RISC-V CPU class.
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

#if defined(__riscv) || defined(__riscv__)

#include "CPURISCV.h"
#include "Utility.h"

#include <stdlib.h>
#include <sys/auxv.h>
#include <asm/hwcap.h>

CPURISCVClass *_CPURISCVClass = NULL;

void CPURISCV_destroy (Any *self)
{
	DEBUG_DESTROY;
	if (!self) {
		return;
	}
	verifyCorrectClassOrSubclass(self,CPURISCV);

	CPU_destroy((CPU*)self);
}

static void CPURISCV_print (CPURISCV* restrict self, FILE *outputFile) 
{ 
	if (!self) {
		return;
	}
	verifyCorrectClass(self,CPURISCV);

	if (!outputFile) {
		outputFile = stdout;
	}
}

static void CPURISCV_describe (CPURISCV* restrict self, FILE *outputFile) 
{ 
	if (!self) {
		return;
	}
	verifyCorrectClass(self,CPURISCV);

	if (!outputFile) {
		outputFile = stdout;
	}

	fprintf (outputFile, "%s", $(self, className));
}

static String* CPURISCV_model (CPURISCV* restrict self) 
{
	if (!self) {
		return NULL;
	}
	verifyCorrectClassOrSubclass(self,CPU);

#if defined(__linux__) 
	char *result = execute_and_return_first_line ("grep uarch /proc/cpuinfo | sed 's/^.*://' | sed 's/^[ ]*//' ");
	if (result) {
		return String_withCString (result);
	}
#endif

	return NULL;
}

static MutableSet* CPURISCV_features (CPURISCV* restrict self)
{
	if (!self) {
		return NULL;
	}
	verifyCorrectClass(self,CPURISCV);

	MutableSet *mut = self->features;
	if (!$(mut, isEmpty)) {
		return mut;
	}

#if defined(__linux__)
	unsigned long hwcap = getauxval(AT_HWCAP);

    #ifdef COMPAT_HWCAP_ISA_I
	if (hwcap & COMPAT_HWCAP_ISA_I) {
		$(mut, add, _String("isa_i"));
	}
    #endif
    #ifdef COMPAT_HWCAP_ISA_M
	if (hwcap & COMPAT_HWCAP_ISA_M) {
		$(mut, add, _String("isa_m"));
	}
    #endif
    #ifdef COMPAT_HWCAP_ISA_A
	if (hwcap & COMPAT_HWCAP_ISA_A) {
		$(mut, add, _String("isa_a"));
	}
    #endif
    #ifdef COMPAT_HWCAP_ISA_F
	if (hwcap & COMPAT_HWCAP_ISA_F) {
		$(mut, add, _String("isa_f"));
	}
    #endif
    #ifdef COMPAT_HWCAP_ISA_D
	if (hwcap & COMPAT_HWCAP_ISA_D) {
		$(mut, add, _String("isa_d"));
	}
    #endif
    #ifdef COMPAT_HWCAP_ISA_C
	if (hwcap & COMPAT_HWCAP_ISA_C) {
		$(mut, add, _String("isa_c"));
	}
    #endif
    #ifdef COMPAT_HWCAP_ISA_V
	if (hwcap & COMPAT_HWCAP_ISA_V) {
		$(mut, add, _String("isa_v"));
		// There are vector registers, but on RISC-V the size can vary
		// by CPU implementation, so we have to try to read the size.
		//
		unsigned long value = 0;
		asm volatile ("csrr %0, vlenb" : "=r"(value));
		unsigned long vector_length = value * 8;
		switch (vector_length) {
		case 128: self->has128bitVectors = true; break;
		case 256: self->has256bitVectors = true; break;
		case 512: self->has512bitVectors = true; break;
		default:
			break;
		}
	}
    #endif
#endif

	return mut;
}

		
static void CPURISCV_memoryFence (CPURISCV* restrict self)
{
	if (!self) {
		return;
	}
	verifyCorrectClass(self,CPURISCV);

	asm volatile("fence w,r" ::: "memory");
}

static void CPURISCV_flushDataCacheAt (CPURISCV* restrict self, void* address, size_t size)
{
	// TODO: No RISC-V instruction for this?
}

static String* CPURISCV_instructionSet (CPURISCV* restrict self) 
{
	if (!self) {
		return NULL;
	}
	verifyCorrectClass(self,CPURISCV);

	// NOTE: No support for riscv32 at this time.

	return _String("riscv64");
}

static String* CPURISCV_family (CPURISCV* restrict self) 
{
	if (!self) {
		return NULL;
	}
	verifyCorrectClass(self,CPURISCV);

	return _String("RISC-V");
}

CPURISCV* CPURISCV_init (CPURISCV* restrict self)
{
	ENSURE_CLASS_READY(CPURISCV);

	if (self) {
		CPU_init ((CPU*) self);
		self->is_a = _CPURISCVClass;

		(void) $(self, nCores);
		(void) $(self, features);
	}

	return self;
}

CPURISCVClass* CPURISCVClass_init (CPURISCVClass* restrict class)
{
	SET_SUPERCLASS(CPU);

	SET_OVERRIDDEN_METHOD_POINTER(CPURISCV,describe);
	SET_OVERRIDDEN_METHOD_POINTER(CPURISCV,print);
	SET_OVERRIDDEN_METHOD_POINTER(CPURISCV,model);
	SET_OVERRIDDEN_METHOD_POINTER(CPURISCV,features);
	SET_OVERRIDDEN_METHOD_POINTER(CPURISCV,memoryFence);
	SET_OVERRIDDEN_METHOD_POINTER(CPURISCV,flushDataCacheAt);
	SET_OVERRIDDEN_METHOD_POINTER(CPURISCV,family);
	SET_OVERRIDDEN_METHOD_POINTER(CPURISCV,instructionSet);

        VALIDATE_CLASS_STRUCT(class);
	return class;
}

#endif // RISC-V
