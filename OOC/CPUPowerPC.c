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

// NOTE: No support for powerpc32 at this time.

#if defined(__powerpc) || defined(__powerpc__)

#include "CPUPowerPC.h"
#include "Utility.h"

#include <stdlib.h>
#include <sys/auxv.h>
#include <bits/hwcap.h> // <- It's not <asm/hwcap.h> on Debian PowerPC.
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
	char *result = execute_and_return_first_line ("grep -i '^model name[ \t]*' /proc/cpuinfo | sed 's/^.*://' | sed 's/^[ ]*//' ");
	if (result && *result) {
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

#ifdef PPC_FEATURE_32
	if (hwcap & PPC_FEATURE_32) {
		$(mut, addCString, "32");
	}
#endif
#ifdef PPC_FEATURE_64
	if (hwcap & PPC_FEATURE_64) {
		$(mut, addCString, "64");
	}
#endif
#ifdef PPC_FEATURE_601_INSTR
	if (hwcap & PPC_FEATURE_601_INSTR) {
		$(mut, addCString, "601_instr");
	}
#endif
#ifdef PPC_FEATURE_HAS_ALTIVEC
	if (hwcap & PPC_FEATURE_HAS_ALTIVEC) {
		$(mut, addCString, "has_altivec");
		self->has128bitVectors = true; // 32 128-bit vector registers
	}
#endif
#ifdef PPC_FEATURE_HAS_FPU
	if (hwcap & PPC_FEATURE_HAS_FPU) {
		$(mut, addCString, "has_fpu");
	}
#endif
#ifdef PPC_FEATURE_HAS_MMU
	if (hwcap & PPC_FEATURE_HAS_MMU) {
		$(mut, addCString, "has_mmu");
	}
#endif
#ifdef PPC_FEATURE_HAS_4xxMAC
	if (hwcap & PPC_FEATURE_HAS_4xxMAC) {
		$(mut, addCString, "has_4xxmac");
	}
#endif
#ifdef PPC_FEATURE_UNIFIED_CACHE
	if (hwcap & PPC_FEATURE_UNIFIED_CACHE) {
		$(mut, addCString, "unified_cache");
	}
#endif
#ifdef PPC_FEATURE_HAS_SPE
	if (hwcap & PPC_FEATURE_HAS_SPE) {
		$(mut, addCString, "has_spe");
	}
#endif
#ifdef PPC_FEATURE_HAS_EFP_SINGLE
	if (hwcap & PPC_FEATURE_HAS_EFP_SINGLE) {
		$(mut, addCString, "has_efp_single");
	}
#endif
#ifdef PPC_FEATURE_HAS_EFP_DOUBLE
	if (hwcap & PPC_FEATURE_HAS_EFP_DOUBLE) {
		$(mut, addCString, "has_efp_double");
	}
#endif
#ifdef PPC_FEATURE_NO_TB
	if (hwcap & PPC_FEATURE_NO_TB) {
		$(mut, addCString, "no_tb");
	}
#endif
#ifdef PPC_FEATURE_POWER4
	if (hwcap & PPC_FEATURE_POWER4) {
		$(mut, addCString, "power4");
	}
#endif
#ifdef PPC_FEATURE_POWER5
	if (hwcap & PPC_FEATURE_POWER5) {
		$(mut, addCString, "power5");
	}
#endif
#ifdef PPC_FEATURE_POWER5_PLUS
	if (hwcap & PPC_FEATURE_POWER5_PLUS) {
		$(mut, addCString, "power5_plus");
	}
#endif
#ifdef PPC_FEATURE_CELL
	if (hwcap & PPC_FEATURE_CELL) {
		$(mut, addCString, "cell");
	}
#endif
#ifdef PPC_FEATURE_BOOKE
	if (hwcap & PPC_FEATURE_BOOKE) {
		$(mut, addCString, "booke");
	}
#endif
#ifdef PPC_FEATURE_SMT
	if (hwcap & PPC_FEATURE_SMT) {
		$(mut, addCString, "smt");
	}
#endif
#ifdef PPC_FEATURE_ICACHE_SNOOP
	if (hwcap & PPC_FEATURE_ICACHE_SNOOP) {
		$(mut, addCString, "icache_snoop");
	}
#endif
#ifdef PPC_FEATURE_ARCH_2_05
	if (hwcap & PPC_FEATURE_ARCH_2_05) {
		$(mut, addCString, "arch_2_05");
	}
#endif
#ifdef PPC_FEATURE_PA6T
	if (hwcap & PPC_FEATURE_PA6T) {
		$(mut, addCString, "pa6t");
	}
#endif
#ifdef PPC_FEATURE_HAS_DFP
	if (hwcap & PPC_FEATURE_HAS_DFP) {
		$(mut, addCString, "has_dfp");
	}
#endif
#ifdef PPC_FEATURE_POWER6_EXT
	if (hwcap & PPC_FEATURE_POWER6_EXT) {
		$(mut, addCString, "power6_ext");
	}
#endif
#ifdef PPC_FEATURE_ARCH_2_06
	if (hwcap & PPC_FEATURE_ARCH_2_06) {
		$(mut, addCString, "arch_2_06");
	}
#endif
#ifdef PPC_FEATURE_HAS_VSX
	if (hwcap & PPC_FEATURE_HAS_VSX) {
		$(mut, addCString, "has_vsx");
		self->has128bitVectors = true; // 64 128-bit vector registers
	}
#endif

	unsigned long hwcap2 = getauxval(AT_HWCAP2);
#ifdef PPC_FEATURE2_ARCH_2_07
	if (hwcap2 & PPC_FEATURE2_ARCH_2_07) {
		$(mut, addCString, "arch_2_07");
	}
#endif
#ifdef PPC_FEATURE2_HTM
	if (hwcap2 & PPC_FEATURE2_HTM) {
		$(mut, addCString, "htm");
	}
#endif
#ifdef PPC_FEATURE2_DSCR
	if (hwcap2 & PPC_FEATURE2_DSCR) {
		$(mut, addCString, "dscr");
	}
#endif
#ifdef PPC_FEATURE2_EBB
	if (hwcap2 & PPC_FEATURE2_EBB) {
		$(mut, addCString, "ebb");
	}
#endif
#ifdef PPC_FEATURE2_ISEL
	if (hwcap2 & PPC_FEATURE2_ISEL) {
		$(mut, addCString, "isel");
	}
#endif
#ifdef PPC_FEATURE2_TAR
	if (hwcap2 & PPC_FEATURE2_TAR) {
		$(mut, addCString, "tar");
	}
#endif
#ifdef PPC_FEATURE2_VEC_CRYPTO
	if (hwcap2 & PPC_FEATURE2_VEC_CRYPTO) {
		$(mut, addCString, "vec_crypto");
	}
#endif
#ifdef PPC_FEATURE2_HTM_NOSC
	if (hwcap2 & PPC_FEATURE2_HTM_NOSC) {
		$(mut, addCString, "htm_nosc");
	}
#endif
#ifdef PPC_FEATURE2_ARCH_3_00
	if (hwcap2 & PPC_FEATURE2_ARCH_3_00) {
		$(mut, addCString, "arch_3_00");
	}
#endif
#ifdef PPC_FEATURE2_HAS_IEEE128
	if (hwcap2 & PPC_FEATURE2_HAS_IEEE128) {
		$(mut, addCString, "has_ieee128");
	}
#endif
#ifdef PPC_FEATURE2_DARN
	if (hwcap2 & PPC_FEATURE2_DARN) {
		$(mut, addCString, "darn");
	}
#endif
#ifdef PPC_FEATURE2_SCV
	if (hwcap2 & PPC_FEATURE2_SCV) {
		$(mut, addCString, "scv");
	}
#endif
#ifdef PPC_FEATURE2_HTM_NO_SUSPEND
	if (hwcap2 & PPC_FEATURE2_HTM_NO_SUSPEND) {
		$(mut, addCString, "htm_no_suspend");
	}
#endif
#ifdef PPC_FEATURE2_ARCH_3_1
	if (hwcap2 & PPC_FEATURE2_ARCH_3_1) {
		$(mut, addCString, "arch_3_1");
	}
#endif
#ifdef PPC_FEATURE2_MMA
	if (hwcap2 & PPC_FEATURE2_MMA) {
		$(mut, addCString, "mma");
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

	// Non-privileged instruction.
	asm volatile("SYNC" ::: "memory");
}

static void CPUPowerPC_flushDataCacheAt (CPUPowerPC* restrict self, void* address, size_t size)
{
	if (!self || !address) {
		return;
	}

	// Non-privileged instruction.
	asm volatile ("DCBF 0, %0" :: "r" ((void*)(address)) : "memory");
}

static String* CPUPowerPC_instructionSet (CPUPowerPC* restrict self) 
{
	if (!self) {
		return NULL;
	}
	verifyCorrectClass(self,CPUPowerPC);

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
