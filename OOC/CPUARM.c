/*============================================================================
  CPUARM, an object-oriented C ARM CPU class.
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

#if defined(__arm__) || defined(__aarch64__)

#include "CPUARM.h"
#include "FileSystem.h"
#include "Utility.h"

#include <stdlib.h>

#if defined(__linux__) 
#include <sys/auxv.h>	// getauxval
#include <sys/mman.h>	// madvise
#endif

#if defined(__CYGWIN__)
#include <sys/mman.h>	// madvise
#endif

#if defined(__APPLE__)
#include <sys/sysctl.h>
#endif

CPUARMClass *_CPUARMClass = NULL;

void CPUARM_destroy (Any *self)
{
	DEBUG_DESTROY;
	if (!self) {
		return;
	}
	verifyCorrectClassOrSubclass(self,CPUARM);

	CPU_destroy((CPU*)self);
}

static void CPUARM_print (CPUARM* restrict self, FILE *outputFile) 
{ 
	if (!self) {
		return;
	}
	verifyCorrectClass(self,CPUARM);

	if (!outputFile) {
		outputFile = stdout;
	}
}

static void CPUARM_describe (CPUARM* restrict self, FILE *outputFile) 
{ 
	if (!self) {
		return;
	}
	verifyCorrectClass(self,CPUARM);

	if (!outputFile) {
		outputFile = stdout;
	}

	fprintf (outputFile, "%s", $(self, className));
}

static String* CPUARM_model (CPUARM* restrict self) 
{
	if (!self) {
		return NULL;
	}
	verifyCorrectClassOrSubclass(self,CPU);

#if defined(__ANDROID__)
	char *model = execute_and_return_first_line ("getprop ro.soc.model | sed 's/\\[//' | sed 's/\\]//'");
	if (model) {
		return String_withCString (model);
	}
#endif
 
#if defined(__linux__)
	char *hardware = execute_and_return_first_line ("grep -i '^Hardware.*:' /proc/cpuinfo | sed 's/^.*: //' | tail -1 | sed 's/, Inc//' | sed 's/,//g'");
	if (hardware) {
		return String_withCString (hardware);
	}
#endif

#ifdef __APPLE__
	char name[48] = {0};
	size_t size = sizeof(name);
	if (!sysctlbyname("machdep.cpu.brand_string", name, &size, NULL, 0)) {
		if (name[0]) {
			name[size] = 0;
			return String_withCString (name);
		}
	}
#endif

	return NULL;
}

static MutableSet* CPUARM_features (CPUARM* restrict self)
{
	if (!self) {
		return NULL;
	}
	verifyCorrectClass(self,CPUARM);

	MutableSet *mut = self->features;
	if (!$(mut, isEmpty)) {
		return mut;
	}

#if defined(__linux__)
	unsigned long hwcap = getauxval(AT_HWCAP);
	unsigned long hwcap2 = getauxval(AT_HWCAP2);

#ifdef __aarch64__
//------------------------------------------
// Note, arm64 hardware capabilities overlap
// with but are different from arm32.
//------------------------------------------
    #ifdef HWCAP_FP
	if (hwcap & HWCAP_FP) {
		$(mut, addCString, "fp");
	}
    #endif
    #ifdef HWCAP_ASIMD
	if (hwcap & HWCAP_ASIMD) {
		self->has128bitVectors = true;
		$(mut, addCString, "asimd");
	}
    #endif
    #ifdef HWCAP_EVTSTRM
	if (hwcap & HWCAP_EVTSTRM) {
		$(mut, addCString, "evtstrm");
	}
    #endif
    #ifdef HWCAP_AES
	if (hwcap & HWCAP_AES) {
		$(mut, addCString, "aes");
	}
    #endif
    #ifdef HWCAP_PMULL
	if (hwcap & HWCAP_PMULL) {
		$(mut, addCString, "pmull");
	}
    #endif
    #ifdef HWCAP_SHA1
	if (hwcap & HWCAP_SHA1) {
		$(mut, addCString, "sha1");
	}
    #endif
    #ifdef HWCAP_SHA2
	if (hwcap & HWCAP_SHA2) {
		$(mut, addCString, "sha2");
	}
    #endif
    #ifdef HWCAP_CRC32
	if (hwcap & HWCAP_CRC32) {
		$(mut, addCString, "crc32");
	}
    #endif
    #ifdef HWCAP_ATOMICS
	if (hwcap & HWCAP_ATOMICS) {
		$(mut, addCString, "atomics");
	}
    #endif
    #ifdef HWCAP_FPHP
	if (hwcap & HWCAP_FPHP) {
		$(mut, addCString, "fphp");
	}
    #endif
    #ifdef HWCAP_ASIMDHP
	if (hwcap & HWCAP_ASIMDHP) {
		$(mut, addCString, "asimdhp");
	}
    #endif
    #ifdef HWCAP_CPUID
	if (hwcap & HWCAP_CPUID) {
		$(mut, addCString, "cpuid");
	}
    #endif
    #ifdef HWCAP_ASIMDRDM
	if (hwcap & HWCAP_ASIMDRDM) {
		$(mut, addCString, "asimdrdm");
	}
    #endif
    #ifdef HWCAP_JSCVT
	if (hwcap & HWCAP_JSCVT) {
		$(mut, addCString, "jscvt");
	}
    #endif
    #ifdef HWCAP_FCMA
	if (hwcap & HWCAP_FCMA) {
		$(mut, addCString, "fcma");
	}
    #endif
    #ifdef HWCAP_LRCPC
	if (hwcap & HWCAP_LRCPC) {
		$(mut, addCString, "lrcpc");
	}
    #endif
    #ifdef HWCAP_DCPOP
	if (hwcap & HWCAP_DCPOP) {
		$(mut, addCString, "dcpop");
	}
    #endif
    #ifdef HWCAP_SHA3
	if (hwcap & HWCAP_SHA3) {
		$(mut, addCString, "sha3");
	}
    #endif
    #ifdef HWCAP_SM3
	if (hwcap & HWCAP_SM3) {
		$(mut, addCString, "sm3");
	}
    #endif
    #ifdef HWCAP_SM4
	if (hwcap & HWCAP_SM4) {
		$(mut, addCString, "sm4");
	}
    #endif
    #ifdef HWCAP_ASIMDDP
	if (hwcap & HWCAP_ASIMDDP) {
		$(mut, addCString, "asimddp");
	}
    #endif
    #ifdef HWCAP_SHA512
	if (hwcap & HWCAP_SHA512) {
		$(mut, addCString, "sha512");
	}
    #endif
    #ifdef HWCAP_SVE
	if (hwcap & HWCAP_SVE) {
		$(mut, addCString, "sve");
		self->has128bitCacheBypass = true;
	}
    #endif
    #ifdef HWCAP_ASIMDFHM
	if (hwcap & HWCAP_ASIMDFHM) {
		$(mut, addCString, "asimdfhm");
	}
    #endif
    #ifdef HWCAP_DIT
	if (hwcap & HWCAP_DIT) {
		$(mut, addCString, "dit");
	}
    #endif
    #ifdef HWCAP_USCAT
	if (hwcap & HWCAP_USCAT) {
		$(mut, addCString, "uscat");
	}
    #endif
    #ifdef HWCAP_ILRCPC
	if (hwcap & HWCAP_ILRCPC) {
		$(mut, addCString, "ilrcpc");
	}
    #endif
    #ifdef HWCAP_FLAGM
	if (hwcap & HWCAP_FLAGM) {
		$(mut, addCString, "flagm");
	}
    #endif
    #ifdef HWCAP_SSBS
	if (hwcap & HWCAP_SSBS) {
		$(mut, addCString, "ssbs");
	}
    #endif
    #ifdef HWCAP_SB
	if (hwcap & HWCAP_SB) {
		$(mut, addCString, "sb");
	}
    #endif
    #ifdef HWCAP_PACA
	if (hwcap & HWCAP_PACA) {
		$(mut, addCString, "paca");
	}
    #endif
    #ifdef HWCAP_PACG
	if (hwcap & HWCAP_PACG) {
		$(mut, addCString, "pacg");
	}
    #endif
    #ifdef HWCAP_GCS
	if (hwcap & HWCAP_GCS) {
		$(mut, addCString, "gcs");
	}
    #endif
    #ifdef HWCAP_CMPBR
	if (hwcap & HWCAP_CMPBR) {
		$(mut, addCString, "cmpbr");
	}
    #endif
    #ifdef HWCAP_F8MM4
	if (hwcap & HWCAP_F8MM4) {
		$(mut, addCString, "f8mm4");
	}
    #endif
    #ifdef HWCAP_F8MM8
	if (hwcap & HWCAP_F8MM8) {
		$(mut, addCString, "f8mm8");
	}
    #endif
    #ifdef HWCAP_FPRCVT
	if (hwcap & HWCAP_FPRCVT) {
		$(mut, addCString, "fprcvt");
	}
    #endif
    #ifdef HWCAP_SME2P2
	if (hwcap & HWCAP_SME2P2) {
		$(mut, addCString, "sme2p2");
	}
    #endif
    #ifdef HWCAP_SME_AES
	if (hwcap & HWCAP_SME_AES) {
		$(mut, addCString, "sme_aes");
	}
    #endif
    #ifdef HWCAP_SME_SBITPERM
	if (hwcap & HWCAP_SME_SBITPERM) {
		$(mut, addCString, "sme_sbitperm");
	}
    #endif
    #ifdef HWCAP_SME_SFEXPA
	if (hwcap & HWCAP_SME_SFEXPA) {
		$(mut, addCString, "sme_sfexpa");
	}
    #endif
    #ifdef HWCAP_SME_SMOP4
	if (hwcap & HWCAP_SME_SMOP4) {
		$(mut, addCString, "sme_smop4");
	}
    #endif
    #ifdef HWCAP_SME_STMOP
	if (hwcap & HWCAP_SME_STMOP) {
		$(mut, addCString, "sme_stmop");
	}
    #endif
    #ifdef HWCAP_SVE2P2
	if (hwcap & HWCAP_SVE2P2) {
		$(mut, addCString, "sve2p2");
	}
    #endif
    #ifdef HWCAP_SVE_AES2
	if (hwcap & HWCAP_SVE_AES2) {
		$(mut, addCString, "sve_aes2");
	}
    #endif
    #ifdef HWCAP_SVE_BFSCALE
	if (hwcap & HWCAP_SVE_BFSCALE) {
		$(mut, addCString, "sve_bfscale");
	}
    #endif
    #ifdef HWCAP_SVE_ELTPERM
	if (hwcap & HWCAP_SVE_ELTPERM) {
		$(mut, addCString, "sve_eltperm");
	}
    #endif
    #ifdef HWCAP_SVE_F16MM
	if (hwcap & HWCAP_SVE_F16MM) {
		$(mut, addCString, "sve_f16mm");
	}
    #endif

    #ifdef HWCAP2_AFP
	if (hwcap2 & HWCAP2_AFP) {
		$(mut, addCString, "afp");
	}
    #endif
    #ifdef HWCAP2_BF16
	if (hwcap2 & HWCAP2_BF16) {
		$(mut, addCString, "bf16");
	}
    #endif
    #ifdef HWCAP2_BTI
	if (hwcap2 & HWCAP2_BTI) {
		$(mut, addCString, "bti");
	}
    #endif
    #ifdef HWCAP2_CSSC
	if (hwcap2 & HWCAP2_CSSC) {
		$(mut, addCString, "cssc");
	}
    #endif
    #ifdef HWCAP2_DCPODP
	if (hwcap2 & HWCAP2_DCPODP) {
		$(mut, addCString, "dcpodp");
	}
    #endif
    #ifdef HWCAP2_DGH
	if (hwcap2 & HWCAP2_DGH) {
		$(mut, addCString, "dgh");
	}
    #endif
    #ifdef HWCAP2_EBF16
	if (hwcap2 & HWCAP2_EBF16) {
		$(mut, addCString, "ebf16");
	}
    #endif
    #ifdef HWCAP2_ECV
	if (hwcap2 & HWCAP2_ECV) {
		$(mut, addCString, "ecv");
	}
    #endif
    #ifdef HWCAP2_F8CVT
	if (hwcap2 & HWCAP2_F8CVT) {
		$(mut, addCString, "f8cvt");
	}
    #endif
    #ifdef HWCAP2_F8DP2
	if (hwcap2 & HWCAP2_F8DP2) {
		$(mut, addCString, "f8dp2");
	}
    #endif
    #ifdef HWCAP2_F8DP4
	if (hwcap2 & HWCAP2_F8DP4) {
		$(mut, addCString, "f8dp4");
	}
    #endif
    #ifdef HWCAP2_F8E4M3
	if (hwcap2 & HWCAP2_F8E4M3) {
		$(mut, addCString, "f8e4m3");
	}
    #endif
    #ifdef HWCAP2_F8E5M2
	if (hwcap2 & HWCAP2_F8E5M2) {
		$(mut, addCString, "f8e5m2");
	}
    #endif
    #ifdef HWCAP2_F8FMA
	if (hwcap2 & HWCAP2_F8FMA) {
		$(mut, addCString, "f8fma");
	}
    #endif
    #ifdef HWCAP2_FAMINMAX
	if (hwcap2 & HWCAP2_FAMINMAX) {
		$(mut, addCString, "faminmax");
	}
    #endif
    #ifdef HWCAP2_FLAGM2
	if (hwcap2 & HWCAP2_FLAGM2) {
		$(mut, addCString, "flagm2");
	}
    #endif
    #ifdef HWCAP2_FPMR
	if (hwcap2 & HWCAP2_FPMR) {
		$(mut, addCString, "fpmr");
	}
    #endif
    #ifdef HWCAP2_FRINT
	if (hwcap2 & HWCAP2_FRINT) {
		$(mut, addCString, "frint");
	}
    #endif
    #ifdef HWCAP2_HBC
	if (hwcap2 & HWCAP2_HBC) {
		$(mut, addCString, "hbc");
	}
    #endif
    #ifdef HWCAP2_I8MM
	if (hwcap2 & HWCAP2_I8MM) {
		$(mut, addCString, "i8mm");
	}
    #endif
    #ifdef HWCAP2_LRCPC3
	if (hwcap2 & HWCAP2_LRCPC3) {
		$(mut, addCString, "lrcpc3");
	}
    #endif
    #ifdef HWCAP2_LSE128
	if (hwcap2 & HWCAP2_LSE128) {
		$(mut, addCString, "lse128");
	}
    #endif
    #ifdef HWCAP2_LUT
	if (hwcap2 & HWCAP2_LUT) {
		$(mut, addCString, "lut");
	}
    #endif
    #ifdef HWCAP2_MOPS
	if (hwcap2 & HWCAP2_MOPS) {
		$(mut, addCString, "mops");
	}
    #endif
    #ifdef HWCAP2_MTE
	if (hwcap2 & HWCAP2_MTE) {
		$(mut, addCString, "mte");
	}
    #endif
    #ifdef HWCAP2_MTE3
	if (hwcap2 & HWCAP2_MTE3) {
		$(mut, addCString, "mte3");
	}
    #endif
    #ifdef HWCAP2_POE
	if (hwcap2 & HWCAP2_POE) {
		$(mut, addCString, "poe");
	}
    #endif
    #ifdef HWCAP2_RNG
	if (hwcap2 & HWCAP2_RNG) {
		$(mut, addCString, "rng");
	}
    #endif
    #ifdef HWCAP2_RPRES
	if (hwcap2 & HWCAP2_RPRES) {
		$(mut, addCString, "rpres");
	}
    #endif
    #ifdef HWCAP2_RPRFM
	if (hwcap2 & HWCAP2_RPRFM) {
		$(mut, addCString, "rprfm");
	}
    #endif
    #ifdef HWCAP2_SME
	if (hwcap2 & HWCAP2_SME) {
		$(mut, addCString, "sme");
	}
    #endif
    #ifdef HWCAP2_SME2
	if (hwcap2 & HWCAP2_SME2) {
		$(mut, addCString, "sme2");
	}
    #endif
    #ifdef HWCAP2_SME2P1
	if (hwcap2 & HWCAP2_SME2P1) {
		$(mut, addCString, "sme2p1");
	}
    #endif
    #ifdef HWCAP2_SMEB16B16
	if (hwcap2 & HWCAP2_SMEB16B16) {
		$(mut, addCString, "smeb16b16");
	}
    #endif
    #ifdef HWCAP2_SMEBI32I32
	if (hwcap2 & HWCAP2_SMEBI32I32) {
		$(mut, addCString, "smebi32i32");
	}
    #endif
    #ifdef HWCAP2_SMEF16F16
	if (hwcap2 & HWCAP2_SMEF16F16) {
		$(mut, addCString, "smef16f16");
	}
    #endif
    #ifdef HWCAP2_SMEI16I32
	if (hwcap2 & HWCAP2_SMEI16I32) {
		$(mut, addCString, "smei16i32");
	}
    #endif
    #ifdef HWCAP2_SME_F8F16
	if (hwcap2 & HWCAP2_SME_F8F16) {
		$(mut, addCString, "sme_f8f16");
	}
    #endif
    #ifdef HWCAP2_SME_F8F32
	if (hwcap2 & HWCAP2_SME_F8F32) {
		$(mut, addCString, "sme_f8f32");
	}
    #endif
    #ifdef HWCAP2_SME_FA64
	if (hwcap2 & HWCAP2_SME_FA64) {
		$(mut, addCString, "sme_fa64");
	}
    #endif
    #ifdef HWCAP2_SME_LUTV2
	if (hwcap2 & HWCAP2_SME_LUTV2) {
		$(mut, addCString, "sme_lutv2");
	}
    #endif
    #ifdef HWCAP2_SME_SF8DP2
	if (hwcap2 & HWCAP2_SME_SF8DP2) {
		$(mut, addCString, "sme_sf8dp2");
	}
    #endif
    #ifdef HWCAP2_SME_SF8DP4
	if (hwcap2 & HWCAP2_SME_SF8DP4) {
		$(mut, addCString, "sme_sf8dp4");
	}
    #endif
    #ifdef HWCAP2_SME_SF8DP4
	if (hwcap2 & HWCAP2_SME_SF8DP4) {
		$(mut, addCString, "sme_sf8dp4");
	}
    #endif
    #ifdef HWCAP2_SME_SF8FMA
	if (hwcap2 & HWCAP2_SME_SF8FMA) {
		$(mut, addCString, "sme_sf8fma");
	}
    #endif
    #ifdef HWCAP2_SVE2
	if (hwcap2 & HWCAP2_SVE2) {
		$(mut, addCString, "sve2");
	}
    #endif
    #ifdef HWCAP2_SVE2P1
	if (hwcap2 & HWCAP2_SVE2P1) {
		$(mut, addCString, "sve2p1");
	}
    #endif
    #ifdef HWCAP2_SVEAES
	if (hwcap2 & HWCAP2_SVEAES) {
		$(mut, addCString, "sveaes");
	}
    #endif
    #ifdef HWCAP2_SVEBF16
	if (hwcap2 & HWCAP2_SVEBF16) {
		$(mut, addCString, "svebf16");
	}
    #endif
    #ifdef HWCAP2_SVEBITPERM
	if (hwcap2 & HWCAP2_SVEBITPERM) {
		$(mut, addCString, "svebitperm");
	}
    #endif
    #ifdef HWCAP2_SVEF32MM
	if (hwcap2 & HWCAP2_SVEF32MM) {
		$(mut, addCString, "svef32mm");
	}
    #endif
    #ifdef HWCAP2_SVEF64MM
	if (hwcap2 & HWCAP2_SVEF64MM) {
		$(mut, addCString, "svef64mm");
	}
    #endif
    #ifdef HWCAP2_SVEI8MM
	if (hwcap2 & HWCAP2_SVEI8MM) {
		$(mut, addCString, "svei8mm");
	}
    #endif
    #ifdef HWCAP2_SVEPMULL
	if (hwcap2 & HWCAP2_SVEPMULL) {
		$(mut, addCString, "svepmull");
	}
    #endif
    #ifdef HWCAP2_SVESHA3
	if (hwcap2 & HWCAP2_SVESHA3) {
		$(mut, addCString, "svesha3");
	}
    #endif
    #ifdef HWCAP2_SVESM4
	if (hwcap2 & HWCAP2_SVESM4) {
		$(mut, addCString, "svesm4");
	}
    #endif
    #ifdef HWCAP2_SVE_B16B16
	if (hwcap2 & HWCAP2_SVE_B16B16) {
		$(mut, addCString, "sve_b16b16");
	}
    #endif
    #ifdef HWCAP2_SVE_EBF16
	if (hwcap2 & HWCAP2_SVE_EBF16) {
		$(mut, addCString, "sve_ebf16");
	}
    #endif
    #ifdef HWCAP2_WFXT
	if (hwcap2 & HWCAP2_WFXT) {
		$(mut, addCString, "wfxt");
	}
    #endif
    #ifdef HWCAP2_SME_I16I64
	if (hwcap2 & HWCAP2_SME_I16I64) {
		$(mut, addCString, "sme_i16i64");
	}
    #endif
    #ifdef HWCAP2_SME_F64F64
	if (hwcap2 & HWCAP2_SME_F64F64) {
		$(mut, addCString, "sme_f64f64");
	}
    #endif
    #ifdef HWCAP2_SME_I8I32
	if (hwcap2 & HWCAP2_SME_I8I32) {
		$(mut, addCString, "sme_i8i32");
	}
    #endif
    #ifdef HWCAP2_SME_F16F32
	if (hwcap2 & HWCAP2_SME_F16F32) {
		$(mut, addCString, "sme_f16f32");
	}
    #endif
    #ifdef HWCAP2_SME_B16F32
	if (hwcap2 & HWCAP2_SME_B16F32) {
		$(mut, addCString, "sme_b16f32");
	}
    #endif
    #ifdef HWCAP2_SME_F32F32
	if (hwcap2 & HWCAP2_SME_F32F32) {
		$(mut, addCString, "sme_f32f32");
	}
    #endif
    #ifdef HWCAP2_SME_I16I32
	if (hwcap2 & HWCAP2_SME_I16I32) {
		$(mut, addCString, "sme_i16i32");
	}
    #endif
    #ifdef HWCAP2_SME_BI32I32
	if (hwcap2 & HWCAP2_SME_BI32I32) {
		$(mut, addCString, "sme_bi32i32");
	}
    #endif
    #ifdef HWCAP2_SME_B16B16
	if (hwcap2 & HWCAP2_SME_B16B16) {
		$(mut, addCString, "sme_b16b16");
	}
    #endif
    #ifdef HWCAP2_SME_F16F16
	if (hwcap2 & HWCAP2_SME_F16F16) {
		$(mut, addCString, "sme_f16f16");
	}
    #endif
#elif defined(__arm__)
//-----------------------------
// arm32 hardware capabilities:
//-----------------------------
    #ifdef HWCAP_26BIT
	if (hwcap & HWCAP_26BIT) {
		$(mut, addCString, "26bit");
	}
    #endif
    #ifdef HWCAP_ASIMDBF16
	if (hwcap & HWCAP_ASIMDBF16) {
		$(mut, addCString, "asimdbf16");
	}
    #endif
    #ifdef HWCAP_ASIMDDP
	if (hwcap & HWCAP_ASIMDDP) {
		$(mut, addCString, "asimddp");
		self->has128bitVectors = true;
	}
    #endif
    #ifdef HWCAP_ASIMDFHM
	if (hwcap & HWCAP_ASIMDFHM) {
		$(mut, addCString, "asimdfhm");
		self->has128bitVectors = true;
	}
    #endif
    #ifdef HWCAP_ASIMDHP
	if (hwcap & HWCAP_ASIMDHP) {
		$(mut, addCString, "asimdhp");
		self->has128bitVectors = true;
	}
    #endif
    #ifdef HWCAP_CRUNCH
	if (hwcap & HWCAP_CRUNCH) {
		$(mut, addCString, "crunch");
	}
    #endif
    #ifdef HWCAP_EDSP
	if (hwcap & HWCAP_EDSP) {
		$(mut, addCString, "edsp");
	}
    #endif
    #ifdef HWCAP_EVTSTRM
	if (hwcap & HWCAP_EVTSTRM) {
		$(mut, addCString, "evtstrm");
	}
    #endif
    #ifdef HWCAP_FAST_MULT
	if (hwcap & HWCAP_FAST_MULT) {
		$(mut, addCString, "fast_mult");
	}
    #endif
    #ifdef HWCAP_FPA
	if (hwcap & HWCAP_FPA) {
		$(mut, addCString, "fpa");
	}
    #endif
    #ifdef HWCAP_FPHP
	if (hwcap & HWCAP_FPHP) {
		$(mut, addCString, "fphp");
	}
    #endif
    #ifdef HWCAP_HALF
	if (hwcap & HWCAP_HALF) {
		$(mut, addCString, "half");
	}
    #endif
    #ifdef HWCAP_I8MM
	if (hwcap & HWCAP_I8MM) {
		$(mut, addCString, "i8mm");
	}
    #endif
    #ifdef HWCAP_IDIV
	if (hwcap & HWCAP_IDIV) {
		$(mut, addCString, "idiv");
	}
    #endif
    #ifdef HWCAP_IDIVA
	if (hwcap & HWCAP_IDIVA) {
		$(mut, addCString, "idiva");
	}
    #endif
    #ifdef HWCAP_IDIVT
	if (hwcap & HWCAP_IDIVT) {
		$(mut, addCString, "idivt");
	}
    #endif
    #ifdef HWCAP_IWMMXT
	if (hwcap & HWCAP_IWMMXT) {
		$(mut, addCString, "iwmmxt");
	}
    #endif
    #ifdef HWCAP_JAVA
	if (hwcap & HWCAP_JAVA) {
		$(mut, addCString, "java");
	}
    #endif
    #ifdef HWCAP_LPAE
	if (hwcap & HWCAP_LPAE) {
		$(mut, addCString, "lpae");
	}
    #endif
    #ifdef HWCAP_NEON
	if (hwcap & HWCAP_NEON) {
		$(mut, addCString, "neon");
		self->has128bitVectors = true;
		// NOTE: Non-temporal stores are an aarch64 feature, not arm32.
	}
    #endif
    #ifdef HWCAP_SWP
	if (hwcap & HWCAP_SWP) {
		$(mut, addCString, "swp");
	}
    #endif
    #ifdef HWCAP_THUMB
	if (hwcap & HWCAP_THUMB) {
		$(mut, addCString, "thumb");
	}
    #endif
    #ifdef HWCAP_THUMBEE
	if (hwcap & HWCAP_THUMBEE) {
		$(mut, addCString, "thumbee");
	}
    #endif
    #ifdef HWCAP_TLS
	if (hwcap & HWCAP_TLS) {
		$(mut, addCString, "tls");
	}
    #endif
    #ifdef HWCAP_VFP
	if (hwcap & HWCAP_VFP) {
		$(mut, addCString, "vfp");
	}
    #endif
    #ifdef HWCAP_VFPD32
	if (hwcap & HWCAP_VFPD32) {
		$(mut, addCString, "vfpd32");
	}
    #endif
    #ifdef HWCAP_VFPv3
	if (hwcap & HWCAP_VFPv3) {
		$(mut, addCString, "vfpv3");
	}
    #endif
    #ifdef HWCAP_VFPv3D16
	if (hwcap & HWCAP_VFPv3D16) {
		$(mut, addCString, "vfpv3d16");
	}
    #endif
    #ifdef HWCAP_VFPv4
	if (hwcap & HWCAP_VFPv4) {
		$(mut, addCString, "vfpv4");
	}
    #endif
    #ifdef HWCAP2_AES
	if (hwcap2 & HWCAP2_AES) {
		$(mut, addCString, "aes");
	}
    #endif
    #ifdef HWCAP2_CRC32
	if (hwcap2 & HWCAP2_CRC32) {
		$(mut, addCString, "crc32");
	}
    #endif
    #ifdef HWCAP2_PMULL
	if (hwcap2 & HWCAP2_PMULL) {
		$(mut, addCString, "pmull");
	}
    #endif
    #ifdef HWCAP2_SB
	if (hwcap2 & HWCAP2_SB) {
		$(mut, addCString, "sb");
	}
    #endif
    #ifdef HWCAP2_SHA1
	if (hwcap2 & HWCAP2_SHA1) {
		$(mut, addCString, "sha1");
	}
    #endif
    #ifdef HWCAP2_SHA2
	if (hwcap2 & HWCAP2_SHA2) {
		$(mut, addCString, "sha2");
	}
    #endif
    #ifdef HWCAP2_SSBS
	if (hwcap2 & HWCAP2_SSBS) {
		$(mut, addCString, "ssbs");
	}
    #endif
#endif // arm32
#endif // Linux

#if defined(__APPLE__) && defined(__aarch64__)
    // MacOS
    // TODO: Detect Apple Silicon hardware capabilities.
    #if defined(__ARM_NEON__) || defined(__ARM_NEON) || defined(WITH_SIMD)
	$(mut, addCString, "neon");
	self->has128bitVectors = true;
	self->has128bitCacheBypass = true;
    #endif
#endif // macOS

	return mut;
}

static void CPUARM_memoryFence (CPUARM* restrict self)
{
	if (!self) {
		return;
	}
	verifyCorrectClass(self,CPUARM);

	asm volatile(
		"DMB SY\n" // Data memory barrier, strong sync.
		"ISB\n"    // Instruction sync barrier.
		::: "memory"
	);
}

static void CPUARM_flushDataCacheAt (CPUARM* restrict self, void* address, size_t size)
{
	if (!self || !address) {
		return;
	}
	verifyCorrectClass(self,CPUARM);

	if (size < 64) {
		// Typical cache line length.
		size = 64;
	}

#if defined(__linux__)
	madvise (address, 64, MADV_DONTNEED);
#endif
}

static String* CPUARM_instructionSet (CPUARM* restrict self) 
{
	if (!self) {
		return NULL;
	}
	verifyCorrectClass(self,CPUARM);

	if (sizeof(long) == 8) {
		return _String("aarch64");
	} else {
		return _String("aarch32");
	}
}

static String* CPUARM_family (CPUARM* restrict self) 
{
	if (!self) {
		return NULL;
	}
	verifyCorrectClass(self,CPUARM);

#if defined(__APPLE__)
	return _String("AppleSiliconARM");
#else
	char family[64];
	if (FileSystem_read_trimmed_line_from_file("/sys/devices/soc0/family", family, sizeof(family))) {
		return _String(family);
	}
	return _String("ARM");
#endif
}

static String* CPUARM_make(CPUARM* restrict self)
{
	if (!self) {
		return NULL;
	}
	verifyCorrectClassOrSubclass(self,CPU);

#if defined(__APPLE__)
	return _String("Apple");
#endif

#if defined(__ANDROID__)
	char vendor[64];
	if (FileSystem_read_trimmed_line_from_file("/sys/devices/soc0/vendor", vendor, sizeof(vendor))) {
		return _String(vendor);
	}
#endif

#if defined(__aarch64__) && defined(__linux__)
	unsigned long midr_el1 = 0;
	if (FileSystem_read_hex_from_file ("/sys/devices/system/cpu/cpu0/regs/identification/midr_el1", &midr_el1)) {
		unsigned long implementer = (midr_el1 >> 24) & 0xff;
		char *result;
		switch (implementer) {
		case 0x41: result = "ARM"; break;
		case 0x42: result = "Broadcom"; break;
		case 0x43: result = "Cavium"; break;
		case 0x44: result = "DigitalEquipment"; break;
		case 0x46: result = "Fujitsu"; break;
		case 0x49: result = "Infineon"; break;
		case 0x4d: result = "MotorolaFreescale"; break;
		case 0x4e: result = "Nvidia"; break;
		case 0x50: result = "AppliedMicroCircuits"; break;
		case 0x51: result = "Qualcomm"; break;
		case 0x56: result = "Marvell"; break;
		case 0x69: result = "Intel"; break;
		case 0xc0: result = "Ampere"; break;
		default:
			result = NULL;
		}
		if (result) {
			return _String(result);
		}
	}
#endif
	return NULL;
} 

#if defined(__APPLE__)
static size_t macos_cache_line_size ()
{
	size_t value = 0;
	size_t len = sizeof(value);
	if (!sysctlbyname ("hw.cachelinesize", &value, &len, NULL, 0)) {
		return value;
	} else {
		perror("sysctlbyname");
	}
	return 0;
}
#endif

CPUARMClass* CPUARMClass_init (CPUARMClass *class)
{
	SET_SUPERCLASS(CPU);

	SET_OVERRIDDEN_METHOD_POINTER(CPUARM,describe);
	SET_OVERRIDDEN_METHOD_POINTER(CPUARM,print);
	SET_OVERRIDDEN_METHOD_POINTER(CPUARM,family);
	SET_OVERRIDDEN_METHOD_POINTER(CPUARM,model);
	SET_OVERRIDDEN_METHOD_POINTER(CPUARM,make);
	SET_OVERRIDDEN_METHOD_POINTER(CPUARM,features);
	SET_OVERRIDDEN_METHOD_POINTER(CPUARM,memoryFence);
	SET_OVERRIDDEN_METHOD_POINTER(CPUARM,flushDataCacheAt);
	SET_OVERRIDDEN_METHOD_POINTER(CPUARM,instructionSet);

        VALIDATE_CLASS_STRUCT(class);
	return class;
}

CPUARM* CPUARM_init (CPUARM* restrict self)
{
	ENSURE_CLASS_READY(CPUARM);

	if (self) {
		CPU_init ((CPU*) self);
		self->is_a = _CPUARMClass;

		(void) $(self, nCores);
		(void) $(self, features);

#if defined(__APPLE__)
		self->cacheLineSize = macos_cache_line_size ();
#elif defined(__linux__)
		unsigned value = 0;
		if (FileSystem_read_unsigned_from_file("/sys/devices/system/cpu/cpu0/cache/index0/coherency_line_size", &value)) {
			if (value) {
				self->cacheLineSize = value;
			}
		}
#endif
	}

	return self;
}

#endif // ARM
