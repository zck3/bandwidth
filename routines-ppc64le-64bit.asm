#============================================================================
# bandwidth, a benchmark to measure memory bandwidth.
# 64-bit PowerPC (ppc64le) routines.
# Achtung! This is little-endian PowerPC code.
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
#	a0 = address, 16-byte aligned
#	a1 = length, multiple of 256
#	a2 = count
# 	a3 = value to write
#-----------------------------------------------------------------------------
.align 4
Writer:
	blr

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
	blr

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
	blr

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
	blr

#-----------------------------------------------------------------------------
# Name: 	RegisterToRegister
# Purpose:	Performs register-to-register transfers.
# Params:
#	a0 = count
#-----------------------------------------------------------------------------
.align 4
RegisterToRegister:
	blr

#------------------------------------------------------------------------------
# Name:		IncrementRegisters
# Purpose:	Increments/decrements 64-bit values in registers.
# Params:	a0 = count
#------------------------------------------------------------------------------
.align 4
IncrementRegisters:
	blr

#------------------------------------------------------------------------------
# Name:		IncrementStack
# Purpose:	Increments 64-bit values on stack.
# Params:	a0 = count
#------------------------------------------------------------------------------
.align 4
IncrementStack:
	blr

#------------------------------------------------------------------------------
# Name:		StackReader
# Purpose:	Reads 64-bit values off the stack into registers of
#		the main register set, effectively testing L1 cache access
#		and effective-address calculation speed.
# Params:	a0 = loops
#------------------------------------------------------------------------------
.align 16
StackReader:
	blr

#------------------------------------------------------------------------------
# Name:		StackWriter
# Purpose:	Writes 64-bit values into the stack from registers of
#		the main register set, effectively testing L1 cache access
#		and effective-address calculation speed.
# Params:	a0 = loops
#------------------------------------------------------------------------------
.align 16
StackWriter:
	blr

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

