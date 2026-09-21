#============================================================================
# bandwidth, a benchmark to measure memory bandwidth.
# 64-bit RISC-V (riscv64) routines.
# Copyright (C) 2023-2024 by Zack T Smith.
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

	.globl 	Reader
	.globl 	RandomReader

	.globl 	Writer
	.globl 	RandomWriter

	.globl	CopyWithMainRegisters

	.globl	IncrementRegisters
	.globl	IncrementStack
	.globl 	StackReader
	.globl 	StackWriter

# Unused:
	.globl 	WriterVector128
	.globl 	ReaderVector128
	.globl 	RandomReaderVector128
	.globl 	RandomWriterVector128
	.globl 	Register16ToVector128
	.globl 	Register32ToVector128
	.globl 	Register64ToVector128
	.globl 	Register8ToVector128
	.globl 	RegisterToRegister
	.globl 	Vector128ToRegister16
	.globl 	Vector128ToRegister32
	.globl 	Vector128ToRegister64
	.globl 	Vector128ToRegister8
	.globl 	VectorToVector128
	.globl 	VectorToVector256
	.globl 	VectorToVector512
	.globl	RegisterToVectorMove
	.globl	VectorToRegisterMove

#-----------------------------------------------------------------------------
# Name: 	Writer
# Purpose:	Performs sequential write into memory, as fast as possible.
# Params:
#	a0 = address, 16-byte aligned
#	a1 = length, multiple of 256
#	a2 = count
# 	a3 = value to write
#-----------------------------------------------------------------------------
.align 4
Writer:
	fence	w, w

	srli 	a0, a0, 4	# Clear lower 4 bits of a0
	slli 	a0, a0, 4
	
	mv	t0, a0
	mv	t1, a1

	mv	a4, a3
	mv	a5, a3
	mv 	a6, a3
	mv 	a7, a3

.Lw0:
	mv	a0, t0
	mv	a1, t1

.Lw1:
	sd	a0, (a0)
	sd	a1, 8(a0)
	sd	a2, 16(a0)
	sd	a3, 24(a0)
	sd	a4, 32(a0)
	sd	a5, 40(a0)
	sd	a6, 48(a0)
	sd	a7, 56(a0)
	addi	a0, a0, 64
	
	sd	a0, (a0)
	sd	a1, 8(a0)
	sd	a2, 16(a0)
	sd	a3, 24(a0)
	sd	a4, 32(a0)
	sd	a5, 40(a0)
	sd	a6, 48(a0)
	sd	a7, 56(a0)
	addi	a0, a0, 64
	
	sd	a0, (a0)
	sd	a1, 8(a0)
	sd	a2, 16(a0)
	sd	a3, 24(a0)
	sd	a4, 32(a0)
	sd	a5, 40(a0)
	sd	a6, 48(a0)
	sd	a7, 56(a0)
	addi	a0, a0, 64
	
	sd	a0, (a0)
	sd	a1, 8(a0)
	sd	a2, 16(a0)
	sd	a3, 24(a0)
	sd	a4, 32(a0)
	sd	a5, 40(a0)
	sd	a6, 48(a0)
	sd	a7, 56(a0)
	addi	a0, a0, 64

	addi	a1, a1, -256
	bne	a1, zero, .Lw1

	addi	a2, a2, -1
	bne	a2, zero, .Lw0

	# Flush all data going to the write area
	# that hasn't made it into RAM yet.
	fence	w, w
	ret

#-----------------------------------------------------------------------------
# Name: 	RandomWriter
# Purpose:	Performs random write into memory, as fast as possible.
# Params:
# 	a0 = pointer to array of chunk pointers
# 	a1 = # of 256-byte chunks
# 	a2 = # loops to do
# 	a3 = value to write
#-----------------------------------------------------------------------------
.align 4
RandomWriter:
	fence	w, w
	addi	sp, sp, -32
	sd	a2, (sp)
	sd	a3, 8(sp)
	sd	a4, 16(sp)
	sd	a5, 24(sp)

.Lrw0:
	li	a5, 0

.Lrw1:
	## Get pointer to chunk in memory.
	slli	t0, a5, 3
	add	t0, t0, a0
	ld	a4, (t0)

	## 32 transfers, 8 bytes each = 256 bytes total.
	ld	a6, 160(a4)
	sd	a3, 160(a4)
	sd	a3, 224(a4)
	sd	a3, 232(a4)
	sd	a3, 96(a4)
	sd	a3, 248(a4)
	sd	a3, 104(a4)
	sd	a3, 136(a4)
	sd	a3, 112(a4)
	sd	a3, 200(a4)
	sd	a3, 128(a4)
	sd	a3, 216(a4)
	sd	a3, (a4)
	sd	a3, 184(a4)
	sd	a3, 48(a4)
	sd	a3, 64(a4)
	sd	a6, 240(a4)
	sd	a3, 24(a4)
	sd	a3, 72(a4)
	sd	a3, 32(a4)
	sd	a3, 80(a4)
	sd	a3, 56(a4)
	sd	a3, 8(a4)
	sd	a3, 208(a4)
	sd	a3, 40(a4)
	sd	a3, 120(a4)
	sd	a3, 176(a4)
	sd	a3, 16(a4)
	sd	a3, 168(a4)
	sd	a3, 88(a4)
	sd	a3, 152(a4)
	sd	a3, 192(a4)
	sd	a3, 144(a4)

	# Proceed to next 256-byte chunk
	addi	a5, a5, 1
	bltu	a5, a1, .Lrw1

	# Next loop
	addi	a2, a2, -1
	bnez	a2, .Lrw0

	ld	a2, (sp)
	ld	a3, 8(sp)
	ld	a4, 16(sp)
	ld	a5, 24(sp)
	addi	sp, sp, 32

	# Flush all data going to the write area
	# that hasn't made it into RAM yet.
	fence	w, w
	ret

#-----------------------------------------------------------------------------
# Name: 	Reader
# Purpose:	Performs sequential reads from memory, as fast as possible.
# Params:
#	a0 = address
#	a1 = length, multiple of 256
#	a2 = count
#-----------------------------------------------------------------------------
.align 4
Reader:
	fence	r, r
	# Save inner loop data
	mv	t0, a0
	mv	t1, a1

.Lr0:
	# Restore inner loop data
	mv	a0, t0
	mv	a1, t1

.Lr1:
	# Total of 32 transfers x 8 bytes = 256 bytes.
	# 8 transfers, 8 bytes each
	ld	a3, (a0)
	ld	a4, 8(a0)
	ld	a5, 16(a0)
	ld	a6, 24(a0)
	ld	a7, 32(a0)
	ld	t2, 40(a0)
	ld	t3, 48(a0)
	ld	t4, 56(a0)
	addi	a0, a0, 64
	# 8 transfers, 8 bytes each
	ld	a3, (a0)
	ld	a4, 8(a0)
	ld	a5, 16(a0)
	ld	a6, 24(a0)
	ld	a7, 32(a0)
	ld	t2, 40(a0)
	ld	t3, 48(a0)
	ld	t4, 56(a0)
	addi	a0, a0, 64
	# 8 transfers, 8 bytes each
	ld	a3, (a0)
	ld	a4, 8(a0)
	ld	a5, 16(a0)
	ld	a6, 24(a0)
	ld	a7, 32(a0)
	ld	t2, 40(a0)
	ld	t3, 48(a0)
	ld	t4, 56(a0)
	addi	a0, a0, 64
	# 8 transfers, 8 bytes each
	ld	a3, (a0)
	ld	a4, 8(a0)
	ld	a5, 16(a0)
	ld	a6, 24(a0)
	ld	a7, 32(a0)
	ld	t2, 40(a0)
	ld	t3, 48(a0)
	ld	t4, 56(a0)
	addi	a0, a0, 64

	addi	a1, a1, -256
	bnez	a1, .Lr1
	
	addi	a2, a2, -1
	bnez	a2, .Lr0

	ret

#-----------------------------------------------------------------------------
# Name: 	RandomReader
# Purpose:	Performs random reads from memory, as fast as possible.
# Params:
# 	a0 = pointer to array of chunk pointers
# 	a1 = # of 256-byte chunks
# 	a2 = # loops to do
#-----------------------------------------------------------------------------
.align 4
RandomReader:
	fence	r, r

.Lrr0:
	mv	a5, zero

.Lrr1:
	# Get pointer to chunk in memory.
	slli	t0, a5, 3
	add	t0, t0, a0
	ld	a4, (t0)

	## Does 32 transfers, 8 bytes each = 256 bytes total.
	ld	a3, 160(a4)
	ld	a3, 224(a4)
	ld	a3, 232(a4)
	ld	a3, 96(a4)
	ld	a3, 248(a4)
	ld	a3, 104(a4)
	ld	a3, 136(a4)
	ld	a3, 112(a4)
	ld	a3, 200(a4)
	ld	a3, 128(a4)
	ld	a3, 216(a4)
	ld	a3, (a4)
	ld	a3, 184(a4)
	ld	a3, 48(a4)
	ld	a3, 64(a4)
	ld	a3, 240(a4)
	ld	a3, 24(a4)
	ld	a3, 72(a4)
	ld	a3, 32(a4)
	ld	a3, 80(a4)
	ld	a3, 56(a4)
	ld	a3, 8(a4)
	ld	a3, 208(a4)
	ld	a3, 40(a4)
	ld	a3, 120(a4)
	ld	a3, 176(a4)
	ld	a3, 16(a4)
	ld	a3, 168(a4)
	ld	a3, 88(a4)
	ld	a3, 152(a4)
	ld	a3, 192(a4)
	ld	a3, 144(a4)

	addi	a5, a5, 1
	bne	a5, a1, .Lrr1

	addi	a2, a2, -1
	#bne	a2, zero, .Lrr0
	bnez	a2, .Lrr0

	ret

#-----------------------------------------------------------------------------
# Name: 	RegisterToRegister
# Purpose:	Performs register-to-register transfers.
# Params:
#	a0 = count
#-----------------------------------------------------------------------------
.align 4
RegisterToRegister:

.Lr2r:
	# Do 64 transfers 
	mv	a1, a2
	mv	t1, a3
	mv	t2, a4
	mv	a1, a5
	mv	a1, a6
	mv	a1, a7
	mv	a1, t0
	mv	a1, t1
	mv	t2, a1
	mv	a2, a3
	mv	t4, a4
	mv	a7, a5
	mv	a2, a6
	mv	a2, a7
	mv	a2, t0
	mv	a2, t1
	mv	a1, a2
	mv	a6, a3
	mv	t1, a4
	mv	a1, a5
	mv	a2, a6
	mv	a1, a7
	mv	a1, t0
	mv	a1, t1
	mv	a1, a2
	mv	a7, a3
	mv	a1, a4
	mv	a3, a5
	mv	t0, a6
	mv	a1, a7
	mv	a6, t0
	mv	a1, t1

	mv	a1, a2
	mv	t1, a3
	mv	t2, a4
	mv	a1, a5
	mv	a1, a6
	mv	a1, a7
	mv	a1, t0
	mv	a1, t1
	mv	t2, a1
	mv	a2, a3
	mv	t4, a4
	mv	a7, a5
	mv	a2, a6
	mv	a2, a7
	mv	a2, t0
	mv	a2, t1
	mv	a1, a2
	mv	a6, a3
	mv	t1, a4
	mv	a1, a5
	mv	a2, a6
	mv	a1, a7
	mv	a1, t0
	mv	a1, t1
	mv	a1, a2
	mv	a7, a3
	mv	a1, a4
	mv	a3, a5
	mv	t0, a6
	mv	a1, a7
	mv	a6, t0
	mv	a1, t1

	addi	a0, a0, -1
	bnez	a0, .Lr2r

	ret

#------------------------------------------------------------------------------
# Name:		IncrementRegisters
# Purpose:	Increments/decrements 64-bit values in registers.
# Params:	a0 = count
#------------------------------------------------------------------------------
.align 4
IncrementRegisters:
	
.Li1:
	# 32 operations
	addi	a1, a1, 1
	addi	a2, a2, 1
	addi	a3, a3, 1
	addi	a4, a4, 1
	addi	a5, a5, 1
	addi	a6, a6, 1
	addi	a7, a7, 1
	addi	t0, t0, 1

	addi	a1, a1, -1
	addi	a2, a2, -1
	addi	a3, a3, -1
	addi	a4, a4, -1
	addi	a5, a5, -1
	addi	a6, a6, -1
	addi	a7, a7, -1
	addi	t0, t0, -1

	addi	a1, a1, 1
	addi	a2, a2, 1
	addi	a3, a3, 1
	addi	a4, a4, 1
	addi	a5, a5, 1
	addi	a6, a6, 1
	addi	a7, a7, 1
	addi	t0, t0, 1

	addi	a1, a1, -1
	addi	a2, a2, -1
	addi	a3, a3, -1
	addi	a4, a4, -1
	addi	a5, a5, -1
	addi	a6, a6, -1
	addi	a7, a7, -1
	addi	t0, t0, -1

	addi	a0, a0, -1
	bnez	a0, .Li1

	ret

#------------------------------------------------------------------------------
# Name:		IncrementStack
# Purpose:	Increments 64-bit values on stack.
# Params:	a0 = count
#------------------------------------------------------------------------------
.align 4
IncrementStack:
	fence	w, r
	addi	sp, sp, -64

.Lis1:
	# Perform 32 increments
	ld	a1, (sp)
	ld	a2, 8(sp)
	ld	a3, 16(sp)
	ld	a4, 24(sp)
	ld	a5, 32(sp)
	ld	a6, 40(sp)
	ld	a7, 48(sp)
	ld	t0, 56(sp)
	addi	a1, a1, 1
	addi	a2, a2, 1
	addi	a3, a3, 1
	addi	a4, a4, 1
	addi	a5, a5, 1
	addi	a6, a6, 1
	addi	a7, a7, 1
	addi	t0, t0, 1
	sd	a1, (sp)
	sd	a2, 8(sp)
	sd	a3, 16(sp)
	sd	a4, 24(sp)
	sd	a5, 32(sp)
	sd	a6, 40(sp)
	sd	a7, 48(sp)
	sd	t0, 56(sp)

	ld	a1, (sp)
	ld	a2, 8(sp)
	ld	a3, 16(sp)
	ld	a4, 24(sp)
	ld	a5, 32(sp)
	ld	a6, 40(sp)
	ld	a7, 48(sp)
	ld	t0, 56(sp)
	addi	a1, a1, 1
	addi	a2, a2, 1
	addi	a3, a3, 1
	addi	a4, a4, 1
	addi	a5, a5, 1
	addi	a6, a6, 1
	addi	a7, a7, 1
	addi	t0, t0, 1
	sd	a1, (sp)
	sd	a2, 8(sp)
	sd	a3, 16(sp)
	sd	a4, 24(sp)
	sd	a5, 32(sp)
	sd	a6, 40(sp)
	sd	a7, 48(sp)
	sd	t0, 56(sp)

	ld	a1, (sp)
	ld	a2, 8(sp)
	ld	a3, 16(sp)
	ld	a4, 24(sp)
	ld	a5, 32(sp)
	ld	a6, 40(sp)
	ld	a7, 48(sp)
	ld	t0, 56(sp)
	addi	a1, a1, 1
	addi	a2, a2, 1
	addi	a3, a3, 1
	addi	a4, a4, 1
	addi	a5, a5, 1
	addi	a6, a6, 1
	addi	a7, a7, 1
	addi	t0, t0, 1
	sd	a1, (sp)
	sd	a2, 8(sp)
	sd	a3, 16(sp)
	sd	a4, 24(sp)
	sd	a5, 32(sp)
	sd	a6, 40(sp)
	sd	a7, 48(sp)
	sd	t0, 56(sp)

	ld	a1, (sp)
	ld	a2, 8(sp)
	ld	a3, 16(sp)
	ld	a4, 24(sp)
	ld	a5, 32(sp)
	ld	a6, 40(sp)
	ld	a7, 48(sp)
	ld	t0, 56(sp)
	addi	a1, a1, 1
	addi	a2, a2, 1
	addi	a3, a3, 1
	addi	a4, a4, 1
	addi	a5, a5, 1
	addi	a6, a6, 1
	addi	a7, a7, 1
	addi	t0, t0, 1
	sd	a1, (sp)
	sd	a2, 8(sp)
	sd	a3, 16(sp)
	sd	a4, 24(sp)
	sd	a5, 32(sp)
	sd	a6, 40(sp)
	sd	a7, 48(sp)
	sd	t0, 56(sp)

	addi	a0, a0, -1
	bnez	a0, .Lis1

	addi	sp, sp, 64
	ret

#------------------------------------------------------------------------------
# Name:		StackReader
# Purpose:	Reads 64-bit values off the stack into registers of
#		the main register set, effectively testing L1 cache access
#		and effective-address calculation speed.
# Params:	a0 = loops
#------------------------------------------------------------------------------
.align 16
StackReader:
	fence	r, r
	addi	sp, sp, -64

.Lsr1:
	# 64 transfers
	ld	a1, 56(sp)
	ld	a2, 16(sp)
	ld	a3, 24(sp)
	ld	a4, 32(sp)
	ld	a5, 40(sp)
	ld	a6, 8(sp)
	ld	a7, 48(sp)
	ld	a1, (sp)
	ld	a2, 16(sp)
	ld	a3, 56(sp)
	ld	a4, 24(sp)
	ld	a5, 32(sp)
	ld	a6, 40(sp)
	ld	a7, 8(sp)
	ld	a1, 48(sp)
	ld	a2, (sp)
	ld	a3, 16(sp)
	ld	a4, 24(sp)
	ld	a5, 32(sp)
	ld	a6, (sp)
	ld	a7, 40(sp)
	ld	a1, 8(sp)
	ld	a2, 48(sp)
	ld	a3, 8(sp)
	ld	a4, 8(sp)
	ld	a5, 16(sp)
	ld	a6, 24(sp)
	ld	a7, 32(sp)
	ld	a1, 40(sp)
	ld	a2, 8(sp)
	ld	a3, 48(sp)
	ld	a4, 8(sp)

	ld	a1, 56(sp)
	ld	a2, 16(sp)
	ld	a3, 24(sp)
	ld	a4, 32(sp)
	ld	a5, 40(sp)
	ld	a6, 8(sp)
	ld	a7, 48(sp)
	ld	a1, (sp)
	ld	a2, 16(sp)
	ld	a3, 56(sp)
	ld	a4, 24(sp)
	ld	a5, 32(sp)
	ld	a6, 40(sp)
	ld	a7, 8(sp)
	ld	a1, 48(sp)
	ld	a2, (sp)
	ld	a3, 16(sp)
	ld	a4, 24(sp)
	ld	a5, 32(sp)
	ld	a6, (sp)
	ld	a7, 40(sp)
	ld	a1, 8(sp)
	ld	a2, 48(sp)
	ld	a3, 8(sp)
	ld	a4, 8(sp)
	ld	a5, 16(sp)
	ld	a6, 24(sp)
	ld	a7, 32(sp)
	ld	a1, 40(sp)
	ld	a2, 8(sp)
	ld	a3, 48(sp)
	ld	a4, 8(sp)

	addi	a0, a0, -1
	bnez	a0, .Lsr1

	addi	sp, sp, 64
	ret

#------------------------------------------------------------------------------
# Name:		StackWriter
# Purpose:	Writes 64-bit values into the stack from registers of
#		the main register set, effectively testing L1 cache access
#		and effective-address calculation speed.
# Params:	a0 = loops
#------------------------------------------------------------------------------
.align 16
StackWriter:
	fence	w, w
	addi	sp, sp, -64

	li	a1, 0x123

.Lsw1:
	# 64 transfers
	sd	a1, 56(sp)
	sd	a1, 16(sp)
	sd	a1, 24(sp)
	sd	a1, 32(sp)
	sd	a1, 40(sp)
	sd	a1, 8(sp)
	sd	a1, 48(sp)
	sd	a1, (sp)
	sd	a1, 16(sp)
	sd	a1, 24(sp)
	sd	a1, 56(sp)
	sd	a1, 32(sp)
	sd	a1, 40(sp)
	sd	a1, 8(sp)
	sd	a1, 48(sp)
	sd	a1, (sp)
	sd	a1, 16(sp)
	sd	a1, 24(sp)
	sd	a1, 32(sp)
	sd	a1, 40(sp)
	sd	a1, 8(sp)
	sd	a1, 48(sp)
	sd	a1, 8(sp)
	sd	a1, 8(sp)
	sd	a1, 16(sp)
	sd	a1, 56(sp)
	sd	a1, 24(sp)
	sd	a1, 32(sp)
	sd	a1, 40(sp)
	sd	a1, 8(sp)
	sd	a1, 48(sp)
	sd	a1, 8(sp)

	sd	a1, 56(sp)
	sd	a1, 16(sp)
	sd	a1, 24(sp)
	sd	a1, 32(sp)
	sd	a1, 40(sp)
	sd	a1, 8(sp)
	sd	a1, 48(sp)
	sd	a1, (sp)
	sd	a1, 16(sp)
	sd	a1, 24(sp)
	sd	a1, 56(sp)
	sd	a1, 32(sp)
	sd	a1, 40(sp)
	sd	a1, 8(sp)
	sd	a1, 48(sp)
	sd	a1, (sp)
	sd	a1, 16(sp)
	sd	a1, 24(sp)
	sd	a1, 32(sp)
	sd	a1, 40(sp)
	sd	a1, 8(sp)
	sd	a1, 48(sp)
	sd	a1, 8(sp)
	sd	a1, 8(sp)
	sd	a1, 16(sp)
	sd	a1, 56(sp)
	sd	a1, 24(sp)
	sd	a1, 32(sp)
	sd	a1, 40(sp)
	sd	a1, 8(sp)
	sd	a1, 48(sp)
	sd	a1, 8(sp)

	addi	a0, a0, -1
	bnez	a0, .Lsw1

	addi	sp, sp, 64

	fence	w, w
	ret

#-----------------------------------------------------------------------------
# Name: 	CopyWithMainRegisters
# Purpose:	Performs memory copy, as fast as possible.
# Params:
# 	a0 = pointer to destination array 
# 	a1 = pointer to source array
# 	a2 = # of bytes (multiple of 256)
# 	a3 = # loops to do
#-----------------------------------------------------------------------------
.align 4
CopyWithMainRegisters:
_CopyWithMainRegisters:
	fence	r, w

	mv	a4, a0
	mv	a5, a1
	mv	a6, a2

.Lcr0:
	mv	a0, a4
	mv	a1, a5
	mv	a2, a6

.Lcr1:
	ld	a7, (a1)
	ld	t0, 8(a1)
	ld	t1, 16(a1)
	ld	t2, 24(a1)
	ld	t3, 32(a1)
	ld	t4, 40(a1)
	ld	t5, 48(a1)
	ld	t6, 56(a1)
	addi	a1, a1, 64
	
	sd	a7, (a0)
	sd	t0, 8(a0)
	sd	t1, 16(a0)
	sd	t2, 24(a0)
	sd	t3, 32(a0)
	sd	t4, 40(a0)
	sd	t5, 48(a0)
	sd	t6, 56(a0)
	addi	a0, a0, 64

	addi	a2, a2, -64
	bnez	a2, .Lcr1

	addi	a3, a3, -1
	bnez	a3, .Lcr0

	# Flush all data going to the destination area
	# that hasn't made it into RAM yet.
	fence	w, w
	ret

# Unused
WriterVector128:
ReaderVector128:
RandomReaderVector128:
RandomWriterVector128:
Register16ToVector128:
Register32ToVector128:
Register64ToVector128:
Register8ToVector128:
RegisterToVectorMove:
VectorToRegisterMove:
Vector128ToRegister16:
Vector128ToRegister32:
Vector128ToRegister64:
Vector128ToRegister8:
VectorToVector128:
VectorToVector256:
VectorToVector512:
	ret

