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

%ifidn __OUTPUT_FORMAT__, win64
; Windows 64 ABI says integer param are put in these registers in this order:
;	rcx, rdx, r8, r9 (floating point is xmm0)
	%define P1 rcx
	%define P2 rdx
	%define P3 r8
	%define P4 r9
%else
; Unix ABI says integer param are put in these registers in this order:
;	rdi, rsi, rdx, rcx, r8, r9
	%define P1 rdi
	%define P2 rsi
	%define P3 rdx
	%define P4 rcx
	%define P5 r8
	%define P6 r9
%endif

bits	64
cpu	ia64

global	CopyVector128
global	CopyVector256
global	CopyVector512
global	CopyWithMainRegisters

global	IncrementRegisters
global	IncrementStack

global	RandomReader
global	RandomReaderVector128
global	RandomReaderVector128_nontemporal
global	RandomReaderVector256
global	RandomWriter
global	RandomWriterVector128
global	RandomWriterVector128_nontemporal
global	RandomWriterVector256
global	RandomWriterVector256_nontemporal
global	Reader
global	ReaderVector128
global	ReaderVector128_nontemporal
global	ReaderVector256
global	ReaderVector256_nontemporal
global	ReaderVector512
global	ReaderVector512_nontemporal
global	Reader_nontemporal
global	Register16ToVector128
global	Register32ToVector128
global	Register64ToVector128
global	Register8ToVector128
global	RegisterToRegister
global	StackReader
global	StackWriter
global	Vector128ToRegister16
global	Vector128ToRegister32
global	Vector128ToRegister64
global	Vector128ToRegister8
global	VectorToVector128
global	VectorToVector256
global	VectorToVector512
global	Writer
global	WriterVector128
global	WriterVector128_nontemporal
global	WriterVector256
global	WriterVector256_nontemporal
global	WriterVector512
global	WriterVector512_nontemporal
global	Writer_nontemporal
global	_CopyVector128
global	_CopyVector256
global	_CopyVector512
global	_CopyWithMainRegisters
global	_IncrementRegisters
global	_IncrementStack
global	_RandomReader
global	_RandomReaderVector128
global	_RandomReaderVector128_nontemporal
global	_RandomReaderVector256
global	_RandomWriter
global	_RandomWriterVector128
global	_RandomWriterVector128_nontemporal
global	_RandomWriterVector256
global	_RandomWriterVector256_nontemporal
global	_Reader
global	_ReaderVector128
global	_ReaderVector128_nontemporal
global	_ReaderVector256
global	_ReaderVector256_nontemporal
global	_ReaderVector512
global	_ReaderVector512_nontemporal
global	_Reader_nontemporal
global	_Register16ToVector128
global	_Register32ToVector128
global	_Register64ToVector128
global	_Register8ToVector128
global	_RegisterToRegister
global	_StackReader
global	_StackWriter
global	_Vector128ToRegister16
global	_Vector128ToRegister32
global	_Vector128ToRegister64
global	_Vector128ToRegister8
global	_VectorToVector128
global	_VectorToVector256
global	_VectorToVector512
global	_Writer
global	_WriterVector128
global	_WriterVector128_nontemporal
global	_WriterVector256
global	_WriterVector256_nontemporal
global	_WriterVector512
global	_WriterVector512_nontemporal
global	_Writer_nontemporal

global	VectorToRegisterMove
global	_VectorToRegisterMove
global	RegisterToVectorMove
global	_RegisterToVectorMove

global	RowHammerTest 
global	_RowHammerTest 

%ifidn __OUTPUT_FORMAT__, elf64
	; Not compatible with macOS:
	section .note.GNU-stack 
%endif

	section .text

;------------------------------------------------------------------------------
; Name:		Reader
; Purpose:	Reads 64-bit values sequentially from an area of memory.
; Params:	P1 = ptr to memory area
; 		P2 = length in bytes
; 		P3 = loops
;------------------------------------------------------------------------------
	align 64
Reader:
_Reader:
	mfence
	push	r10

	add	P2, P1	; P2 now points to end.

.L1:
	mov	r10, P1

.L2:
	mov	rax, [r10]
	mov	rax, [8+r10]
	mov	rax, [16+r10]
	mov	rax, [24+r10]
	mov	rax, [32+r10]
	mov	rax, [40+r10]
	mov	rax, [48+r10]
	mov	rax, [56+r10]
	mov	rax, [64+r10]
	mov	rax, [72+r10]
	mov	rax, [80+r10]
	mov	rax, [88+r10]
	mov	rax, [96+r10]
	mov	rax, [104+r10]
	mov	rax, [112+r10]
	mov	rax, [120+r10]
	mov	rax, [128+r10]
	mov	rax, [136+r10]
	mov	rax, [144+r10]
	mov	rax, [152+r10]
	mov	rax, [160+r10]
	mov	rax, [168+r10]
	mov	rax, [176+r10]
	mov	rax, [184+r10]
	mov	rax, [192+r10]
	mov	rax, [200+r10]
	mov	rax, [208+r10]
	mov	rax, [216+r10]
	mov	rax, [224+r10]
	mov	rax, [232+r10]
	mov	rax, [240+r10]
	mov	rax, [248+r10]

	add	r10, 256
	cmp	r10, P2
	jb	.L2

	dec	P3
	jnz	.L1

	pop	r10

	ret

;------------------------------------------------------------------------------
; Name:		RandomReader
; Purpose:	Reads 64-bit values randomly from an area of memory.
; Params:	P1 = ptr to array of chunk pointers
; 		P2 = # of chunks
; 		P3 = loops
;------------------------------------------------------------------------------
	align 64
RandomReader:
_RandomReader:
	mfence
	push	r10
	push	r11

.L1:
	xor	r11, r11

.L2:
	mov	r10, [P1 + 8*r11]	; Note, 64-bit pointers.

	mov	rax, [96+r10]
	mov	rax, [r10]
	mov	rax, [120+r10]
	mov	rax, [184+r10]
	mov	rax, [160+r10]
	mov	rax, [176+r10]
	mov	rax, [112+r10]
	mov	rax, [80+r10]
	mov	rax, [32+r10]
	mov	rax, [128+r10]
	mov	rax, [88+r10]
	mov	rax, [40+r10]
	mov	rax, [48+r10]
	mov	rax, [72+r10]
	mov	rax, [200+r10]
	mov	rax, [24+r10]
	mov	rax, [152+r10]
	mov	rax, [16+r10]
	mov	rax, [248+r10]
	mov	rax, [56+r10]
	mov	rax, [240+r10]
	mov	rax, [208+r10]
	mov	rax, [104+r10]
	mov	rax, [216+r10]
	mov	rax, [136+r10]
	mov	rax, [232+r10]
	mov	rax, [64+r10]
	mov	rax, [224+r10]
	mov	rax, [144+r10]
	mov	rax, [192+r10]
	mov	rax, [8+r10]
	mov	rax, [168+r10]

	inc	r11
	cmp	r11, P2
	jb	.L2

	dec	P3
	jnz	.L1

	pop	r11
	pop	r10

	ret

;------------------------------------------------------------------------------
; Name:		RandomReaderVector128
; Purpose:	Reads 128-bit values randomly from an area of memory.
; Params:	P1 = ptr to array of chunk pointers
; 		P2 = # of chunks
; 		P3 = loops
;------------------------------------------------------------------------------
	align 64
RandomReaderVector128:
_RandomReaderVector128:
	mfence
	push	r10
	push	r11

.L1:
	xor	r11, r11

.L2:
	mov	r10, [P1 + 8*r11]

	movdqa	xmm0, [240+r10]
	movdqa	xmm0, [128+r10]
	movdqa	xmm0, [64+r10]
	movdqa	xmm0, [208+r10]
	movdqa	xmm0, [112+r10]
	movdqa	xmm0, [176+r10]
	movdqa	xmm0, [144+r10]
	movdqa	xmm0, [r10]
	movdqa	xmm0, [96+r10]
	movdqa	xmm0, [16+r10]
	movdqa	xmm0, [192+r10]
	movdqa	xmm0, [160+r10]
	movdqa	xmm0, [32+r10]
	movdqa	xmm0, [48+r10]
	movdqa	xmm0, [224+r10]
	movdqa	xmm0, [80+r10]

	inc	r11
	cmp	r11, P2
	jb	.L2

	dec	P3
	jnz	.L1

	pop	r11
	pop	r10

	ret

;------------------------------------------------------------------------------
; Name:		RandomReaderVector128_nontemporal (using SSE4)
; Purpose:	Reads 128-bit values randomly from an area of memory.
; Params:	P1 = ptr to array of chunk pointers
; 		P2 = # of chunks
; 		P3 = loops
;------------------------------------------------------------------------------
	align 64
RandomReaderVector128_nontemporal:
_RandomReaderVector128_nontemporal:
	mfence
	push	r10
	push	r11

.L1:
	xor	r11, r11

.L2:
	mov	r10, [P1 + 8*r11]

	prefetchnta	[r10+192]
	movntdqa	xmm0, [240+r10]
	prefetchnta	[r10]
	movntdqa	xmm0, [r10]
	prefetchnta	[r10+128]
	movntdqa	xmm0, [128+r10]
	prefetchnta	[r10+64]
	movntdqa	xmm0, [64+r10]
	movntdqa	xmm0, [208+r10]
	movntdqa	xmm0, [112+r10]
	movntdqa	xmm0, [48+r10]
	movntdqa	xmm0, [176+r10]
	movntdqa	xmm0, [144+r10]
	movntdqa	xmm0, [96+r10]
	movntdqa	xmm0, [16+r10]
	movntdqa	xmm0, [160+r10]
	movntdqa	xmm0, [32+r10]
	movntdqa	xmm0, [224+r10]
	movntdqa	xmm0, [80+r10]
	movntdqa	xmm0, [192+r10]

	inc	r11
	cmp	r11, P2
	jb	.L2

	dec	P3
	jnz	.L1

	pop	r11
	pop	r10

	ret

;------------------------------------------------------------------------------
; Name:		RandomWriter
; Purpose:	Writes 64-bit values randomly to an area of memory.
; Params:	P1 = ptr to array of chunk pointers
; 		P2 = # of chunks
; 		P3 = loops
; 		P4 = datum to write
;------------------------------------------------------------------------------
	align 64
RandomWriter:
_RandomWriter:
	sfence
	push	r10
	push	r11

.L1:
	xor	r11, r11

.L2:
	mov	r10, [P1 + 8*r11]	; Note, 64-bit pointers.

	mov	[96+r10], P4
	mov	[r10], P4
	mov	[120+r10], P4
	mov	[184+r10], P4
	mov	[160+r10], P4
	mov	[176+r10], P4
	mov	[112+r10], P4
	mov	[80+r10], P4
	mov	[32+r10], P4
	mov	[128+r10], P4
	mov	[88+r10], P4
	mov	[40+r10], P4
	mov	[48+r10], P4
	mov	[72+r10], P4
	mov	[200+r10], P4
	mov	[24+r10], P4
	mov	[152+r10], P4
	mov	[16+r10], P4
	mov	[248+r10], P4
	mov	[56+r10], P4
	mov	[240+r10], P4
	mov	[208+r10], P4
	mov	[104+r10], P4
	mov	[216+r10], P4
	mov	[136+r10], P4
	mov	[232+r10], P4
	mov	[64+r10], P4
	mov	[224+r10], P4
	mov	[144+r10], P4
	mov	[192+r10], P4
	mov	[8+r10], P4
	mov	[168+r10], P4

	inc	r11
	cmp	r11, P2
	jb	.L2

	dec	P3
	jnz	.L1

	pop	r11
	pop	r10

	ret

;------------------------------------------------------------------------------
; Name:		RandomWriterVector128
; Purpose:	Writes 128-bit values randomly to an area of memory.
; Params:	P1 = ptr to array of chunk pointers
; 		P2 = # of chunks
; 		P3 = loops
; 		P4 = datum to write
;------------------------------------------------------------------------------
	align 64
RandomWriterVector128:
_RandomWriterVector128:
	sfence
	push	r10
	push	r11

	movq	xmm0, P4	; Create duplicated 128-bit datum
	movq	xmm1, P4
	pslldq	xmm1, 64
	por	xmm0, xmm1

.L1:
	xor	r11, r11

.L2:
	mov	r10, [P1 + 8*r11]	; Note, 64-bit pointers.

	movdqa	[240+r10], xmm0
	movdqa	[128+r10], xmm0
	movdqa	[208+r10], xmm0
	movdqa	[112+r10], xmm0
	movdqa	[64+r10], xmm0
	movdqa	[176+r10], xmm0
	movdqa	[144+r10], xmm0
	movdqa	[r10], xmm0
	movdqa	[96+r10], xmm0
	movdqa	[16+r10], xmm0
	movdqa	[192+r10], xmm0
	movdqa	[160+r10], xmm0
	movdqa	[32+r10], xmm0
	movdqa	[48+r10], xmm0
	movdqa	[224+r10], xmm0
	movdqa	[80+r10], xmm0

	inc	r11
	cmp	r11, P2
	jb	.L2

	dec	P3
	jnz	.L1

	pop	r11
	pop	r10

	ret

;------------------------------------------------------------------------------
; Name:		RandomWriterVector128_nontemporal
; Purpose:	Nontemporal writes of 128-bit values randomly into memory.
; Params:	P1 = ptr to array of chunk pointers
; 		P2 = # of chunks
; 		P3 = loops
; 		P4 = datum to write
;------------------------------------------------------------------------------
	align 64
RandomWriterVector128_nontemporal:
_RandomWriterVector128_nontemporal:
	sfence
	push	r10
	push	r11

	movq	xmm0, P4	; Create duplicated 128-bit datum
	movq	xmm1, P4
	pslldq	xmm1, 64
	por	xmm0, xmm1

.L1:
	xor	r11, r11

.L2:
	mov	r10, [P1 + 8*r11]	; Note, 64-bit pointers.

	movntdq	[240+r10], xmm0
	movntdq	[128+r10], xmm0
	movntdq	[208+r10], xmm0
	movntdq	[112+r10], xmm0
	movntdq	[64+r10], xmm0
	movntdq	[176+r10], xmm0
	movntdq	[144+r10], xmm0
	movntdq	[r10], xmm0
	movntdq	[96+r10], xmm0
	movntdq	[16+r10], xmm0
	movntdq	[192+r10], xmm0
	movntdq	[160+r10], xmm0
	movntdq	[32+r10], xmm0
	movntdq	[48+r10], xmm0
	movntdq	[224+r10], xmm0
	movntdq	[80+r10], xmm0

	inc	r11
	cmp	r11, P2
	jb	.L2

	dec	P3
	jnz	.L1

	pop	r11
	pop	r10

	sfence
	ret

;------------------------------------------------------------------------------
; Name:		ReaderVector128
; Purpose:	Reads 128-bit values sequentially from an area of memory.
; Params:	P1 = ptr to memory area
; 		P2 = length in bytes
; 		P3 = loops
;------------------------------------------------------------------------------
	align 64
ReaderVector128:
_ReaderVector128:
	lfence
	push	r10

	add	P2, P1	; P2 now points to end.

.L1:
	mov	r10, P1

.L2:
	movdqa	xmm0, [r10]	; Read aligned to 16-byte boundary.
	movdqa	xmm0, [16+r10]
	movdqa	xmm0, [32+r10]
	movdqa	xmm0, [48+r10]
	movdqa	xmm0, [64+r10]
	movdqa	xmm0, [80+r10]
	movdqa	xmm0, [96+r10]
	movdqa	xmm0, [112+r10]

	movdqa	xmm0, [128+r10]
	movdqa	xmm0, [144+r10]
	movdqa	xmm0, [160+r10]
	movdqa	xmm0, [176+r10]
	movdqa	xmm0, [192+r10]
	movdqa	xmm0, [208+r10]
	movdqa	xmm0, [224+r10]
	movdqa	xmm0, [240+r10]

	add	r10, 256
	cmp	r10, P2
	jb	.L2

	dec	P3
	jnz	.L1
	
	pop	r10

	ret

;------------------------------------------------------------------------------
; Name:		ReaderVector256 (using AVX)
; Purpose:	Reads 256-bit values sequentially from an area of memory.
; Params:	P1 = ptr to memory area
; 		P2 = length in bytes
; 		P3 = loops
;------------------------------------------------------------------------------
	align 64
ReaderVector256:
_ReaderVector256:
	lfence
	sub	rsp, 32
	vmovdqu	[rsp], ymm0
	sub	rsp, 32
	vmovdqu	[rsp], ymm1
	sub	rsp, 32
	vmovdqu	[rsp], ymm2
	sub	rsp, 32
	vmovdqu	[rsp], ymm3

	push	r10

	add	P2, P1	; P2 now points to end.

.L1:
	mov	r10, P1

.L2:
	vmovdqa	ymm0, [r10]	; Read aligned to 32-byte boundary.
	vmovdqa	ymm1, [32+r10]
	vmovdqa	ymm2, [64+r10]
	vmovdqa	ymm3, [96+r10]
	vmovdqa	ymm0, [128+r10]
	vmovdqa	ymm1, [160+r10]
	vmovdqa	ymm2, [192+r10]
	vmovdqa	ymm3, [224+r10]

	add	r10, 256
	cmp	r10, P2
	jb	.L2

	dec	P3
	jnz	.L1
	
	pop	r10
	vmovdqu	ymm3, [rsp]
	add	rsp, 32
	vmovdqu	ymm2, [rsp]
	add	rsp, 32
	vmovdqu	ymm1, [rsp]
	add	rsp, 32
	vmovdqu	ymm0, [rsp]
	add	rsp, 32

	ret

;------------------------------------------------------------------------------
; Name:		ReaderVector512 (using AVX-512)
; Purpose:	Reads 512-bit values sequentially from an area of memory.
; Params:	P1 = ptr to memory area
; 		P2 = length in bytes
; 		P3 = loops
;------------------------------------------------------------------------------
	align 64
ReaderVector512:
_ReaderVector512:
	lfence
	sub	rsp, 64
	vmovdqu64	[rsp], zmm0
	sub	rsp, 64
	vmovdqu64	[rsp], zmm1
	sub	rsp, 64
	vmovdqu64	[rsp], zmm2
	sub	rsp, 64
	vmovdqu64	[rsp], zmm3

	push	r10

	add	P2, P1	; P2 now points to end.

.L1:
	mov	r10, P1

.L2:
	vmovdqu64	zmm0, [r10]	; Read aligned to 64-byte boundary.
	vmovdqu64	zmm1, [64+r10]
	vmovdqu64	zmm2, [128+r10]
	vmovdqu64	zmm3, [192+r10]

	add	r10, 256
	cmp	r10, P2
	jb	.L2

	dec	P3
	jnz	.L1
	
	pop	r10

	vmovdqu64	zmm3, [rsp]
	add	rsp, 64
	vmovdqu64	zmm2, [rsp]
	add	rsp, 64
	vmovdqu64	zmm1, [rsp]
	add	rsp, 64
	vmovdqu64	zmm0, [rsp]
	add	rsp, 64

	ret

;------------------------------------------------------------------------------
; Name:		RandomReaderVector256 (using AVX)
; Purpose:	Reads 256-bit values in somewhat random order from RAM.
; Params:	P1 = ptr to chunk pointers 
; 		P2 = # chunks
; 		P3 = loops
;------------------------------------------------------------------------------
	align 64
RandomReaderVector256:
_RandomReaderVector256:
	lfence
	sub	rsp, 32
	vmovdqu	[rsp], ymm0
	sub	rsp, 32
	vmovdqu	[rsp], ymm1
	sub	rsp, 32
	vmovdqu	[rsp], ymm2
	sub	rsp, 32
	vmovdqu	[rsp], ymm3

	push	r10
	push	r11

.L1:
	xor	r11, r11
.L2:
        mov     r10, [P1 + 8*r11]

	; Read aligned to 32-byte boundary.
	vmovdqa	ymm0, [96+r10]
	vmovdqa	ymm1, [192+r10]
	vmovdqa	ymm2, [64+r10]
	vmovdqa	ymm3, [224+r10]
	vmovdqa	ymm0, [r10]	
	vmovdqa	ymm1, [160+r10]
	vmovdqa	ymm2, [128+r10]
	vmovdqa	ymm3, [32+r10]

	inc	r11
	cmp	r11, P2
	jb	.L2

	dec	P3
	jnz	.L1
	
	pop	r11
	pop	r10

	vmovdqu	ymm3, [rsp]
	add	rsp, 32
	vmovdqu	ymm2, [rsp]
	add	rsp, 32
	vmovdqu	ymm1, [rsp]
	add	rsp, 32
	vmovdqu	ymm0, [rsp]
	add	rsp, 32

	ret

;------------------------------------------------------------------------------
; Name:		ReaderVector128_nontemporal (using SSE4)
; Purpose:	Reads 128-bit values sequentially from an area of memory.
; Params:	P1 = ptr to memory area
; 		P2 = length in bytes
; 		P3 = loops
;------------------------------------------------------------------------------
	align 64
ReaderVector128_nontemporal:
_ReaderVector128_nontemporal:
	lfence
	push	r10

	add	P2, P1	; P2 now points to end.

.L1:
	mov	r10, P1

.L2:
	prefetchnta	[r10]
	prefetchnta	[r10+64]

	movntdqa	xmm0, [r10]	; Read aligned to 16-byte boundary.
	movntdqa	xmm0, [16+r10]
	movntdqa	xmm0, [32+r10]
	movntdqa	xmm0, [48+r10]
	movntdqa	xmm0, [64+r10]
	movntdqa	xmm0, [80+r10]
	movntdqa	xmm0, [96+r10]
	movntdqa	xmm0, [112+r10]

	prefetchnta	[r10+128]
	prefetchnta	[r10+192]

	movntdqa	xmm0, [128+r10]
	movntdqa	xmm0, [144+r10]
	movntdqa	xmm0, [160+r10]
	movntdqa	xmm0, [176+r10]
	movntdqa	xmm0, [192+r10]
	movntdqa	xmm0, [208+r10]
	movntdqa	xmm0, [224+r10]
	movntdqa	xmm0, [240+r10]

	add	r10, 256
	cmp	r10, P2
	jb	.L2

	dec	P3
	jnz	.L1
	
	pop	r10

	ret

;------------------------------------------------------------------------------
; Name:		Writer
; Purpose:	Writes 64-bit value sequentially to an area of memory.
; Params:	P1 = ptr to memory area
; 		P2 = length in bytes
; 		P3 = loops
; 		P4 = quad to write
;------------------------------------------------------------------------------
	align 64
Writer:
_Writer:
	sfence
	push	r10

	add	P2, P1	; P2 now points to end.

.L1:
	mov	r10, P1

.L2:
	mov	[r10], P4
	mov	[8+r10], P4
	mov	[16+r10], P4
	mov	[24+r10], P4
	mov	[32+r10], P4
	mov	[40+r10], P4
	mov	[48+r10], P4
	mov	[56+r10], P4
	mov	[64+r10], P4
	mov	[72+r10], P4
	mov	[80+r10], P4
	mov	[88+r10], P4
	mov	[96+r10], P4
	mov	[104+r10], P4
	mov	[112+r10], P4
	mov	[120+r10], P4
	mov	[128+r10], P4
	mov	[136+r10], P4
	mov	[144+r10], P4
	mov	[152+r10], P4
	mov	[160+r10], P4
	mov	[168+r10], P4
	mov	[176+r10], P4
	mov	[184+r10], P4
	mov	[192+r10], P4
	mov	[200+r10], P4
	mov	[208+r10], P4
	mov	[216+r10], P4
	mov	[224+r10], P4
	mov	[232+r10], P4
	mov	[240+r10], P4
	mov	[248+r10], P4

	add	r10, 256
	cmp	r10, P2
	jb	.L2

	dec	P3
	jnz	.L1

	pop	r10

	ret

;------------------------------------------------------------------------------
; Name:		Writer_nontemporal
; Purpose:	Writes 64-bit value sequentially to memory w/nontemporal hint.
; Params:	P1 = ptr to memory area
; 		P2 = length in bytes
; 		P3 = loops
; 		P4 = quad to write
;------------------------------------------------------------------------------
	align 64
Writer_nontemporal:
_Writer_nontemporal:
	sfence
	push	r10

	add	P2, P1	; P2 now points to end.

.L1:
	mov	r10, P1

.L2:
	movnti	[r10], P4
	movnti	[8+r10], P4
	movnti	[16+r10], P4
	movnti	[24+r10], P4
	movnti	[32+r10], P4
	movnti	[40+r10], P4
	movnti	[48+r10], P4
	movnti	[56+r10], P4
	movnti	[64+r10], P4
	movnti	[72+r10], P4
	movnti	[80+r10], P4
	movnti	[88+r10], P4
	movnti	[96+r10], P4
	movnti	[104+r10], P4
	movnti	[112+r10], P4
	movnti	[120+r10], P4
	movnti	[128+r10], P4
	movnti	[136+r10], P4
	movnti	[144+r10], P4
	movnti	[152+r10], P4
	movnti	[160+r10], P4
	movnti	[168+r10], P4
	movnti	[176+r10], P4
	movnti	[184+r10], P4
	movnti	[192+r10], P4
	movnti	[200+r10], P4
	movnti	[208+r10], P4
	movnti	[216+r10], P4
	movnti	[224+r10], P4
	movnti	[232+r10], P4
	movnti	[240+r10], P4
	movnti	[248+r10], P4

	add	r10, 256
	cmp	r10, P2
	jb	.L2

	dec	P3
	jnz	.L1

	pop	r10

	ret

;------------------------------------------------------------------------------
; Name:		WriterVector128
; Purpose:	Writes 128-bit value sequentially to an area of memory.
; Params:	P1 = ptr to memory area
; 		P2 = length in bytes
; 		P3 = loops
; 		P4 = quad to write
;------------------------------------------------------------------------------
	align 64
WriterVector128:
_WriterVector128:
	sfence
	push	r10

	add	P2, P1	; P2 now points to end.

	movq	xmm0, P4

.L1:
	mov	r10, P1

.L2:
	movdqa	[r10], xmm0
	movdqa	[16+r10], xmm0
	movdqa	[32+r10], xmm0
	movdqa	[48+r10], xmm0
	movdqa	[64+r10], xmm0
	movdqa	[80+r10], xmm0
	movdqa	[96+r10], xmm0
	movdqa	[112+r10], xmm0

	movdqa	[128+r10], xmm0
	movdqa	[144+r10], xmm0
	movdqa	[160+r10], xmm0
	movdqa	[176+r10], xmm0
	movdqa	[192+r10], xmm0
	movdqa	[208+r10], xmm0
	movdqa	[224+r10], xmm0
	movdqa	[240+r10], xmm0

	add	r10, 256
	cmp	r10, P2
	jb	.L2

	dec	P3
	jnz	.L1

	pop	r10

	ret

;------------------------------------------------------------------------------
; Name:		WriterVector256 (using AVX)
; Purpose:	Writes 256-bit value sequentially to an area of memory.
; Params:	P1 = ptr to memory area
; 		P2 = length in bytes
; 		P3 = loops
; 		P4 = quad to write
;------------------------------------------------------------------------------
	align 64
WriterVector256:
_WriterVector256:
	sfence
	vzeroupper

	push	r10

	add	P2, P1	; P2 now points to end.

	pinsrq	xmm0, P4, 0
	pinsrq	xmm0, P4, 1

.L1:
	mov	r10, P1

.L2:
	vmovdqa	[r10], ymm0
	vmovdqa	[32+r10], ymm0
	vmovdqa	[64+r10], ymm0
	vmovdqa	[96+r10], ymm0
	vmovdqa	[128+r10], ymm0
	vmovdqa	[160+r10], ymm0
	vmovdqa	[192+r10], ymm0
	vmovdqa	[224+r10], ymm0

	add	r10, 256
	cmp	r10, P2
	jb	.L2

	dec	P3
	jnz	.L1

	pop	r10

	ret

;------------------------------------------------------------------------------
; Name:		WriterVector512 (using AVX-512)
; Purpose:	Writes 512-bit value sequentially to an area of memory.
; Params:	P1 = ptr to memory area
; 		P2 = length in bytes
; 		P3 = loops
; 		P4 = quad to write
;------------------------------------------------------------------------------
	align 64
WriterVector512:
_WriterVector512:
	sfence
	sub	rsp, 64
	vmovdqu64	[rsp], zmm0

	push	r10

	add	P2, P1	; P2 now points to end.

	push	P4
	push	P4
	vbroadcasti64x2	zmm0, [rsp]
	add	rsp, 16

.L1:
	mov	r10, P1

.L2:
	vmovdqa64	[r10], zmm0
	vmovdqa64	[64+r10], zmm0
	vmovdqa64	[128+r10], zmm0
	vmovdqa64	[192+r10], zmm0

	add	r10, 256
	cmp	r10, P2
	jb	.L2

	dec	P3
	jnz	.L1

	pop	r10

	vmovdqu64	zmm0, [rsp]
	add	rsp, 64

	ret

;------------------------------------------------------------------------------
; Name:		WriterVector512_nontemporal (using AVX-512)
; Purpose:	Writes 512-bit value sequentially to an area of memory.
; Params:	P1 = ptr to memory area
; 		P2 = length in bytes
; 		P3 = loops
; 		P4 = quad to write
;------------------------------------------------------------------------------
	align 64
WriterVector512_nontemporal:
_WriterVector512_nontemporal:
	sfence
	sub	rsp, 64
	vmovdqu64	[rsp], zmm0

	push	r10

	add	P2, P1	; P2 now points to end.

	push	P4
	push	P4
	vbroadcasti64x2	zmm0, [rsp]
	add	rsp, 16

.L1:
	mov	r10, P1

.L2:
	vmovntdq	[r10], zmm0
	vmovntdq	[64+r10], zmm0
	vmovntdq	[128+r10], zmm0
	vmovntdq	[192+r10], zmm0

	add	r10, 256
	cmp	r10, P2
	jb	.L2

	dec	P3
	jnz	.L1

	pop	r10

	vmovdqu64	zmm0, [rsp]
	add	rsp, 64

	ret

;------------------------------------------------------------------------------
; Name:		RandomWriterVector256 (using AVX)
; Purpose:	Writes 256-bit value in somewhat random order to RAM.
; Params:	P1 = ptr to array of chunk pointers
; 		P2 = # of chunks
; 		P3 = loops
; 		P4 = datum to write
;------------------------------------------------------------------------------
	align 64
RandomWriterVector256:
_RandomWriterVector256:
	sfence
	push	r10
	push	r11

	sub	rsp, 32
	mov	[rsp], P4
	mov	[rsp+8], P4
	mov	[rsp+16], P4
	mov	[rsp+24], P4
	vmovdqu	ymm0, [rsp]
	add	rsp, 32

.L1:
	xor	r11, r11

.L2:
	mov	r10, [P1 + 8*r11]	; Note, 64-bit pointers.

	vmovdqa	[192+r10], ymm0
	vmovdqa	[224+r10], ymm0
	vmovdqa	[64+r10], ymm0
	vmovdqa	[128+r10], ymm0
	vmovdqa	[r10], ymm0
	vmovdqa	[96+r10], ymm0
	vmovdqa	[160+r10], ymm0
	vmovdqa	[32+r10], ymm0

	inc	r11
	cmp	r11, P2
	jb	.L2

	dec	P3
	jnz	.L1

	pop	r11
	pop	r10

	ret

;------------------------------------------------------------------------------
; Name:		RandomWriterVector256_nontemporal (using AVX)
; Purpose:	Writes 256-bit value in somewhat random order to RAM.
; Params:	P1 = ptr to array of chunk pointers
; 		P2 = # of chunks
; 		P3 = loops
; 		P4 = datum to write
;------------------------------------------------------------------------------
	align 64
RandomWriterVector256_nontemporal:
_RandomWriterVector256_nontemporal:
	sfence
	push	r10
	push	r11

	sub	rsp, 32
	mov	[rsp], P4
	mov	[rsp+8], P4
	mov	[rsp+16], P4
	mov	[rsp+24], P4
	vmovdqu	ymm0, [rsp]
	add	rsp, 32

.L1:
	xor	r11, r11

.L2:
	mov	r10, [P1 + 8*r11]	; Note, 64-bit pointers.

	vmovntdq	[192+r10], ymm0
	vmovntdq	[64+r10], ymm0
	vmovntdq	[224+r10], ymm0
	vmovntdq	[r10], ymm0
	vmovntdq	[128+r10], ymm0
	vmovntdq	[32+r10], ymm0
	vmovntdq	[96+r10], ymm0
	vmovntdq	[160+r10], ymm0

	inc	r11
	cmp	r11, P2
	jb	.L2

	dec	P3
	jnz	.L1

	pop	r11
	pop	r10

	ret

;------------------------------------------------------------------------------
; Name:		WriterVector128_nontemporal (using SSE2)
; Purpose:	Writes 128-bit value sequentially to an area of memory.
; Params:	P1 = ptr to memory area
; 		P2 = length in bytes
; 		P3 = loops
; 		P4 = quad to write
;------------------------------------------------------------------------------
	align 64
WriterVector128_nontemporal:
_WriterVector128_nontemporal:
	sfence
	push	r10

	add	P2, P1	; P2 now points to end.

	movq	xmm0, P4

.L1:
	mov	r10, P1

.L2:
	movntdq	[r10], xmm0	
	movntdq	[16+r10], xmm0
	movntdq	[32+r10], xmm0
	movntdq	[48+r10], xmm0
	movntdq	[64+r10], xmm0
	movntdq	[80+r10], xmm0
	movntdq	[96+r10], xmm0
	movntdq	[112+r10], xmm0

	movntdq	[128+r10], xmm0
	movntdq	[144+r10], xmm0
	movntdq	[160+r10], xmm0
	movntdq	[176+r10], xmm0
	movntdq	[192+r10], xmm0
	movntdq	[208+r10], xmm0
	movntdq	[224+r10], xmm0
	movntdq	[240+r10], xmm0

	add	r10, 256
	cmp	r10, P2
	jb	.L2

	dec	P3
	jnz	.L1

	pop	r10

	ret

;------------------------------------------------------------------------------
; Name:		WriterVector256_nontemporal (using AVX)
; Purpose:	Nontemporal writes of 256-bit values sequentially into memory.
; Params:	P1 = ptr to memory area
; 		P2 = length in bytes
; 		P3 = loops
; 		P4 = quad to write
;------------------------------------------------------------------------------
	align 64
WriterVector256_nontemporal:
_WriterVector256_nontemporal:
	sfence
	vzeroupper

	push	r10

	add	P2, P1	; P2 now points to end.

	movq	xmm0, P4

.L1:
	mov	r10, P1

.L2:
	vmovntdq	[r10], ymm0	
	vmovntdq	[32+r10], ymm0
	vmovntdq	[64+r10], ymm0
	vmovntdq	[96+r10], ymm0
	vmovntdq	[128+r10], ymm0
	vmovntdq	[160+r10], ymm0
	vmovntdq	[192+r10], ymm0
	vmovntdq	[224+r10], ymm0

	add	r10, 256
	cmp	r10, P2
	jb	.L2

	dec	P3
	jnz	.L1

	pop	r10

	ret

;------------------------------------------------------------------------------
; Name:		ReaderVector256_nontemporal (using AVX)
; Purpose:	Nontemporal reads of 256-bit values sequentially from memory.
; Params:	P1 = ptr to memory area
; 		P2 = length in bytes
; 		P3 = loops
;------------------------------------------------------------------------------
	align 64
ReaderVector256_nontemporal:
_ReaderVector256_nontemporal:
	lfence
	push	r10

	add	P2, P1	; P2 now points to end.

.L1:
	mov	r10, P1

.L2:
	; This requires AVX512F.
	vmovntdqa	ymm0, [r10]	
	vmovntdqa	ymm0, [32+r10]
	vmovntdqa	ymm0, [64+r10]
	vmovntdqa	ymm0, [96+r10]
	vmovntdqa	ymm0, [128+r10]
	vmovntdqa	ymm0, [160+r10]
	vmovntdqa	ymm0, [192+r10]
	vmovntdqa	ymm0, [224+r10]

	add	r10, 256
	cmp	r10, P2
	jb	.L2

	dec	P3
	jnz	.L1

	pop	r10

	ret

;------------------------------------------------------------------------------
; Name:		ReaderVector512_nontemporal (using AVX)
; Purpose:	Nontemporal reads of 512-bit values sequentially from memory.
; Params:	P1 = ptr to memory area
; 		P2 = length in bytes
; 		P3 = loops
;------------------------------------------------------------------------------
	align 64
ReaderVector512_nontemporal:
_ReaderVector512_nontemporal:
	lfence
	push	r10

	add	P2, P1	; P2 now points to end.

.L1:
	mov	r10, P1

.L2:
	; This requires AVX512F.
	vmovntdqa	zmm0, [r10]	
	vmovntdqa	zmm0, [64+r10]
	vmovntdqa	zmm0, [128+r10]
	vmovntdqa	zmm0, [192+r10]

	add	r10, 256
	cmp	r10, P2
	jb	.L2

	dec	P3
	jnz	.L1

	pop	r10

	ret

;------------------------------------------------------------------------------
; Name:		StackReader
; Purpose:	Reads 64-bit values off the stack into registers of
;		the main register set, effectively testing L1 cache access
;		*and* effective-address calculation speed.
; Params:	P1 = loops
;------------------------------------------------------------------------------
	align 64
StackReader:
_StackReader:
	lfence
	push	qword 8000	; [rsp+56]
	push	qword 7000	; [rsp+48]
	push	qword 6000	; [rsp+40]
	push	qword 5000	; [rsp+32]
	push	qword 4000	; [rsp+24]
	push	qword 3000	; [rsp+16]
	push	qword 2000	; [rsp+8]
	push	qword 1000	; [rsp]

.L1:
	; 64 transfers
	mov	rax, [rsp]
	mov	rax, [rsp+16]
	mov	rax, [rsp+24]
	mov	rax, [rsp+32]
	mov	rax, [rsp+40]
	mov	rax, [rsp+8]
	mov	rax, [rsp+48]
	mov	rax, [rsp]
	mov	rax, [rsp+56]
	mov	rax, [rsp+16]
	mov	rax, [rsp+24]
	mov	rax, [rsp+32]
	mov	rax, [rsp+40]
	mov	rax, [rsp+8]
	mov	rax, [rsp+48]
	mov	rax, [rsp+56]
	mov	rax, [rsp]
	mov	rax, [rsp+16]
	mov	rax, [rsp+24]
	mov	rax, [rsp+32]
	mov	rax, [rsp+40]
	mov	rax, [rsp+8]
	mov	rax, [rsp+48]
	mov	rax, [rsp+8]
	mov	rax, [rsp+56]
	mov	rax, [rsp+16]
	mov	rax, [rsp+24]
	mov	rax, [rsp+32]
	mov	rax, [rsp+40]
	mov	rax, [rsp+8]
	mov	rax, [rsp+48]
	mov	rax, [rsp+56]
	mov	rax, [rsp]

	mov	rax, [rsp+16]
	mov	rax, [rsp+24]
	mov	rax, [rsp+32]
	mov	rax, [rsp+40]
	mov	rax, [rsp+8]
	mov	rax, [rsp+48]
	mov	rax, [rsp]
	mov	rax, [rsp+56]
	mov	rax, [rsp+16]
	mov	rax, [rsp+24]
	mov	rax, [rsp+32]
	mov	rax, [rsp+40]
	mov	rax, [rsp+8]
	mov	rax, [rsp+48]
	mov	rax, [rsp+56]
	mov	rax, [rsp]
	mov	rax, [rsp+16]
	mov	rax, [rsp+24]
	mov	rax, [rsp+32]
	mov	rax, [rsp+40]
	mov	rax, [rsp+8]
	mov	rax, [rsp+48]
	mov	rax, [rsp+8]
	mov	rax, [rsp+56]
	mov	rax, [rsp+16]
	mov	rax, [rsp+24]
	mov	rax, [rsp+32]
	mov	rax, [rsp+40]
	mov	rax, [rsp+8]
	mov	rax, [rsp+48]
	mov	rax, [rsp+56]

	sub	P1, 1
	jnz	.L1

	add	rsp, 64

	ret

;------------------------------------------------------------------------------
; Name:		StackWriter
; Purpose:	Writes 64-bit values into the stack from registers of
;		the main register set, effectively testing L1 cache access
;		*and* effective-address calculation speed.
; Params:	P1 = loops
;------------------------------------------------------------------------------
	align 64
StackWriter:
_StackWriter:
	sfence
	push	qword 8000	; [rsp+56]
	push	qword 7000	; [rsp+48]
	push	qword 6000	; [rsp+40]
	push	qword 5000	; [rsp+32]
	push	qword 4000	; [rsp+24]
	push	qword 3000	; [rsp+16]
	push	qword 2000	; [rsp+8]
	push	qword 1000	; [rsp]

	xor	rax, rax

.L1:
	; 64 transfers
	mov	[rsp], rax
	mov	[rsp+16], rax
	mov	[rsp+24], rax
	mov	[rsp+32], rax
	mov	[rsp+40], rax
	mov	[rsp+8], rax
	mov	[rsp+48], rax
	mov	[rsp], rax
	mov	[rsp+56], rax
	mov	[rsp+16], rax
	mov	[rsp+24], rax
	mov	[rsp+32], rax
	mov	[rsp+40], rax
	mov	[rsp+8], rax
	mov	[rsp+48], rax
	mov	[rsp+56], rax
	mov	[rsp], rax
	mov	[rsp+16], rax
	mov	[rsp+24], rax
	mov	[rsp+32], rax
	mov	[rsp+40], rax
	mov	[rsp+8], rax
	mov	[rsp+48], rax
	mov	[rsp+8], rax
	mov	[rsp+56], rax
	mov	[rsp+16], rax
	mov	[rsp+24], rax
	mov	[rsp+32], rax
	mov	[rsp+40], rax
	mov	[rsp+8], rax
	mov	[rsp+48], rax
	mov	[rsp+56], rax

	mov	[rsp], rax
	mov	[rsp+16], rax
	mov	[rsp+24], rax
	mov	[rsp+32], rax
	mov	[rsp+40], rax
	mov	[rsp+8], rax
	mov	[rsp+48], rax
	mov	[rsp], rax
	mov	[rsp+56], rax
	mov	[rsp+16], rax
	mov	[rsp+24], rax
	mov	[rsp+32], rax
	mov	[rsp+40], rax
	mov	[rsp+8], rax
	mov	[rsp+48], rax
	mov	[rsp+56], rax
	mov	[rsp], rax
	mov	[rsp+16], rax
	mov	[rsp+24], rax
	mov	[rsp+32], rax
	mov	[rsp+40], rax
	mov	[rsp+8], rax
	mov	[rsp+48], rax
	mov	[rsp+8], rax
	mov	[rsp+56], rax
	mov	[rsp+16], rax
	mov	[rsp+24], rax
	mov	[rsp+32], rax
	mov	[rsp+40], rax
	mov	[rsp+8], rax
	mov	[rsp+48], rax
	mov	[rsp+56], rax

	sub	P1, 1
	jnz	.L1

	add	rsp, 64

	ret

;------------------------------------------------------------------------------
; Name:		RegisterToRegister
; Purpose:	Reads/writes 64-bit values between registers of 
;		the main register set. Total bytes transferred is 8*32=256.
; Params:	P1 = loops
;------------------------------------------------------------------------------
	align 64
RegisterToRegister:
_RegisterToRegister:
.L1:
	mov	rax, rbx	; 64 transfers
	mov	rax, P4
	mov	rax, P3
	mov	rax, P2
	mov	rax, P1
	mov	rax, rbp
	mov	rax, rsp
	mov	rax, rbx
	mov	rax, rbx
	mov	rax, P4
	mov	rax, P3
	mov	rax, P2
	mov	rax, P1
	mov	rax, rbp
	mov	rax, rsp
	mov	rax, rbx
	mov	rax, rbx
	mov	rax, P4
	mov	rax, P3
	mov	rax, P2
	mov	rax, P1
	mov	rax, rbp
	mov	rax, rsp
	mov	rax, rbx
	mov	rax, rbx
	mov	rax, P4
	mov	rax, P3
	mov	rax, P2
	mov	rax, P1
	mov	rax, rbp
	mov	rax, rsp
	mov	rax, rbx

	mov	rax, rbx
	mov	rax, P4
	mov	rax, P3
	mov	rax, P2
	mov	rax, P1
	mov	rax, rbp
	mov	rax, rsp
	mov	rax, rbx
	mov	rax, rbx
	mov	rax, P4
	mov	rax, P3
	mov	rax, P2
	mov	rax, P1
	mov	rax, rbp
	mov	rax, rsp
	mov	rax, rbx
	mov	rax, rbx
	mov	rax, P4
	mov	rax, P3
	mov	rax, P2
	mov	rax, P1
	mov	rax, rbp
	mov	rax, rsp
	mov	rax, rbx
	mov	rax, rbx
	mov	rax, P4
	mov	rax, P3
	mov	rax, P2
	mov	rax, P1
	mov	rax, rbp
	mov	rax, rsp
	mov	rax, rbx

	sub	P1, 1
	jnz	.L1
	ret

;------------------------------------------------------------------------------
; Name:		VectorToVector128
; Purpose:	Reads/writes 128-bit values between registers of 
;		the vector register set, in this case XMM.
; Params:	P1 = loops
;------------------------------------------------------------------------------
	align 64
VectorToVector128:
_VectorToVector128:
.L1:
	; 32 transfers, 16 bytes each
	movq	xmm0, xmm1
	movq	xmm14, xmm2
	movq	xmm0, xmm4
	movq	xmm9, xmm0
	movq	xmm1, xmm12
	movq	xmm7, xmm1
	movq	xmm8, xmm3
	movq	xmm3, xmm1

	movq	xmm13, xmm2
	movq	xmm11, xmm3
	movq	xmm2, xmm10
	movq	xmm6, xmm1
	movq	xmm1, xmm2
	movq	xmm15, xmm1
	movq	xmm0, xmm3
	movq	xmm3, xmm14

	movq	xmm0, xmm1
	movq	xmm14, xmm2
	movq	xmm0, xmm4
	movq	xmm9, xmm0
	movq	xmm1, xmm12
	movq	xmm7, xmm1
	movq	xmm8, xmm3
	movq	xmm3, xmm1

	movq	xmm13, xmm2
	movq	xmm11, xmm3
	movq	xmm2, xmm10
	movq	xmm6, xmm1
	movq	xmm1, xmm2
	movq	xmm15, xmm1
	movq	xmm0, xmm3
	movq	xmm3, xmm14

	sub	P1, 1
	jnz	.L1
	ret

;------------------------------------------------------------------------------
; Name:		VectorToVector256
; Purpose:	Reads/writes 256-bit values between vector registers,
;		which on the x86 are YMM registers.
; Params:	P1 = loops
;------------------------------------------------------------------------------
	align 64
VectorToVector256:
_VectorToVector256:
	vzeroupper

.L1:
	; 32 transfers, 32 bytes each
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

	sub	P1, 1
	jnz	.L1
	ret

;------------------------------------------------------------------------------
; Name:		VectorToVector512
; Purpose:	Reads/writes 512-bit values between vector registers,
;		which on the x86 are ZMM registers.
; Params:	P1 = loops
;------------------------------------------------------------------------------
	align 64
VectorToVector512:
_VectorToVector512:

.L1:
	; 32 transfers, 64 bytes each
	vmovdqa64	zmm0, zmm1	
	vmovdqa64	zmm0, zmm2
	vmovdqa64	zmm0, zmm3
	vmovdqa64	zmm2, zmm0
	vmovdqa64	zmm1, zmm2
	vmovdqa64	zmm2, zmm1
	vmovdqa64	zmm0, zmm3
	vmovdqa64	zmm3, zmm1

	vmovdqa64	zmm0, zmm1	
	vmovdqa64	zmm0, zmm2
	vmovdqa64	zmm0, zmm3
	vmovdqa64	zmm2, zmm0
	vmovdqa64	zmm1, zmm2
	vmovdqa64	zmm2, zmm1
	vmovdqa64	zmm0, zmm3
	vmovdqa64	zmm3, zmm1

	vmovdqa64	zmm0, zmm1	
	vmovdqa64	zmm0, zmm2
	vmovdqa64	zmm0, zmm3
	vmovdqa64	zmm2, zmm0
	vmovdqa64	zmm1, zmm2
	vmovdqa64	zmm2, zmm1
	vmovdqa64	zmm0, zmm3
	vmovdqa64	zmm3, zmm1

	vmovdqa64	zmm0, zmm1	
	vmovdqa64	zmm0, zmm2
	vmovdqa64	zmm0, zmm3
	vmovdqa64	zmm2, zmm0
	vmovdqa64	zmm1, zmm2
	vmovdqa64	zmm2, zmm1
	vmovdqa64	zmm0, zmm3
	vmovdqa64	zmm3, zmm1

	sub	P1, 1
	jnz	.L1
	ret

;------------------------------------------------------------------------------
; Name:		RegisterToVectorMove
; Purpose:	Writes 64-bit main register values into 128-bit vector register
;		clearing the upper unused bits.
; Params:	P1 = loops
;------------------------------------------------------------------------------
	align 64
RegisterToVectorMove:
_RegisterToVectorMove:
.L1:
	; 64 moves
	movq	xmm1, rax 
	movq	xmm2, P2
	movq	xmm3, rbx
	movq	xmm1, P4
	movq	xmm2, P2
	movq	xmm3, rsp
	movq	xmm0, P1
	movq	xmm0, P3

	movq	xmm0, rax 	
	movq	xmm1, P2
	movq	xmm2, rbx
	movq	xmm3, P4
	movq	xmm0, P2
	movq	xmm3, rsp
	movq	xmm2, P1
	movq	xmm1, P3

	movq	xmm0, rax 	
	movq	xmm1, P2
	movq	xmm2, rbx
	movq	xmm3, P4
	movq	xmm0, P2
	movq	xmm3, rsp
	movq	xmm2, P1
	movq	xmm1, P3

	movq	xmm0, rax 	
	movq	xmm1, P2
	movq	xmm2, rbx
	movq	xmm3, P4
	movq	xmm0, P2
	movq	xmm3, rsp
	movq	xmm2, P1
	movq	xmm1, P3

	movq	xmm1, rax 
	movq	xmm2, P2
	movq	xmm3, rbx
	movq	xmm1, P4
	movq	xmm2, P2
	movq	xmm3, rsp
	movq	xmm0, P1
	movq	xmm0, P3

	movq	xmm0, rax 	
	movq	xmm1, P2
	movq	xmm2, rbx
	movq	xmm3, P4
	movq	xmm0, P2
	movq	xmm3, rsp
	movq	xmm2, P1
	movq	xmm1, P3

	movq	xmm0, rax 	
	movq	xmm1, P2
	movq	xmm2, rbx
	movq	xmm3, P4
	movq	xmm0, P2
	movq	xmm3, rsp
	movq	xmm2, P1
	movq	xmm1, P3

	movq	xmm0, rax 	
	movq	xmm1, P2
	movq	xmm2, rbx
	movq	xmm3, P4
	movq	xmm0, P2
	movq	xmm3, rsp
	movq	xmm2, P1
	movq	xmm1, P3

	dec	P1
	jnz	.L1
	ret

;------------------------------------------------------------------------------
; Name:		VectorToRegisterMove
; Purpose:	Writes lower 64 bits of vector register into 64-bit main 
;		register.
; Params:	P1 = loops
;------------------------------------------------------------------------------
	align 64
VectorToRegisterMove:
_VectorToRegisterMove:
.L1:
	; 64 transfers
	movq	rax, xmm1
	movq	rax, xmm2
	movq	rax, xmm3
	movq	rax, xmm1
	movq	rax, xmm2
	movq	rax, xmm3
	movq	rax, xmm0
	movq	rax, xmm0

	movq	rax, xmm0
	movq	rax, xmm1
	movq	rax, xmm2
	movq	rax, xmm3
	movq	rax, xmm0
	movq	rax, xmm3
	movq	rax, xmm2
	movq	rax, xmm1

	movq	rax, xmm0
	movq	rax, xmm1
	movq	rax, xmm2
	movq	rax, xmm3
	movq	rax, xmm0
	movq	rax, xmm3
	movq	rax, xmm2
	movq	rax, xmm1

	movq	rax, xmm0
	movq	rax, xmm1
	movq	rax, xmm2
	movq	rax, xmm3
	movq	rax, xmm0
	movq	rax, xmm3
	movq	rax, xmm2
	movq	rax, xmm1

	movq	rax, xmm1
	movq	rax, xmm2
	movq	rax, xmm3
	movq	rax, xmm1
	movq	rax, xmm2
	movq	rax, xmm3
	movq	rax, xmm0
	movq	rax, xmm0

	movq	rax, xmm0
	movq	rax, xmm1
	movq	rax, xmm2
	movq	rax, xmm3
	movq	rax, xmm0
	movq	rax, xmm3
	movq	rax, xmm2
	movq	rax, xmm1

	movq	rax, xmm0
	movq	rax, xmm1
	movq	rax, xmm2
	movq	rax, xmm3
	movq	rax, xmm0
	movq	rax, xmm3
	movq	rax, xmm2
	movq	rax, xmm1

	movq	rax, xmm0
	movq	rax, xmm1
	movq	rax, xmm2
	movq	rax, xmm3
	movq	rax, xmm0
	movq	rax, xmm3
	movq	rax, xmm2
	movq	rax, xmm1

	dec	P1
	jnz .L1
	ret

;------------------------------------------------------------------------------
; Name:		Register8ToVector128
; Purpose:	Writes 8-bit main register values into 128-bit vector register
;		without clearing the unused bits.
; Params:	P1 = loops
;------------------------------------------------------------------------------
	align 64
Register8ToVector128:
_Register8ToVector128:

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

	pinsrb	xmm1, r8b, 0
	pinsrb	xmm2, r9b, 1
	pinsrb	xmm3, r10b, 2
	pinsrb	xmm1, r11b, 3
	pinsrb	xmm2, r12b, 4
	pinsrb	xmm3, al, 5
	pinsrb	xmm0, cl, 6
	pinsrb	xmm0, bl, 7

	pinsrb	xmm0, r8b, 0
	pinsrb	xmm0, r9b, 1
	pinsrb	xmm0, r10b, 2
	pinsrb	xmm0, r11b, 3
	pinsrb	xmm0, r12b, 4
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

	pinsrb	xmm1, r8b, 10
	pinsrb	xmm2, r9b, 11
	pinsrb	xmm3, r10b, 12
	pinsrb	xmm1, r11b, 13
	pinsrb	xmm2, r12b, 14
	pinsrb	xmm3, al, 15
	pinsrb	xmm0, cl, 6
	pinsrb	xmm0, bl, 7

	pinsrb	xmm0, r8b, 9
	pinsrb	xmm0, r9b, 8
	pinsrb	xmm0, r10b, 11
	pinsrb	xmm0, r11b, 3
	pinsrb	xmm0, r12b, 4
	pinsrb	xmm0, al, 5
	pinsrb	xmm0, cl, 6
	pinsrb	xmm0, bl, 7

	dec	P1
	jnz .L1
	ret

;------------------------------------------------------------------------------
; Name:		Register16ToVector128
; Purpose:	Writes 16-bit main register values into 128-bit vector register
;		without clearing the unused bits.
; Params:	P1 = loops
;------------------------------------------------------------------------------
	align 64
Register16ToVector128:
_Register16ToVector128:

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

	pinsrw	xmm1, r8w, 0
	pinsrw	xmm2, r9w, 1
	pinsrw	xmm3, r10w, 2
	pinsrw	xmm1, r11w, 3
	pinsrw	xmm2, r12w, 4
	pinsrw	xmm3, ax, 5
	pinsrw	xmm0, bp, 6
	pinsrw	xmm0, bx, 7

	pinsrw	xmm0, r8w, 0
	pinsrw	xmm0, r9w, 1
	pinsrw	xmm0, r10w, 2
	pinsrw	xmm0, r11w, 3
	pinsrw	xmm0, r12w, 4
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

	pinsrw	xmm1, r8w, 0
	pinsrw	xmm2, r9w, 1
	pinsrw	xmm3, r10w, 2
	pinsrw	xmm1, r11w, 3
	pinsrw	xmm2, r12w, 4
	pinsrw	xmm3, ax, 5
	pinsrw	xmm0, bp, 6
	pinsrw	xmm0, bx, 7

	pinsrw	xmm0, r8w, 0
	pinsrw	xmm0, r9w, 1
	pinsrw	xmm0, r10w, 2
	pinsrw	xmm0, r11w, 3
	pinsrw	xmm0, r12w, 4
	pinsrw	xmm0, ax, 5
	pinsrw	xmm0, bp, 6
	pinsrw	xmm0, bx, 7

	dec	P1
	jnz .L1
	ret

;------------------------------------------------------------------------------
; Name:		Register32ToVector128
; Purpose:	Writes 32-bit main register values into 128-bit vector register
;		without clearing the unused bits.
; Params:	P1 = loops
;------------------------------------------------------------------------------
	align 64
Register32ToVector128:
_Register32ToVector128:
	mov	eax, 0xcafef00d
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

	pinsrd	xmm1, r8d, 0
	pinsrd	xmm2, r9d, 1
	pinsrd	xmm3, r10d, 2
	pinsrd	xmm1, r11d, 3
	pinsrd	xmm2, r12d, 0
	pinsrd	xmm3, eax, 1
	pinsrd	xmm0, ebp, 2
	pinsrd	xmm0, ebx, 3

	pinsrd	xmm0, r8d, 0
	pinsrd	xmm0, r9d, 1
	pinsrd	xmm0, r10d, 2
	pinsrd	xmm0, r11d, 3
	pinsrd	xmm0, r12d, 0
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

	pinsrd	xmm1, r8d, 0
	pinsrd	xmm2, r9d, 1
	pinsrd	xmm3, r10d, 2
	pinsrd	xmm1, r11d, 3
	pinsrd	xmm2, r12d, 0
	pinsrd	xmm3, eax, 1
	pinsrd	xmm0, ebp, 2
	pinsrd	xmm0, ebx, 3

	pinsrd	xmm0, r8d, 0
	pinsrd	xmm0, r9d, 1
	pinsrd	xmm0, r10d, 2
	pinsrd	xmm0, r11d, 3
	pinsrd	xmm0, r12d, 0
	pinsrd	xmm0, eax, 0
	pinsrd	xmm0, ebp, 0
	pinsrd	xmm0, ebx, 0

	dec	P1
	jnz .L1
	ret

;------------------------------------------------------------------------------
; Name:		Register64ToVector128
; Purpose:	Writes 64-bit main register values into 128-bit vector register
;		without clearing the unused bits.
; Params:	P1 = loops
;------------------------------------------------------------------------------
	align 64
Register64ToVector128:
_Register64ToVector128:

.L1:
	pinsrq	xmm1, r8, 0	; Each xfer moves 8 bytes, therefore to do
	pinsrq	xmm2, r9, 1	; 256 bytes we need 32 transfers.
	pinsrq	xmm3, r10, 0
	pinsrq	xmm1, r11, 1
	pinsrq	xmm2, r12, 0
	pinsrq	xmm3, rax, 1
	pinsrq	xmm0, rbp, 0
	pinsrq	xmm0, rbx, 1

	pinsrq	xmm0, r8, 0
	pinsrq	xmm0, r9, 1
	pinsrq	xmm0, r10, 1
	pinsrq	xmm0, r11, 1
	pinsrq	xmm0, r12, 0
	pinsrq	xmm0, rax, 0
	pinsrq	xmm0, rbp, 0
	pinsrq	xmm0, rbx, 0

	pinsrq	xmm0, r8, 0
	pinsrq	xmm0, r9, 1
	pinsrq	xmm0, r10, 1
	pinsrq	xmm0, r11, 1
	pinsrq	xmm0, r12, 0
	pinsrq	xmm0, rax, 0
	pinsrq	xmm0, rbp, 0
	pinsrq	xmm0, rbx, 0

	pinsrq	xmm0, r8, 0
	pinsrq	xmm0, r9, 1
	pinsrq	xmm0, r10, 1
	pinsrq	xmm0, r11, 1
	pinsrq	xmm0, r12, 0
	pinsrq	xmm0, rax, 0
	pinsrq	xmm0, rbp, 0
	pinsrq	xmm0, rbx, 0

	pinsrq	xmm1, r8, 0	; Each xfer moves 8 bytes, therefore to do
	pinsrq	xmm2, r9, 1	; 256 bytes we need 32 transfers.
	pinsrq	xmm3, r10, 0
	pinsrq	xmm1, r11, 1
	pinsrq	xmm2, r12, 0
	pinsrq	xmm3, rax, 1
	pinsrq	xmm0, rbp, 0
	pinsrq	xmm0, rbx, 1

	pinsrq	xmm0, r8, 0
	pinsrq	xmm0, r9, 1
	pinsrq	xmm0, r10, 1
	pinsrq	xmm0, r11, 1
	pinsrq	xmm0, r12, 0
	pinsrq	xmm0, rax, 0
	pinsrq	xmm0, rbp, 0
	pinsrq	xmm0, rbx, 0

	pinsrq	xmm0, r8, 0
	pinsrq	xmm0, r9, 1
	pinsrq	xmm0, r10, 1
	pinsrq	xmm0, r11, 1
	pinsrq	xmm0, r12, 0
	pinsrq	xmm0, rax, 0
	pinsrq	xmm0, rbp, 0
	pinsrq	xmm0, rbx, 0

	pinsrq	xmm0, r8, 0
	pinsrq	xmm0, r9, 1
	pinsrq	xmm0, r10, 1
	pinsrq	xmm0, r11, 1
	pinsrq	xmm0, r12, 0
	pinsrq	xmm0, rax, 0
	pinsrq	xmm0, rbp, 0
	pinsrq	xmm0, rbx, 0

	dec	P1
	jnz .L1
	ret


;------------------------------------------------------------------------------
; Name:		Vector128ToRegister8
; Purpose:	Writes 8-bit vector register values into main register.
; Params:	P1 = loops
;------------------------------------------------------------------------------
	align 64
Vector128ToRegister8:
_Vector128ToRegister8:

.L1:
	pextrb	eax, xmm1, 0	; 64 transfers x 1 bytes = 64 bytes 
	pextrb	eax, xmm2, 1
	pextrb	eax, xmm3, 2
	pextrb	eax, xmm1, 3
	pextrb	eax, xmm2, 4
	pextrb	eax, xmm3, 5
	pextrb	eax, xmm0, 6
	pextrb	eax, xmm0, 7

	pextrb	eax, xmm0, 0
	pextrb	eax, xmm1, 1
	pextrb	eax, xmm2, 2
	pextrb	eax, xmm3, 3
	pextrb	eax, xmm3, 4
	pextrb	eax, xmm2, 5
	pextrb	eax, xmm1, 6
	pextrb	eax, xmm0, 7

	pextrb	eax, xmm1, 0
	pextrb	eax, xmm2, 1
	pextrb	eax, xmm3, 2
	pextrb	eax, xmm1, 3
	pextrb	eax, xmm2, 4
	pextrb	eax, xmm3, 5
	pextrb	eax, xmm0, 6
	pextrb	eax, xmm0, 7

	pextrb	eax, xmm0, 0
	pextrb	eax, xmm0, 1
	pextrb	eax, xmm0, 2
	pextrb	eax, xmm0, 3
	pextrb	eax, xmm0, 4
	pextrb	eax, xmm0, 5
	pextrb	eax, xmm0, 6
	pextrb	eax, xmm0, 7

	pextrb	eax, xmm1, 0
	pextrb	eax, xmm2, 1
	pextrb	eax, xmm3, 2
	pextrb	eax, xmm1, 3
	pextrb	eax, xmm2, 4
	pextrb	eax, xmm3, 5
	pextrb	eax, xmm0, 6
	pextrb	eax, xmm0, 7

	pextrb	eax, xmm0, 0
	pextrb	eax, xmm1, 1
	pextrb	eax, xmm2, 2
	pextrb	eax, xmm3, 3
	pextrb	eax, xmm3, 4
	pextrb	eax, xmm2, 5
	pextrb	eax, xmm1, 6
	pextrb	eax, xmm0, 7

	pextrb	eax, xmm1, 0
	pextrb	eax, xmm2, 1
	pextrb	eax, xmm3, 2
	pextrb	eax, xmm1, 3
	pextrb	eax, xmm2, 4
	pextrb	eax, xmm3, 5
	pextrb	eax, xmm0, 6
	pextrb	eax, xmm0, 7

	pextrb	eax, xmm0, 0
	pextrb	eax, xmm0, 1
	pextrb	eax, xmm0, 2
	pextrb	eax, xmm0, 3
	pextrb	eax, xmm0, 4
	pextrb	eax, xmm0, 5
	pextrb	eax, xmm0, 6
	pextrb	eax, xmm0, 7

	dec	P1
	jnz .L1
	ret

;------------------------------------------------------------------------------
; Name:		Vector128ToRegister16
; Purpose:	Writes 16-bit vector register values into main register.
; Params:	P1 = loops
;------------------------------------------------------------------------------
	align 64
Vector128ToRegister16:
_Vector128ToRegister16:

.L1:
	pextrw	eax, xmm1, 0	; 64 transfers x 2 bytes = 128 bytes 
	pextrw	eax, xmm2, 1
	pextrw	eax, xmm3, 2
	pextrw	eax, xmm1, 3
	pextrw	eax, xmm2, 4
	pextrw	eax, xmm3, 5
	pextrw	eax, xmm0, 6
	pextrw	eax, xmm0, 7

	pextrw	eax, xmm0, 0
	pextrw	eax, xmm1, 1
	pextrw	eax, xmm2, 2
	pextrw	eax, xmm3, 3
	pextrw	eax, xmm3, 4
	pextrw	eax, xmm2, 5
	pextrw	eax, xmm1, 6
	pextrw	eax, xmm0, 7

	pextrw	eax, xmm1, 0
	pextrw	eax, xmm2, 1
	pextrw	eax, xmm3, 2
	pextrw	eax, xmm1, 3
	pextrw	eax, xmm2, 4
	pextrw	eax, xmm3, 5
	pextrw	eax, xmm0, 6
	pextrw	eax, xmm0, 7

	pextrw	eax, xmm0, 0
	pextrw	eax, xmm0, 1
	pextrw	eax, xmm0, 2
	pextrw	eax, xmm0, 3
	pextrw	eax, xmm0, 4
	pextrw	eax, xmm0, 5
	pextrw	eax, xmm0, 6
	pextrw	eax, xmm0, 7

	pextrw	eax, xmm1, 0	
	pextrw	eax, xmm2, 1
	pextrw	eax, xmm3, 2
	pextrw	eax, xmm1, 3
	pextrw	eax, xmm2, 4
	pextrw	eax, xmm3, 5
	pextrw	eax, xmm0, 6
	pextrw	eax, xmm0, 7

	pextrw	eax, xmm0, 0
	pextrw	eax, xmm1, 1
	pextrw	eax, xmm2, 2
	pextrw	eax, xmm3, 3
	pextrw	eax, xmm3, 4
	pextrw	eax, xmm2, 5
	pextrw	eax, xmm1, 6
	pextrw	eax, xmm0, 7

	pextrw	eax, xmm1, 0
	pextrw	eax, xmm2, 1
	pextrw	eax, xmm3, 2
	pextrw	eax, xmm1, 3
	pextrw	eax, xmm2, 4
	pextrw	eax, xmm3, 5
	pextrw	eax, xmm0, 6
	pextrw	eax, xmm0, 7

	pextrw	eax, xmm0, 0
	pextrw	eax, xmm0, 1
	pextrw	eax, xmm0, 2
	pextrw	eax, xmm0, 3
	pextrw	eax, xmm0, 4
	pextrw	eax, xmm0, 5
	pextrw	eax, xmm0, 6
	pextrw	eax, xmm0, 7

	dec	P1
	jnz .L1
	ret

;------------------------------------------------------------------------------
; Name:		Vector128ToRegister32
; Purpose:	Writes 32-bit vector register values into main register.
; Params:	P1 = loops
;------------------------------------------------------------------------------
	align 64
Vector128ToRegister32:
_Vector128ToRegister32:
	
.L1:
	pextrd	eax, xmm1, 0	; 64 xfers x 4 bytes = 256 bytes
	pextrd	eax, xmm2, 1
	pextrd	eax, xmm3, 2
	pextrd	eax, xmm1, 3
	pextrd	eax, xmm2, 0
	pextrd	eax, xmm3, 1
	pextrd	eax, xmm0, 2
	pextrd	eax, xmm0, 3

	pextrd	eax, xmm0, 0
	pextrd	eax, xmm1, 1
	pextrd	eax, xmm2, 2
	pextrd	eax, xmm3, 3
	pextrd	eax, xmm3, 3
	pextrd	eax, xmm2, 2
	pextrd	eax, xmm1, 1
	pextrd	eax, xmm0, 0

	pextrd	eax, xmm1, 0
	pextrd	eax, xmm2, 1
	pextrd	eax, xmm3, 2
	pextrd	eax, xmm1, 3
	pextrd	eax, xmm2, 0
	pextrd	eax, xmm3, 1
	pextrd	eax, xmm0, 2
	pextrd	eax, xmm0, 3

	pextrd	eax, xmm0, 0
	pextrd	eax, xmm0, 1
	pextrd	eax, xmm0, 2
	pextrd	eax, xmm0, 3
	pextrd	eax, xmm0, 0
	pextrd	eax, xmm0, 0
	pextrd	eax, xmm0, 0
	pextrd	eax, xmm0, 0

	pextrd	eax, xmm1, 0
	pextrd	eax, xmm2, 1
	pextrd	eax, xmm3, 2
	pextrd	eax, xmm1, 3
	pextrd	eax, xmm2, 0
	pextrd	eax, xmm3, 1
	pextrd	eax, xmm0, 2
	pextrd	eax, xmm0, 3

	pextrd	eax, xmm0, 0
	pextrd	eax, xmm1, 1
	pextrd	eax, xmm2, 2
	pextrd	eax, xmm3, 3
	pextrd	eax, xmm3, 3
	pextrd	eax, xmm2, 2
	pextrd	eax, xmm1, 1
	pextrd	eax, xmm0, 0

	pextrd	eax, xmm1, 0
	pextrd	eax, xmm2, 1
	pextrd	eax, xmm3, 2
	pextrd	eax, xmm1, 3
	pextrd	eax, xmm2, 0
	pextrd	eax, xmm3, 1
	pextrd	eax, xmm0, 2
	pextrd	eax, xmm0, 3

	pextrd	eax, xmm0, 0
	pextrd	eax, xmm0, 1
	pextrd	eax, xmm0, 2
	pextrd	eax, xmm0, 3
	pextrd	eax, xmm0, 0
	pextrd	eax, xmm0, 1
	pextrd	eax, xmm0, 2
	pextrd	eax, xmm0, 3

	dec	P1
	jnz .L1
	ret

;------------------------------------------------------------------------------
; Name:		Vector128ToRegister64
; Purpose:	Writes 64-bit vector register values into main register.
; Params:	P1 = loops
;------------------------------------------------------------------------------
	align 64
Vector128ToRegister64:
_Vector128ToRegister64:
	
.L1:
	pextrq	rax, xmm1, 0	; 64 transfers 
	pextrq	rax, xmm2, 1
	pextrq	rax, xmm3, 0
	pextrq	rax, xmm1, 1
	pextrq	rax, xmm2, 0
	pextrq	rax, xmm3, 1
	pextrq	rax, xmm0, 0
	pextrq	rax, xmm0, 1

	pextrq	rax, xmm0, 0
	pextrq	rax, xmm0, 1
	pextrq	rax, xmm0, 0
	pextrq	rax, xmm0, 1
	pextrq	rax, xmm0, 0
	pextrq	rax, xmm0, 1
	pextrq	rax, xmm0, 0
	pextrq	rax, xmm0, 1

	pextrq	rax, xmm1, 0
	pextrq	rax, xmm2, 1
	pextrq	rax, xmm3, 0
	pextrq	rax, xmm1, 1
	pextrq	rax, xmm2, 0
	pextrq	rax, xmm3, 1
	pextrq	rax, xmm0, 0
	pextrq	rax, xmm0, 1

	pextrq	rax, xmm0, 0
	pextrq	rax, xmm0, 1
	pextrq	rax, xmm0, 0
	pextrq	rax, xmm0, 1
	pextrq	rax, xmm0, 0
	pextrq	rax, xmm0, 1
	pextrq	rax, xmm0, 0
	pextrq	rax, xmm0, 1

	pextrq	rax, xmm1, 0
	pextrq	rax, xmm2, 1
	pextrq	rax, xmm3, 0
	pextrq	rax, xmm1, 1
	pextrq	rax, xmm2, 0
	pextrq	rax, xmm3, 1
	pextrq	rax, xmm0, 0
	pextrq	rax, xmm0, 1

	pextrq	rax, xmm0, 0
	pextrq	rax, xmm0, 1
	pextrq	rax, xmm0, 0
	pextrq	rax, xmm0, 1
	pextrq	rax, xmm0, 0
	pextrq	rax, xmm0, 1
	pextrq	rax, xmm0, 0
	pextrq	rax, xmm0, 1

	pextrq	rax, xmm1, 0
	pextrq	rax, xmm2, 1
	pextrq	rax, xmm3, 0
	pextrq	rax, xmm1, 1
	pextrq	rax, xmm2, 0
	pextrq	rax, xmm3, 1
	pextrq	rax, xmm0, 0
	pextrq	rax, xmm0, 1

	pextrq	rax, xmm0, 0
	pextrq	rax, xmm0, 1
	pextrq	rax, xmm0, 0
	pextrq	rax, xmm0, 1
	pextrq	rax, xmm0, 0
	pextrq	rax, xmm0, 1
	pextrq	rax, xmm0, 0
	pextrq	rax, xmm0, 1

	dec	P1
	jnz .L1
	ret

;------------------------------------------------------------------------------
; Name:		CopyWithMainRegisters
; Purpose:	Copies memory chunks that are 32-byte aligned.
; Params:	P1 = ptr to destination memory area
;		P2 = ptr to source memory area
; 		P3 = length in bytes
; 		P4 = loops
;------------------------------------------------------------------------------
	align 64
CopyWithMainRegisters:
_CopyWithMainRegisters:
	mfence

	push	r10
	push	r11
	push	r12
	push	r13
	push	r14

	shr	P3, 8	; Ensure length is multiple of 256.
	shl	P3, 8

	prefetcht0	[P2]

.L1:
	mov	r10, P3

.L2:
	mov	r11, [P2]
	mov	r12, [8+P2]
	mov	r13, [16+P2]
	mov	r14, [24+P2]
	mov	[P1], r11
	mov	[8+P1], r12
	mov	[16+P1], r13
	mov	[24+P1], r14

	mov	r11, [32+P2]
	mov	r12, [40+P2]
	mov	r13, [48+P2]
	mov	r14, [56+P2]
	mov	[32+P1], r11
	mov	[40+P1], r12
	mov	[48+P1], r13
	mov	[56+P1], r14

	mov	r11, [64+P2]
	mov	r12, [72+P2]
	mov	r13, [80+P2]
	mov	r14, [88+P2]
	mov	[64+P1], r11
	mov	[72+P1], r12
	mov	[80+P1], r13
	mov	[88+P1], r14

	mov	r11, [96+P2]
	mov	r12, [104+P2]
	mov	r13, [112+P2]
	mov	r14, [120+P2]
	mov	[96+P1], r11
	mov	[104+P1], r12
	mov	[112+P1], r13
	mov	[120+P1], r14

	mov	r11, [128+P2]
	mov	r12, [128+8+P2]
	mov	r13, [128+16+P2]
	mov	r14, [128+24+P2]
	mov	[128+P1], r11
	mov	[128+8+P1], r12
	mov	[128+16+P1], r13
	mov	[128+24+P1], r14

	mov	r11, [128+32+P2]
	mov	r12, [128+40+P2]
	mov	r13, [128+48+P2]
	mov	r14, [128+56+P2]
	mov	[128+32+P1], r11
	mov	[128+40+P1], r12
	mov	[128+48+P1], r13
	mov	[128+56+P1], r14

	mov	r11, [128+64+P2]
	mov	r12, [128+72+P2]
	mov	r13, [128+80+P2]
	mov	r14, [128+88+P2]
	mov	[128+64+P1], r11
	mov	[128+72+P1], r12
	mov	[128+80+P1], r13
	mov	[128+88+P1], r14

	mov	r11, [128+96+P2]
	mov	r12, [128+104+P2]
	mov	r13, [128+112+P2]
	mov	r14, [128+120+P2]
	mov	[128+96+P1], r11
	mov	[128+104+P1], r12
	mov	[128+112+P1], r13
	mov	[128+120+P1], r14

	add	P2, 256
	add	P1, 256

	sub	r10, 256
	jnz	.L2

	sub	P2, P3	; P2 now points to start.
	sub	P1, P3	; P1 now points to start.

	dec	P4
	jnz	.L1

	pop	r14
	pop	r13
	pop	r12
	pop	r11
	pop	r10

	ret

;------------------------------------------------------------------------------
; Name:		CopyVector512 (using AVX512)
; Purpose:	Copies memory chunks that are 64-byte aligned.
; Params:	P1 = ptr to destination memory area
;		P2 = ptr to source memory area
; 		P3 = length in bytes
; 		P4 = loops
;------------------------------------------------------------------------------
	align 64
CopyVector512:
_CopyVector512:
	mfence

	push	r10

	shr	P3, 8	; Ensure length is multiple of 256.
	shl	P3, 8

	prefetcht0	[P2]

.L1:
	mov	r10, P3

.L2:
	vmovdqa64	zmm0, [P2]
	vmovdqa64	zmm1, [64+P2]
	vmovdqa64	zmm2, [128+P2]
	vmovdqa64	zmm3, [192+P2]

	vmovdqa64	[P1], zmm0
	vmovdqa64	[64+P1], zmm1
	vmovdqa64	[128+P1], zmm2
	vmovdqa64	[192+P1], zmm3

	add	P2, 256
	add	P1, 256

	sub	r10, 256
	jnz	.L2

	sub	P2, P3	; P2 now points to start.
	sub	P1, P3	; P1 now points to start.

	dec	P4
	jnz	.L1

	pop	r10

	ret

;------------------------------------------------------------------------------
; Name:		CopyVector256 (using AVX)
; Purpose:	Copies memory chunks that are 32-byte aligned.
; Params:	P1 = ptr to destination memory area
;		P2 = ptr to source memory area
; 		P3 = length in bytes
; 		P4 = loops
;------------------------------------------------------------------------------
	align 64
CopyVector256:
_CopyVector256:
	mfence
	vzeroupper

	push	r10

	shr	P3, 8	; Ensure length is multiple of 256.
	shl	P3, 8

	prefetcht0	[P2]

.L1:
	mov	r10, P3

.L2:
	vmovdqa	ymm0, [P2]
	vmovdqa	ymm1, [32+P2]
	vmovdqa	ymm2, [64+P2]
	vmovdqa	ymm3, [96+P2]

	vmovdqa	[P1], ymm0
	vmovdqa	[32+P1], ymm1
	vmovdqa	[64+P1], ymm2
	vmovdqa	[96+P1], ymm3

	vmovdqa	ymm0, [128+P2]
	vmovdqa	ymm1, [128+32+P2]
	vmovdqa	ymm2, [128+64+P2]
	vmovdqa	ymm3, [128+96+P2]

	vmovdqa	[128+P1], ymm0
	vmovdqa	[128+32+P1], ymm1
	vmovdqa	[128+64+P1], ymm2
	vmovdqa	[128+96+P1], ymm3

	add	P2, 256
	add	P1, 256

	sub	r10, 256
	jnz	.L2

	sub	P2, P3	; P2 now points to start.
	sub	P1, P3	; P1 now points to start.

	dec	P4
	jnz	.L1

	pop	r10

	ret

;------------------------------------------------------------------------------
; Name:		CopyVector128 (using SSE2)
; Purpose:	Copies memory chunks that are 16-byte aligned.
; Params:	P1 = ptr to destination memory area
;		P2 = ptr to source memory area
; 		P3 = length in bytes
; 		P4 = loops
;------------------------------------------------------------------------------
	align 64
CopyVector128:
_CopyVector128:
	mfence	

	push	r10

	shr	P3, 8	; Ensure length is multiple of 256.
	shl	P3, 8

	prefetcht0	[P2]

	; Save our non-parameter XMM registers.
	sub	rsp, 192
	movdqu	[rsp], xmm4
	movdqu	[16+rsp], xmm5
	movdqu	[32+rsp], xmm6
	movdqu	[48+rsp], xmm7
	movdqu	[64+rsp], xmm8
	movdqu	[80+rsp], xmm9
	movdqu	[96+rsp], xmm10
	movdqu	[112+rsp], xmm11
	movdqu	[128+rsp], xmm12
	movdqu	[144+rsp], xmm13
	movdqu	[160+rsp], xmm14
	movdqu	[176+rsp], xmm15

.L1:
	mov	r10, P3

.L2:
	movdqa	xmm0, [P2]
	movdqa	xmm1, [16+P2]
	movdqa	xmm2, [32+P2]
	movdqa	xmm3, [48+P2]

	movdqa	[P1], xmm0
	movdqa	[16+P1], xmm1
	movdqa	[32+P1], xmm2
	movdqa	[48+P1], xmm3

	movdqa	xmm4, [64+P2]
	movdqa	xmm5, [80+P2]
	movdqa	xmm6, [96+P2]
	movdqa	xmm7, [112+P2]

	movdqa	[64+P1], xmm4
	movdqa	[80+P1], xmm5
	movdqa	[96+P1], xmm6
	movdqa	[112+P1], xmm7

	movdqa	xmm8, [128+P2]
	movdqa	xmm9, [144+P2]
	movdqa	xmm10, [160+P2]
	movdqa	xmm11, [176+P2]

	movdqa	[128+P1], xmm8
	movdqa	[144+P1], xmm9
	movdqa	[160+P1], xmm10
	movdqa	[176+P1], xmm11

	movdqa	xmm12, [192+P2]
	movdqa	xmm13, [208+P2]
	movdqa	xmm14, [224+P2]
	movdqa	xmm15, [240+P2]

	movdqa	[192+P1], xmm12
	movdqa	[208+P1], xmm13
	movdqa	[224+P1], xmm14
	movdqa	[240+P1], xmm15

	add	P2, 256
	add	P1, 256

	sub	r10, 256
	jnz	.L2

	sub	P2, P3	; P2 now points to start.
	sub	P1, P3	; P1 now points to start.

	dec	P4
	jnz	.L1

	movdqu	xmm4, [rsp]
	movdqu	xmm5, [16+rsp]
	movdqu	xmm6, [32+rsp]
	movdqu	xmm7, [48+rsp]
	movdqu	xmm8, [64+rsp]
	movdqu	xmm9, [80+rsp]
	movdqu	xmm10, [96+rsp]
	movdqu	xmm11, [112+rsp]
	movdqu	xmm12, [128+rsp]
	movdqu	xmm13, [144+rsp]
	movdqu	xmm14, [160+rsp]
	movdqu	xmm15, [176+rsp]
	add	rsp, 192

	pop	r10

	ret

;------------------------------------------------------------------------------
; Name:		IncrementRegisters
; Purpose:	Increments 64-bit values in registers.
; Params:	P1 = loops
;------------------------------------------------------------------------------
	align 64
IncrementRegisters:
_IncrementRegisters:
.L1:
	inc	rax
	inc	rbx
	inc	P4
	inc	P3
	inc	P2
	inc	P1
	inc	rbp
	inc	rsp
	inc	r8
	inc	r9
	inc	r10
	inc	r11
	inc	r12
	inc	r13
	inc	r14
	inc	r15

	dec	rax
	dec	rbx
	dec	P4
	dec	P3
	dec	P2
	dec	P1
	dec	rbp
	dec	rsp
	dec	r8
	dec	r9
	dec	r10
	dec	r11
	dec	r12
	dec	r13
	dec	r14
	dec	r15

	dec	P1
	jnz	.L1
	ret


;------------------------------------------------------------------------------
; Name:		IncrementStack
; Purpose:	Increments 64-bit values on stack.
; Params:	P1 = loops
;------------------------------------------------------------------------------
	align 64
IncrementStack:
_IncrementStack:
	mfence
	sub	rsp, 128
.L1:
	inc	qword [rsp]
	inc	qword [rsp+8]
	inc	qword [rsp+16]
	inc	qword [rsp+24]
	inc	qword [rsp+32]
	inc	qword [rsp+40]
	inc	qword [rsp+48]
	inc	qword [rsp+56]
	inc	qword [rsp+64]
	inc	qword [rsp+72]
	inc	qword [rsp+80]
	inc	qword [rsp+88]
	inc	qword [rsp+96]
	inc	qword [rsp+104]
	inc	qword [rsp+112]
	inc	qword [rsp+120]

	dec	qword [rsp]
	dec	qword [rsp+8]
	dec	qword [rsp+16]
	dec	qword [rsp+24]
	dec	qword [rsp+32]
	dec	qword [rsp+40]
	dec	qword [rsp+48]
	dec	qword [rsp+56]
	dec	qword [rsp+64]
	dec	qword [rsp+72]
	dec	qword [rsp+80]
	dec	qword [rsp+88]
	dec	qword [rsp+96]
	dec	qword [rsp+104]
	dec	qword [rsp+112]
	dec	qword [rsp+120]

	dec	P1
	jnz	.L1

	add	rsp, 128

	ret

Reader_nontemporal:
_Reader_nontemporal:
	ret

;------------------------------------------------------------------------------
; Name:		RowHammerTest
; Purpose:	Perform RowHammer test
; Params:	P1 = ptr to memory area
; 		P2 = length in bytes
;		P3 = number of hammerings
; Returns:	1 if bit flip detected, 0 if not.
;------------------------------------------------------------------------------
	align 64
RowHammerTest:
_RowHammerTest:
	mfence
	push	r10
	push	r11
	push	r12

	; Because any two 4k pages may be in entirely different parts of
	; physical RAM due to the nature of virtual memory, this test 
	; has to be done within one 4k page. This also means that the 
	; P1 pointer ought to be 4k aligned.

	; Reset the provided buffer to all 1 bits.
	mov	r10, 0
	xor	rax, rax
	dec	rax
.L1:
	mov	[P1 + r10], rax
	add	r10, 8
	cmp	r10, P2
	jb	.L1

	mov	r11, P3

.Lhammer:
	mov	r12, 0

.Linner:
	mov	rax, [P1+r12]
	add	rax, [P1+r12+8]
	add	rax, [P1+r12+16]
	add	rax, [P1+r12+24]
	clflush	[P1+r12]
	add	r12, 32
	cmp	r12, P2
	jb	.Linner
	
	dec	r11
	jnz	.Lhammer

; Now that the hammering is done, check all of the bytes for flipped bits.
	mov	rax, 0
	dec	rax
	
	mov	r10, 0
.Lcheck:
	and	rax, [P1 + r10]
	add	r10, 8
	cmp	r10, P2
	jb	.Lcheck

	; Check if all bits were still 1.
	inc	rax
	test	rax, rax

	; RAX was zero, which happens to be the value for a passed test.
	jz	.Lreturn

	mov	rax, 1

.Lreturn:
	pop	r12
	pop	r11
	pop	r10
	ret


