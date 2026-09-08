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
	bool have_sse = false;
	bool have_avx = false;
	ebx = get_cpuid1_ebx ();
	ecx = get_cpuid1_ecx ();
	edx = get_cpuid1_edx ();

	self->cacheLineSize = ((ebx >> 8) & 0xff) * 8;

	if (ecx & CPUID1_ECX_PCLMULQDQ)	$(mut, add, _String("pcmulqdq"));
	if (ecx & CPUID1_ECX_DTES64)	$(mut, add, _String("dtes64"));
	if (ecx & CPUID1_ECX_MONITOR)	$(mut, add, _String("monitor"));
	if (ecx & CPUID1_ECX_DS_CPL)	$(mut, add, _String("dscpl"));
	if (ecx & CPUID1_ECX_VMX)	$(mut, add, _String("vmx"));
	if (ecx & CPUID1_ECX_SMX)	$(mut, add, _String("smx"));
	if (ecx & CPUID1_ECX_EIST)	$(mut, add, _String("eist"));
	if (ecx & CPUID1_ECX_CNXTID)	$(mut, add, _String("cnxtid"));
	if (ecx & CPUID1_ECX_SDBG)	$(mut, add, _String("sdbg"));
	if (ecx & CPUID1_ECX_FMA)	$(mut, add, _String("fma"));
	if (ecx & CPUID1_ECX_CMPXCHG16B)	$(mut, add, _String("cmpxchg16b"));
	if (ecx & CPUID1_ECX_XTPR)	$(mut, add, _String("xtpr"));
	if (ecx & CPUID1_ECX_PDCM)	$(mut, add, _String("pdcm"));
	if (ecx & CPUID1_ECX_PCID)	$(mut, add, _String("pcid"));
	if (ecx & CPUID1_ECX_DCA)	$(mut, add, _String("dca"));
	if (ecx & CPUID1_ECX_X2APIC)	$(mut, add, _String("x2apic"));
	if (ecx & CPUID1_ECX_MOVBE)	$(mut, add, _String("movbe"));
	if (ecx & CPUID1_ECX_F16C)	$(mut, add, _String("f16c"));
	if (ecx & CPUID1_ECX_SSE3)	$(mut, add, _String("sse3"));
	if (ecx & CPUID1_ECX_SSSE3)	$(mut, add, _String("ssse3"));
	if (ecx & CPUID1_ECX_SSE41) {
		$(mut, add, _String("sse41"));
	}
	if (ecx & CPUID1_ECX_SSE42)	$(mut, add, _String("sse42"));
	if (ecx & CPUID1_ECX_AESNI)	$(mut, add, _String("aesni"));
	if (ecx & CPUID1_ECX_XSAVE)	$(mut, add, _String("xsave"));
	if (ecx & CPUID1_ECX_OSXSAVE)	$(mut, add, _String("osxsave"));
	if (ecx & CPUID1_ECX_RDRAND)	$(mut, add, _String("rdrand"));
	if (ecx & CPUID1_ECX_AVX) {
		have_avx = true;
		$(mut, add, _String("avx"));
	}
	if (ecx & CPUID1_ECX_HYPER_GUEST)	$(mut, add, _String("hyper"));
	if (ecx & CPUID1_ECX_POPCNT)	$(mut, add, _String("popcnt"));
	if (ecx & CPUID1_ECX_TM2)	$(mut, add, _String("tm2"));

	if (edx & CPUID1_EDX_FPU)	$(mut, add, _String("fpu"));
	if (edx & CPUID1_EDX_VME)	$(mut, add, _String("vme"));
	if (edx & CPUID1_EDX_DE)	$(mut, add, _String("de"));
	if (edx & CPUID1_EDX_PSE)	$(mut, add, _String("pse"));
	if (edx & CPUID1_EDX_TSC)	$(mut, add, _String("tsc"));
	if (edx & CPUID1_EDX_MSR)	$(mut, add, _String("msr"));
	if (edx & CPUID1_EDX_PAE)	$(mut, add, _String("pae"));
	if (edx & CPUID1_EDX_MCE)	$(mut, add, _String("mce"));
	if (edx & CPUID1_EDX_CX8)	$(mut, add, _String("cx8"));
	if (edx & CPUID1_EDX_APIC)	$(mut, add, _String("apic"));
	if (edx & CPUID1_EDX_SEP)	$(mut, add, _String("sep"));
	if (edx & CPUID1_EDX_MTRR)	$(mut, add, _String("mtrr"));
	if (edx & CPUID1_EDX_PGE)	$(mut, add, _String("pge"));
	if (edx & CPUID1_EDX_MCA)	$(mut, add, _String("mca"));
	if (edx & CPUID1_EDX_TM)	$(mut, add, _String("tm"));
	if (edx & CPUID1_EDX_CMOV)	$(mut, add, _String("cmov"));
	if (edx & CPUID1_EDX_ACPI)	$(mut, add, _String("acpi"));
	if (edx & CPUID1_EDX_HTT)	$(mut, add, _String("htt"));
	if (edx & CPUID1_EDX_SSE2)	$(mut, add, _String("sse2"));
	if (edx & CPUID1_EDX_MMX)	$(mut, add, _String("mmx"));
	if (edx & CPUID1_EDX_PAT)	$(mut, add, _String("pat"));
	if (edx & CPUID1_EDX_PSE36)	$(mut, add, _String("pse36"));
	if (edx & CPUID1_EDX_PSN)	$(mut, add, _String("psn"));
	if (edx & CPUID1_EDX_CLFSH)	$(mut, add, _String("clfsh"));
	if (edx & CPUID1_EDX_DS)	$(mut, add, _String("ds"));
	if (edx & CPUID1_EDX_SS)	$(mut, add, _String("ss"));
	if (edx & CPUID1_EDX_FXSR)	$(mut, add, _String("fxsr"));
	if (edx & CPUID1_EDX_PBE)	$(mut, add, _String("pbe"));
	if (edx & CPUID1_EDX_SSE) {
		have_sse = true;
		$(mut, add, _String("sse"));
	}

	//-------------------
	// CPUID eax=7, ecx=0
	//
	eax = get_cpuid7_eax (); 
	ebx = get_cpuid7_ebx (); 
	ecx = get_cpuid7_ecx (); 
	edx = get_cpuid7_edx ();

	if (ebx & CPUID7_EBX_ADX)	$(mut, add, _String("adx"));
	if (ebx & CPUID7_EBX_BMI1)	$(mut, add, _String("bmi1"));
	if (ebx & CPUID7_EBX_BMI2)	$(mut, add, _String("bmi2"));
	if (ebx & CPUID7_EBX_CLFLUSHOPT)	$(mut, add, _String("clflushopt"));
	if (ebx & CPUID7_EBX_CLWB)	$(mut, add, _String("clwb"));
	if (ebx & CPUID7_EBX_HLE)	$(mut, add, _String("hle"));
	if (ebx & CPUID7_EBX_INVPCID)	$(mut, add, _String("invpcid"));
	if (ebx & CPUID7_EBX_MPX)	$(mut, add, _String("mpx"));
	if (ebx & CPUID7_EBX_PROC_TRACE)	$(mut, add, _String("trace"));
	if (ebx & CPUID7_EBX_RDSEED)	$(mut, add, _String("rdseed"));
	if (ebx & CPUID7_EBX_REP_MOVSB)	$(mut, add, _String("repmovsb"));
	if (ebx & CPUID7_EBX_RTM)	$(mut, add, _String("rtm"));
	if (ebx & CPUID7_EBX_SGX)	$(mut, add, _String("sgx"));
	if (ebx & CPUID7_EBX_SHA)	$(mut, add, _String("sha"));
	if (ebx & CPUID7_EBX_SMAP)	$(mut, add, _String("smap"));
	bool have_avx512 = false;
	if (have_avx) {
		if (ebx & CPUID7_EBX_AVX2)	$(mut, add, _String("avx2"));
		if (ebx & CPUID7_EBX_AVX512_VL)	$(mut, add, _String("avx512vl"));
		if (ebx & CPUID7_EBX_AVX512_F)	$(mut, add, _String("avx512f"));
		if (ebx & CPUID7_EBX_AVX512_DQ)	$(mut, add, _String("avx512dq"));
		if (ebx & CPUID7_EBX_AVX512_IFMA)	$(mut, add, _String("avx512ifma"));
		if (ebx & CPUID7_EBX_AVX512_PF)	$(mut, add, _String("avx512pf"));
		if (ebx & CPUID7_EBX_AVX512_ER)	$(mut, add, _String("avx512er"));
		if (ebx & CPUID7_EBX_AVX512_CD)	$(mut, add, _String("avx512cd"));
		if (ebx & CPUID7_EBX_AVX512_BW)	$(mut, add, _String("avx512bw"));

		if ((ebx & CPUID7_EBX_AVX512_F) && (ebx & CPUID7_EBX_AVX512_DQ)) {
			have_avx512 = true;
		}
	}

	if (ecx & CPUID7_ECX_UMIP)	$(mut, add, _String("umip"));
	if (ecx & CPUID7_ECX_PKU)	$(mut, add, _String("pku"));
	if (ecx & CPUID7_ECX_WAITPKG)	$(mut, add, _String("waitpkg"));
	if (ecx & CPUID7_ECX_UMIP)	$(mut, add, _String("umip"));
	if (ecx & CPUID7_ECX_GFNI)	$(mut, add, _String("gfni"));
	if (ecx & CPUID7_ECX_KL)	$(mut, add, _String("kl"));
	if (ecx & CPUID7_ECX_LA57)	$(mut, add, _String("la57"));
	if (ecx & CPUID7_ECX_PKS)	$(mut, add, _String("pks"));
	if (ecx & CPUID7_ECX_RDPID)	$(mut, add, _String("rdpid"));
	if (ecx & CPUID7_ECX_CLDEMOTE)	$(mut, add, _String("cldemote"));
	if (ecx & CPUID7_ECX_ENQCMD)	$(mut, add, _String("enqcmd"));
	if (ecx & CPUID7_ECX_CET)	$(mut, add, _String("cet"));
	if (ecx & CPUID7_ECX_VAES)	$(mut, add, _String("vaes"));
	if (ecx & CPUID7_ECX_VPCLMULQDQ)	$(mut, add, _String("vpclmulqdq"));
	if (ecx & CPUID7_ECX_MOVDIRI)	$(mut, add, _String("movdiri"));
	if (ecx & CPUID7_ECX_MOVDIR64B)	$(mut, add, _String("movdir64b"));
	if (have_avx512) {
		if (ecx & CPUID7_ECX_AVX512_VNNI)	$(mut, add, _String("avx512vnni"));
		if (ecx & CPUID7_ECX_AVX512_VBMI)	$(mut, add, _String("avx512vbmi"));
		if (ecx & CPUID7_ECX_AVX512_VBMI2)	$(mut, add, _String("avx512vbmi2"));
		if (ecx & CPUID7_ECX_AVX512_BITALG)	$(mut, add, _String("avx512bitalg"));
		if (ecx & CPUID7_ECX_AVX512_VPOPCNTDQ)	$(mut, add, _String("avx512vpopcntdq"));
		if (edx & CPUID7_EDX_AVX512_FP16)	$(mut, add, _String("avx512fp16"));
		if (edx & CPUID7_EDX_AVX512_VP2INTERSECT)	$(mut, add, _String("avx512vp2intersect"));
		if (edx & CPUID7_EDX_AVX512_4VNNIW)	$(mut, add, _String("avx512_4vnniw"));
		if (edx & CPUID7_EDX_AVX512_4FMAPS)	$(mut, add, _String("avx512_4fmaps"));
	}

	if (edx & CPUID7_EDX_HYBRID)	$(mut, add, _String("hybrid"));
	if (edx & CPUID7_EDX_AMX_BF16)	$(mut, add, _String("amx_bf16"));
	if (edx & CPUID7_EDX_AMX_TILE)	$(mut, add, _String("amx_tile"));
	if (edx & CPUID7_EDX_AMX_INT8)	$(mut, add, _String("amx_int8"));
	if (edx & CPUID7_EDX_REPMOV)	$(mut, add, _String("fastrepmov"));
	if (edx & CPUID7_EDX_UINTR)	$(mut, add, _String("uintr"));
	if (edx & CPUID7_EDX_MDCLEAR)	$(mut, add, _String("mdclear"));
	if (edx & CPUID7_EDX_SERIAL)	$(mut, add, _String("serial"));
	if (edx & CPUID7_EDX_TSXLDTRK)	$(mut, add, _String("tsxldtrk"));
	if (edx & CPUID7_EDX_PCONFIG)	$(mut, add, _String("pconfig"));
	if (edx & CPUID7_EDX_ALBRS)	$(mut, add, _String("albrs"));
	if (edx & CPUID7_EDX_CET_IBT)	$(mut, add, _String("cet_ibt"));

	//-------------------
	eax = get_cpuid6_eax (); 
	if (eax & CPUID6_EAX_THERMOMETER)	$(mut, add, _String("thermometer"));
	if (eax & CPUID6_EAX_TURBOBOOST)	$(mut, add, _String("turboboost"));
	if (eax & CPUID6_EAX_ARAT)	$(mut, add, _String("arat"));
	if (eax & CPUID6_EAX_PLN)	$(mut, add, _String("pln"));
	if (eax & CPUID6_EAX_ECMD)	$(mut, add, _String("ecmd"));
	if (eax & CPUID6_EAX_PTM)	$(mut, add, _String("ptm"));
	if (eax & CPUID6_EAX_BOOSTMAX3)	$(mut, add, _String("boostmax3"));
	if (eax & CPUID6_EAX_HWP)	$(mut, add, _String("hwp"));
	if (eax & CPUID6_EAX_THREAD_DIRECTOR)	$(mut, add, _String("thread_dir"));

	//-------------------
	// CPUID eax=80000001
	//
	uint32_t ecx2 = get_cpuid_80000001_ecx ();
	if (ecx2 & CPUID80000001_ECX_LZCNT) $(mut, add, _String("lzcnt"));
	if (ecx2 & CPUID80000001_ECX_PREFETCHW) $(mut, add, _String("prefetchw"));

	uint32_t edx2 = get_cpuid_80000001_edx ();
	if (edx2 & CPUID80000001_EDX_NX) $(mut, add, _String("nx"));
	if (edx2 & CPUID80000001_EDX_MMXEXT) $(mut, add, _String("mmxext"));
	if (edx2 & CPUID80000001_EDX_INTEL64) $(mut, add, _String("intel64"));

	self->has128bitVectors = have_sse;
	self->has256bitVectors = have_avx;
	self->has512bitVectors = have_avx512;

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

	const char *ptr = address;
	asm volatile(
		"CLFLUSH (%0)\n" 
		: 
		: "r"(&ptr) 
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
