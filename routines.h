/*============================================================================
  bandwidth, a benchmark to measure memory transfer bandwidth.
  Copyright (C) 2005-2024 by Zack T Smith.

  This program is free software; you can redistribute it and/or modify
  it under the terms of the GNU General Public License as published by
  the Free Software Foundation; either version 3 of the License, or
  (at your option) any later version.

  This program is distributed in the hope that it will be useful,
  but WITHOUT ANY WARRANTY; without even the implied warranty of
  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
  GNU General Public License for more details.

  You should have received a copy of the GNU General Public License
  along with this program; if not, write to the Free Software
  Foundation, Inc., 51 Franklin St, Fifth Floor, Boston, MA  02110-1301  USA

  The author may be reached at 3 at zs3 dot me.
 *===========================================================================*/

#ifndef _ROUTINES_H
#define _ROUTINES_H

#include <stdint.h>

extern int Reader (void *ptr, unsigned long size, unsigned long loops);
extern int Reader_nontemporal (void *ptr, unsigned long size, unsigned long loops);

extern int RandomReader (void **ptr, unsigned long n_chunks, unsigned long loops);

extern int Writer (void *ptr, unsigned long size, unsigned long loops, unsigned long value);
extern int Writer_nontemporal (void *ptr, unsigned long size, unsigned long loops, unsigned long value);

extern int RegisterToRegister (unsigned long);

extern int StackReader (unsigned long);
extern int StackWriter (unsigned long);

extern int Register8ToVector128 (unsigned long);	// SSE2
extern int Register16ToVector128 (unsigned long);	// SSE2
extern int Register32ToVector128 (unsigned long);	// SSE2
extern int Register64ToVector128 (unsigned long);	// SSE2

extern int VectorToVector128 (unsigned long);	// SSE2
extern int VectorToVector256 (unsigned long);	// AVX
extern int VectorToVector512 (unsigned long);	// AVX512

extern int RegisterToVectorMove (unsigned long);// MOVD
extern int VectorToRegisterMove (unsigned long);// MOVD

extern int Vector128ToRegister8 (unsigned long);	// SSE2
extern int Vector128ToRegister16 (unsigned long);	// SSE2
extern int Vector128ToRegister32 (unsigned long);	// SSE2
extern int Vector128ToRegister64 (unsigned long);	// SSE2

extern int CopyWithMainRegisters (void*, void*, unsigned long, unsigned long);	
extern int CopyVector128 (void*, void*, unsigned long, unsigned long); 
extern int CopyVector256 (void*, void*, unsigned long, unsigned long);
extern int CopyVector512 (void*, void*, unsigned long, unsigned long);

extern int ReaderVector128 (void *ptr, unsigned long, unsigned long); // SSE2
extern int ReaderVector256 (void *ptr, unsigned long, unsigned long);
extern int ReaderVector512 (void *ptr, unsigned long, unsigned long);

extern int RandomReaderVector128 (void **ptr, unsigned long, unsigned long); // SSE2
extern int RandomReaderVector128_nontemporal (void **ptr, unsigned long, unsigned long); // SSE4

extern int RandomReaderVector256 (void **ptr, unsigned long, unsigned long);

extern int ReaderVector128_nontemporal (void *ptr, unsigned long, unsigned long); // SSE4
extern int ReaderVector256_nontemporal (void *ptr, unsigned long, unsigned long);
extern int ReaderVector512_nontemporal (void *ptr, unsigned long, unsigned long);

extern int RandomWriter (void **ptr, unsigned long size, unsigned long loops, unsigned long value);
extern int RandomWriterVector128 (void **ptr, unsigned long, unsigned long, unsigned long);
extern int RandomWriterVector256 (void **ptr, unsigned long, unsigned long, unsigned long);

extern int RandomWriterVector128_nontemporal (void **ptr, unsigned long, unsigned long, unsigned long);
extern int RandomWriterVector256_nontemporal (void **ptr, unsigned long, unsigned long, unsigned long);

extern int WriterVector128 (void *ptr, unsigned long, unsigned long, unsigned long);
extern int WriterVector256 (void *ptr, unsigned long, unsigned long, unsigned long);
extern int WriterVector512 (void *ptr, unsigned long, unsigned long, unsigned long);

extern int WriterVector128_nontemporal (void *ptr, unsigned long, unsigned long, unsigned long);
extern int WriterVector256_nontemporal (void *ptr, unsigned long, unsigned long, unsigned long);
extern int WriterVector512_nontemporal (void *ptr, unsigned long, unsigned long, unsigned long);

extern void IncrementRegisters (unsigned long count);
extern void IncrementStack (unsigned long count);

extern int RowHammerTest (unsigned long *ptr, unsigned long length, unsigned nCycles);

//-----------------------------------
// utility*asm routines and constants
//
extern void get_cpuid_family (char *family_return); // for 32bit
extern unsigned get_cpuid_family1 (); // for 64-bit
extern unsigned get_cpuid_family2 (); // for 64-bit
extern unsigned get_cpuid_family3 (); // for 64-bit
extern void get_cpuid_cache_info (uint32_t *array, int index);
extern unsigned get_cpuid1_ecx ();
extern unsigned get_cpuid1_edx ();
extern unsigned get_cpuid7_ebx ();
extern unsigned get_cpuid7_ecx ();
extern unsigned get_cpuid7_edx ();
extern unsigned get_cpuid8_ecx ();
extern unsigned get_cpuid_80000001_ecx ();
extern unsigned get_cpuid_80000001_edx ();

#define CPUID80000001_EDX_INTEL64 (1<<29)	// "Long Mode" on AMD.
#define CPUID80000001_ECX_SSE4A (1<<6)
#define CPUID80000001_EDX_NX (1<<20)
#define CPUID80000001_EDX_MMXEXT (1<<22)

#define CPUID1_EDX_MMX (1<<23)
#define CPUID1_EDX_SSE (1<<25)
#define CPUID1_EDX_SSE2 (1<<26)
#define CPUID1_ECX_SSE3 (1)
#define CPUID1_ECX_SSSE3 (1<<9)
#define CPUID1_ECX_SSE41 (1<<19)
#define CPUID1_ECX_SSE42 (1<<20)
#define CPUID1_ECX_AES (1<<25)	// Encryption.
#define CPUID1_ECX_AVX (1<<28)	// 256-bit YMM registers.
#define CPUID1_ECX_HYPER_GUEST (1<<31)
#define CPUID1_EDX_HTT (1<<28)

#define CPUID7_EBX_ADX (1<<19)
#define CPUID7_EBX_SHA (1<<29)
#define CPUID7_EBX_SGX (1<<2)
#define CPUID7_EBX_AVX2 (1<<5)
#define CPUID7_EBX_BMI1 (1<<3)
#define CPUID7_EBX_BMI2 (1<<8)

#define CPUID7_ECX_CET (1<<7)

#define CPUID7_EBX_AVX512_F (1<<16)
#define CPUID7_EBX_AVX512_DQ (1<<17)
#define CPUID7_EBX_AVX512_IFMA (1<<21)
#define CPUID7_EBX_AVX512_PF (1<<26)
#define CPUID7_EBX_AVX512_ER (1<<27)
#define CPUID7_EBX_AVX512_CD (1<<28)
#define CPUID7_EBX_AVX512_BW (1<<30)
#define CPUID7_EBX_AVX512_VL (1<<31)

#define CPUID7_ECX_AVX512_VBMI (1<<1)
#define CPUID7_ECX_AVX512_VBMI2 (1<<6)
#define CPUID7_ECX_AVX512_VNNI (1<<11)
#define CPUID7_ECX_AVX512_BITALG (1<<12)
#define CPUID7_ECX_AVX512_VPOPCNTDQ (1<<14)

#define CPUID7_EDX_AVX512_4VNNIW (1<<2)
#define CPUID7_EDX_AVX512_4FMAPS (1<<3)
#define CPUID7_EDX_AVX512_VP2INTERSECT (1<<8)
#define CPUID7_EDX_AVX512_FP16 (1<<23)

#endif

