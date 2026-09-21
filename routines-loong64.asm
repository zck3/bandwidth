#============================================================================
# bandwidth, a benchmark to measure memory bandwidth.
# 64-bit LoongArch64 (loong64) routines.
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
# For testing, lacking physical hardware I used Qemu:
#
# https://cdimage.debian.org/cdimage/ports/snapshots/2025-12-06/
# Upgraded to Debian 14 forky.
# deb http://deb.debian.org/debian forky main 
# deb http://deb.debian.org/debian forky-updates main 
# deb http://security.debian.org/debian-security forky-security main 
#
# sudo apt install qemu-efi-loongarch64
# qemu-system-loongarch64 \
#     -machine virt \
#     -cpu la464 \
#     -m 2G \
#     -bios /usr/share/qemu-efi-loongarch64/QEMU_EFI.fd \
#     -device bochs-display \
#     -device qemu-xhci \
#     -device usb-kbd \
#     -device usb-tablet \
#     -serial stdio \
#     -display gtk \
#     -display gtk,zoom-to-fit=on \
#     -drive file=debian-13.0.0-loong64-NETINST-1.iso,media=cdrom,readonly=on \
#     -drive file=~/loong64v13.img,format=raw,media=disk \
#     -netdev user,id=net0 \
#     -device virtio-net-pci,netdev=net0 \
#     -monitor none
#-----------------------------------------------------------------------------

.macro zero reg
  or \reg, $r0, $r0
.endm
.macro inc reg
  addi.d \reg, \reg, 1
.endm
.macro dec reg
  addi.d \reg, \reg, -1
.endm
.macro subi reg, value
  addi.d \reg, \reg, -\value
.endm

	.globl 	Reader
	.globl 	RandomReader

	.globl 	Writer
	.globl 	RandomWriter

	.globl	CopyWithMainRegisters

	.globl	IncrementRegisters
	.globl	IncrementStack
	.globl 	StackReader
	.globl 	StackWriter
	.globl 	RegisterToRegister

#-----------------------------------------------------------------------------
# Name: 	Writer
# Purpose:	Performs sequential write into memory, as fast as possible.
# Params:
#	$a0 = address, 16-byte aligned
#	$a1 = length, multiple of 256
#	$a2 = count
# 	$a3 = value to write
#-----------------------------------------------------------------------------
.align 16
Writer:
	dbar	0

	srli.d	$a0, $a0, 4
	slli.d	$a0, $a0, 4
	
	move	$t0, $a0
	move	$t1, $a1

	move	$a4, $a3
	move	$a5, $a3
	move 	$a6, $a3
	move 	$a7, $a3

.Lw0:
	move	$a0, $t0
	move	$a1, $t1

.Lw1:
	st.d	$a0, $a0, 0
	st.d	$a1, $a0, 8
	st.d	$a2, $a0, 16
	st.d	$a3, $a0, 24
	st.d	$a4, $a0, 32
	st.d	$a5, $a0, 40
	st.d	$a6, $a0, 48
	st.d	$a7, $a0, 56
	addi.d	$a0, $a0, 64
	
	st.d	$a0, $a0, 0
	st.d	$a1, $a0, 8
	st.d	$a2, $a0, 16
	st.d	$a3, $a0, 24
	st.d	$a4, $a0, 32
	st.d	$a5, $a0, 40
	st.d	$a6, $a0, 48
	st.d	$a7, $a0, 56
	addi.d	$a0, $a0, 64
	
	st.d	$a0, $a0, 0
	st.d	$a1, $a0, 8
	st.d	$a2, $a0, 16
	st.d	$a3, $a0, 24
	st.d	$a4, $a0, 32
	st.d	$a5, $a0, 40
	st.d	$a6, $a0, 48
	st.d	$a7, $a0, 56
	addi.d	$a0, $a0, 64
	
	st.d	$a0, $a0, 0
	st.d	$a1, $a0, 8
	st.d	$a2, $a0, 16
	st.d	$a3, $a0, 24
	st.d	$a4, $a0, 32
	st.d	$a5, $a0, 40
	st.d	$a6, $a0, 48
	st.d	$a7, $a0, 56
	addi.d	$a0, $a0, 64

	subi	$a1, 256
	bnez	$a1, .Lw1

	dec	$a2
	bnez	$a2, .Lw0

	ret

#-----------------------------------------------------------------------------
# Name: 	RandomWriter
# Purpose:	Performs random write into memory, $as fast as possible.
# Params:
# 	$a0 = pointer to array of chunk pointers
# 	$a1 = # of 256-byte chunks
# 	$a2 = # loops to do
# 	$a3 = value to write
#-----------------------------------------------------------------------------
.align 4
RandomWriter:
	dbar	0

	subi	$sp, 32
	st.d	$a2, $sp, 0
	st.d	$a3, $sp, 8
	st.d	$a4, $sp, 16
	st.d	$a5, $sp, 24

.Lrw0:
	li.d	$a5, 0

.Lrw1:
	## Get pointer to chunk in memory.
	slli.d	$t0, $a5, 3
	add.d	$t0, $t0, $a0
	ld.d	$a4, $t0, 0

	## 32 transfers, 8 bytes each = 256 bytes total.
	ld.d	$a6, $a4, 160
	st.d	$a3, $a4, 160
	st.d	$a3, $a4, 224
	st.d	$a3, $a4, 232
	st.d	$a3, $a4, 96
	st.d	$a3, $a4, 248
	st.d	$a3, $a4, 104
	st.d	$a3, $a4, 136
	st.d	$a3, $a4, 112
	st.d	$a3, $a4, 200
	st.d	$a3, $a4, 128
	st.d	$a3, $a4, 216
	st.d	$a3, $a4, 0
	st.d	$a3, $a4, 184
	st.d	$a3, $a4, 48
	st.d	$a3, $a4, 64
	st.d	$a6, $a4, 240
	st.d	$a3, $a4, 24
	st.d	$a3, $a4, 72
	st.d	$a3, $a4, 32
	st.d	$a3, $a4, 80
	st.d	$a3, $a4, 56
	st.d	$a3, $a4, 8
	st.d	$a3, $a4, 208
	st.d	$a3, $a4, 40
	st.d	$a3, $a4, 120
	st.d	$a3, $a4, 176
	st.d	$a3, $a4, 16
	st.d	$a3, $a4, 168
	st.d	$a3, $a4, 88
	st.d	$a3, $a4, 152
	st.d	$a3, $a4, 192
	st.d	$a3, $a4, 144

	# Proceed to next 256-byte chunk
	inc	$a5
	bltu	$a5, $a1, .Lrw1

	# Next loop
	dec	$a2
	bnez	$a2, .Lrw0

	ld.d	$a2, $sp, 0
	ld.d	$a3, $sp, 8
	ld.d	$a4, $sp, 16
	ld.d	$a5, $sp, 24
	addi.d	$sp, $sp, 32

	ret

#-----------------------------------------------------------------------------
# Name: 	Reader
# Purpose:	Performs sequential reads from memory, $as fast as possible.
# Params:
#	$a0 = address
#	$a1 = length, multiple of 256
#	$a2 = count
#-----------------------------------------------------------------------------
.align 4
Reader:
	dbar	0

	srli.d	$a0, $a0, 4
	slli.d	$a0, $a0, 4

	# Save inner loop data
	move	$t0, $a0
	move	$t1, $a1

.Lr0:
	# Restore inner loop data
	move	$a0, $t0
	move	$a1, $t1

.Lr1:
	# Total of 32 transfers x 8 bytes = 256 bytes.
	# 8 transfers, 8 bytes each
	ld.d	$a3, $a0, 0
	ld.d	$a4, $a0, 8
	ld.d	$a5, $a0, 16
	ld.d	$a6, $a0, 24
	ld.d	$a7, $a0, 32
	ld.d	$t2, $a0, 40
	ld.d	$t3, $a0, 48
	ld.d	$t4, $a0, 56
	addi.d	$a0, $a0, 64
	# 8 transfers, 8 bytes each
	ld.d	$a3, $a0, 0
	ld.d	$a4, $a0, 8
	ld.d	$a5, $a0, 16
	ld.d	$a6, $a0, 24
	ld.d	$a7, $a0, 32
	ld.d	$t2, $a0, 40
	ld.d	$t3, $a0, 48
	ld.d	$t4, $a0, 56
	addi.d	$a0, $a0, 64
	# 8 transfers, 8 bytes each
	ld.d	$a3, $a0, 0
	ld.d	$a4, $a0, 8
	ld.d	$a5, $a0, 16
	ld.d	$a6, $a0, 24
	ld.d	$a7, $a0, 32
	ld.d	$t2, $a0, 40
	ld.d	$t3, $a0, 48
	ld.d	$t4, $a0, 56
	addi.d	$a0, $a0, 64
	# 8 transfers, 8 bytes each
	ld.d	$a3, $a0, 0
	ld.d	$a4, $a0, 8
	ld.d	$a5, $a0, 16
	ld.d	$a6, $a0, 24
	ld.d	$a7, $a0, 32
	ld.d	$t2, $a0, 40
	ld.d	$t3, $a0, 48
	ld.d	$t4, $a0, 56
	addi.d	$a0, $a0, 64

	subi	$a1, 256
	bnez	$a1, .Lr1
	
	dec	$a2
	bnez	$a2, .Lr0

	ret

#-----------------------------------------------------------------------------
# Name: 	RandomReader
# Purpose:	Performs random reads from memory, $as fast as possible.
# Params:
# 	$a0 = pointer to array of chunk pointers
# 	$a1 = # of 256-byte chunks
# 	$a2 = # loops to do
#-----------------------------------------------------------------------------
.align 4
RandomReader:
	dbar	0

.Lrr0:
	zero	$a5

.Lrr1:
	# Get pointer to chunk in memory.
	slli.d	$t0, $a5, 3
	add.d	$t0, $t0, $a0
	ld.d	$a4, $t0, 0

	## Does 32 transfers, 8 bytes each = 256 bytes total.
	ld.d	$a3, $a4, 160
	ld.d	$a3, $a4, 224
	ld.d	$a3, $a4, 232
	ld.d	$a3, $a4, 96
	ld.d	$a3, $a4, 248
	ld.d	$a3, $a4, 104
	ld.d	$a3, $a4, 136
	ld.d	$a3, $a4, 112
	ld.d	$a3, $a4, 200
	ld.d	$a3, $a4, 128
	ld.d	$a3, $a4, 216
	ld.d	$a3, $a4, 0
	ld.d	$a3, $a4, 184
	ld.d	$a3, $a4, 48
	ld.d	$a3, $a4, 64
	ld.d	$a3, $a4, 240
	ld.d	$a3, $a4, 24
	ld.d	$a3, $a4, 72
	ld.d	$a3, $a4, 32
	ld.d	$a3, $a4, 80
	ld.d	$a3, $a4, 56
	ld.d	$a3, $a4, 8
	ld.d	$a3, $a4, 208
	ld.d	$a3, $a4, 40
	ld.d	$a3, $a4, 120
	ld.d	$a3, $a4, 176
	ld.d	$a3, $a4, 16
	ld.d	$a3, $a4, 168
	ld.d	$a3, $a4, 88
	ld.d	$a3, $a4, 152
	ld.d	$a3, $a4, 192
	ld.d	$a3, $a4, 144

	inc	$a5
	bne	$a5, $a1, .Lrr1

	dec	$a2
	bnez	$a2, .Lrr0

	ret

#-----------------------------------------------------------------------------
# Name: 	RegisterToRegister
# Purpose:	Performs register-to-register transfers.
# Params:
#	$a0 = count
#-----------------------------------------------------------------------------
.align 4
RegisterToRegister:

.Lr2r:
	# Do 64 transfers 
	move	$a1, $a2
	move	$t1, $a3
	move	$t2, $a4
	move	$a1, $a5
	move	$a1, $a6
	move	$a1, $a7
	move	$a1, $t0
	move	$a1, $t1
	move	$t2, $a1
	move	$a2, $a3
	move	$t4, $a4
	move	$a7, $a5
	move	$a2, $a6
	move	$a2, $a7
	move	$a2, $t0
	move	$a2, $t1
	move	$a1, $a2
	move	$a6, $a3
	move	$t1, $a4
	move	$a1, $a5
	move	$a2, $a6
	move	$a1, $a7
	move	$a1, $t0
	move	$a1, $t1
	move	$a1, $a2
	move	$a7, $a3
	move	$a1, $a4
	move	$a3, $a5
	move	$t0, $a6
	move	$a1, $a7
	move	$a6, $t0
	move	$a1, $t1

	move	$a1, $a2
	move	$t1, $a3
	move	$t2, $a4
	move	$a1, $a5
	move	$a1, $a6
	move	$a1, $a7
	move	$a1, $t0
	move	$a1, $t1
	move	$t2, $a1
	move	$a2, $a3
	move	$t4, $a4
	move	$a7, $a5
	move	$a2, $a6
	move	$a2, $a7
	move	$a2, $t0
	move	$a2, $t1
	move	$a1, $a2
	move	$a6, $a3
	move	$t1, $a4
	move	$a1, $a5
	move	$a2, $a6
	move	$a1, $a7
	move	$a1, $t0
	move	$a1, $t1
	move	$a1, $a2
	move	$a7, $a3
	move	$a1, $a4
	move	$a3, $a5
	move	$t0, $a6
	move	$a1, $a7
	move	$a6, $t0
	move	$a1, $t1

	dec	$a0 
	bnez	$a0, .Lr2r

	ret

#------------------------------------------------------------------------------
# Name:		IncrementRegisters
# Purpose:	Increments/decrements 64-bit values in registers.
# Params:	$a0 = count
#------------------------------------------------------------------------------
.align 4
IncrementRegisters:
	
.Li1:
	# 32 operations
	inc	$a1
	inc	$a2
	inc	$a3
	inc	$a4
	inc	$a5
	inc	$a6
	inc	$a7
	inc	$t0

	dec	$a1
	dec	$a2
	dec	$a3
	dec	$a4
	dec	$a5
	dec	$a6
	dec	$a7
	dec	$t0

	inc	$a1
	inc	$a2
	inc	$a3
	inc	$a4
	inc	$a5
	inc	$a6
	inc	$a7
	inc	$t0

	dec	$a1
	dec	$a2
	dec	$a3
	dec	$a4
	dec	$a5
	dec	$a6
	dec	$a7
	dec	$t0

	dec	$a0
	bnez	$a0, .Li1

	ret

#------------------------------------------------------------------------------
# Name:		IncrementStack
# Purpose:	Increments 64-bit values on stack.
# Params:	$a0 = count
#------------------------------------------------------------------------------
.align 4
IncrementStack:
	dbar	0
	subi	$sp, 64

.Lis1:
	# Perform 32 increments
	ld.d	$a1, $sp, 0
	ld.d	$a2, $sp, 8
	ld.d	$a3, $sp, 16
	ld.d	$a4, $sp, 24
	ld.d	$a5, $sp, 32
	ld.d	$a6, $sp, 40
	ld.d	$a7, $sp, 48
	ld.d	$t0, $sp, 56
	addi.d	$a1, $a1, 1
	addi.d	$a2, $a2, 1
	addi.d	$a3, $a3, 1
	addi.d	$a4, $a4, 1
	addi.d	$a5, $a5, 1
	addi.d	$a6, $a6, 1
	addi.d	$a7, $a7, 1
	addi.d	$t0, $t0, 1
	st.d	$a1, $sp, 0
	st.d	$a2, $sp, 8
	st.d	$a3, $sp, 16
	st.d	$a4, $sp, 24
	st.d	$a5, $sp, 32
	st.d	$a6, $sp, 40
	st.d	$a7, $sp, 48
	st.d	$t0, $sp, 56

	ld.d	$a1, $sp, 0
	ld.d	$a2, $sp, 8
	ld.d	$a3, $sp, 16
	ld.d	$a4, $sp, 24
	ld.d	$a5, $sp, 32
	ld.d	$a6, $sp, 40
	ld.d	$a7, $sp, 48
	ld.d	$t0, $sp, 56
	addi.d	$a1, $a1, 1
	addi.d	$a2, $a2, 1
	addi.d	$a3, $a3, 1
	addi.d	$a4, $a4, 1
	addi.d	$a5, $a5, 1
	addi.d	$a6, $a6, 1
	addi.d	$a7, $a7, 1
	addi.d	$t0, $t0, 1
	st.d	$a1, $sp, 0
	st.d	$a2, $sp, 8
	st.d	$a3, $sp, 16
	st.d	$a4, $sp, 24
	st.d	$a5, $sp, 32
	st.d	$a6, $sp, 40
	st.d	$a7, $sp, 48
	st.d	$t0, $sp, 56

	ld.d	$a1, $sp, 0
	ld.d	$a2, $sp, 8
	ld.d	$a3, $sp, 16
	ld.d	$a4, $sp, 24
	ld.d	$a5, $sp, 32
	ld.d	$a6, $sp, 40
	ld.d	$a7, $sp, 48
	ld.d	$t0, $sp, 56
	inc	$a1
	inc	$a2
	inc	$a3
	inc	$a4
	inc	$a5
	inc	$a6
	inc	$a7
	inc	$t0
	st.d	$a1, $sp, 0
	st.d	$a2, $sp, 8
	st.d	$a3, $sp, 16
	st.d	$a4, $sp, 24
	st.d	$a5, $sp, 32
	st.d	$a6, $sp, 40
	st.d	$a7, $sp, 48
	st.d	$t0, $sp, 56

	ld.d	$a1, $sp, 0
	ld.d	$a2, $sp, 8
	ld.d	$a3, $sp, 16
	ld.d	$a4, $sp, 24
	ld.d	$a5, $sp, 32
	ld.d	$a6, $sp, 40
	ld.d	$a7, $sp, 48
	ld.d	$t0, $sp, 56
	inc	$a1
	inc	$a2
	inc	$a3
	inc	$a4
	inc	$a5
	inc	$a6
	inc	$a7
	inc	$t0
	st.d	$a1, $sp, 0
	st.d	$a2, $sp, 8
	st.d	$a3, $sp, 16
	st.d	$a4, $sp, 24
	st.d	$a5, $sp, 32
	st.d	$a6, $sp, 40
	st.d	$a7, $sp, 48
	st.d	$t0, $sp, 56

	dec	$a0
	bnez	$a0, .Lis1

	addi.d	$sp, $sp, 64
	ret

#------------------------------------------------------------------------------
# Name:		StackReader
# Purpose:	Reads 64-bit values off the stack into registers of
#		$the main register set, effectively testing L1 cache access
#		$and effective-address calculation speed.
# Params:	$a0 = loops
#------------------------------------------------------------------------------
.align 16
StackReader:
	dbar	0
	subi 	$sp, 64

.Lsr1:
	# 64 transfers
	ld.d	$a1, $sp, 56
	ld.d	$a2, $sp, 16
	ld.d	$a3, $sp, 24
	ld.d	$a4, $sp, 32
	ld.d	$a5, $sp, 40
	ld.d	$a6, $sp, 8
	ld.d	$a7, $sp, 48
	ld.d	$a1, $sp, 0
	ld.d	$a2, $sp, 16
	ld.d	$a3, $sp, 56
	ld.d	$a4, $sp, 24
	ld.d	$a5, $sp, 32
	ld.d	$a6, $sp, 40
	ld.d	$a7, $sp, 8
	ld.d	$a1, $sp, 48
	ld.d	$a2, $sp, 0
	ld.d	$a3, $sp, 16
	ld.d	$a4, $sp, 24
	ld.d	$a5, $sp, 32
	ld.d	$a6, $sp, 0
	ld.d	$a7, $sp, 40
	ld.d	$a1, $sp, 8
	ld.d	$a2, $sp, 48
	ld.d	$a3, $sp, 8
	ld.d	$a4, $sp, 8
	ld.d	$a5, $sp, 16
	ld.d	$a6, $sp, 24
	ld.d	$a7, $sp, 32
	ld.d	$a1, $sp, 40
	ld.d	$a2, $sp, 8
	ld.d	$a3, $sp, 48
	ld.d	$a4, $sp, 8

	ld.d	$a1, $sp, 56
	ld.d	$a2, $sp, 16
	ld.d	$a3, $sp, 24
	ld.d	$a4, $sp, 32
	ld.d	$a5, $sp, 40
	ld.d	$a6, $sp, 8
	ld.d	$a7, $sp, 48
	ld.d	$a1, $sp, 0
	ld.d	$a2, $sp, 16
	ld.d	$a3, $sp, 56
	ld.d	$a4, $sp, 24
	ld.d	$a5, $sp, 32
	ld.d	$a6, $sp, 40
	ld.d	$a7, $sp, 8
	ld.d	$a1, $sp, 48
	ld.d	$a2, $sp, 0
	ld.d	$a3, $sp, 16
	ld.d	$a4, $sp, 24
	ld.d	$a5, $sp, 32
	ld.d	$a6, $sp, 0
	ld.d	$a7, $sp, 40
	ld.d	$a1, $sp, 8
	ld.d	$a2, $sp, 48
	ld.d	$a3, $sp, 8
	ld.d	$a4, $sp, 8
	ld.d	$a5, $sp, 16
	ld.d	$a6, $sp, 24
	ld.d	$a7, $sp, 32
	ld.d	$a1, $sp, 40
	ld.d	$a2, $sp, 8
	ld.d	$a3, $sp, 48
	ld.d	$a4, $sp, 8

	dec	$a0
	bnez	$a0, .Lsr1

	addi.d	$sp, $sp, 64
	ret

#------------------------------------------------------------------------------
# Name:		StackWriter
# Purpose:	Writes 64-bit values into the stack from registers of
#		$the main register set, effectively testing L1 cache access
#		$and effective-address calculation speed.
# Params:	$a0 = loops
#------------------------------------------------------------------------------
.align 16
StackWriter:
	dbar	0

	subi	$sp, 64

	li.d	$a1, 0x123

.Lsw1:
	# 64 transfers
	st.d	$a1, $sp, 56
	st.d	$a1, $sp, 16
	st.d	$a1, $sp, 24
	st.d	$a1, $sp, 32
	st.d	$a1, $sp, 40
	st.d	$a1, $sp, 8
	st.d	$a1, $sp, 48
	st.d	$a1, $sp, 0
	st.d	$a1, $sp, 16
	st.d	$a1, $sp, 24
	st.d	$a1, $sp, 56
	st.d	$a1, $sp, 32
	st.d	$a1, $sp, 40
	st.d	$a1, $sp, 8
	st.d	$a1, $sp, 48
	st.d	$a1, $sp, 0
	st.d	$a1, $sp, 16
	st.d	$a1, $sp, 24
	st.d	$a1, $sp, 32
	st.d	$a1, $sp, 40
	st.d	$a1, $sp, 8
	st.d	$a1, $sp, 48
	st.d	$a1, $sp, 8
	st.d	$a1, $sp, 8
	st.d	$a1, $sp, 16
	st.d	$a1, $sp, 56
	st.d	$a1, $sp, 24
	st.d	$a1, $sp, 32
	st.d	$a1, $sp, 40
	st.d	$a1, $sp, 8
	st.d	$a1, $sp, 48
	st.d	$a1, $sp, 8

	st.d	$a1, $sp, 56
	st.d	$a1, $sp, 16
	st.d	$a1, $sp, 24
	st.d	$a1, $sp, 32
	st.d	$a1, $sp, 40
	st.d	$a1, $sp, 8
	st.d	$a1, $sp, 48
	st.d	$a1, $sp, 0
	st.d	$a1, $sp, 16
	st.d	$a1, $sp, 24
	st.d	$a1, $sp, 56
	st.d	$a1, $sp, 32
	st.d	$a1, $sp, 40
	st.d	$a1, $sp, 8
	st.d	$a1, $sp, 48
	st.d	$a1, $sp, 0
	st.d	$a1, $sp, 16
	st.d	$a1, $sp, 24
	st.d	$a1, $sp, 32
	st.d	$a1, $sp, 40
	st.d	$a1, $sp, 8
	st.d	$a1, $sp, 48
	st.d	$a1, $sp, 8
	st.d	$a1, $sp, 8
	st.d	$a1, $sp, 16
	st.d	$a1, $sp, 56
	st.d	$a1, $sp, 24
	st.d	$a1, $sp, 32
	st.d	$a1, $sp, 40
	st.d	$a1, $sp, 8
	st.d	$a1, $sp, 48
	st.d	$a1, $sp, 8

	dec	$a0
	bnez	$a0, .Lsw1

	addi.d	$sp, $sp, 64
	ret

#-----------------------------------------------------------------------------
# Name: 	CopyWithMainRegisters
# Purpose:	Performs memory copy, $as fast as possible.
# Params:
# 	$a0 = pointer to destination array 
# 	$a1 = pointer to source array
# 	$a2 = # of bytes (multiple of 256)
# 	$a3 = # loops to do
#-----------------------------------------------------------------------------
.align 4
CopyWithMainRegisters:
_CopyWithMainRegisters:
	dbar	0

	move	$a4, $a0
	move	$a5, $a1
	move	$a6, $a2

.Lcr0:
	move	$a0, $a4
	move	$a1, $a5
	move	$a2, $a6

.Lcr1:
	ld.d	$a7, $a1, 0
	ld.d	$t0, $a1, 8
	ld.d	$t1, $a1, 16
	ld.d	$t2, $a1, 24
	ld.d	$t3, $a1, 32
	ld.d	$t4, $a1, 40
	ld.d	$t5, $a1, 48
	ld.d	$t6, $a1, 56
	addi.d	$a1, $a1, 64
	
	st.d	$a7, $a0, 0
	st.d	$t0, $a0, 8
	st.d	$t1, $a0, 16
	st.d	$t2, $a0, 24
	st.d	$t3, $a0, 32
	st.d	$t4, $a0, 40
	st.d	$t5, $a0, 48
	st.d	$t6, $a0, 56
	addi.d	$a0, $a0, 64

	subi	$a2, 64
	bnez	$a2, .Lcr1

	dec	$a3
	bnez	$a3, .Lcr0
	ret

