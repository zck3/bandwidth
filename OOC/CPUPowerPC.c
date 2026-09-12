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

#if defined(__powerpc) || defined(__powerpc__)

#include "CPUPowerPC.h"
#include "Utility.h"

#include <stdlib.h>
#include <sys/auxv.h>
#include <bits/hwcap.h> // <- It's not <asm/hwcap.h> on PowerPC.
#include <endian.h>

CPUPowerPCClass *_CPUPowerPCClass = NULL;

void CPUPowerPC_destroy (Any *self)
{
	DEBUG_DESTROY;
	if (!self) {
		return;
	}
	verifyCorrectClassOrSubclass(self,CPUPowerPC);

	CPU_destroy((CPU*)self);
}

static void CPUPowerPC_print (CPUPowerPC* restrict self, FILE *outputFile) 
{ 
	if (!self) {
		return;
	}
	verifyCorrectClass(self,CPUPowerPC);

	if (!outputFile) {
		outputFile = stdout;
	}
}

static void CPUPowerPC_describe (CPUPowerPC* restrict self, FILE *outputFile) 
{ 
	if (!self) {
		return;
	}
	verifyCorrectClass(self,CPUPowerPC);

	if (!outputFile) {
		outputFile = stdout;
	}

	fprintf (outputFile, "%s", $(self, className));
}

static String* CPUPowerPC_model (CPUPowerPC* restrict self) 
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

static MutableSet* CPUPowerPC_features (CPUPowerPC* restrict self)
{
	if (!self) {
		return NULL;
	}
	verifyCorrectClass(self,CPUPowerPC);

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
	}
    #endif
#endif

	return mut;
}

		
static void CPUPowerPC_memoryFence (CPUPowerPC* restrict self)
{
	if (!self) {
		return;
	}
	verifyCorrectClass(self,CPUPowerPC);

	//asm volatile("" ::: "memory");
}

static void CPUPowerPC_flushDataCacheAt (CPUPowerPC* restrict self, void* address, size_t size)
{
	//asm volatile("" ::: "memory");
}

static String* CPUPowerPC_instructionSet (CPUPowerPC* restrict self) 
{
	if (!self) {
		return NULL;
	}
	verifyCorrectClass(self,CPUPowerPC);

	// NOTE: No support for powerpc32 at this time.

#if __BYTE_ORDER == __ORDER_LITTLE_ENDIAN__
	return _String("ppc64le");
#else
	return _String("ppc64");
#endif
}

static String* CPUPowerPC_family (CPUPowerPC* restrict self) 
{
	if (!self) {
		return NULL;
	}
	verifyCorrectClass(self,CPUPowerPC);

	return _String("PowerPC");
}

CPUPowerPC* CPUPowerPC_init (CPUPowerPC* restrict self)
{
	ENSURE_CLASS_READY(CPUPowerPC);

	if (self) {
		CPU_init ((CPU*) self);
		self->is_a = _CPUPowerPCClass;

		(void) $(self, nCores);
		(void) $(self, features);
	}

	return self;
}

CPUPowerPCClass* CPUPowerPCClass_init (CPUPowerPCClass* restrict class)
{
	SET_SUPERCLASS(CPU);

	SET_OVERRIDDEN_METHOD_POINTER(CPUPowerPC,describe);
	SET_OVERRIDDEN_METHOD_POINTER(CPUPowerPC,print);
	SET_OVERRIDDEN_METHOD_POINTER(CPUPowerPC,model);
	SET_OVERRIDDEN_METHOD_POINTER(CPUPowerPC,features);
	SET_OVERRIDDEN_METHOD_POINTER(CPUPowerPC,memoryFence);
	SET_OVERRIDDEN_METHOD_POINTER(CPUPowerPC,flushDataCacheAt);
	SET_OVERRIDDEN_METHOD_POINTER(CPUPowerPC,family);
	SET_OVERRIDDEN_METHOD_POINTER(CPUPowerPC,instructionSet);

        VALIDATE_CLASS_STRUCT(class);
	return class;
}

#endif // PowerPC
