#============================================================================
# bandwidth, a benchmark to measure memory bandwidth.
# 64-bit little-endian PowerPC routines, i.e. ppc64le.
# Copyright (C) 2026 by Zack T Smith.
#
# This program is free software; you can redistribute it and/or modify
# it under the terms of the GNU General Public License as published by
# the Free Software Foundation; either version 3 of the License, or
# (at your option) any later version.
#
# This program is distributed in the hope that it will be useful,
# but WITHOUT ANY WARRANTY; without even the implied warranty of
# MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
# GNU General Public License for more details.
#
# You should have received a copy of the GNU General Public License
# along with this program; if not, write to the Free Software
# Foundation, Inc., 51 Franklin St, Fifth Floor, Boston, MA  02110-1301  USA
#
# The author may be reached at 3 at zs3 dot me.
#=============================================================================
# Linux ABI:
# r3 function parameter and return value
# r4-r10 function parameters
# r14-r31 callee saved work area
#-----------------------------------------------------------------------------
# I tested this code in a Podman container running Debian.
# podman build --platform=linux/ppc64le -t ppc64le-image -f ~/Dockerfile ~/docker
# podman run -it ppc64le-image 
# FROM --platform=linux/ppc64le debian:trixie
# Et cetera.
#-----------------------------------------------------------------------------

	.text
	.machine power8
	.machine altivec
	.abiversion 2

	.globl 	Reader
	.globl 	RandomReader
	.type	Reader, @function
	.type	RandomReader, @function

	.globl 	Writer
	.globl 	RandomWriter
	.type	Writer, @function
	.type	RandomWriter, @function

	.globl	CopyWithMainRegisters
	.type	CopyWithMainRegisters, @function

	.globl	IncrementRegisters
	.globl	IncrementStack
	.globl 	StackReader
	.globl 	StackWriter
	.type	IncrementRegisters, @function
	.type	IncrementStack, @function
	.type	StackReader, @function
	.type	StackWriter, @function

# Unused:
	.globl 	WriterVector
	.globl 	ReaderVector
	.globl 	RandomReaderVector
	.globl 	RandomWriterVector
	.globl 	Register16ToVector
	.globl 	Register32ToVector
	.globl 	Register64ToVector
	.globl 	Register8ToVector
	.globl 	RegisterToRegister
	.globl 	RegisterToVector
	.globl 	Vector16ToRegister
	.globl 	Vector32ToRegister
	.globl 	Vector64ToRegister
	.globl 	Vector8ToRegister
	.globl 	VectorToRegister
	.globl 	VectorToVector
	.globl 	VectorToVector128
	.globl 	VectorToVector256

#-----------------------------------------------------------------------------
# Name: 	Writer
# Purpose:	Performs sequential write into memory, as fast as possible.
# Params:
#	r3 = address, 16-byte aligned
#	r4 = length, multiple of 256
#	r5 = count
# 	r6 = value to write
#-----------------------------------------------------------------------------
.align 16
Writer:
	sync

	mr	r9, r3
	mr	r10, r4
.Lw0:
	mr	r3, r9
	mr	r4, r10
.Lw1:
	std	r6, 0(r3)
	std	r6, 8(r3)
	std	r6, 16(r3)
	std	r6, 24(r3)
	std	r6, 32(r3)
	std	r6, 40(r3)
	std	r6, 48(r3)
	std	r6, 56(r3)
	std	r6, 64(r3)
	std	r6, 72(r3)
	std	r6, 80(r3)
	std	r6, 88(r3)
	std	r6, 96(r3)
	std	r6, 104(r3)
	std	r6, 112(r3)
	std	r6, 120(r3)
	addi	r3, r3, 128
	subi	r4, r4, 128
	cmpwi	r4, 0
	bne	.Lw1

	subi	r5, r5, 1
	cmpwi	r5, 0
	bne	.Lw0

	blr

#-----------------------------------------------------------------------------
# Name: 	RandomWriter
# Purpose:	Performs random write into memory, as fast as possible.
# Params:
# 	r3 = pointer to array of chunk pointers
# 	r4 = # of 256-byte chunks
# 	r5 = # loops to do
# 	r6 = value to write
#-----------------------------------------------------------------------------
# Variables:
#	r7 = chunk pointer
#	r8 = index into chunk pointers array
#	r9 = temporary left shift index
#-----------------------------------------------------------------------------
.align 16
RandomWriter:
	sync

.Lrw0:
	li	r8, 0

.Lrw1:
	## Get pointer to chunk in memory.
	sldi	r9, r8, 3
	add	r9, r9, r3
	ld	r7, 0(r9)

	## 32 transfers, 8 bytes each = 256 bytes total.
	std	r6, 160(r7)
	std	r6, 224(r7)
	std	r6, 232(r7)
	std	r6, 96(r7)
	std	r6, 248(r7)
	std	r6, 104(r7)
	std	r6, 136(r7)
	std	r6, 112(r7)
	std	r6, 200(r7)
	std	r6, 128(r7)
	std	r6, 216(r7)
	std	r6, 0(r7)
	std	r6, 184(r7)
	std	r6, 48(r7)
	std	r6, 64(r7)
	std	r6, 240(r7)
	std	r6, 24(r7)
	std	r6, 72(r7)
	std	r6, 32(r7)
	std	r6, 80(r7)
	std	r6, 56(r7)
	std	r6, 8(r7)
	std	r6, 208(r7)
	std	r6, 40(r7)
	std	r6, 120(r7)
	std	r6, 176(r7)
	std	r6, 16(r7)
	std	r6, 168(r7)
	std	r6, 88(r7)
	std	r6, 152(r7)
	std	r6, 192(r7)
	std	r6, 144(r7)

	# Proceed to next 256-byte chunk
	addi	r8, r8, 1
	cmpd	r8, r4
	bne	.Lrw1

	# Next loop
	subi	r5, r5, 1
	cmpwi	r5, 0
	bne	.Lrw0

	blr

#-----------------------------------------------------------------------------
# Name: 	Reader
# Purpose:	Performs sequential reads from memory, as fast as possible.
# Params:
#	r3 = address
#	r4 = length, multiple of 256
#	r5 = count
#-----------------------------------------------------------------------------
.align 16
Reader:
	sync

	mr	r9, r3
	mr	r10, r4
.Lr0:
	mr	r3, r9
	mr	r4, r10
.Lr1:
	ld	r6, 0(r3)
	ld	r6, 8(r3)
	ld	r6, 16(r3)
	ld	r6, 24(r3)
	ld	r6, 32(r3)
	ld	r6, 40(r3)
	ld	r6, 48(r3)
	ld	r6, 56(r3)
	ld	r6, 64(r3)
	ld	r6, 72(r3)
	ld	r6, 80(r3)
	ld	r6, 88(r3)
	ld	r6, 96(r3)
	ld	r6, 104(r3)
	ld	r6, 112(r3)
	ld	r6, 120(r3)
	addi	r3, r3, 128
	subi	r4, r4, 128
	cmpwi	r4, 0
	bne	.Lr1

	subi	r5, r5, 1
	cmpwi	r5, 0
	bne	.Lr0

	blr
	blr

#-----------------------------------------------------------------------------
# Name: 	RandomReader
# Purpose:	Performs random reads from memory, as fast as possible.
# Params:
# 	r3 = pointer to array of chunk pointers
# 	r4 = # of 256-byte chunks
# 	r5 = # loops to do
#-----------------------------------------------------------------------------
# Variables:
#	r6 = value that was read
#	r7 = pointer to current chunk
#	r8 = index within array of chunk pointers
#	r9 = temporary index * 8
#-----------------------------------------------------------------------------
.align 16
RandomReader:
	sync

.Lrr0:
	li	r8, 0

.Lrr1:
	# Get pointer to chunk in memory.
	sldi	r9, r8, 3
	add	r9, r9, r3
	ld	r7, 0(r9)

	## Does 32 transfers, 8 bytes each = 256 bytes total.
	ld	r6, 160(r7)
	ld	r6, 224(r7)
	ld	r6, 232(r7)
	ld	r6, 96(r7)
	ld	r6, 248(r7)
	ld	r6, 104(r7)
	ld	r6, 136(r7)
	ld	r6, 112(r7)
	ld	r6, 200(r7)
	ld	r6, 128(r7)
	ld	r6, 216(r7)
	ld	r6, 0(r7)
	ld	r6, 184(r7)
	ld	r6, 48(r7)
	ld	r6, 64(r7)
	ld	r6, 240(r7)
	ld	r6, 24(r7)
	ld	r6, 72(r7)
	ld	r6, 32(r7)
	ld	r6, 80(r7)
	ld	r6, 56(r7)
	ld	r6, 8(r7)
	ld	r6, 208(r7)
	ld	r6, 40(r7)
	ld	r6, 120(r7)
	ld	r6, 176(r7)
	ld	r6, 16(r7)
	ld	r6, 168(r7)
	ld	r6, 88(r7)
	ld	r6, 152(r7)
	ld	r6, 192(r7)
	ld	r6, 144(r7)

	addi	r8, r8, 1
	cmpd	r8, r4
	bne	.Lrr1

	subi	r5, r5, 1
	cmpwi	r5, 0
	bne	.Lrr0

	blr

#-----------------------------------------------------------------------------
# Name: 	RegisterToRegister
# Purpose:	Performs register-to-register transfers.
# Params:
#	r3 = count
#-----------------------------------------------------------------------------
.align 16
RegisterToRegister:
.L8:
	# Do 64 transfers 
	mr	r4, r5
	mr	r4, r6
	mr	r4, r7
	mr	r4, r8
	mr	r4, r9
	mr	r4, r10
	mr	r4, r14
	mr	r4, r15
	mr	r5, r4
	mr	r5, r6
	mr	r5, r7
	mr	r5, r8
	mr	r5, r9
	mr	r5, r10
	mr	r5, r14
	mr	r5, r15
	mr	r4, r5
	mr	r4, r6
	mr	r4, r7
	mr	r4, r8
	mr	r4, r9
	mr	r4, r10
	mr	r4, r14
	mr	r4, r15
	mr	r4, r5
	mr	r4, r6
	mr	r4, r7
	mr	r4, r8
	mr	r4, r9
	mr	r4, r10
	mr	r4, r14
	mr	r4, r15

	mr	r4, r5
	mr	r4, r6
	mr	r4, r7
	mr	r4, r8
	mr	r4, r9
	mr	r4, r10
	mr	r4, r14
	mr	r4, r15
	mr	r5, r4
	mr	r5, r6
	mr	r5, r7
	mr	r5, r8
	mr	r5, r9
	mr	r5, r10
	mr	r5, r14
	mr	r5, r15
	mr	r4, r5
	mr	r4, r6
	mr	r4, r7
	mr	r4, r8
	mr	r4, r9
	mr	r4, r10
	mr	r4, r14
	mr	r4, r15
	mr	r4, r5
	mr	r4, r6
	mr	r4, r7
	mr	r4, r8
	mr	r4, r9
	mr	r4, r10
	mr	r4, r14
	mr	r4, r15

	subi	r3, r3, 1
	cmpwi	r3, 0
	bne	.L8

	blr

#------------------------------------------------------------------------------
# Name:		IncrementRegisters
# Purpose:	Increments/decrements 64-bit values in registers.
# Params:	r3 = count
#------------------------------------------------------------------------------
.align 16
IncrementRegisters:
.Li1:
	addi	r4, r4, 1
	addi	r5, r5, 1
	addi	r6, r6, 1
	addi	r7, r7, 1
	addi	r8, r8, 1
	addi	r9, r9, 1
	addi	r10, r10, 1
	addi	r14, r14, 1

	subi	r4, r4, 1
	subi	r5, r5, 1
	subi	r6, r6, 1
	subi	r7, r7, 1
	subi	r8, r8, 1
	subi	r9, r9, 1
	subi	r10, r10, 1
	subi	r14, r14, 1

	addi	r4, r4, 1
	addi	r5, r5, 1
	addi	r6, r6, 1
	addi	r7, r7, 1
	addi	r8, r8, 1
	addi	r9, r9, 1
	addi	r10, r10, 1
	addi	r14, r14, 1

	subi	r4, r4, 1
	subi	r5, r5, 1
	subi	r6, r6, 1
	subi	r7, r7, 1
	subi	r8, r8, 1
	subi	r9, r9, 1
	subi	r10, r10, 1
	subi	r14, r14, 1

	subi	r3, r3, 1
	cmpwi	r3, 0
	bne	.Li1

	blr

#------------------------------------------------------------------------------
# Name:		IncrementStack
# Purpose:	Increments 64-bit values on stack.
# Params:	r3 = count
#------------------------------------------------------------------------------
.align 16
IncrementStack:
	sync

	subi	r1, r1, 16
	std	r30, 0(r1)
	std	r31, 8(r1)

	# Use r30 as the stack array pointer.
	subi	r30, r1, 64

.Lis:
	# Perform 32 increments
	ld	r4, 0(r30)
	ld	r5, 8(r30)
	ld	r6, 16(r30)
	ld	r7, 24(r30)
	ld	r8, 32(r30)
	ld	r9, 40(r30)
	ld	r10, 48(r30)
	ld	r31, 56(r30)
	addi	r4, r4, 1
	addi	r5, r5, 1
	addi	r6, r6, 1
	addi	r7, r7, 1
	addi	r8, r8, 1
	addi	r9, r9, 1
	addi	r10, r10, 1
	addi	r31, r31, 1
	std	r4, 0(r30)
	std	r5, 8(r30)
	std	r6, 16(r30)
	std	r7, 24(r30)
	std	r8, 32(r30)
	std	r9, 40(r30)
	std	r10, 48(r30)
	std	r31, 56(r30)

	ld	r4, 0(r30)
	ld	r5, 8(r30)
	ld	r6, 16(r30)
	ld	r7, 24(r30)
	ld	r8, 32(r30)
	ld	r9, 40(r30)
	ld	r10, 48(r30)
	ld	r31, 56(r30)
	subi	r4, r4, 1
	subi	r5, r5, 1
	subi	r6, r6, 1
	subi	r7, r7, 1
	subi	r8, r8, 1
	subi	r9, r9, 1
	subi	r10, r10, 1
	subi	r31, r31, 1
	std	r4, 0(r30)
	std	r5, 8(r30)
	std	r6, 16(r30)
	std	r7, 24(r30)
	std	r8, 32(r30)
	std	r9, 40(r30)
	std	r10, 48(r30)
	std	r31, 56(r30)

	ld	r4, 0(r30)
	ld	r5, 8(r30)
	ld	r6, 16(r30)
	ld	r7, 24(r30)
	ld	r8, 32(r30)
	ld	r9, 40(r30)
	ld	r10, 48(r30)
	ld	r31, 56(r30)
	addi	r4, r4, 1
	addi	r5, r5, 1
	addi	r6, r6, 1
	addi	r7, r7, 1
	addi	r8, r8, 1
	addi	r9, r9, 1
	addi	r10, r10, 1
	addi	r31, r31, 1
	std	r4, 0(r30)
	std	r5, 8(r30)
	std	r6, 16(r30)
	std	r7, 24(r30)
	std	r8, 32(r30)
	std	r9, 40(r30)
	std	r10, 48(r30)
	std	r31, 56(r30)

	ld	r4, 0(r30)
	ld	r5, 8(r30)
	ld	r6, 16(r30)
	ld	r7, 24(r30)
	ld	r8, 32(r30)
	ld	r9, 40(r30)
	ld	r10, 48(r30)
	ld	r31, 56(r30)
	subi	r4, r4, 1
	subi	r5, r5, 1
	subi	r6, r6, 1
	subi	r7, r7, 1
	subi	r8, r8, 1
	subi	r9, r9, 1
	subi	r10, r10, 1
	subi	r31, r31, 1
	std	r4, 0(r30)
	std	r5, 8(r30)
	std	r6, 16(r30)
	std	r7, 24(r30)
	std	r8, 32(r30)
	std	r9, 40(r30)
	std	r10, 48(r30)
	std	r31, 56(r30)
	
	subi	r3, r3, 1
	cmpwi	r3, 0
	bne	.Lis

	ld	r30, 0(r1)
	ld	r31, 8(r1)
	addi	r1, r1, 16

	blr

#------------------------------------------------------------------------------
# Name:		StackReader
# Purpose:	Reads 64-bit values off the stack into registers of
#		the main register set, effectively testing L1 cache access
#		and effective-address calculation speed.
# Params:	r3 = loops
#------------------------------------------------------------------------------
.align 16
StackReader:
	sync

	subi	r1, r1, 64

.Lsr:
	# 64 transfers
	ld	r4, 0(r1)
	ld	r4, 16(r1)
	ld	r4, 24(r1)
	ld	r4, 32(r1)
	ld	r4, 40(r1)
	ld	r4, 8(r1)
	ld	r4, 48(r1)
	ld	r4, 0(r1)
	ld	r4, 56(r1)
	ld	r4, 16(r1)
	ld	r4, 24(r1)
	ld	r4, 32(r1)
	ld	r7, 40(r1)
	ld	r7, 8(r1)
	ld	r7, 48(r1)
	ld	r7, 56(r1)
	ld	r7, 0(r1)
	ld	r7, 16(r1)
	ld	r7, 24(r1)
	ld	r7, 32(r1)
	ld	r4, 40(r1)
	ld	r4, 8(r1)
	ld	r4, 48(r1)
	ld	r4, 8(r1)
	ld	r4, 56(r1)
	ld	r4, 16(r1)
	ld	r4, 24(r1)
	ld	r4, 32(r1)
	ld	r4, 40(r1)
	ld	r5, 8(r1)
	ld	r5, 48(r1)
	ld	r5, 56(r1)
	ld	r5, 0(r1)
	ld	r5, 16(r1)
	ld	r5, 24(r1)
	ld	r5, 32(r1)
	ld	r5, 40(r1)
	ld	r5, 8(r1)
	ld	r5, 48(r1)
	ld	r5, 0(r1)
	ld	r5, 56(r1)
	ld	r5, 16(r1)
	ld	r5, 24(r1)
	ld	r5, 32(r1)
	ld	r6, 40(r1)
	ld	r6, 8(r1)
	ld	r6, 48(r1)
	ld	r6, 56(r1)
	ld	r6, 0(r1)
	ld	r6, 16(r1)
	ld	r6, 24(r1)
	ld	r6, 32(r1)
	ld	r6, 40(r1)
	ld	r6, 8(r1)
	ld	r6, 48(r1)
	ld	r6, 8(r1)
	ld	r5, 56(r1)
	ld	r5, 16(r1)
	ld	r5, 24(r1)
	ld	r5, 32(r1)
	ld	r5, 40(r1)
	ld	r5, 8(r1)
	ld	r5, 48(r1)
	ld	r5, 56(r1)

	subi	r3, r3, 1
	cmpwi	r3, 0
	bne	.Lsr

	addi	r1, r1, 64
	blr

#------------------------------------------------------------------------------
# Name:		StackWriter
# Purpose:	Writes 64-bit values into the stack from registers of
#		the main register set, effectively testing L1 cache speed.
# Params:	r3 = loops
#------------------------------------------------------------------------------
.align 16
StackWriter:
	sync

	subi	r1, r1, 64

.Lsw:
	# 64 transfers
	std	r3, 0(r1)
	std	r3, 16(r1)
	std	r3, 24(r1)
	std	r3, 32(r1)
	std	r3, 40(r1)
	std	r3, 8(r1)
	std	r3, 48(r1)
	std	r3, 0(r1)
	std	r3, 56(r1)
	std	r3, 16(r1)
	std	r3, 24(r1)
	std	r3, 32(r1)
	std	r3, 40(r1)
	std	r3, 8(r1)
	std	r3, 48(r1)
	std	r3, 56(r1)
	std	r3, 0(r1)
	std	r3, 16(r1)
	std	r3, 24(r1)
	std	r3, 32(r1)
	std	r3, 40(r1)
	std	r3, 8(r1)
	std	r3, 48(r1)
	std	r3, 8(r1)
	std	r3, 56(r1)
	std	r3, 16(r1)
	std	r3, 24(r1)
	std	r3, 32(r1)
	std	r3, 40(r1)
	std	r3, 8(r1)
	std	r3, 48(r1)
	std	r3, 56(r1)
	std	r3, 0(r1)
	std	r3, 16(r1)
	std	r3, 24(r1)
	std	r3, 32(r1)
	std	r3, 40(r1)
	std	r3, 8(r1)
	std	r3, 48(r1)
	std	r3, 0(r1)
	std	r3, 56(r1)
	std	r3, 16(r1)
	std	r3, 24(r1)
	std	r3, 32(r1)
	std	r3, 40(r1)
	std	r3, 8(r1)
	std	r3, 48(r1)
	std	r3, 56(r1)
	std	r3, 0(r1)
	std	r3, 16(r1)
	std	r3, 24(r1)
	std	r3, 32(r1)
	std	r3, 40(r1)
	std	r3, 8(r1)
	std	r3, 48(r1)
	std	r3, 8(r1)
	std	r3, 56(r1)
	std	r3, 16(r1)
	std	r3, 24(r1)
	std	r3, 32(r1)
	std	r3, 40(r1)
	std	r3, 8(r1)
	std	r3, 48(r1)
	std	r3, 56(r1)

	subi	r3, r3, 1
	cmpwi	r3, 0
	bne	.Lsw

	addi	r1, r1, 64
	blr

#-----------------------------------------------------------------------------
# Name: 	CopyWithMainRegisters
# Purpose:	Performs memory copy, as fast as possible.
# Params:
# 	r3 = pointer to destination array 
# 	r4 = pointer to source array
# 	r5 = # of bytes (multiple of 256)
# 	r6 = # loops to do
#-----------------------------------------------------------------------------
# Variables:
#	r7 = backup of r3
#	r8 = backup of r4
#	r9 = backup of r5
#	r10 = buffer word 0
#	r14 = buffer word 1
#	r15 = buffer word 2
#	r16 = buffer word 3
#	r17 = buffer word 4
#	r18 = buffer word 5
#	r19 = buffer word 6
#	r20 = buffer word 7
#-----------------------------------------------------------------------------
.align 16
CopyWithMainRegisters:
_CopyWithMainRegisters:
	sync

	subi	sp, sp, 56
	std	r14, 0(sp)
	std	r15, 8(sp)
	std	r16, 16(sp)
	std	r17, 24(sp)
	std	r18, 32(sp)
	std	r19, 40(sp)
	std	r20, 48(sp)

	mr	r7, r3
	mr	r8, r4
	mr	r9, r5

.Lcr0:
	mr	r3, r7
	mr	r4, r8
	mr	r5, r9

.Lcr1:
	ld	r10, 0(r4)
	ld	r14, 8(r4)
	ld	r15, 16(r4)
	ld	r16, 24(r4)
	ld	r17, 32(r4)
	ld	r18, 40(r4)
	ld	r19, 48(r4)
	ld	r20, 56(r4)
	addi	r4, r4, 64
	
	std	r10, 0(r3)
	std	r14, 8(r3)
	std	r15, 16(r3)
	std	r16, 24(r3)
	std	r17, 32(r3)
	std	r18, 40(r3)
	std	r19, 48(r3)
	std	r20, 56(r3)
	addi	r3, r3, 64

	subi	r5, r5, 64
	cmpwi	r5, 0
	bne	.Lcr1

	subi	r6, r6, 1
	cmpwi	r6, 0
	bne	.Lcr0

	ld	r14, 0(sp)
	ld	r15, 8(sp)
	ld	r16, 16(sp)
	ld	r17, 24(sp)
	ld	r18, 32(sp)
	ld	r19, 40(sp)
	ld	r20, 48(sp)
	addi	sp, sp, 56

	blr

WriterVector:
ReaderVector:
RandomReaderVector:
RandomWriterVector:
Register16ToVector:
Register32ToVector:
Register64ToVector:
Register8ToVector:
RegisterToVector:
Vector16ToRegister:
Vector32ToRegister:
Vector64ToRegister:
Vector8ToRegister:
VectorToRegister:
VectorToVector:
VectorToVector128:
VectorToVector256:
	blr

