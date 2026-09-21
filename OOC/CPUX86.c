/*============================================================================
  CPUX86, an object-oriented C x86 CPU class.
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

#if defined(__x86_64__) || defined(__i386__) 

#include "CPUX86.h"
#include "Utility.h"
#include "utility-x86.h"

#include <stdlib.h>
#include <string.h>
#include <immintrin.h>

#ifdef __linux__
#include <sys/auxv.h>
#endif

CPUX86Class *_CPUX86Class = NULL;

void CPUX86_destroy (Any *self)
{
	DEBUG_DESTROY;
	if (!self) {
		return;
	}
	verifyCorrectClassOrSubclass(self,CPUX86);

	CPU_destroy((CPU*)self);
}

static void CPUX86_print (CPUX86* restrict self, FILE *outputFile) 
{ 
	if (!self) {
		return;
	}
	verifyCorrectClass(self,CPUX86);

	if (!outputFile) {
		outputFile = stdout;
	}
}

static void CPUX86_describe (CPUX86* restrict self, FILE *outputFile) 
{ 
	if (!self) {
		return;
	}
	verifyCorrectClass(self,CPUX86);

	if (!outputFile) {
		outputFile = stdout;
	}

	fprintf (outputFile, "%s", $(self, className));
}

static String* CPUX86_model (CPUX86* restrict self) 
{
	if (!self) {
		return NULL;
	}
	verifyCorrectClassOrSubclass(self,CPUX86);

	char temp[49];
	get_cpuid_model (temp);
	temp[48] = 0;
	return String_withCString (temp);
}

static String* CPUX86_make (CPUX86* restrict self) 
{
	if (!self) {
		return NULL;
	}
	verifyCorrectClassOrSubclass(self,CPUX86);

	char family[17];
	ooc_bzero (family, sizeof(family));

#if defined(__x86_64__) || defined(__WIN64__)
	//------------
	// x86_64
	//
	uint32_t *ptr = (uint32_t*) family;
	ptr[0] = get_cpuid_family1();
	ptr[1] = get_cpuid_family2();
	ptr[2] = get_cpuid_family3();
#endif

#if defined(__i386__) || defined(_WIN32) || defined(__WIN32__) || defined(__MINGW32__) || defined(__i386)
	//------------
	// i386
	//
	get_cpuid_family (family);
#endif

	if (!strcmp (family, "GenuineIntel")) {
		self->isIntel = true;
	}

	return String_withCString (family);
}

static MutableSet* CPUX86_features (CPUX86* restrict self)
{
	if (!self) {
		return NULL;
	}
	verifyCorrectClass(self,CPUX86);

	MutableSet *mut = self->features;
	if (!$(mut, isEmpty)) {
		return mut;
	}

	uint32_t eax, ebx, ecx, edx;

	//------------
	// CPUID eax=1
	//
	ebx = get_cpuid1_ebx ();
	ecx = get_cpuid1_ecx ();
	edx = get_cpuid1_edx ();

	self->cacheLineSize = ((ebx >> 8) & 0xff) * 8;

	if (ecx & CPUID1_ECX_PCLMULQDQ)	$(mut, addCString, "pcmulqdq");
	if (ecx & CPUID1_ECX_DTES64)	$(mut, addCString, "dtes64");
	if (ecx & CPUID1_ECX_MONITOR)	$(mut, addCString, "monitor");
	if (ecx & CPUID1_ECX_DS_CPL)	$(mut, addCString, "dscpl");
	if (ecx & CPUID1_ECX_VMX)	$(mut, addCString, "vmx");
	if (ecx & CPUID1_ECX_SMX)	$(mut, addCString, "smx");
	if (ecx & CPUID1_ECX_EIST)	$(mut, addCString, "eist");
	if (ecx & CPUID1_ECX_CNXTID)	$(mut, addCString, "cnxtid");
	if (ecx & CPUID1_ECX_SDBG)	$(mut, addCString, "sdbg");
	if (ecx & CPUID1_ECX_FMA)	$(mut, addCString, "fma");
	if (ecx & CPUID1_ECX_CMPXCHG16B)	$(mut, addCString, "cmpxchg16b");
	if (ecx & CPUID1_ECX_XTPR)	$(mut, addCString, "xtpr");
	if (ecx & CPUID1_ECX_PDCM)	$(mut, addCString, "pdcm");
	if (ecx & CPUID1_ECX_PCID)	$(mut, addCString, "pcid");
	if (ecx & CPUID1_ECX_DCA)	$(mut, addCString, "dca");
	if (ecx & CPUID1_ECX_X2APIC)	$(mut, addCString, "x2apic");
	if (ecx & CPUID1_ECX_MOVBE)	$(mut, addCString, "movbe");
	if (ecx & CPUID1_ECX_F16C)	$(mut, addCString, "f16c");
	if (ecx & CPUID1_ECX_SSE3)	$(mut, addCString, "sse3");
	if (ecx & CPUID1_ECX_SSSE3)	$(mut, addCString, "ssse3");
	if (ecx & CPUID1_ECX_SSE41) {
		$(mut, addCString, "sse41");
		self->hasSSE4 = true;
	}
	if (ecx & CPUID1_ECX_SSE42)	$(mut, addCString, "sse42");
	if (ecx & CPUID1_ECX_AESNI)	$(mut, addCString, "aesni");
	if (ecx & CPUID1_ECX_XSAVE)	$(mut, addCString, "xsave");
	if (ecx & CPUID1_ECX_OSXSAVE)	$(mut, addCString, "osxsave");
	if (ecx & CPUID1_ECX_RDRAND)	$(mut, addCString, "rdrand");
	if (ecx & CPUID1_ECX_AVX) {
		$(mut, addCString, "avx");
		self->hasAVX = true;
	}
	if (ecx & CPUID1_ECX_HYPER_GUEST)	$(mut, addCString, "hyper");
	if (ecx & CPUID1_ECX_POPCNT)	$(mut, addCString, "popcnt");
	if (ecx & CPUID1_ECX_TM2)	$(mut, addCString, "tm2");

	if (edx & CPUID1_EDX_FPU)	$(mut, addCString, "fpu");
	if (edx & CPUID1_EDX_VME)	$(mut, addCString, "vme");
	if (edx & CPUID1_EDX_DE)	$(mut, addCString, "de");
	if (edx & CPUID1_EDX_PSE)	$(mut, addCString, "pse");
	if (edx & CPUID1_EDX_TSC)	$(mut, addCString, "tsc");
	if (edx & CPUID1_EDX_MSR)	$(mut, addCString, "msr");
	if (edx & CPUID1_EDX_PAE)	$(mut, addCString, "pae");
	if (edx & CPUID1_EDX_MCE)	$(mut, addCString, "mce");
	if (edx & CPUID1_EDX_CX8)	$(mut, addCString, "cx8");
	if (edx & CPUID1_EDX_APIC)	$(mut, addCString, "apic");
	if (edx & CPUID1_EDX_SEP)	$(mut, addCString, "sep");
	if (edx & CPUID1_EDX_MTRR)	$(mut, addCString, "mtrr");
	if (edx & CPUID1_EDX_PGE)	$(mut, addCString, "pge");
	if (edx & CPUID1_EDX_MCA)	$(mut, addCString, "mca");
	if (edx & CPUID1_EDX_TM)	$(mut, addCString, "tm");
	if (edx & CPUID1_EDX_CMOV)	$(mut, addCString, "cmov");
	if (edx & CPUID1_EDX_ACPI)	$(mut, addCString, "acpi");
	if (edx & CPUID1_EDX_HTT)	$(mut, addCString, "htt");
	if (edx & CPUID1_EDX_SSE2) {
		$(mut, addCString, "sse2");
		self->hasSSE2 = true;
	}
	if (edx & CPUID1_EDX_MMX)	$(mut, addCString, "mmx");
	if (edx & CPUID1_EDX_PAT)	$(mut, addCString, "pat");
	if (edx & CPUID1_EDX_PSE36)	$(mut, addCString, "pse36");
	if (edx & CPUID1_EDX_PSN)	$(mut, addCString, "psn");
	if (edx & CPUID1_EDX_CLFSH)	$(mut, addCString, "clfsh");
	if (edx & CPUID1_EDX_DS)	$(mut, addCString, "ds");
	if (edx & CPUID1_EDX_SS)	$(mut, addCString, "ss");
	if (edx & CPUID1_EDX_FXSR)	$(mut, addCString, "fxsr");
	if (edx & CPUID1_EDX_PBE)	$(mut, addCString, "pbe");
	if (edx & CPUID1_EDX_SSE) {
		$(mut, addCString, "sse");
		self->hasSSE = true;
	}

	//-------------------
	// CPUID eax=7, ecx=0
	//
	eax = get_cpuid7_eax (); 
	ebx = get_cpuid7_ebx (); 
	ecx = get_cpuid7_ecx (); 
	edx = get_cpuid7_edx ();

	if (ebx & CPUID7_EBX_ADX)	$(mut, addCString, "adx");
	if (ebx & CPUID7_EBX_BMI1)	$(mut, addCString, "bmi1");
	if (ebx & CPUID7_EBX_BMI2)	$(mut, addCString, "bmi2");
	if (ebx & CPUID7_EBX_CLFLUSHOPT)	$(mut, addCString, "clflushopt");
	if (ebx & CPUID7_EBX_CLWB)	$(mut, addCString, "clwb");
	if (ebx & CPUID7_EBX_HLE)	$(mut, addCString, "hle");
	if (ebx & CPUID7_EBX_INVPCID)	$(mut, addCString, "invpcid");
	if (ebx & CPUID7_EBX_MPX)	$(mut, addCString, "mpx");
	if (ebx & CPUID7_EBX_PROC_TRACE)	$(mut, addCString, "trace");
	if (ebx & CPUID7_EBX_RDSEED)	$(mut, addCString, "rdseed");
	if (ebx & CPUID7_EBX_REP_MOVSB)	$(mut, addCString, "repmovsb");
	if (ebx & CPUID7_EBX_RTM)	$(mut, addCString, "rtm");
	if (ebx & CPUID7_EBX_SGX)	$(mut, addCString, "sgx");
	if (ebx & CPUID7_EBX_SHA)	$(mut, addCString, "sha");
	if (ebx & CPUID7_EBX_SMAP)	$(mut, addCString, "smap");
	if (self->hasAVX) {
		if (ebx & CPUID7_EBX_AVX2)	$(mut, addCString, "avx2");
		if (ebx & CPUID7_EBX_AVX512_VL)	$(mut, addCString, "avx512vl");
		if (ebx & CPUID7_EBX_AVX512_F)	$(mut, addCString, "avx512f");
		if (ebx & CPUID7_EBX_AVX512_DQ)	$(mut, addCString, "avx512dq");
		if (ebx & CPUID7_EBX_AVX512_IFMA)	$(mut, addCString, "avx512ifma");
		if (ebx & CPUID7_EBX_AVX512_PF)	$(mut, addCString, "avx512pf");
		if (ebx & CPUID7_EBX_AVX512_ER)	$(mut, addCString, "avx512er");
		if (ebx & CPUID7_EBX_AVX512_CD)	$(mut, addCString, "avx512cd");
		if (ebx & CPUID7_EBX_AVX512_BW)	$(mut, addCString, "avx512bw");

		if ((ebx & CPUID7_EBX_AVX512_F) && (ebx & CPUID7_EBX_AVX512_DQ)) {
			self->hasAVX512 = true;
		}
	}

	if (ecx & CPUID7_ECX_UMIP)	$(mut, addCString, "umip");
	if (ecx & CPUID7_ECX_PKU)	$(mut, addCString, "pku");
	if (ecx & CPUID7_ECX_WAITPKG)	$(mut, addCString, "waitpkg");
	if (ecx & CPUID7_ECX_UMIP)	$(mut, addCString, "umip");
	if (ecx & CPUID7_ECX_GFNI)	$(mut, addCString, "gfni");
	if (ecx & CPUID7_ECX_KL)	$(mut, addCString, "kl");
	if (ecx & CPUID7_ECX_LA57)	$(mut, addCString, "la57");
	if (ecx & CPUID7_ECX_PKS)	$(mut, addCString, "pks");
	if (ecx & CPUID7_ECX_RDPID)	$(mut, addCString, "rdpid");
	if (ecx & CPUID7_ECX_CLDEMOTE)	$(mut, addCString, "cldemote");
	if (ecx & CPUID7_ECX_ENQCMD)	$(mut, addCString, "enqcmd");
	if (ecx & CPUID7_ECX_CET)	$(mut, addCString, "cet");
	if (ecx & CPUID7_ECX_VAES)	$(mut, addCString, "vaes");
	if (ecx & CPUID7_ECX_VPCLMULQDQ)	$(mut, addCString, "vpclmulqdq");
	if (ecx & CPUID7_ECX_MOVDIRI)	$(mut, addCString, "movdiri");
	if (ecx & CPUID7_ECX_MOVDIR64B)	$(mut, addCString, "movdir64b");
	if (self->hasAVX512) {
		if (ecx & CPUID7_ECX_AVX512_VNNI)	$(mut, addCString, "avx512vnni");
		if (ecx & CPUID7_ECX_AVX512_VBMI)	$(mut, addCString, "avx512vbmi");
		if (ecx & CPUID7_ECX_AVX512_VBMI2)	$(mut, addCString, "avx512vbmi2");
		if (ecx & CPUID7_ECX_AVX512_BITALG)	$(mut, addCString, "avx512bitalg");
		if (ecx & CPUID7_ECX_AVX512_VPOPCNTDQ)	$(mut, addCString, "avx512vpopcntdq");
		if (edx & CPUID7_EDX_AVX512_FP16)	$(mut, addCString, "avx512fp16");
		if (edx & CPUID7_EDX_AVX512_VP2INTERSECT)	$(mut, addCString, "avx512vp2intersect");
		if (edx & CPUID7_EDX_AVX512_4VNNIW)	$(mut, addCString, "avx512_4vnniw");
		if (edx & CPUID7_EDX_AVX512_4FMAPS)	$(mut, addCString, "avx512_4fmaps");
	}

	if (edx & CPUID7_EDX_HYBRID)	$(mut, addCString, "hybrid");
	if (edx & CPUID7_EDX_AMX_BF16)	$(mut, addCString, "amx_bf16");
	if (edx & CPUID7_EDX_AMX_TILE)	$(mut, addCString, "amx_tile");
	if (edx & CPUID7_EDX_AMX_INT8)	$(mut, addCString, "amx_int8");
	if (edx & CPUID7_EDX_REPMOV)	$(mut, addCString, "fastrepmov");
	if (edx & CPUID7_EDX_UINTR)	$(mut, addCString, "uintr");
	if (edx & CPUID7_EDX_MDCLEAR)	$(mut, addCString, "mdclear");
	if (edx & CPUID7_EDX_SERIAL)	$(mut, addCString, "serial");
	if (edx & CPUID7_EDX_TSXLDTRK)	$(mut, addCString, "tsxldtrk");
	if (edx & CPUID7_EDX_PCONFIG)	$(mut, addCString, "pconfig");
	if (edx & CPUID7_EDX_ALBRS)	$(mut, addCString, "albrs");
	if (edx & CPUID7_EDX_CET_IBT)	$(mut, addCString, "cet_ibt");

	//-------------------
	eax = get_cpuid6_eax (); 
	if (eax & CPUID6_EAX_THERMOMETER)	$(mut, addCString, "thermometer");
	if (eax & CPUID6_EAX_TURBOBOOST)	$(mut, addCString, "turboboost");
	if (eax & CPUID6_EAX_ARAT)	$(mut, addCString, "arat");
	if (eax & CPUID6_EAX_PLN)	$(mut, addCString, "pln");
	if (eax & CPUID6_EAX_ECMD)	$(mut, addCString, "ecmd");
	if (eax & CPUID6_EAX_PTM)	$(mut, addCString, "ptm");
	if (eax & CPUID6_EAX_BOOSTMAX3)	$(mut, addCString, "boostmax3");
	if (eax & CPUID6_EAX_HWP)	$(mut, addCString, "hwp");
	if (eax & CPUID6_EAX_THREAD_DIRECTOR)	$(mut, addCString, "thread_dir");

	//-------------------
	// CPUID eax=80000001
	//
	uint32_t ecx2 = get_cpuid_80000001_ecx ();
	if (ecx2 & CPUID80000001_ECX_LZCNT) $(mut, addCString, "lzcnt");
	if (ecx2 & CPUID80000001_ECX_PREFETCHW) $(mut, addCString, "prefetchw");

	uint32_t edx2 = get_cpuid_80000001_edx ();
	if (edx2 & CPUID80000001_EDX_NX) $(mut, addCString, "nx");
	if (edx2 & CPUID80000001_EDX_MMXEXT) $(mut, addCString, "mmxext");
	if (edx2 & CPUID80000001_EDX_INTEL64) $(mut, addCString, "intel64");

	self->has128bitVectors = self->hasSSE && self->hasSSE2;
	self->has256bitVectors = self->hasAVX;
	self->has512bitVectors = self->hasAVX512;

	return self->features;
}

static void CPUX86_memoryFence (CPUX86* restrict self)
{
	if (!self) {
		return;
	}
	verifyCorrectClass(self,CPUX86);

        asm volatile("mfence" ::: "memory"); 
}

static void CPUX86_flushDataCacheAt (CPUX86* restrict self, void *address, size_t size)
{
	if (!self || !address || !size) {
		return;
	}
	verifyCorrectClass(self,CPUX86);

	asm volatile(
		"CLFLUSH (%0)\n" 
		: 
		: "r"(address) 
		: "memory");
}

static String* CPUX86_instructionSet (CPUX86* restrict self) 
{
	if (!self) {
		return NULL;
	}
	verifyCorrectClass(self,CPUX86);

#if defined(__x86_64__) || defined(__WIN64__)
	return _String("x86_64");
#endif
#if defined(__i386__) || defined(_WIN32) || defined(__WIN32__) || defined(__MINGW32__) || defined(__i386) 
	return _String("i386");
#endif
}

static String* CPUX86_family (CPUX86* restrict self) 
{
	if (!self) {
		return NULL;
	}
	verifyCorrectClass(self,CPUX86);

	return _String("X86");
}

CPUX86Class* CPUX86Class_init (CPUX86Class* restrict class)
{
	SET_SUPERCLASS(CPU);

	SET_OVERRIDDEN_METHOD_POINTER(CPUX86,describe);
	SET_OVERRIDDEN_METHOD_POINTER(CPUX86,print);
	SET_OVERRIDDEN_METHOD_POINTER(CPUX86,family);
	SET_OVERRIDDEN_METHOD_POINTER(CPUX86,model);
	SET_OVERRIDDEN_METHOD_POINTER(CPUX86,make);
	SET_OVERRIDDEN_METHOD_POINTER(CPUX86,features);
//	SET_OVERRIDDEN_METHOD_POINTER(CPUX86,levelNCacheSize);
//	SET_OVERRIDDEN_METHOD_POINTER(CPUX86,nDataTLBEntries);
//	SET_OVERRIDDEN_METHOD_POINTER(CPUX86,nUnifiedTLBEntries);
//	SET_OVERRIDDEN_METHOD_POINTER(CPUX86,nInstructionTLBEntries);
	SET_OVERRIDDEN_METHOD_POINTER(CPUX86,memoryFence);
	SET_OVERRIDDEN_METHOD_POINTER(CPUX86,flushDataCacheAt);
	SET_OVERRIDDEN_METHOD_POINTER(CPUX86,instructionSet);

        VALIDATE_CLASS_STRUCT(class);
	return class;
}

CPUX86* CPUX86_init (CPUX86* restrict self)
{
	ENSURE_CLASS_READY(CPUX86);

	if (self) {
		CPU_init ((CPU*) self);
		self->is_a = _CPUX86Class;

		(void) $(self, nCores);
		(void) $(self, features);
	}

	return self;
}

#endif // x86
