/*============================================================================
  CPULoong, an object-oriented C Loong64 CPU class.
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

// NOTE: No support for loongarch32 at this time.

#if defined(__loongarch64)

#include "CPULoong.h"
#include "OS.h"
#include "Utility.h"

#include <stdlib.h>
#include <sys/auxv.h>
#include <asm/hwcap.h>

CPULoongClass *_CPULoongClass = NULL;

void CPULoong_destroy (Any *self)
{
	DEBUG_DESTROY;
	if (!self) {
		return;
	}
	verifyCorrectClassOrSubclass(self,CPULoong);

	CPU_destroy((CPU*)self);
}

static void CPULoong_print (CPULoong* restrict self, FILE *outputFile) 
{ 
	if (!self) {
		return;
	}
	verifyCorrectClass(self,CPULoong);

	if (!outputFile) {
		outputFile = stdout;
	}
}

static void CPULoong_describe (CPULoong* restrict self, FILE *outputFile) 
{ 
	if (!self) {
		return;
	}
	verifyCorrectClass(self,CPULoong);

	if (!outputFile) {
		outputFile = stdout;
	}

	fprintf (outputFile, "%s", $(self, className));
}

static String* CPULoong_model (CPULoong* restrict self) 
{
	if (!self) {
		return NULL;
	}
	verifyCorrectClassOrSubclass(self,CPU);

#if defined(__linux__) 
	char *result = execute_and_return_first_line ("grep '^Model Name' /proc/cpuinfo | sed 's/^.*://' | sed 's/^[ ]*//' ");
	if (result) {
		return String_withCString (result);
	}
#endif

	return NULL;
}

static MutableSet* CPULoong_features (CPULoong* restrict self)
{
	if (!self) {
		return NULL;
	}
	verifyCorrectClass(self,CPULoong);

	MutableSet *mut = self->features;
	if (!$(mut, isEmpty)) {
		return mut;
	}

#if defined(__linux__)
	unsigned long hwcap = getauxval(AT_HWCAP);

#ifdef HWCAP_LOONGARCH_CPUCFG
	if (hwcap & HWCAP_LOONGARCH_CPUCFG) {
		$(mut, addCString, "cpucfg");
	}
#endif
#ifdef HWCAP_LOONGARCH_LAM
	if (hwcap & HWCAP_LOONGARCH_LAM) {
		$(mut, addCString, "lam");
	}
#endif
#ifdef HWCAP_LOONGARCH_UAL
	if (hwcap & HWCAP_LOONGARCH_UAL) {
		$(mut, addCString, "ual");
	}
#endif
#ifdef HWCAP_LOONGARCH_FPU
	if (hwcap & HWCAP_LOONGARCH_FPU) {
		$(mut, addCString, "fpu");
	}
#endif
#ifdef HWCAP_LOONGARCH_LSX
	if (hwcap & HWCAP_LOONGARCH_LSX) {
		$(mut, addCString, "lsx");
		self->has128bitVectors = true;
	}
#endif
#ifdef HWCAP_LOONGARCH_LASX
	if (hwcap & HWCAP_LOONGARCH_LASX) {
		$(mut, addCString, "lasx");
		self->has256bitVectors = true;
	}
#endif
#ifdef HWCAP_LOONGARCH_CRC32
	if (hwcap & HWCAP_LOONGARCH_CRC32) {
		$(mut, addCString, "crc32");
	}
#endif
#ifdef HWCAP_LOONGARCH_COMPLEX
	if (hwcap & HWCAP_LOONGARCH_COMPLEX) {
		$(mut, addCString, "complex");
	}
#endif
#ifdef HWCAP_LOONGARCH_CRYPTO
	if (hwcap & HWCAP_LOONGARCH_CRYPTO) {
		$(mut, addCString, "crypto");
	}
#endif
#ifdef HWCAP_LOONGARCH_LVZ
	if (hwcap & HWCAP_LOONGARCH_LVZ) {
		$(mut, addCString, "lvz");
	}
#endif
#ifdef HWCAP_LOONGARCH_LBT_X86
	if (hwcap & HWCAP_LOONGARCH_LBT_X86) {
		$(mut, addCString, "lbt_x86");
	}
#endif
#ifdef HWCAP_LOONGARCH_LBT_ARM
	if (hwcap & HWCAP_LOONGARCH_LBT_ARM) {
		$(mut, addCString, "lbt_arm");
	}
#endif
#ifdef HWCAP_LOONGARCH_LBT_MIPS
	if (hwcap & HWCAP_LOONGARCH_LBT_MIPS) {
		$(mut, addCString, "lbt_mips");
	}
#endif
#ifdef HWCAP_LOONGARCH_PTW
	if (hwcap & HWCAP_LOONGARCH_PTW) {
		$(mut, addCString, "ptw");
	}
#endif
#ifdef HWCAP_LOONGARCH_LSPW
	if (hwcap & HWCAP_LOONGARCH_LSPW) {
		$(mut, addCString, "lspw");
	}
#endif
#ifdef HWCAP_LOONGARCH_SCQ
	if (hwcap & HWCAP_LOONGARCH_SCQ) {
		$(mut, addCString, "scq");
	}
#endif
#ifdef HWCAP_LOONGARCH_LAM_BH
	if (hwcap & HWCAP_LOONGARCH_LAM_BH) {
		$(mut, addCString, "lam_bh");
	}
#endif
#endif

	return mut;
}

		
static void CPULoong_memoryFence (CPULoong* restrict self)
{
	if (!self) {
		return;
	}
	verifyCorrectClass(self,CPULoong);

	asm volatile ("DBAR 0" ::: "memory");
}

static void CPULoong_flushDataCacheAt (CPULoong* restrict self, void* address, size_t size)
{
	if (!self || !address) {
		return;
	}

	if (OS_isRoot()) {
		asm volatile(
			"CACOP 0x11, %0, 0" // 0x11 writeback+invalidate
			:: "r" (address) : "memory");
	}
}

static String* CPULoong_instructionSet (CPULoong* restrict self) 
{
	if (!self) {
		return NULL;
	}
	verifyCorrectClass(self,CPULoong);

	return _String("LoongArch64");
}

static String* CPULoong_family (CPULoong* restrict self) 
{
	if (!self) {
		return NULL;
	}
	verifyCorrectClass(self,CPULoong);

#if defined(__linux__) 
	char *result = execute_and_return_first_line ("grep '^CPU Family' /proc/cpuinfo | sed 's/^.*://' | sed 's/^[ ]*//' ");
	if (result) {
		return String_withCString (result);
	}
#endif

	return _String("Loong");
}

CPULoong* CPULoong_init (CPULoong* restrict self)
{
	ENSURE_CLASS_READY(CPULoong);

	if (self) {
		CPU_init ((CPU*) self);
		self->is_a = _CPULoongClass;

		(void) $(self, nCores);
		(void) $(self, features);
	}

	return self;
}

CPULoongClass* CPULoongClass_init (CPULoongClass* restrict class)
{
	SET_SUPERCLASS(CPU);

	SET_OVERRIDDEN_METHOD_POINTER(CPULoong,describe);
	SET_OVERRIDDEN_METHOD_POINTER(CPULoong,print);
	SET_OVERRIDDEN_METHOD_POINTER(CPULoong,model);
	SET_OVERRIDDEN_METHOD_POINTER(CPULoong,features);
	SET_OVERRIDDEN_METHOD_POINTER(CPULoong,memoryFence);
	SET_OVERRIDDEN_METHOD_POINTER(CPULoong,flushDataCacheAt);
	SET_OVERRIDDEN_METHOD_POINTER(CPULoong,family);
	SET_OVERRIDDEN_METHOD_POINTER(CPULoong,instructionSet);

        VALIDATE_CLASS_STRUCT(class);
	return class;
}

#endif // Loong
