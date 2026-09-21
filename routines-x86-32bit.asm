;============================================================================
; bandwidth, a benchmark to measure memory transfer bandwidth.
; Copyright (C) 2005-2024, 2026 by Zack T Smith.
;
; This program is free software; you can redistribute it and/or modify
; it under the terms of the GNU General Public License as published by
; the Free Software Foundation; either version 3 of the License, or
; (at your option) any later version.
;
; This program is distributed in the hope that it will be useful,
; but WITHOUT ANY WARRANTY; without even the implied warranty of
; MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
; GNU General Public License for more details.
;
; You should have received a copy of the GNU General Public License
; along with this program; if not, write to the Free Software
; Foundation, Inc., 51 Franklin St, Fifth Floor, Boston, MA  02110-1301  USA
;
; The author may be reached at 3 at zs3 dot me.
;=============================================================================

bits	32
cpu	IA64

global  IncrementRegisters
global  _IncrementRegisters
global  IncrementStack
global  _IncrementStack

; Cygwin requires the underbar-prefixed symbols.
global	_WriterVector128
global	WriterVector128

global	_WriterVector256
global	WriterVector256

global	_ReaderVector256
global	ReaderVector256

global	_ReaderVector128
global	ReaderVector128

global	ReaderVector128_nontemporal
global	_ReaderVector128_nontemporal

global	_RandomReaderVector128
global	RandomReaderVector128

global	_RandomReaderVector128_nontemporal
global	RandomReaderVector128_nontemporal

global  WriterVector256_nontemporal
global  _WriterVector256_nontemporal

global	_WriterVector128_nontemporal
global	WriterVector128_nontemporal

global	_RandomWriterVector128_nontemporal
global	RandomWriterVector128_nontemporal

global	VectorToRegisterMove
global	_VectorToRegisterMove

global	RegisterToVectorMove
global	_RegisterToVectorMove

global	Reader
global	_Reader

global	Writer
global	_Writer

global	RandomReader
global	_RandomReader

global	RandomWriter
global	_RandomWriter

global	RandomWriterVector128
global	_RandomWriterVector128

global	CopyVector128
global	_CopyVector128

global CopyVector256
global _CopyVector256

global	CopyWithMainRegisters
global	_CopyWithMainRegisters

global	RegisterToRegister
global	_RegisterToRegister

global	VectorToVector128
global	_VectorToVector128

global	VectorToVector256
global	_VectorToVector256

global	Register8ToVector128
global	Register16ToVector128
global	Register32ToVector128
global	Register64ToVector128
global	Vector128ToRegister8
global	Vector128ToRegister16
global	Vector128ToRegister32
global	Vector128ToRegister64

global	_Register8ToVector128
global	_Register16ToVector128
global	_Register32ToVector128
global	_Register64ToVector128
global	_Vector128ToRegister8
global	_Vector128ToRegister16
global	_Vector128ToRegister32
global	_Vector128ToRegister64

global	StackReader
global	_StackReader

global	StackWriter
global	_StackWriter

global	Reader_nontemporal
global	Writer_nontemporal
global	ReaderVector256_nontemporal
global	RandomWriterVector256_nontemporal
global	RandomReaderVector256
global	RandomWriterVector256
global	ReaderVector512
global	WriterVector512
global	ReaderVector512_nontemporal
global	WriterVector512_nontemporal
global	CopyVector512
global	VectorToVector512

	section .note.GNU-stack 

	section .text

;------------------------------------------------------------------------------
; Name:		Reader
; Purpose:	Reads 32-bit values sequentially from an area of memory.
; Params:	
;		[esp+4]	= ptr to memory area
; 		[esp+8] = length in bytes
; 		[esp+12] = loops
;------------------------------------------------------------------------------
	align 64
Reader:
_Reader:
	lfence
	push	ebx
	push	ecx
	push	edx

	mov	ecx, [esp+12+12]	; loops to do.

	mov	edx, [esp+4+12]	; ptr to memory chunk.
	mov	ebx, edx	; ebx = limit in memory
	add	ebx, [esp+8+12]

.L1:
	mov	edx, [esp+4+12]	

.L2:
	mov	eax, [edx]
	mov	eax, [4+edx]
	mov	eax, [8+edx]
	mov	eax, [12+edx]
	mov	eax, [16+edx]
	mov	eax, [20+edx]
	mov	eax, [24+edx]
	mov	eax, [28+edx]
	mov	eax, [32+edx]
	mov	eax, [36+edx]
	mov	eax, [40+edx]
	mov	eax, [44+edx]
	mov	eax, [48+edx]
	mov	eax, [52+edx]
	mov	eax, [56+edx]
	mov	eax, [60+edx]
	mov	eax, [64+edx]
	mov	eax, [68+edx]
	mov	eax, [72+edx]
	mov	eax, [76+edx]
	mov	eax, [80+edx]
	mov	eax, [84+edx]
	mov	eax, [88+edx]
	mov	eax, [92+edx]
	mov	eax, [96+edx]
	mov	eax, [100+edx]
	mov	eax, [104+edx]
	mov	eax, [108+edx]
	mov	eax, [112+edx]
	mov	eax, [116+edx]
	mov	eax, [120+edx]
	mov	eax, [124+edx]

	mov	eax, [edx+128]
	mov	eax, [edx+132]
	mov	eax, [edx+136]
	mov	eax, [edx+140]
	mov	eax, [edx+144]
	mov	eax, [edx+148]
	mov	eax, [edx+152]
	mov	eax, [edx+156]
	mov	eax, [edx+160]
	mov	eax, [edx+164]
	mov	eax, [edx+168]
	mov	eax, [edx+172]
	mov	eax, [edx+176]
	mov	eax, [edx+180]
	mov	eax, [edx+184]
	mov	eax, [edx+188]
	mov	eax, [edx+192]
	mov	eax, [edx+196]
	mov	eax, [edx+200]
	mov	eax, [edx+204]
	mov	eax, [edx+208]
	mov	eax, [edx+212]
	mov	eax, [edx+216]
	mov	eax, [edx+220]
	mov	eax, [edx+224]
	mov	eax, [edx+228]
	mov	eax, [edx+232]
	mov	eax, [edx+236]
	mov	eax, [edx+240]
	mov	eax, [edx+244]
	mov	eax, [edx+248]
	mov	eax, [edx+252]

	add	edx, 256
	cmp	edx, ebx
	jb	.L2

	sub	ecx, 1
	jnz	.L1

	pop	edx
	pop	ecx
	pop	ebx

	ret

;------------------------------------------------------------------------------
; Name:		Writer
; Purpose:	Writes 32-bit value sequentially to an area of memory.
; Params:	
;		[esp+4]	= ptr to memory area
; 		[esp+8] = length in bytes
; 		[esp+12] = loops
; 		[esp+16] = long to write
;------------------------------------------------------------------------------
	align 64
Writer:
_Writer:
	sfence
	push	ebx
	push	ecx
	push	edx

	mov	ecx, [esp+12+12]
	mov	eax, [esp+16+12]

	mov	edx, [esp+4+12]	; edx = ptr to chunk
	mov	ebx, edx
	add	ebx, [esp+8+12]	; ebx = limit in memory

.L1:
	mov	edx, [esp+4+12]

.L2:
	mov	[edx], eax
	mov	[4+edx], eax
	mov	[8+edx], eax
	mov	[12+edx], eax
	mov	[16+edx], eax
	mov	[20+edx], eax
	mov	[24+edx], eax
	mov	[28+edx], eax
	mov	[32+edx], eax
	mov	[36+edx], eax
	mov	[40+edx], eax
	mov	[44+edx], eax
	mov	[48+edx], eax
	mov	[52+edx], eax
	mov	[56+edx], eax
	mov	[60+edx], eax
	mov	[64+edx], eax
	mov	[68+edx], eax
	mov	[72+edx], eax
	mov	[76+edx], eax
	mov	[80+edx], eax
	mov	[84+edx], eax
	mov	[88+edx], eax
	mov	[92+edx], eax
	mov	[96+edx], eax
	mov	[100+edx], eax
	mov	[104+edx], eax
	mov	[108+edx], eax
	mov	[112+edx], eax
	mov	[116+edx], eax
	mov	[120+edx], eax
	mov	[124+edx], eax

	mov	[edx+128], eax
	mov	[edx+132], eax
	mov	[edx+136], eax
	mov	[edx+140], eax
	mov	[edx+144], eax
	mov	[edx+148], eax
	mov	[edx+152], eax
	mov	[edx+156], eax
	mov	[edx+160], eax
	mov	[edx+164], eax
	mov	[edx+168], eax
	mov	[edx+172], eax
	mov	[edx+176], eax
	mov	[edx+180], eax
	mov	[edx+184], eax
	mov	[edx+188], eax
	mov	[edx+192], eax
	mov	[edx+196], eax
	mov	[edx+200], eax
	mov	[edx+204], eax
	mov	[edx+208], eax
	mov	[edx+212], eax
	mov	[edx+216], eax
	mov	[edx+220], eax
	mov	[edx+224], eax
	mov	[edx+228], eax
	mov	[edx+232], eax
	mov	[edx+236], eax
	mov	[edx+240], eax
	mov	[edx+244], eax
	mov	[edx+248], eax
	mov	[edx+252], eax

	add	edx, 256
	cmp	edx, ebx
	jb	.L2

	sub	ecx, 1
	jnz	.L1

	pop	edx
	pop	ecx
	pop	ebx

	ret

;------------------------------------------------------------------------------
; Name:		Writer_nontemporal
; Purpose:	Writes 32-bit value sequentially to an area of memory.
; Params:	
;		[esp+4]	= ptr to memory area
; 		[esp+8] = length in bytes
; 		[esp+12] = loops
; 		[esp+16] = long to write
;------------------------------------------------------------------------------
	align 64
Writer_nontemporal:
_Writer_nontemporal:
	sfence
	push	ebx
	push	ecx
	push	edx

	mov	ecx, [esp+12+12]
	mov	eax, [esp+16+12]

	mov	edx, [esp+4+12]	; edx = ptr to chunk
	mov	ebx, edx
	add	ebx, [esp+8+12]	; ebx = limit in memory

.L1:
	mov	edx, [esp+4+12]

.L2:
	movnti	[edx], eax
	movnti	[4+edx], eax
	movnti	[8+edx], eax
	movnti	[12+edx], eax
	movnti	[16+edx], eax
	movnti	[20+edx], eax
	movnti	[24+edx], eax
	movnti	[28+edx], eax
	movnti	[32+edx], eax
	movnti	[36+edx], eax
	movnti	[40+edx], eax
	movnti	[44+edx], eax
	movnti	[48+edx], eax
	movnti	[52+edx], eax
	movnti	[56+edx], eax
	movnti	[60+edx], eax
	movnti	[64+edx], eax
	movnti	[68+edx], eax
	movnti	[72+edx], eax
	movnti	[76+edx], eax
	movnti	[80+edx], eax
	movnti	[84+edx], eax
	movnti	[88+edx], eax
	movnti	[92+edx], eax
	movnti	[96+edx], eax
	movnti	[100+edx], eax
	movnti	[104+edx], eax
	movnti	[108+edx], eax
	movnti	[112+edx], eax
	movnti	[116+edx], eax
	movnti	[120+edx], eax
	movnti	[124+edx], eax

	movnti	[edx+128], eax
	movnti	[edx+132], eax
	movnti	[edx+136], eax
	movnti	[edx+140], eax
	movnti	[edx+144], eax
	movnti	[edx+148], eax
	movnti	[edx+152], eax
	movnti	[edx+156], eax
	movnti	[edx+160], eax
	movnti	[edx+164], eax
	movnti	[edx+168], eax
	movnti	[edx+172], eax
	movnti	[edx+176], eax
	movnti	[edx+180], eax
	movnti	[edx+184], eax
	movnti	[edx+188], eax
	movnti	[edx+192], eax
	movnti	[edx+196], eax
	movnti	[edx+200], eax
	movnti	[edx+204], eax
	movnti	[edx+208], eax
	movnti	[edx+212], eax
	movnti	[edx+216], eax
	movnti	[edx+220], eax
	movnti	[edx+224], eax
	movnti	[edx+228], eax
	movnti	[edx+232], eax
	movnti	[edx+236], eax
	movnti	[edx+240], eax
	movnti	[edx+244], eax
	movnti	[edx+248], eax
	movnti	[edx+252], eax

	add	edx, 256
	cmp	edx, ebx
	jb	.L2

	sub	ecx, 1
	jnz	.L1

	pop	edx
	pop	ecx
	pop	ebx

	ret

;------------------------------------------------------------------------------
; Name:		ReaderVector256 (using AVX)
; Purpose:	Reads 128-bit values sequentially from an area of memory.
; Params:	[esp+4] = ptr to memory area
; 		[esp+8] = length in bytes
; 		[esp+12] = loops
;------------------------------------------------------------------------------
	align 64
ReaderVector256:
_ReaderVector256:
	lfence
	vzeroupper

	push	ebx
	push	ecx

	mov	ecx, [esp+12+8]

	mov	eax, [esp+4+8]
	mov	ebx, eax
	add	ebx, [esp+8+8]	; ebx points to end.

.L1:
	mov	eax, [esp+4+8]

.L2:
	vmovdqa	ymm0, [eax]	; Read aligned @ 16-byte boundary.
	vmovdqa	ymm0, [32+eax]
	vmovdqa	ymm0, [64+eax]
	vmovdqa	ymm0, [96+eax]
	vmovdqa	ymm0, [128+eax]
	vmovdqa	ymm0, [160+eax]
	vmovdqa	ymm0, [192+eax]
	vmovdqa	ymm0, [224+eax]

	add	eax, 256
	cmp	eax, ebx
	jb	.L2

	sub	ecx, 1
	jnz	.L1
	
	pop	ecx
	pop	ebx

	ret

;------------------------------------------------------------------------------
; Name:		ReaderVector128 (using SSE2)
; Purpose:	Reads 128-bit values sequentially from an area of memory.
; Params:	[esp+4] = ptr to memory area
; 		[esp+8] = length in bytes
; 		[esp+12] = loops
;------------------------------------------------------------------------------
	align 64
ReaderVector128:
_ReaderVector128:
	lfence
	push	ebx
	push	ecx

	mov	ecx, [esp+12+8]

	mov	eax, [esp+4+8]
	mov	ebx, eax
	add	ebx, [esp+8+8]	; ebx points to end.

.L1:
	mov	eax, [esp+4+8]

.L2:
	movdqa	xmm0, [eax]	; Read aligned @ 16-byte boundary.
	movdqa	xmm0, [16+eax]
	movdqa	xmm0, [32+eax]
	movdqa	xmm0, [48+eax]
	movdqa	xmm0, [64+eax]
	movdqa	xmm0, [80+eax]
	movdqa	xmm0, [96+eax]
	movdqa	xmm0, [112+eax]

	movdqa	xmm0, [128+eax]
	movdqa	xmm0, [144+eax]
	movdqa	xmm0, [160+eax]
	movdqa	xmm0, [176+eax]
	movdqa	xmm0, [192+eax]
	movdqa	xmm0, [208+eax]
	movdqa	xmm0, [224+eax]
	movdqa	xmm0, [240+eax]

	add	eax, 256
	cmp	eax, ebx
	jb	.L2

	sub	ecx, 1
	jnz	.L1
	
	pop	ecx
	pop	ebx

	ret

;------------------------------------------------------------------------------
; Name:		ReaderVector128_nontemporal (using SSE4)
; Purpose:	Reads 128-bit values sequentially from an area of memory.
; Params:	[esp+4] = ptr to memory area
; 		[esp+8] = length in bytes
; 		[esp+12] = loops
; Note:    	SSE4 provides MOVNTDQ to write memory, 
; 		whereas SSE4 provides MOVNTDQA to read memory.
;------------------------------------------------------------------------------
	align 64
ReaderVector128_nontemporal:
_ReaderVector128_nontemporal:
	lfence
	push	ebx
	push	ecx

	mov	ecx, [esp+12+8]

	mov	eax, [esp+4+8]
	mov	ebx, eax
	add	ebx, [esp+8+8]	; ebx points to end.

.L1:
	mov	eax, [esp+4+8]

.L2:
        prefetchnta     [eax]
	movntdqa	xmm0, [eax]	; Read aligned @ 16-byte boundary.
	movntdqa	xmm0, [16+eax]
	movntdqa	xmm0, [32+eax]
	movntdqa	xmm0, [48+eax]

        prefetchnta     [eax+64]
	movntdqa	xmm0, [64+eax]
	movntdqa	xmm0, [80+eax]
	movntdqa	xmm0, [96+eax]
	movntdqa	xmm0, [112+eax]

        prefetchnta     [eax+128]
	movntdqa	xmm0, [128+eax]
	movntdqa	xmm0, [144+eax]
	movntdqa	xmm0, [160+eax]
	movntdqa	xmm0, [176+eax]

        prefetchnta     [eax+192]
	movntdqa	xmm0, [192+eax]
	movntdqa	xmm0, [208+eax]
	movntdqa	xmm0, [224+eax]
	movntdqa	xmm0, [240+eax]

	add	eax, 256
	cmp	eax, ebx
	jb	.L2

	sub	ecx, 1
	jnz	.L1
	
	pop	ecx
	pop	ebx

	ret

;------------------------------------------------------------------------------
; Name:		WriterVector256
; Purpose:	Write 256-bit values sequentially from an area of memory.
; Params:	[esp+4] = ptr to memory area
; 		[esp+8] = length in bytes
; 		[esp+12] = loops
; 		[esp+16] = value (ignored)
;------------------------------------------------------------------------------
	align 64
WriterVector256:
_WriterVector256:
	sfence
	vzeroupper

	push	ebx
	push	ecx

	mov	eax, [esp+16+8]
	movd	xmm0, eax	; Create a 128-bit replication of the 32-bit
	movd	xmm1, eax	; value that was provided.
	movd	xmm2, eax
	movd	xmm3, eax
	pslldq	xmm1, 32
	pslldq	xmm2, 64
	pslldq	xmm3, 96
	por	xmm0, xmm1
	por	xmm0, xmm2
	por	xmm0, xmm3

	mov	ecx, [esp+12+8]

	mov	eax, [esp+4+8]
	mov	ebx, eax
	add	ebx, [esp+8+8]	; ebx points to end.

.L1:
	mov	eax, [esp+4+8]

.L2:
	vmovdqa	[eax], ymm0	
	vmovdqa	[32+eax], ymm0
	vmovdqa	[64+eax], ymm0
	vmovdqa	[96+eax], ymm0
	vmovdqa	[128+eax], ymm0
	vmovdqa	[160+eax], ymm0
	vmovdqa	[192+eax], ymm0
	vmovdqa	[224+eax], ymm0

	add	eax, 256
	cmp	eax, ebx
	jb	.L2

	sub	ecx, 1
	jnz	.L1
	
	pop	ecx
	pop	ebx

	ret

;------------------------------------------------------------------------------
; Name:		WriterVector128 (using SSE2)
; Purpose:	Write 128-bit values sequentially from an area of memory.
; Params:	[esp+4] = ptr to memory area
; 		[esp+8] = length in bytes
; 		[esp+12] = loops
; 		[esp+16] = value (ignored)
;------------------------------------------------------------------------------
	align 64
WriterVector128:
_WriterVector128:
	sfence
	push	ebx
	push	ecx

	mov	eax, [esp+16+8]
	movd	xmm0, eax	; Create a 128-bit replication of the 32-bit
	movd	xmm1, eax	; value that was provided.
	movd	xmm2, eax
	movd	xmm3, eax
	pslldq	xmm1, 32	; Parallel shift left logical double quadword 
	pslldq	xmm2, 64
	pslldq	xmm3, 96
	por	xmm0, xmm1
	por	xmm0, xmm2
	por	xmm0, xmm3

	mov	ecx, [esp+12+8]

	mov	eax, [esp+4+8]
	mov	ebx, eax
	add	ebx, [esp+8+8]	; ebx points to end.

.L1:
	mov	eax, [esp+4+8]

.L2:
	movdqa	[eax], xmm0	
	movdqa	[16+eax], xmm0
	movdqa	[32+eax], xmm0
	movdqa	[48+eax], xmm0
	movdqa	[64+eax], xmm0
	movdqa	[80+eax], xmm0
	movdqa	[96+eax], xmm0
	movdqa	[112+eax], xmm0

	movdqa	[128+eax], xmm0
	movdqa	[144+eax], xmm0
	movdqa	[160+eax], xmm0
	movdqa	[176+eax], xmm0
	movdqa	[192+eax], xmm0
	movdqa	[208+eax], xmm0
	movdqa	[224+eax], xmm0
	movdqa	[240+eax], xmm0

	add	eax, 256
	cmp	eax, ebx
	jb	.L2

	sub	ecx, 1
	jnz	.L1
	
	pop	ecx
	pop	ebx

	ret

;------------------------------------------------------------------------------
; Name:		WriterVector256_nontemporal
; Purpose:	Nontemporal writes 256-bit values sequentially to area of memory.
; Params:	[esp+4] = ptr to memory area
; 		[esp+8] = length in bytes
; 		[esp+12] = loops
; 		[esp+16] = value (ignored)
;------------------------------------------------------------------------------

	align 64
WriterVector256_nontemporal:
_WriterVector256_nontemporal:
	sfence
	vzeroupper

	push	ebx
	push	ecx

	mov	eax, [esp+16+8]
	movd	xmm0, eax	; Create a 128-bit replication of the 32-bit
	movd	xmm1, eax	; value that was provided.
	movd	xmm2, eax
	movd	xmm3, eax
	pslldq	xmm1, 32	; Parallel shift left logical double quadword 
	pslldq	xmm2, 64
	pslldq	xmm3, 96
	por	xmm0, xmm1
	por	xmm0, xmm2
	por	xmm0, xmm3

	mov	ecx, [esp+12+8]

	mov	eax, [esp+4+8]
	mov	ebx, eax
	add	ebx, [esp+8+8]	; ebx points to end.

.L1:
	mov	eax, [esp+4+8]

.L2:
	vmovntdq	[eax], ymm0
	vmovntdq	[32+eax], ymm0
	vmovntdq	[64+eax], ymm0
	vmovntdq	[96+eax], ymm0
	vmovntdq	[128+eax], ymm0
	vmovntdq	[160+eax], ymm0
	vmovntdq	[192+eax], ymm0
	vmovntdq	[224+eax], ymm0

	add	eax, 256
	cmp	eax, ebx
	jb	.L2

	sub	ecx, 1
	jnz	.L1
	
	pop	ecx
	pop	ebx

	ret

;------------------------------------------------------------------------------
; Name:		WriterVector128_nontemporal
; Purpose:	Nontemporal writes of 128-bit values sequentially to area of memory.
; Params:	[esp+4] = ptr to memory area
; 		[esp+8] = length in bytes
; 		[esp+12] = loops
; 		[esp+16] = value (ignored)
;------------------------------------------------------------------------------
	align 64
WriterVector128_nontemporal:
_WriterVector128_nontemporal:
	sfence
	push	ebx
	push	ecx

	mov	eax, [esp+16+8]
	movd	xmm0, eax	; Create a 128-bit replication of the 32-bit
	movd	xmm1, eax	; value that was provided.
	movd	xmm2, eax
	movd	xmm3, eax
	pslldq	xmm1, 32	; Parallel shift left logical double quadword 
	pslldq	xmm2, 64
	pslldq	xmm3, 96
	por	xmm0, xmm1
	por	xmm0, xmm2
	por	xmm0, xmm3

	mov	ecx, [esp+12+8]

	mov	eax, [esp+4+8]
	mov	ebx, eax
	add	ebx, [esp+8+8]	; ebx points to end.

.L1:
	mov	eax, [esp+4+8]

.L2:
	movntdq	[eax], xmm0	
	movntdq	[16+eax], xmm0
	movntdq	[32+eax], xmm0
	movntdq	[48+eax], xmm0
	movntdq	[64+eax], xmm0
	movntdq	[80+eax], xmm0
	movntdq	[96+eax], xmm0
	movntdq	[112+eax], xmm0

	movntdq	[128+eax], xmm0
	movntdq	[144+eax], xmm0
	movntdq	[160+eax], xmm0
	movntdq	[176+eax], xmm0
	movntdq	[192+eax], xmm0
	movntdq	[208+eax], xmm0
	movntdq	[224+eax], xmm0
	movntdq	[240+eax], xmm0

	add	eax, 256
	cmp	eax, ebx
	jb	.L2

	sub	ecx, 1
	jnz	.L1
	
	pop	ecx
	pop	ebx

	ret

;------------------------------------------------------------------------------
; Name:		RandomReader
; Purpose:	Reads 32-bit values randomly from an area of memory.
; Params:	
;		[esp+4]	= ptr to array of chunk pointers
; 		[esp+8] = # of 128-byte chunks
; 		[esp+12] = loops
;------------------------------------------------------------------------------
	align 64
RandomReader:
_RandomReader:
	lfence
	push	ebx
	push	ecx
	push	edx

	mov	ecx, [esp+12+12]	; loops to do.

.L0:
	mov	ebx, [esp+8+12]		; # chunks to do

.L1:
	sub	ebx, 1
	jc	.L2

	mov	edx, [esp+4+12]  	; get ptr to memory chunk.
	mov	edx, [edx + 4*ebx]

	mov	eax, [edx+160]
	mov	eax, [edx+232]
	mov	eax, [edx+224]
	mov	eax, [96+edx]
	mov	eax, [edx+164]
	mov	eax, [76+edx]
	mov	eax, [100+edx]
	mov	eax, [edx+220]
	mov	eax, [edx+248]
	mov	eax, [104+edx]
	mov	eax, [4+edx]
	mov	eax, [edx+136]
	mov	eax, [112+edx]
	mov	eax, [edx+200]
	mov	eax, [12+edx]
	mov	eax, [edx+128]
	mov	eax, [edx+148]
	mov	eax, [edx+196]
	mov	eax, [edx+216]
	mov	eax, [edx]
	mov	eax, [84+edx]
	mov	eax, [edx+140]
	mov	eax, [edx+204]
	mov	eax, [edx+184]
	mov	eax, [124+edx]
	mov	eax, [48+edx]
	mov	eax, [64+edx]
	mov	eax, [edx+212]
	mov	eax, [edx+240]
	mov	eax, [edx+236]
	mov	eax, [24+edx]
	mov	eax, [edx+252]
	mov	eax, [68+edx]
	mov	eax, [20+edx]
	mov	eax, [72+edx]
	mov	eax, [32+edx]
	mov	eax, [28+edx]
	mov	eax, [52+edx]
	mov	eax, [edx+244]
	mov	eax, [edx+180]
	mov	eax, [80+edx]
	mov	eax, [60+edx]
	mov	eax, [8+edx]
	mov	eax, [56+edx]
	mov	eax, [edx+208]
	mov	eax, [edx+228]
	mov	eax, [40+edx]
	mov	eax, [edx+172]
	mov	eax, [120+edx]
	mov	eax, [edx+176]
	mov	eax, [108+edx]
	mov	eax, [edx+132]
	mov	eax, [16+edx]
	mov	eax, [44+edx]
	mov	eax, [92+edx]
	mov	eax, [edx+168]
	mov	eax, [edx+152]
	mov	eax, [edx+156]
	mov	eax, [edx+188]
	mov	eax, [36+edx]
	mov	eax, [88+edx]
	mov	eax, [116+edx]
	mov	eax, [edx+192]
	mov	eax, [edx+144]

	jmp	.L1

.L2:
	sub	ecx, 1
	jnz	.L0

	pop	edx
	pop	ecx
	pop	ebx

	ret

;------------------------------------------------------------------------------
; Name:		RandomReaderVector128 (using SSE2)
; Purpose:	Reads 128-bit values sequentially from an area of memory.
; Params:	
;		[esp+4]	= ptr to array of chunk pointers
; 		[esp+8] = # of 128-byte chunks
; 		[esp+12] = loops
;------------------------------------------------------------------------------
	align 64
RandomReaderVector128:
_RandomReaderVector128:
	lfence
	push	ebx
	push	ecx
	push	edx

	mov	ecx, [esp+12+12]	; loops to do.

.L0:
	mov	ebx, [esp+8+12]		; # chunks to do

.L1:
	sub	ebx, 1
	jc	.L2

	mov	edx, [esp+4+12]  	; get ptr to memory chunk.
	mov	edx, [edx + 4*ebx]

; Read aligned @ 16-byte boundary.
	movdqa	xmm0, [240+edx]
	movdqa	xmm0, [128+edx]
	movdqa	xmm0, [64+edx]
	movdqa	xmm0, [208+edx]
	movdqa	xmm0, [112+edx]
	movdqa	xmm0, [176+edx]
	movdqa	xmm0, [144+edx]
	movdqa	xmm0, [edx]
	movdqa	xmm0, [96+edx]
	movdqa	xmm0, [16+edx]
	movdqa	xmm0, [192+edx]
	movdqa	xmm0, [160+edx]
	movdqa	xmm0, [32+edx]
	movdqa	xmm0, [48+edx]
	movdqa	xmm0, [224+edx]
	movdqa	xmm0, [80+edx]

	jmp	.L1

.L2:
	sub	ecx, 1
	jnz	.L0

	pop	edx
	pop	ecx
	pop	ebx

	ret

;------------------------------------------------------------------------------
; Name:		RandomReaderVector128_nontemporal (using SSE4)
; Purpose:	Reads 128-bit values sequentially from an area of memory.
; Params:	
;		[esp+4]	= ptr to array of chunk pointers
; 		[esp+8] = # of 128-byte chunks
; 		[esp+12] = loops
;------------------------------------------------------------------------------
	align 64
RandomReaderVector128_nontemporal:
_RandomReaderVector128_nontemporal:
	lfence
	push	ebx
	push	ecx
	push	edx

	mov	ecx, [esp+12+12]	; loops to do.

.L0:
	mov	ebx, [esp+8+12]		; # chunks to do

.L1:
	sub	ebx, 1
	jc	.L2

	mov	edx, [esp+4+12]  	; get ptr to memory chunk.
	mov	edx, [edx + 4*ebx]

; Read aligned @ 16-byte boundary.
        prefetchnta     [edx+192]
	movntdqa	xmm0, [240+edx]
        prefetchnta     [edx]
	movntdqa	xmm0, [edx]
        prefetchnta     [edx+128]
	movntdqa	xmm0, [128+edx]
        prefetchnta     [edx+64]
	movntdqa	xmm0, [64+edx]
	movntdqa	xmm0, [208+edx]
	movntdqa	xmm0, [112+edx]
	movntdqa	xmm0, [32+edx]
	movntdqa	xmm0, [176+edx]
	movntdqa	xmm0, [144+edx]
	movntdqa	xmm0, [96+edx]
	movntdqa	xmm0, [16+edx]
	movntdqa	xmm0, [160+edx]
	movntdqa	xmm0, [192+edx]
	movntdqa	xmm0, [48+edx]
	movntdqa	xmm0, [224+edx]
	movntdqa	xmm0, [80+edx]

	jmp	.L1

.L2:
	sub	ecx, 1
	jnz	.L0

	pop	edx
	pop	ecx
	pop	ebx

	ret

;------------------------------------------------------------------------------
; Name:		RandomWriter
; Purpose:	Writes 32-bit value sequentially to an area of memory.
; Params:	
;		[esp+4]	= ptr to memory area
; 		[esp+8] = length in bytes
; 		[esp+12] = loops
; 		[esp+16] = long to write
;------------------------------------------------------------------------------
	align 64
RandomWriter:
_RandomWriter:
	sfence
	push	ebx
	push	ecx
	push	edx

	mov	eax, [esp+16+12]	; get datum.
	mov	ecx, [esp+12+12]	; loops to do.

.L0:
	mov	ebx, [esp+8+12]		; # chunks to do

.L1:
	sub	ebx, 1
	jc	.L2

	mov	edx, [esp+4+12]  	; get ptr to memory chunk.
	mov	edx, [edx + 4*ebx]

	mov	[edx+212], eax
	mov	[edx+156], eax
	mov	[edx+132], eax
	mov	[20+edx], eax
	mov	[edx+172], eax
	mov	[edx+196], eax
	mov	[edx+248], eax
	mov	[edx], eax
	mov	[edx+136], eax
	mov	[edx+228], eax
	mov	[edx+160], eax
	mov	[80+edx], eax
	mov	[76+edx], eax
	mov	[32+edx], eax
	mov	[64+edx], eax
	mov	[68+edx], eax
	mov	[120+edx], eax
	mov	[edx+216], eax
	mov	[124+edx], eax
	mov	[28+edx], eax
	mov	[edx+152], eax
	mov	[36+edx], eax
	mov	[edx+220], eax
	mov	[edx+188], eax
	mov	[48+edx], eax
	mov	[104+edx], eax
	mov	[72+edx], eax
	mov	[96+edx], eax
	mov	[edx+184], eax
	mov	[112+edx], eax
	mov	[edx+236], eax
	mov	[edx+224], eax
	mov	[edx+252], eax
	mov	[88+edx], eax
	mov	[edx+180], eax
	mov	[60+edx], eax
	mov	[24+edx], eax
	mov	[edx+192], eax
	mov	[edx+164], eax
	mov	[edx+204], eax
	mov	[44+edx], eax
	mov	[edx+168], eax
	mov	[92+edx], eax
	mov	[edx+208], eax
	mov	[8+edx], eax
	mov	[edx+144], eax
	mov	[edx+148], eax
	mov	[edx+128], eax
	mov	[52+edx], eax
	mov	[4+edx], eax
	mov	[108+edx], eax
	mov	[12+edx], eax
	mov	[56+edx], eax
	mov	[edx+200], eax
	mov	[edx+232], eax
	mov	[16+edx], eax
	mov	[edx+244], eax
	mov	[40+edx], eax
	mov	[edx+140], eax
	mov	[84+edx], eax
	mov	[100+edx], eax
	mov	[116+edx], eax
	mov	[edx+176], eax
	mov	[edx+240], eax

	jmp	.L1

.L2:
	sub	ecx, 1
	jnz	.L0

	pop	edx
	pop	ecx
	pop	ebx

	ret

;------------------------------------------------------------------------------
; Name:		RandomWriterVector128
; Purpose:	Writes 128-bit value randomly to an area of memory.
; Params:	
;		[esp+4]	= ptr to memory area
; 		[esp+8] = length in bytes
; 		[esp+12] = loops
; 		[esp+16] = long to write
;------------------------------------------------------------------------------
	align 64
RandomWriterVector128:
_RandomWriterVector128:
	lfence
	push	ebx
	push	ecx
	push	edx

	mov	eax, [esp+16+8]
	movd	xmm0, eax	; Create a 128-bit replication of the 32-bit
	movd	xmm1, eax	; value that was provided.
	movd	xmm2, eax
	movd	xmm3, eax
	pslldq	xmm1, 32
	pslldq	xmm2, 64
	pslldq	xmm3, 96
	por	xmm0, xmm1
	por	xmm0, xmm2
	por	xmm0, xmm3

	mov	ecx, [esp+12+12]	; loops to do.

.L0:
	mov	ebx, [esp+8+12]		; # chunks to do

.L1:
	sub	ebx, 1
	jc	.L2

	mov	edx, [esp+4+12]  	; get ptr to memory chunk.
	mov	edx, [edx + 4*ebx]

	movdqa	[64+edx], xmm0
	movdqa	[208+edx], xmm0
	movdqa	[128+edx], xmm0
	movdqa	[112+edx], xmm0
	movdqa	[176+edx], xmm0
	movdqa	[144+edx], xmm0
	movdqa	[edx], xmm0
	movdqa	[96+edx], xmm0
	movdqa	[48+edx], xmm0
	movdqa	[16+edx], xmm0
	movdqa	[192+edx], xmm0
	movdqa	[160+edx], xmm0
	movdqa	[32+edx], xmm0
	movdqa	[240+edx], xmm0
	movdqa	[224+edx], xmm0
	movdqa	[80+edx], xmm0

	jmp	.L1

.L2:
	sub	ecx, 1
	jnz	.L0

	pop	edx
	pop	ecx
	pop	ebx

	ret

;------------------------------------------------------------------------------
; Name:		RandomWriterVector128_nontemporal (using SSE2)
; Purpose:	Nontemporal writes of 128-bit value randomly into memory.
; Params:	
;		[esp+4]	= ptr to memory area
; 		[esp+8] = length in bytes
; 		[esp+12] = loops
; 		[esp+16] = long to write
;------------------------------------------------------------------------------
	align 64
RandomWriterVector128_nontemporal:
_RandomWriterVector128_nontemporal:
	sfence
	push	ebx
	push	ecx
	push	edx

	mov	eax, [esp+16+8]
	movd	xmm0, eax	; Create a 128-bit replication of the 32-bit
	movd	xmm1, eax	; value that was provided.
	movd	xmm2, eax
	movd	xmm3, eax
	pslldq	xmm1, 32
	pslldq	xmm2, 64
	pslldq	xmm3, 96
	por	xmm0, xmm1
	por	xmm0, xmm2
	por	xmm0, xmm3

	mov	ecx, [esp+12+12]	; loops to do.

.L0:
	mov	ebx, [esp+8+12]		; # chunks to do

.L1:
	sub	ebx, 1
	jc	.L2

	mov	edx, [esp+4+12]  	; get ptr to memory chunk.
	mov	edx, [edx + 4*ebx]

	movntdq	[128+edx], xmm0
	movntdq	[240+edx], xmm0
	movntdq	[112+edx], xmm0
	movntdq	[64+edx], xmm0
	movntdq	[176+edx], xmm0
	movntdq	[144+edx], xmm0
	movntdq	[edx], xmm0
	movntdq	[208+edx], xmm0
	movntdq	[80+edx], xmm0
	movntdq	[96+edx], xmm0
	movntdq	[48+edx], xmm0
	movntdq	[16+edx], xmm0
	movntdq	[192+edx], xmm0
	movntdq	[160+edx], xmm0
	movntdq	[224+edx], xmm0
	movntdq	[32+edx], xmm0

	jmp	.L1

.L2:
	sub	ecx, 1
	jnz	.L0

	pop	edx
	pop	ecx
	pop	ebx

	ret

;------------------------------------------------------------------------------
; Name:		RegisterToRegister
; Purpose:	Reads/writes 32-bit values between registers of 
;		the main register set.
; Params:	
; 		dword [esp+4] = loops
;------------------------------------------------------------------------------
	align 64
RegisterToRegister:
_RegisterToRegister:
	push	ebx
	push	ecx

	mov	ecx, [esp+4+8]	; loops to do.

.L1:
	mov	eax, ebx	; 64 transfers 
	mov	eax, ecx
	mov	eax, edx
	mov	eax, esi
	mov	eax, edi
	mov	eax, ebp
	mov	eax, esp
	mov	eax, ebx
	mov	eax, ebx
	mov	eax, ecx
	mov	eax, edx
	mov	eax, esi
	mov	eax, edi
	mov	eax, ebp
	mov	eax, esp
	mov	eax, ebx
	mov	eax, ebx
	mov	eax, ecx
	mov	eax, edx
	mov	eax, esi
	mov	eax, edi
	mov	eax, ebp
	mov	eax, esp
	mov	eax, ebx
	mov	eax, ebx
	mov	eax, ecx
	mov	eax, edx
	mov	eax, esi
	mov	eax, edi
	mov	eax, ebp
	mov	eax, esp
	mov	eax, ebx

	mov	ebx, eax
	mov	ebx, ecx
	mov	ebx, edx
	mov	ebx, esi
	mov	ebx, edi
	mov	ebx, ebp
	mov	ebx, esp
	mov	ebx, eax
	mov	ebx, eax
	mov	ebx, ecx
	mov	ebx, edx
	mov	ebx, esi
	mov	ebx, edi
	mov	ebx, ebp
	mov	ebx, esp
	mov	ebx, eax
	mov	ebx, eax
	mov	ebx, ecx
	mov	ebx, edx
	mov	ebx, esi
	mov	ebx, edi
	mov	ebx, ebp
	mov	ebx, esp
	mov	ebx, eax
	mov	ebx, eax
	mov	ebx, ecx
	mov	ebx, edx
	mov	ebx, esi
	mov	ebx, edi
	mov	ebx, ebp
	mov	ebx, esp
	mov	ebx, eax

	dec	ecx
	jnz	.L1

	pop	ecx
	pop	ebx

	ret

;------------------------------------------------------------------------------
; Name:		VectorToVector256
; Purpose:	Reads/writes 256-bit values between vector registers,
;		which on the x86 are YMM registers.
; Params:	dword [esp+4] = loops
;------------------------------------------------------------------------------
	align 64
VectorToVector256:
_VectorToVector256:
	vzeroupper

	mov	eax, [esp + 4]
.L1:
	vmovdqa	ymm0, ymm1
	vmovdqa	ymm0, ymm2
	vmovdqa	ymm0, ymm3
	vmovdqa	ymm2, ymm0
	vmovdqa	ymm1, ymm2
	vmovdqa	ymm2, ymm1
	vmovdqa	ymm0, ymm3
	vmovdqa	ymm3, ymm1

	vmovdqa	ymm0, ymm1
	vmovdqa	ymm0, ymm2
	vmovdqa	ymm0, ymm3
	vmovdqa	ymm2, ymm0
	vmovdqa	ymm1, ymm2
	vmovdqa	ymm2, ymm1
	vmovdqa	ymm0, ymm3
	vmovdqa	ymm3, ymm1

	vmovdqa	ymm0, ymm1
	vmovdqa	ymm0, ymm2
	vmovdqa	ymm0, ymm3
	vmovdqa	ymm2, ymm0
	vmovdqa	ymm1, ymm2
	vmovdqa	ymm2, ymm1
	vmovdqa	ymm0, ymm3
	vmovdqa	ymm3, ymm1

	vmovdqa	ymm0, ymm1
	vmovdqa	ymm0, ymm2
	vmovdqa	ymm0, ymm3
	vmovdqa	ymm2, ymm0
	vmovdqa	ymm1, ymm2
	vmovdqa	ymm2, ymm1
	vmovdqa	ymm0, ymm3
	vmovdqa	ymm3, ymm1

	dec	eax
	jnz	.L1

	ret

;------------------------------------------------------------------------------
; Name:		VectorToVector128
; Purpose:	Reads/writes 128-bit values between registers of 
;		the vector register set, in this case XMM.
; Params:	dword [esp + 4] = count.
;------------------------------------------------------------------------------
	align 64
VectorToVector128:
_VectorToVector128:
	mov	eax, [esp + 4]
.L1:
	movdqa	xmm0, xmm1
	movdqa	xmm0, xmm2
	movdqa	xmm0, xmm3
	movdqa	xmm2, xmm0
	movdqa	xmm1, xmm2
	movdqa	xmm2, xmm1
	movdqa	xmm0, xmm3
	movdqa	xmm3, xmm1
	movdqa	xmm3, xmm2
	movdqa	xmm1, xmm3
	movdqa	xmm2, xmm1
	movdqa	xmm0, xmm1
	movdqa	xmm1, xmm2
	movdqa	xmm0, xmm1
	movdqa	xmm0, xmm3
	movdqa	xmm3, xmm0
	movdqa	xmm0, xmm1
	movdqa	xmm0, xmm2
	movdqa	xmm0, xmm3
	movdqa	xmm2, xmm0
	movdqa	xmm1, xmm2
	movdqa	xmm2, xmm1
	movdqa	xmm0, xmm3
	movdqa	xmm3, xmm1
	movdqa	xmm3, xmm2
	movdqa	xmm1, xmm3
	movdqa	xmm2, xmm1
	movdqa	xmm0, xmm1
	movdqa	xmm1, xmm2
	movdqa	xmm0, xmm1
	movdqa	xmm0, xmm3
	movdqa	xmm3, xmm0

	dec	eax
	jnz	.L1

	ret

;------------------------------------------------------------------------------
; Name:		RegisterToVectorMove
; Purpose:	Moves 32-bit main register values into 128-bit vector register
;		clearing the upper unused bits.
; Params:	dword [esp + 4] = count.
;------------------------------------------------------------------------------
	align 64
RegisterToVectorMove:
_RegisterToVectorMove:
	mov 	eax, [esp + 4]
.L1:
	; 64 moves
	movd	xmm1, eax
	movd	xmm2, eax
	movd	xmm3, eax
	movd	xmm0, eax
	movd	xmm1, eax
	movd	xmm2, eax
	movd	xmm3, eax
	movd	xmm0, eax

	movd	xmm1, eax
	movd	xmm3, eax
	movd	xmm2, eax
	movd	xmm0, eax
	movd	xmm1, eax
	movd	xmm2, eax
	movd	xmm3, eax
	movd	xmm0, eax

	movd	xmm0, eax
	movd	xmm2, eax
	movd	xmm0, eax
	movd	xmm3, eax
	movd	xmm1, eax
	movd	xmm3, eax
	movd	xmm2, eax
	movd	xmm0, eax

	movd	xmm0, eax
	movd	xmm3, eax
	movd	xmm1, eax
	movd	xmm2, eax
	movd	xmm0, eax
	movd	xmm2, eax
	movd	xmm3, eax
	movd	xmm0, eax

	movd	xmm1, eax
	movd	xmm2, eax
	movd	xmm3, eax
	movd	xmm0, eax
	movd	xmm1, eax
	movd	xmm2, eax
	movd	xmm3, eax
	movd	xmm0, eax

	movd	xmm1, eax
	movd	xmm3, eax
	movd	xmm2, eax
	movd	xmm0, eax
	movd	xmm1, eax
	movd	xmm2, eax
	movd	xmm3, eax
	movd	xmm0, eax

	movd	xmm0, eax
	movd	xmm2, eax
	movd	xmm0, eax
	movd	xmm3, eax
	movd	xmm1, eax
	movd	xmm3, eax
	movd	xmm2, eax
	movd	xmm0, eax

	movd	xmm0, eax
	movd	xmm3, eax
	movd	xmm1, eax
	movd	xmm2, eax
	movd	xmm0, eax
	movd	xmm2, eax
	movd	xmm3, eax
	movd	xmm0, eax

	dec	eax
	jnz	.L1

	ret

;------------------------------------------------------------------------------
; Name:		VectorToRegisterMove
; Purpose:	Writes lowest 32 bits of vector registers into 32-bit main
;		register.
; Params:	dword [esp + 4] = count.
;------------------------------------------------------------------------------
	align 64
VectorToRegisterMove:
_VectorToRegisterMove:
	mov 	eax, [esp + 4]
	push	ebx
.L1:
	; 64 moves
	movd	ebx, xmm1	
	movd	ebx, xmm2
	movd	ebx, xmm3
	movd	ebx, xmm0
	movd	ebx, xmm1
	movd	ebx, xmm2
	movd	ebx, xmm3
	movd	ebx, xmm0

	movd	ebx, xmm1
	movd	ebx, xmm3
	movd	ebx, xmm2
	movd	ebx, xmm0
	movd	ebx, xmm1
	movd	ebx, xmm2
	movd	ebx, xmm3
	movd	ebx, xmm0

	movd	ebx, xmm0
	movd	ebx, xmm2
	movd	ebx, xmm0
	movd	ebx, xmm3
	movd	ebx, xmm1
	movd	ebx, xmm3
	movd	ebx, xmm2
	movd	ebx, xmm0

	movd	ebx, xmm0
	movd	ebx, xmm3
	movd	ebx, xmm1
	movd	ebx, xmm2
	movd	ebx, xmm0
	movd	ebx, xmm2
	movd	ebx, xmm3
	movd	ebx, xmm0

	movd	ebx, xmm1	
	movd	ebx, xmm2
	movd	ebx, xmm3
	movd	ebx, xmm0
	movd	ebx, xmm1
	movd	ebx, xmm2
	movd	ebx, xmm3
	movd	ebx, xmm0

	movd	ebx, xmm1
	movd	ebx, xmm3
	movd	ebx, xmm2
	movd	ebx, xmm0
	movd	ebx, xmm1
	movd	ebx, xmm2
	movd	ebx, xmm3
	movd	ebx, xmm0

	movd	ebx, xmm0
	movd	ebx, xmm2
	movd	ebx, xmm0
	movd	ebx, xmm3
	movd	ebx, xmm1
	movd	ebx, xmm3
	movd	ebx, xmm2
	movd	ebx, xmm0

	movd	ebx, xmm0
	movd	ebx, xmm3
	movd	ebx, xmm1
	movd	ebx, xmm2
	movd	ebx, xmm0
	movd	ebx, xmm2
	movd	ebx, xmm3
	movd	ebx, xmm0

	dec	eax
	jnz	.L1

	pop	ebx
	ret

;------------------------------------------------------------------------------
; Name:		StackReader
; Purpose:	Reads 32-bit values off the stack into registers of
;		the main register set, effectively testing L1 cache access
;		*and* effective-address calculation speed.
; Params:	
; 		dword [esp+4] = loops
;------------------------------------------------------------------------------
	align 64
StackReader:
_StackReader:
	lfence
	push	ebx
	push	ecx

	mov	ecx, [esp+4+8]	; loops to do.

.L1:
	sub	esp, 32

	; 64 transfers
	mov	eax, [esp]
	mov	eax, [esp+16]
	mov	eax, [esp+12]
	mov	eax, [esp+28]
	mov	eax, [esp+20]
	mov	eax, [esp+4]
	mov	eax, [esp]
	mov	eax, [esp+8]
	mov	eax, [esp]
	mov	eax, [esp+16]
	mov	eax, [esp+24]
	mov	eax, [esp+16]
	mov	eax, [esp+20]
	mov	eax, [esp+4]
	mov	eax, [esp+24]
	mov	eax, [esp]
	mov	eax, [esp+16]
	mov	eax, [esp]
	mov	eax, [esp+12]
	mov	eax, [esp+28]
	mov	eax, [esp+20]
	mov	eax, [esp+4]
	mov	eax, [esp+24]
	mov	eax, [esp+4]
	mov	eax, [esp+8]
	mov	eax, [esp+16]
	mov	eax, [esp+12]
	mov	eax, [esp+20]
	mov	eax, [esp+28]
	mov	eax, [esp+8]
	mov	eax, [esp+24]
	mov	eax, [esp+4]

	mov	eax, [esp]
	mov	eax, [esp+16]
	mov	eax, [esp+12]
	mov	eax, [esp+28]
	mov	eax, [esp+20]
	mov	eax, [esp+4]
	mov	eax, [esp]
	mov	eax, [esp+8]
	mov	eax, [esp]
	mov	eax, [esp+16]
	mov	eax, [esp+24]
	mov	eax, [esp+16]
	mov	eax, [esp+20]
	mov	eax, [esp+4]
	mov	eax, [esp+24]
	mov	eax, [esp]
	mov	eax, [esp+16]
	mov	eax, [esp]
	mov	eax, [esp+12]
	mov	eax, [esp+28]
	mov	eax, [esp+20]
	mov	eax, [esp+4]
	mov	eax, [esp+24]
	mov	eax, [esp+4]
	mov	eax, [esp+8]
	mov	eax, [esp+16]
	mov	eax, [esp+12]
	mov	eax, [esp+20]
	mov	eax, [esp+28]
	mov	eax, [esp+8]
	mov	eax, [esp+24]
	mov	eax, [esp+4]

	add	esp, 32

	dec	ecx
	jnz	.L1

	pop	ecx
	pop	ebx

	ret

;------------------------------------------------------------------------------
; Name:		StackWriter
; Purpose:	Writes 32-bit values into the stack from registers of
;		the main register set, effectively testing L1 cache access
;		*and* effective-address calculation speed.
; Params:	
; 		dword [esp+4] = loops
;------------------------------------------------------------------------------
	align 64
StackWriter:
_StackWriter:
	sfence
	push	ebx
	push	ecx

	mov	ecx, [esp+4+8]	; loops to do.

	xor	eax, eax
	mov	ebx, 0xffffffff

.L1:
	sub	esp, 32

	mov	[esp], eax
	mov	[esp+16], eax
	mov	[esp+12], eax
	mov	[esp+28], eax
	mov	[esp+20], eax
	mov	[esp+4], eax
	mov	[esp], eax
	mov	[esp+8], eax
	mov	[esp], eax
	mov	[esp+16], eax
	mov	[esp+24], eax
	mov	[esp+16], eax
	mov	[esp+20], eax
	mov	[esp+4], eax
	mov	[esp+24], eax
	mov	[esp], eax
	mov	[esp+16], eax
	mov	[esp], eax
	mov	[esp+12], eax
	mov	[esp+28], eax
	mov	[esp+20], eax
	mov	[esp+4], eax
	mov	[esp+24], eax
	mov	[esp+4], eax
	mov	[esp+8], eax
	mov	[esp+16], eax
	mov	[esp+12], eax
	mov	[esp+20], eax
	mov	[esp+28], eax
	mov	[esp+8], eax
	mov	[esp+24], eax
	mov	[esp+4], eax

	mov	[esp], ebx
	mov	[esp+16], ebx
	mov	[esp+12], ebx
	mov	[esp+28], ebx
	mov	[esp+20], ebx
	mov	[esp+4], ebx
	mov	[esp], ebx
	mov	[esp+8], ebx
	mov	[esp], ebx
	mov	[esp+16], ebx
	mov	[esp+24], ebx
	mov	[esp+16], ebx
	mov	[esp+20], ebx
	mov	[esp+4], ebx
	mov	[esp+24], ebx
	mov	[esp], ebx
	mov	[esp+16], ebx
	mov	[esp], ebx
	mov	[esp+12], ebx
	mov	[esp+28], ebx
	mov	[esp+20], ebx
	mov	[esp+4], ebx
	mov	[esp+24], ebx
	mov	[esp+4], ebx
	mov	[esp+8], ebx
	mov	[esp+16], ebx
	mov	[esp+12], ebx
	mov	[esp+20], ebx
	mov	[esp+28], ebx
	mov	[esp+8], ebx
	mov	[esp+24], ebx
	mov	[esp+4], ebx

	add	esp, 32

	dec	ecx
	jnz	.L1

	pop	ecx
	pop	ebx

	ret

;------------------------------------------------------------------------------
; Name:		Register8ToVector128
; Purpose:	Writes 8-bit main register values into 128-bit vector register
;		without clearing the unused bits.
; Params:	dword [esp + 4] = loops to do
;------------------------------------------------------------------------------
	align 64
Register8ToVector128:
_Register8ToVector128:
	mov	eax, [esp + 4]

.L1:
	pinsrb	xmm1, al, 0	; 64 transfers x 1 byte = 64 bytes
	pinsrb	xmm2, bl, 1
	pinsrb	xmm3, cl, 2
	pinsrb	xmm1, dl, 3
	pinsrb	xmm2, sil, 4
	pinsrb	xmm3, dil, 5
	pinsrb	xmm0, bpl, 6
	pinsrb	xmm0, spl, 7

	pinsrb	xmm0, al, 0
	pinsrb	xmm1, bl, 1
	pinsrb	xmm2, cl, 2
	pinsrb	xmm3, dl, 3
	pinsrb	xmm3, al, 4
	pinsrb	xmm2, bl, 5
	pinsrb	xmm1, bpl, 6
	pinsrb	xmm0, spl, 7

	pinsrb	xmm1, al, 0
	pinsrb	xmm2, al, 1
	pinsrb	xmm3, al, 2
	pinsrb	xmm1, al, 3
	pinsrb	xmm2, al, 4
	pinsrb	xmm3, al, 5
	pinsrb	xmm0, cl, 6
	pinsrb	xmm0, bl, 7

	pinsrb	xmm0, al, 0
	pinsrb	xmm0, al, 1
	pinsrb	xmm0, al, 2
	pinsrb	xmm0, al, 3
	pinsrb	xmm0, al, 4
	pinsrb	xmm0, al, 5
	pinsrb	xmm0, cl, 6
	pinsrb	xmm0, bl, 7

	pinsrb	xmm1, al, 0
	pinsrb	xmm2, bl, 1
	pinsrb	xmm3, cl, 2
	pinsrb	xmm1, dl, 3
	pinsrb	xmm2, sil, 4
	pinsrb	xmm3, dil, 5
	pinsrb	xmm0, bpl, 6
	pinsrb	xmm0, spl, 7

	pinsrb	xmm0, al, 10
	pinsrb	xmm1, bl, 11
	pinsrb	xmm2, cl, 12
	pinsrb	xmm3, dl, 13
	pinsrb	xmm3, dil, 14
	pinsrb	xmm2, cl, 15
	pinsrb	xmm1, al, 6
	pinsrb	xmm0, bpl, 7

	pinsrb	xmm1, al, 10
	pinsrb	xmm2, al, 11
	pinsrb	xmm3, al, 12
	pinsrb	xmm1, al, 13
	pinsrb	xmm2, al, 14
	pinsrb	xmm3, al, 15
	pinsrb	xmm0, cl, 6
	pinsrb	xmm0, bl, 7

	pinsrb	xmm0, al, 9
	pinsrb	xmm0, al, 8
	pinsrb	xmm0, al, 11
	pinsrb	xmm0, al, 3
	pinsrb	xmm0, al, 4
	pinsrb	xmm0, al, 5
	pinsrb	xmm0, cl, 6
	pinsrb	xmm0, bl, 7

	dec	eax
	jnz .L1
	ret

;------------------------------------------------------------------------------
; Name:		Register16ToVector128
; Purpose:	Writes 16-bit main register values into 128-bit vector register
;		without clearing the unused bits.
; Params:	dword [esp + 4] = loops to do
;------------------------------------------------------------------------------
	align 64
Register16ToVector128:
_Register16ToVector128:
	mov	eax, [esp + 4]
	
.L1:
	pinsrw	xmm1, ax, 0	; 64 transfers x 2 bytes = 128 bytes
	pinsrw	xmm2, bx, 1
	pinsrw	xmm3, cx, 2
	pinsrw	xmm1, dx, 3
	pinsrw	xmm2, si, 4
	pinsrw	xmm3, di, 5
	pinsrw	xmm0, bp, 6
	pinsrw	xmm0, sp, 7

	pinsrw	xmm0, ax, 0
	pinsrw	xmm1, bx, 1
	pinsrw	xmm2, cx, 2
	pinsrw	xmm3, dx, 3
	pinsrw	xmm3, si, 4
	pinsrw	xmm2, di, 5
	pinsrw	xmm1, bp, 6
	pinsrw	xmm0, sp, 7

	pinsrw	xmm1, ax, 0
	pinsrw	xmm2, ax, 1
	pinsrw	xmm3, ax, 2
	pinsrw	xmm1, ax, 3
	pinsrw	xmm2, ax, 4
	pinsrw	xmm3, ax, 5
	pinsrw	xmm0, bp, 6
	pinsrw	xmm0, bx, 7

	pinsrw	xmm0, ax, 0
	pinsrw	xmm0, ax, 1
	pinsrw	xmm0, ax, 2
	pinsrw	xmm0, ax, 3
	pinsrw	xmm0, ax, 4
	pinsrw	xmm0, ax, 5
	pinsrw	xmm0, bp, 6
	pinsrw	xmm0, bx, 7

	pinsrw	xmm1, ax, 0
	pinsrw	xmm2, bx, 1
	pinsrw	xmm3, cx, 2
	pinsrw	xmm1, dx, 3
	pinsrw	xmm2, si, 4
	pinsrw	xmm3, di, 5
	pinsrw	xmm0, bp, 6
	pinsrw	xmm0, sp, 7

	pinsrw	xmm0, ax, 0
	pinsrw	xmm1, bx, 1
	pinsrw	xmm2, cx, 2
	pinsrw	xmm3, dx, 3
	pinsrw	xmm3, si, 4
	pinsrw	xmm2, di, 5
	pinsrw	xmm1, bp, 6
	pinsrw	xmm0, sp, 7

	pinsrw	xmm1, ax, 0
	pinsrw	xmm2, ax, 1
	pinsrw	xmm3, ax, 2
	pinsrw	xmm1, ax, 3
	pinsrw	xmm2, ax, 4
	pinsrw	xmm3, ax, 5
	pinsrw	xmm0, bp, 6
	pinsrw	xmm0, bx, 7

	pinsrw	xmm0, ax, 0
	pinsrw	xmm0, ax, 1
	pinsrw	xmm0, ax, 2
	pinsrw	xmm0, ax, 3
	pinsrw	xmm0, ax, 4
	pinsrw	xmm0, ax, 5
	pinsrw	xmm0, bp, 6
	pinsrw	xmm0, bx, 7

	dec	eax
	jnz .L1
	ret

;------------------------------------------------------------------------------
; Name:		Register32ToVector128
; Purpose:	Writes 32-bit main register values into 128-bit vector register
;		without clearing the unused bits.
; Params:	dword [esp + 4] = loops to do
;------------------------------------------------------------------------------
	align 64
Register32ToVector128:
_Register32ToVector128:
	mov	eax, [esp + 4]
	
.L1:
	pinsrd	xmm1, eax, 0	; Each xfer moves 4 bytes so to move 256 bytes
	pinsrd	xmm2, ebx, 1	; we need 64 transfers.
	pinsrd	xmm3, ecx, 2
	pinsrd	xmm1, edx, 3
	pinsrd	xmm2, esi, 0
	pinsrd	xmm3, edi, 1
	pinsrd	xmm0, ebp, 2
	pinsrd	xmm0, esp, 3

	pinsrd	xmm0, eax, 0
	pinsrd	xmm1, ebx, 1
	pinsrd	xmm2, ecx, 2
	pinsrd	xmm3, edx, 3
	pinsrd	xmm3, esi, 3
	pinsrd	xmm2, edi, 2
	pinsrd	xmm1, ebp, 1
	pinsrd	xmm0, esp, 0

	pinsrd	xmm1, eax, 0
	pinsrd	xmm2, eax, 1
	pinsrd	xmm3, eax, 2
	pinsrd	xmm1, eax, 3
	pinsrd	xmm2, eax, 0
	pinsrd	xmm3, eax, 1
	pinsrd	xmm0, ebp, 2
	pinsrd	xmm0, ebx, 3

	pinsrd	xmm0, eax, 0
	pinsrd	xmm0, eax, 1
	pinsrd	xmm0, eax, 2
	pinsrd	xmm0, eax, 3
	pinsrd	xmm0, eax, 0
	pinsrd	xmm0, eax, 0
	pinsrd	xmm0, ebp, 0
	pinsrd	xmm0, ebx, 0

	pinsrd	xmm1, eax, 0	
	pinsrd	xmm2, ebx, 1
	pinsrd	xmm3, ecx, 2
	pinsrd	xmm1, edx, 3
	pinsrd	xmm2, esi, 0
	pinsrd	xmm3, edi, 1
	pinsrd	xmm0, ebp, 2
	pinsrd	xmm0, esp, 3

	pinsrd	xmm0, eax, 0
	pinsrd	xmm1, ebx, 1
	pinsrd	xmm2, ecx, 2
	pinsrd	xmm3, edx, 3
	pinsrd	xmm3, esi, 3
	pinsrd	xmm2, edi, 2
	pinsrd	xmm1, ebp, 1
	pinsrd	xmm0, esp, 0

	pinsrd	xmm1, eax, 0
	pinsrd	xmm2, eax, 1
	pinsrd	xmm3, eax, 2
	pinsrd	xmm1, eax, 3
	pinsrd	xmm2, eax, 0
	pinsrd	xmm3, eax, 1
	pinsrd	xmm0, ebp, 2
	pinsrd	xmm0, ebx, 3

	pinsrd	xmm0, eax, 0
	pinsrd	xmm0, eax, 1
	pinsrd	xmm0, eax, 2
	pinsrd	xmm0, eax, 3
	pinsrd	xmm0, eax, 0
	pinsrd	xmm0, eax, 0
	pinsrd	xmm0, ebp, 0
	pinsrd	xmm0, ebx, 0
	pinsrd	xmm0, esp, 0

	dec	eax
	jnz .L1
	ret

;------------------------------------------------------------------------------
; Name:		Register64ToVector128
; Purpose:	Writes 64-bit main register values into 128-bit vector register
;		without clearing the unused bits.
; Params:	dword [esp + 4] = loops to do
;------------------------------------------------------------------------------
Register64ToVector128:
_Register64ToVector128:
	; N/A because i386 general purpose registers (GPRs) are 32-bits.
	ret

;------------------------------------------------------------------------------
; Name:		Vector128ToRegister8
; Purpose:	Writes 8-bit vector register values into main register.
; Params:	dword [esp + 4] = loops to do
;------------------------------------------------------------------------------
	align 64
Vector128ToRegister8:
_Vector128ToRegister8:
	mov	eax, [esp + 4]

	push 	ebx
.L1:
	pextrb	ebx, xmm1, 0
	pextrb	ebx, xmm2, 1
	pextrb	ebx, xmm3, 2
	pextrb	ebx, xmm1, 3
	pextrb	ebx, xmm2, 4
	pextrb	ebx, xmm3, 5
	pextrb	ebx, xmm0, 6
	pextrb	ebx, xmm0, 7

	pextrb	ebx, xmm0, 0
	pextrb	ebx, xmm1, 1
	pextrb	ebx, xmm2, 2
	pextrb	ebx, xmm3, 3
	pextrb	ebx, xmm3, 4
	pextrb	ebx, xmm2, 15
	pextrb	ebx, xmm1, 6
	pextrb	ebx, xmm0, 7

	pextrb	ebx, xmm1, 0
	pextrb	ebx, xmm2, 1
	pextrb	ebx, xmm3, 2
	pextrb	ebx, xmm1, 3
	pextrb	ebx, xmm2, 4
	pextrb	ebx, xmm3, 5
	pextrb	ebx, xmm0, 6
	pextrb	ebx, xmm0, 7

	pextrb	ebx, xmm0, 0
	pextrb	ebx, xmm1, 1
	pextrb	ebx, xmm2, 2
	pextrb	ebx, xmm3, 3
	pextrb	ebx, xmm3, 4
	pextrb	ebx, xmm2, 5
	pextrb	ebx, xmm1, 6
	pextrb	ebx, xmm0, 7

	pextrb	ebx, xmm1, 0
	pextrb	ebx, xmm2, 1
	pextrb	ebx, xmm3, 2
	pextrb	ebx, xmm1, 13
	pextrb	ebx, xmm2, 14
	pextrb	ebx, xmm3, 15
	pextrb	ebx, xmm0, 6
	pextrb	ebx, xmm0, 7

	pextrb	ebx, xmm0, 10
	pextrb	ebx, xmm1, 11
	pextrb	ebx, xmm2, 12
	pextrb	ebx, xmm3, 13
	pextrb	ebx, xmm3, 14
	pextrb	ebx, xmm2, 15
	pextrb	ebx, xmm1, 6
	pextrb	ebx, xmm0, 7

	pextrb	ebx, xmm1, 0
	pextrb	ebx, xmm2, 1
	pextrb	ebx, xmm3, 2
	pextrb	ebx, xmm1, 3
	pextrb	ebx, xmm2, 4
	pextrb	ebx, xmm3, 5
	pextrb	ebx, xmm0, 6
	pextrb	ebx, xmm0, 7

	pextrb	ebx, xmm0, 0
	pextrb	ebx, xmm1, 1
	pextrb	ebx, xmm2, 2
	pextrb	ebx, xmm3, 3
	pextrb	ebx, xmm3, 4
	pextrb	ebx, xmm2, 5
	pextrb	ebx, xmm1, 6
	pextrb	ebx, xmm0, 7

	dec	eax
	jnz .L1
	pop	ebx
	ret

;------------------------------------------------------------------------------
; Name:		Vector128ToRegister16
; Purpose:	Writes 16-bit vector register values into main register.
; Params:	dword [esp + 4] = loops to do
;------------------------------------------------------------------------------
	align 64
Vector128ToRegister16:
_Vector128ToRegister16:
	mov	eax, [esp + 4]
	
	push 	ebx
.L1:
	pextrw	ebx, xmm1, 0
	pextrw	ebx, xmm2, 1
	pextrw	ebx, xmm3, 2
	pextrw	ebx, xmm1, 3
	pextrw	ebx, xmm2, 4
	pextrw	ebx, xmm3, 5
	pextrw	ebx, xmm0, 6
	pextrw	ebx, xmm0, 7

	pextrw	ebx, xmm0, 0
	pextrw	ebx, xmm1, 1
	pextrw	ebx, xmm2, 2
	pextrw	ebx, xmm3, 3
	pextrw	ebx, xmm3, 4
	pextrw	ebx, xmm2, 5
	pextrw	ebx, xmm1, 6
	pextrw	ebx, xmm0, 7

	pextrw	ebx, xmm1, 0
	pextrw	ebx, xmm2, 1
	pextrw	ebx, xmm3, 2
	pextrw	ebx, xmm1, 3
	pextrw	ebx, xmm2, 4
	pextrw	ebx, xmm3, 5
	pextrw	ebx, xmm0, 6
	pextrw	ebx, xmm0, 7

	pextrw	ebx, xmm0, 0
	pextrw	ebx, xmm1, 1
	pextrw	ebx, xmm2, 2
	pextrw	ebx, xmm3, 3
	pextrw	ebx, xmm3, 4
	pextrw	ebx, xmm2, 5
	pextrw	ebx, xmm1, 6
	pextrw	ebx, xmm0, 7

	pextrw	ebx, xmm1, 0
	pextrw	ebx, xmm2, 1
	pextrw	ebx, xmm3, 2
	pextrw	ebx, xmm1, 3
	pextrw	ebx, xmm2, 4
	pextrw	ebx, xmm3, 5
	pextrw	ebx, xmm0, 6
	pextrw	ebx, xmm0, 7

	pextrw	ebx, xmm0, 0
	pextrw	ebx, xmm1, 1
	pextrw	ebx, xmm2, 2
	pextrw	ebx, xmm3, 3
	pextrw	ebx, xmm3, 4
	pextrw	ebx, xmm2, 5
	pextrw	ebx, xmm1, 6
	pextrw	ebx, xmm0, 7

	pextrw	ebx, xmm1, 0
	pextrw	ebx, xmm2, 1
	pextrw	ebx, xmm3, 2
	pextrw	ebx, xmm1, 3
	pextrw	ebx, xmm2, 4
	pextrw	ebx, xmm3, 5
	pextrw	ebx, xmm0, 6
	pextrw	ebx, xmm0, 7

	pextrw	ebx, xmm0, 0
	pextrw	ebx, xmm1, 1
	pextrw	ebx, xmm2, 2
	pextrw	ebx, xmm3, 3
	pextrw	ebx, xmm3, 4
	pextrw	ebx, xmm2, 5
	pextrw	ebx, xmm1, 6
	pextrw	ebx, xmm0, 7

	dec	eax
	jnz .L1
	pop	ebx
	ret

;------------------------------------------------------------------------------
; Name:		Vector128ToRegister32
; Purpose:	Writes 32-bit vector register values into main register.
; Params:	eax = loops
;------------------------------------------------------------------------------
	align 64
Vector128ToRegister32:
_Vector128ToRegister32:
	mov	eax, [esp + 4]
	push 	ebx
.L1:
	pextrd	ebx, xmm1, 0
	pextrd	ebx, xmm2, 1
	pextrd	ebx, xmm3, 2
	pextrd	ebx, xmm1, 3
	pextrd	ebx, xmm2, 0
	pextrd	ebx, xmm3, 1
	pextrd	ebx, xmm0, 2
	pextrd	ebx, xmm0, 3

	pextrd	ebx, xmm0, 0
	pextrd	ebx, xmm1, 1
	pextrd	ebx, xmm2, 2
	pextrd	ebx, xmm3, 3
	pextrd	ebx, xmm3, 3
	pextrd	ebx, xmm2, 2
	pextrd	ebx, xmm1, 1
	pextrd	ebx, xmm0, 0

	pextrd	ebx, xmm1, 0
	pextrd	ebx, xmm2, 1
	pextrd	ebx, xmm3, 2
	pextrd	ebx, xmm1, 3
	pextrd	ebx, xmm2, 0
	pextrd	ebx, xmm3, 1
	pextrd	ebx, xmm0, 2
	pextrd	ebx, xmm0, 3

	pextrd	ebx, xmm0, 0
	pextrd	ebx, xmm1, 1
	pextrd	ebx, xmm2, 2
	pextrd	ebx, xmm3, 3
	pextrd	ebx, xmm3, 3
	pextrd	ebx, xmm2, 2
	pextrd	ebx, xmm1, 1
	pextrd	ebx, xmm0, 0

	pextrd	ebx, xmm1, 0
	pextrd	ebx, xmm2, 1
	pextrd	ebx, xmm3, 2
	pextrd	ebx, xmm1, 3
	pextrd	ebx, xmm2, 0
	pextrd	ebx, xmm3, 1
	pextrd	ebx, xmm0, 2
	pextrd	ebx, xmm0, 3

	pextrd	ebx, xmm0, 0
	pextrd	ebx, xmm1, 1
	pextrd	ebx, xmm2, 2
	pextrd	ebx, xmm3, 3
	pextrd	ebx, xmm3, 3
	pextrd	ebx, xmm2, 2
	pextrd	ebx, xmm1, 1
	pextrd	ebx, xmm0, 0

	pextrd	ebx, xmm1, 0
	pextrd	ebx, xmm2, 1
	pextrd	ebx, xmm3, 2
	pextrd	ebx, xmm1, 3
	pextrd	ebx, xmm2, 0
	pextrd	ebx, xmm3, 1
	pextrd	ebx, xmm0, 2
	pextrd	ebx, xmm0, 3

	pextrd	ebx, xmm0, 0
	pextrd	ebx, xmm1, 1
	pextrd	ebx, xmm2, 2
	pextrd	ebx, xmm3, 3
	pextrd	ebx, xmm3, 3
	pextrd	ebx, xmm2, 2
	pextrd	ebx, xmm1, 1
	pextrd	ebx, xmm0, 0

	dec	eax
	jnz .L1
	pop	ebx
	ret

;------------------------------------------------------------------------------
; Name:		Vector128ToRegister64
; Purpose:	Writes 64-bit vector register values into main register.
; Params:	dword [esp + 4] = loops to do
;------------------------------------------------------------------------------
Vector128ToRegister64:
_Vector128ToRegister64:
	; N/A because i386 has 32-bit GPRs.
	ret

;------------------------------------------------------------------------------
; Name:		CopyWithMainRegisters
; Purpose:	Copies memory chunks using the 32-bit main registers.
; Params:	dword [esp + 4] = pointer to destination 
;		dword [esp + 8] = pointer to source memory area
; 		dword [esp + 12] = length in bytes
; 		dword [esp + 16] = loops
;------------------------------------------------------------------------------
	align 64
CopyWithMainRegisters:
_CopyWithMainRegisters:
	mfence
	mov	eax, [esp + 16]

	push	esi
	push	edi
	push	ebx
	push	ecx
	push	edx
	push	ebp

	; eax = loops
	; esi = source
	; edi = destination
	; ebp = length
	; ebx, ecx, edx = for copying

.L1:
	mov	edi, [esp + 4 + 24]
	mov	esi, [esp + 8 + 24]
	mov	ebp, [esp + 12 + 24]

	; Make sure length is a multiple of 64.
	shr	ebp, 6
	shl	ebp, 6

.L2:
	mov	ebx, [esi]
	mov	ecx, [esi+4]
	mov	edx, [esi+8]
	mov	[edi], ebx
	mov	[edi+4], ecx
	mov	[edi+8], edx
	lea	esi, [esi+12]
	lea	edi, [edi+12]

	mov	ebx, [esi]
	mov	ecx, [esi+4]
	mov	edx, [esi+8]
	mov	[edi], ebx
	mov	[edi+4], ecx
	mov	[edi+8], edx
	lea	esi, [esi+12]
	lea	edi, [edi+12]

	mov	ebx, [esi]
	mov	ecx, [esi+4]
	mov	edx, [esi+8]
	mov	[edi], ebx
	mov	[edi+4], ecx
	mov	[edi+8], edx
	lea	esi, [esi+12]
	lea	edi, [edi+12]

	mov	ebx, [esi]
	mov	ecx, [esi+4]
	mov	edx, [esi+8]
	mov	[edi], ebx
	mov	[edi+4], ecx
	mov	[edi+8], edx
	lea	esi, [esi+12]
	lea	edi, [edi+12]

	mov	ebx, [esi]
	mov	ecx, [esi+4]
	mov	edx, [esi+8]
	mov	[edi], ebx
	mov	[edi+4], ecx
	mov	[edi+8], edx
	mov	ebx, [esi+12]
	mov	[edi+12], ebx
	lea	esi, [esi+16]
	lea	edi, [edi+16]

	sub	ebp, 64
	jnz	.L2

	dec	eax
	jnz	.L1

	pop	ebp
	pop	edx
	pop	ecx
	pop	ebx
	pop	edi
	pop	esi

	ret


;------------------------------------------------------------------------------
; Name:		CopyVector256
; Purpose:	Copies memory chunks that are 32-byte aligned.
; Params:	[esp + 4]	= ptr to destination memory area
;		[esp + 8]	= ptr to source memory area
; 		[esp + 12]	= length in bytes
; 		[esp + 16]	= loops
;------------------------------------------------------------------------------
	align 64
CopyVector256:
_CopyVector256:
	mfence
	vzeroupper
	; Register usage:
	; esi = source
	; edi = dest
	; ecx = loops
	; edx = length
	push	esi
	push	edi
	push	ecx
	push	edx

	mov	edi, [esp + 4 + 16]
	mov	esi, [esp + 8 + 16]
	mov	edx, [esp + 12 + 16]
	mov	ecx, [esp + 16 + 16]

	shr	edx, 8	; Ensure length is multiple of 256.
	shl	edx, 8

.L1:
	mov	eax, edx

.L2:
	vmovdqa	ymm0, [esi]
	vmovdqa	ymm1, [32+esi]
	vmovdqa	ymm2, [64+esi]
	vmovdqa	ymm3, [96+esi]

	vmovdqa	[edi], ymm0
	vmovdqa	[32+edi], ymm1
	vmovdqa	[64+edi], ymm2
	vmovdqa	[96+edi], ymm3

	vmovdqa	ymm0, [128+esi]
	vmovdqa	ymm1, [128+32+esi]
	vmovdqa	ymm2, [128+64+esi]
	vmovdqa	ymm3, [128+96+esi]

	vmovdqa	[128+edi], ymm0
	vmovdqa	[128+32+edi], ymm1
	vmovdqa	[128+64+edi], ymm2
	vmovdqa	[128+96+edi], ymm3

	add	esi, 256
	add	edi, 256

	sub	eax, 256
	jnz	.L2

	sub	esi, edx	
	sub	edi, edx	

	dec	ecx
	jnz	.L1

	pop	edx
	pop	ecx
	pop	edi
	pop	esi

	ret

;------------------------------------------------------------------------------
; Name:		CopyVector128
; Purpose:	Copies memory chunks that are 16-byte aligned.
; Params:	[esp + 4]	= ptr to destination memory area
;		[esp + 8]	= ptr to source memory area
; 		[esp + 12]	= length in bytes
; 		[esp + 16]	= loops
;------------------------------------------------------------------------------
	align 64
CopyVector128:
_CopyVector128:
	mfence
	; Register usage:
	; esi = source
	; edi = dest
	; ecx = loops
	; edx = length
	push	esi
	push	edi
	push	ecx
	push	edx

	mov	edi, [esp + 4 + 16]
	mov	esi, [esp + 8 + 16]
	mov	edx, [esp + 12 + 16]
	mov	ecx, [esp + 16 + 16]

	shr	edx, 7	; Ensure length is multiple of 128.
	shl	edx, 7

	; Save our non-parameter XMM registers.
	sub	esp, 64
	movdqu	[esp], xmm4
	movdqu	[16+esp], xmm5
	movdqu	[32+esp], xmm6
	movdqu	[48+esp], xmm7

.L1:
	mov	eax, edx

.L2:
	movdqa	xmm0, [esi]
	movdqa	xmm1, [16+esi]
	movdqa	xmm2, [32+esi]
	movdqa	xmm3, [48+esi]
	movdqa	xmm4, [64+esi]
	movdqa	xmm5, [80+esi]
	movdqa	xmm6, [96+esi]
	movdqa	xmm7, [112+esi]

	; NOTE! 32-bit lacks xmm8 - xmm15.

	movdqa	[edi], xmm0
	movdqa	[16+edi], xmm1
	movdqa	[32+edi], xmm2
	movdqa	[48+edi], xmm3
	movdqa	[64+edi], xmm4
	movdqa	[80+edi], xmm5
	movdqa	[96+edi], xmm6
	movdqa	[112+edi], xmm7

	add	esi, 128
	add	edi, 128

	sub	eax, 128
	jnz	.L2

	sub	esi, edx	
	sub	edi, edx	

	dec	ecx
	jnz	.L1

	movdqu	xmm4, [0+esp]
	movdqu	xmm5, [16+esp]
	movdqu	xmm6, [32+esp]
	movdqu	xmm7, [48+esp]
	add	esp, 64

	pop	edx
	pop	ecx
	pop	edi
	pop	esi

	ret

;------------------------------------------------------------------------------
; Name:		IncrementRegisters
; Purpose:	Increments 32-bit values in registers.
; Params:	[esp + 4]	= loops
;------------------------------------------------------------------------------
	align 64
IncrementRegisters:
_IncrementRegisters:
	push	ebp
	mov	ebp, [esp+8]
.L1:
	inc	eax
	inc	ebx
	inc	ecx
	inc	edx
	inc	esi
	inc	edi
	inc	ebp
	inc	esp

	dec	eax
	dec	ebx
	dec	ecx
	dec	edx
	dec	esi
	dec	edi
	dec	ebp
	dec	esp

	inc	eax
	inc	ebx
	inc	ecx
	inc	edx
	inc	esi
	inc	edi
	inc	ebp
	inc	esp

	dec	eax
	dec	ebx
	dec	ecx
	dec	edx
	dec	esi
	dec	edi
	dec	ebp
	dec	esp

	dec	ebp
	jnz	.L1
	pop	ebp

	ret


;------------------------------------------------------------------------------
; Name:		IncrementStack
; Purpose:	Increments 32-bit values on stack.
; Params:	[esp + 4]	= loops
;------------------------------------------------------------------------------
	align 64
IncrementStack:
_IncrementStack:
	mfence
	push	ebp
	mov	ebp, [esp+8]
	sub	esp, 64
.L1:
	inc	dword [esp]
	inc	dword [esp+4]
	inc	dword [esp+8]
	inc	dword [esp+12]
	inc	dword [esp+16]
	inc	dword [esp+20]
	inc	dword [esp+24]
	inc	dword [esp+28]
	inc	dword [esp+32]
	inc	dword [esp+36]
	inc	dword [esp+40]
	inc	dword [esp+44]
	inc	dword [esp+48]
	inc	dword [esp+52]
	inc	dword [esp+56]
	inc	dword [esp+60]

	inc	dword [esp]
	inc	dword [esp+4]
	inc	dword [esp+8]
	inc	dword [esp+12]
	inc	dword [esp+16]
	inc	dword [esp+20]
	inc	dword [esp+24]
	inc	dword [esp+28]
	inc	dword [esp+32]
	inc	dword [esp+36]
	inc	dword [esp+40]
	inc	dword [esp+44]
	inc	dword [esp+48]
	inc	dword [esp+52]
	inc	dword [esp+56]
	inc	dword [esp+60]

	dec	ebp
	jnz	.L1

	add	esp, 64
	pop	ebp

	ret

Reader_nontemporal:
	ret

; AVX not supported on i386:
ReaderVector256_nontemporal:
RandomWriterVector256_nontemporal:
RandomReaderVector256:
RandomWriterVector256:
	ret

; AVX512 not supported on i386:
ReaderVector512:
WriterVector512:
ReaderVector512_nontemporal:
WriterVector512_nontemporal:
CopyVector512:
VectorToVector512:
	ret


