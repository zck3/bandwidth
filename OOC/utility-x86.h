//===========================================================================
// Object-Oriented C (OOC)
// Copyright (C) 2005-2024 by Zack T Smith.
//
// This program is free software; you can redistribute it and/or modify
// it under the terms of the GNU Lesser General Public License version 3 as published by
// the Free Software Foundation.
//
// This program is distributed in the hope that it will be useful,
// but WITHOUT ANY WARRANTY; without even the implied warranty of
// MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
// GNU Lesser General Public License for more details.
//
// You should have received a copy of the GNU Lesser General Public License
// along with this program; if not, write to the Free Software
// Foundation, Inc., 51 Franklin St, Fifth Floor, Boston, MA  02110-1301  USA
//
// The author may be reached at 3 at zs3 dot me.
//============================================================================

// This file derives from my benchmark called bandwidth.

#ifndef _UTILITY_x86_H
#define _UTILITY_x86_H

//----------------------------------------
// utility-x86*.asm routines and constants
//
extern unsigned get_cpuid_cache_info (uint32_t *, unsigned);

extern unsigned get_cpuid_TLB_info_18h (uint32_t *, unsigned subleaf);
extern unsigned get_cpuid_TLB_info_02h (uint32_t *);

extern void get_cpuid_family (char *family_return); // for 32bit
extern unsigned get_cpuid_family1 (); // for 64-bit
extern unsigned get_cpuid_family2 (); // for 64-bit
extern unsigned get_cpuid_family3 (); // for 64-bit

extern void get_cpuid_model (char *model_return); // returns 48 bytes

extern unsigned get_cpuid1_ebx ();
extern unsigned get_cpuid1_ecx ();
extern unsigned get_cpuid1_edx ();

extern unsigned get_cpuid6_eax ();

extern unsigned get_cpuid7_eax ();
extern unsigned get_cpuid7_ebx ();
extern unsigned get_cpuid7_ecx ();
extern unsigned get_cpuid7_edx ();
extern unsigned get_cpuid8_ecx ();
extern unsigned get_cpuid_80000001_ecx ();
extern unsigned get_cpuid_80000001_edx ();

#define CPUID80000001_ECX_SSE4A (1<<6)
#define CPUID80000001_ECX_LZCNT (1<<5)
#define CPUID80000001_ECX_PREFETCHW (1<<8)
#define CPUID80000001_EDX_INTEL64 (1<<29)	// "Long Mode" on AMD.
#define CPUID80000001_EDX_NX (1<<20)
#define CPUID80000001_EDX_MMXEXT (1<<22)

#define CPUID1_EDX_FPU (1<<0)
#define CPUID1_EDX_VME (1<<1)
#define CPUID1_EDX_DE (1<<2)
#define CPUID1_EDX_PSE (1<<3)
#define CPUID1_EDX_TSC (1<<4)
#define CPUID1_EDX_MSR (1<<5)
#define CPUID1_EDX_PAE (1<<6)
#define CPUID1_EDX_MCE (1<<7)
#define CPUID1_EDX_CX8 (1<<8)
#define CPUID1_EDX_APIC (1<<9)
#define CPUID1_EDX_SEP (1<<11)
#define CPUID1_EDX_MTRR (1<<12)
#define CPUID1_EDX_PGE (1<<13)
#define CPUID1_EDX_MCA (1<<14) 
#define CPUID1_EDX_CMOV (1<<15)
#define CPUID1_EDX_PAT (1<<16)
#define CPUID1_EDX_PSE36 (1<<17)
#define CPUID1_EDX_PSN (1<<18)
#define CPUID1_EDX_CLFSH (1<<19)
#define CPUID1_EDX_DS (1<<21)
#define CPUID1_EDX_ACPI (1<<22)
#define CPUID1_EDX_MMX (1<<23)
#define CPUID1_EDX_FXSR (1<<24)
#define CPUID1_EDX_SSE (1<<25)
#define CPUID1_EDX_SSE2 (1<<26)
#define CPUID1_EDX_SS (1<<27)
#define CPUID1_EDX_HTT (1<<28)
#define CPUID1_EDX_TM (1<<29)
#define CPUID1_EDX_PBE (1<<31)

#define CPUID1_ECX_SSE3 (1<<0)
#define CPUID1_ECX_PCLMULQDQ (1<<1)
#define CPUID1_ECX_DTES64 (1<<2)
#define CPUID1_ECX_MONITOR (1<<3)
#define CPUID1_ECX_DS_CPL (1<<4)
#define CPUID1_ECX_VMX (1<<5)
#define CPUID1_ECX_SMX (1<<6)
#define CPUID1_ECX_EIST (1<<7)
#define CPUID1_ECX_TM2 (1<<8)
#define CPUID1_ECX_SSSE3 (1<<9)
#define CPUID1_ECX_CNXTID (1<<10)
#define CPUID1_ECX_SDBG (1<<11)
#define CPUID1_ECX_FMA (1<<12)
#define CPUID1_ECX_CMPXCHG16B (1<<13)
#define CPUID1_ECX_XTPR (1<<14)
#define CPUID1_ECX_PDCM (1<<15)
#define CPUID1_ECX_PCID (1<<17)
#define CPUID1_ECX_DCA (1<<18)
#define CPUID1_ECX_SSE41 (1<<19)
#define CPUID1_ECX_SSE42 (1<<20)
#define CPUID1_ECX_X2APIC (1<<21)
#define CPUID1_ECX_MOVBE (1<<22)
#define CPUID1_ECX_POPCNT (1<<23)
#define CPUID1_ECX_AESNI (1<<25)	// Encryption.
#define CPUID1_ECX_XSAVE (1<<26)
#define CPUID1_ECX_OSXSAVE (1<<27)
#define CPUID1_ECX_AVX (1<<28)
#define CPUID1_ECX_F16C (1<<29)
#define CPUID1_ECX_RDRAND (1<<30)
#define CPUID1_ECX_HYPER_GUEST (1<<31)

#define CPUID7_EBX_SGX (1<<2)
#define CPUID7_EBX_BMI1 (1<<3)
#define CPUID7_EBX_HLE (1<<4)
#define CPUID7_EBX_AVX2 (1<<5)
#define CPUID7_EBX_SMEP (1<<7)
#define CPUID7_EBX_BMI2 (1<<8)
#define CPUID7_EBX_REP_MOVSB (1<<9)
#define CPUID7_EBX_INVPCID (1<<10)
#define CPUID7_EBX_RTM (1<<11)
#define CPUID7_EBX_MPX (1<<14)
#define CPUID7_EBX_AVX512_F (1<<16)
#define CPUID7_EBX_AVX512_DQ (1<<17)
#define CPUID7_EBX_RDSEED (1<<18)
#define CPUID7_EBX_ADX (1<<19)
#define CPUID7_EBX_SMAP (1<<20)
#define CPUID7_EBX_AVX512_IFMA (1<<21)
#define CPUID7_EBX_CLFLUSHOPT (1<<23)
#define CPUID7_EBX_CLWB (1<<24)
#define CPUID7_EBX_PROC_TRACE (1<<25)
#define CPUID7_EBX_AVX512_PF (1<<26)
#define CPUID7_EBX_AVX512_ER (1<<27)
#define CPUID7_EBX_AVX512_CD (1<<28)
#define CPUID7_EBX_SHA (1<<29)
#define CPUID7_EBX_AVX512_BW (1<<30)
#define CPUID7_EBX_AVX512_VL (1<<31)

#define CPUID7_ECX_AVX512_BITALG (1<<12)
#define CPUID7_ECX_AVX512_VBMI (1<<1)
#define CPUID7_ECX_AVX512_VBMI2 (1<<6)
#define CPUID7_ECX_AVX512_VNNI (1<<11)
#define CPUID7_ECX_AVX512_VPOPCNTDQ (1<<14)
#define CPUID7_ECX_CET (1<<7)
#define CPUID7_ECX_CLDEMOTE (1<<25)
#define CPUID7_ECX_ENQCMD (1<<29)
#define CPUID7_ECX_GFNI (1<<8)
#define CPUID7_ECX_KL (1<<23)
#define CPUID7_ECX_LA57 (1<<16)
#define CPUID7_ECX_MOVDIR64B (1<<28)
#define CPUID7_ECX_MOVDIRI (1<<27)
#define CPUID7_ECX_PKS (1<<31)
#define CPUID7_ECX_PKU (1<<3)
#define CPUID7_ECX_RDPID (1<<22)
#define CPUID7_ECX_UMIP (1<<2)
#define CPUID7_ECX_VAES (1<<9)
#define CPUID7_ECX_VPCLMULQDQ (1<<10)
#define CPUID7_ECX_WAITPKG (1<<5)

#define CPUID7_ECX1_EAX_SHA512 (1)
#define CPUID7_ECX1_EAX_AMX_FP16 (1<<21)

#define CPUID7_EDX_AVX512_4VNNIW (1<<2)
#define CPUID7_EDX_AVX512_4FMAPS (1<<3)
#define CPUID7_EDX_AVX512_VP2INTERSECT (1<<8)
#define CPUID7_EDX_REPMOV (1<<4)
#define CPUID7_EDX_UINTR (1<<5)
#define CPUID7_EDX_MDCLEAR (1<<10)
#define CPUID7_EDX_SERIAL (1<<14)
#define CPUID7_EDX_HYBRID (1<<15)
#define CPUID7_EDX_TSXLDTRK (1<<16)
#define CPUID7_EDX_PCONFIG (1<<18)
#define CPUID7_EDX_ALBRS (1<<19)
#define CPUID7_EDX_CET_IBT (1<<20)
#define CPUID7_EDX_AMX_BF16 (1<<22)
#define CPUID7_EDX_AMX_TILE (1<<24)
#define CPUID7_EDX_AMX_INT8 (1<<25)
#define CPUID7_EDX_AVX512_FP16 (1<<23)

#define CPUID6_EAX_THERMOMETER (1<<0)
#define CPUID6_EAX_TURBOBOOST (1<<1)
#define CPUID6_EAX_ARAT (1<<2)
#define CPUID6_EAX_PLN (1<<4)
#define CPUID6_EAX_ECMD (1<<5)
#define CPUID6_EAX_PTM (1<<6)
#define CPUID6_EAX_BOOSTMAX3 (1<<14)
#define CPUID6_EAX_HWP (1<<15)
#define CPUID6_EAX_THREAD_DIRECTOR (1<<23)

#endif

