#============================================================================
# bandwidth, a benchmark to estimate memory bandwidth.
#
# 64-bit LOONGARCH (loongarch64) vector routines.
#
# This file is Copyright (C) 2026 by KatyushaScarlet.
# Various edits Copyright (C) 2026 by Zack T Smith.
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
# The author may be reached at KatyushaScarlet at Outlook dot com.
#=============================================================================

.macro zero reg
  or \reg, $r0, $r0
.endm
.macro inc reg
  addi.d \reg, \reg, 1
.endm
.macro dec reg
  addi.d \reg, \reg, -1
.endm

# LSX/128-bit vector routines
	.globl 	WriterVector128
	.globl 	ReaderVector128
	.globl 	RandomReaderVector128
	.globl 	RandomWriterVector128
	.globl 	CopyVector128
	.globl 	_CopyVector128
# LASX/256-bit vector routines (reuse x86 API names)
	.globl 	WriterVector256
	.globl 	ReaderVector256
	.globl 	RandomReaderVector256
	.globl 	RandomWriterVector256
	.globl 	CopyVector256
	.globl 	_CopyVector256
# Register <-> vector transfers
	.globl 	Register16ToVector128
	.globl 	Register32ToVector128
	.globl 	Register64ToVector128
	.globl 	Register8ToVector128
	.globl 	RegisterToVector128
	.globl 	Vector128ToRegister16
	.globl 	Vector128ToRegister32
	.globl 	Vector128ToRegister64
	.globl 	Vector128ToRegister8
	.globl 	VectorToVector128
	.globl 	VectorToVector256
# Unused:
	.globl 	VectorToVector512

#-----------------------------------------------------------------------------
# Name: 	WriterVector128 (LSX 128-bit sequential write)
# Params:	$a0 = ptr, 16-byte aligned
#		$a1 = size, multiple of 256
#		$a2 = loops
# 		$a3 = value to write
#-----------------------------------------------------------------------------
.align 4
WriterVector128:
	srli.d 	$a0, $a0, 4
	slli.d 	$a0, $a0, 4
	addi.d	$t0, $a0, 0
	addi.d	$t1, $a1, 0
	# Duplicate 64-bit value into both lanes of 128-bit VR
	vreplgr2vr.d $vr0, $a3

.Lwv0:
	addi.d	$a0, $t0, 0
	addi.d	$a1, $t1, 0

.Lwv1:
	# 16 x 128-bit stores = 256 bytes
	vst	$vr0, $a0, 0
	vst	$vr0, $a0, 16
	vst	$vr0, $a0, 32
	vst	$vr0, $a0, 48
	vst	$vr0, $a0, 64
	vst	$vr0, $a0, 80
	vst	$vr0, $a0, 96
	vst	$vr0, $a0, 112
	vst	$vr0, $a0, 128
	vst	$vr0, $a0, 144
	vst	$vr0, $a0, 160
	vst	$vr0, $a0, 176
	vst	$vr0, $a0, 192
	vst	$vr0, $a0, 208
	vst	$vr0, $a0, 224
	vst	$vr0, $a0, 240
	addi.d	$a0, $a0, 256

	addi.d	$a1, $a1, -256
	bnez	$a1, .Lwv1

	dec	$a2
	bnez	$a2, .Lwv0

	dbar 0
	ret

#-----------------------------------------------------------------------------
# Name: 	ReaderVector128 (LSX 128-bit sequential read)
# Params:	$a0 = ptr
#		$a1 = size, multiple of 256
#		$a2 = loops
#-----------------------------------------------------------------------------
.align 4
ReaderVector128:
	addi.d	$t0, $a0, 0
	addi.d	$t1, $a1, 0

.Lrv0:
	addi.d	$a0, $t0, 0
	addi.d	$a1, $t1, 0

.Lrv1:
	# 16 x 128-bit loads = 256 bytes
	vld	$vr0, $a0, 0
	vld	$vr1, $a0, 16
	vld	$vr2, $a0, 32
	vld	$vr3, $a0, 48
	vld	$vr0, $a0, 64
	vld	$vr1, $a0, 80
	vld	$vr2, $a0, 96
	vld	$vr3, $a0, 112
	vld	$vr0, $a0, 128
	vld	$vr1, $a0, 144
	vld	$vr2, $a0, 160
	vld	$vr3, $a0, 176
	vld	$vr0, $a0, 192
	vld	$vr1, $a0, 208
	vld	$vr2, $a0, 224
	vld	$vr3, $a0, 240
	addi.d	$a0, $a0, 256

	addi.d	$a1, $a1, -256
	bnez	$a1, .Lrv1

	dec	$a2
	bnez	$a2, .Lrv0

	dbar 0
	ret

#-----------------------------------------------------------------------------
# Name: 	RandomWriterVector128 (LSX 128-bit random write)
# Params: 	$a0 = ptr to chunk pointers array
# 		$a1 = # of 256-byte chunks
# 		$a2 = # loops
# 		$a3 = value to write
#-----------------------------------------------------------------------------
.align 4
RandomWriterVector128:
	addi.d	$sp, $sp, -32
	st.d	$a2, $sp, 0
	st.d	$a3, $sp, 8
	st.d	$a4, $sp, 16
	st.d	$a5, $sp, 24
	vreplgr2vr.d $vr0, $a3

.Lrwv0:
	li.d	$a5, 0

.Lrwv1:
	slli.d	$t0, $a5, 3
	add.d	$t0, $t0, $a0
	ld.d	$a4, $t0, 0

	# 16 x 128-bit stores = 256 bytes
	vst	$vr0, $a4, 0
	vst	$vr0, $a4, 128
	vst	$vr0, $a4, 208
	vst	$vr0, $a4, 112
	vst	$vr0, $a4, 64
	vst	$vr0, $a4, 176
	vst	$vr0, $a4, 144
	vst	$vr0, $a4, 240
	vst	$vr0, $a4, 96
	vst	$vr0, $a4, 16
	vst	$vr0, $a4, 192
	vst	$vr0, $a4, 160
	vst	$vr0, $a4, 32
	vst	$vr0, $a4, 48
	vst	$vr0, $a4, 224
	vst	$vr0, $a4, 80

	inc	$a5
	bltu	$a5, $a1, .Lrwv1

	dec	$a2
	bnez	$a2, .Lrwv0

	ld.d	$a2, $sp, 0
	ld.d	$a3, $sp, 8
	ld.d	$a4, $sp, 16
	ld.d	$a5, $sp, 24
	addi.d	$sp, $sp, 32

	dbar 0
	ret

#-----------------------------------------------------------------------------
# Name: 	RandomReaderVector128 (LSX 128-bit random read)
# Params: 	$a0 = ptr to chunk pointers array
# 		$a1 = # of 256-byte chunks
# 		$a2 = # loops
#-----------------------------------------------------------------------------
.align 4
RandomReaderVector128:

.Lrrv0:
	addi.d	$a5, $zero, 0

.Lrrv1:
	slli.d	$t0, $a5, 3
	add.d	$t0, $t0, $a0
	ld.d	$a4, $t0, 0

	# 16 x 128-bit loads = 256 bytes
	vld	$vr0, $a4, 0
	vld	$vr1, $a4, 128
	vld	$vr2, $a4, 208
	vld	$vr3, $a4, 112
	vld	$vr0, $a4, 64
	vld	$vr1, $a4, 176
	vld	$vr2, $a4, 144
	vld	$vr3, $a4, 240
	vld	$vr0, $a4, 96
	vld	$vr1, $a4, 16
	vld	$vr2, $a4, 192
	vld	$vr3, $a4, 160
	vld	$vr0, $a4, 32
	vld	$vr1, $a4, 48
	vld	$vr2, $a4, 224
	vld	$vr3, $a4, 80

	inc	$a5
	bne	$a5, $a1, .Lrrv1

	dec	$a2
	bnez	$a2, .Lrrv0

	dbar 0
	ret

#-----------------------------------------------------------------------------
# Name: 	CopyVector128 (LSX 128-bit sequential copy)
# Params: 	$a0 = dst
# 		$a1 = src
# 		$a2 = size, multiple of 256
# 		$a3 = loops
#-----------------------------------------------------------------------------
.align 4
CopyVector128:
_CopyVector128:
	addi.d	$a4, $a0, 0
	addi.d	$a5, $a1, 0
	addi.d	$a6, $a2, 0

.Lcs0:
	addi.d	$a0, $a4, 0
	addi.d	$a1, $a5, 0
	addi.d	$a2, $a6, 0

.Lcs1:
	# 8 x 128-bit loads + 8 x 128-bit stores = 128 bytes
	vld	$vr0, $a1, 0
	vld	$vr1, $a1, 16
	vld	$vr2, $a1, 32
	vld	$vr3, $a1, 48
	vld	$vr4, $a1, 64
	vld	$vr5, $a1, 80
	vld	$vr6, $a1, 96
	vld	$vr7, $a1, 112
	addi.d	$a1, $a1, 128

	vst	$vr0, $a0, 0
	vst	$vr1, $a0, 16
	vst	$vr2, $a0, 32
	vst	$vr3, $a0, 48
	vst	$vr4, $a0, 64
	vst	$vr5, $a0, 80
	vst	$vr6, $a0, 96
	vst	$vr7, $a0, 112
	addi.d	$a0, $a0, 128

	addi.d	$a2, $a2, -128
	bnez	$a2, .Lcs1

	dec	$a3
	bnez	$a3, .Lcs0

	dbar 0
	ret

#-----------------------------------------------------------------------------
# Name: 	WriterVector256 (LASX 256-bit sequential write)
# Params:	$a0 = ptr, 32-byte aligned
#		$a1 = size, multiple of 256
#		$a2 = loops
# 		$a3 = value to write
#-----------------------------------------------------------------------------
.align 4
WriterVector256:
	srli.d 	$a0, $a0, 5
	slli.d 	$a0, $a0, 5

	move	$t0, $a0
	move	$t1, $a1
	
	xvreplgr2vr.d $xr0, $a3

.Lwavx0:
	move	$a0, $t0
	move	$a1, $t1

.Lwavx1:
	# 8 x 256-bit stores = 256 bytes
	xvst	$xr0, $a0, 0
	xvst	$xr0, $a0, 32
	xvst	$xr0, $a0, 64
	xvst	$xr0, $a0, 96
	xvst	$xr0, $a0, 128
	xvst	$xr0, $a0, 160
	xvst	$xr0, $a0, 192
	xvst	$xr0, $a0, 224
	addi.d	$a0, $a0, 256

	addi.d	$a1, $a1, -256
	bnez	$a1, .Lwavx1

	dec	$a2
	bnez	$a2, .Lwavx0

	dbar 0
	ret

#-----------------------------------------------------------------------------
# Name: 	ReaderVector256 (LASX 256-bit sequential read)
# Params:	$a0 = ptr
#		$a1 = size, multiple of 256
#		$a2 = loops
#-----------------------------------------------------------------------------
.align 4
ReaderVector256:
	addi.d	$sp, $sp, -128
	xvst	$xr0, $sp, 0
	xvst	$xr1, $sp, 32
	xvst	$xr2, $sp, 64
	xvst	$xr3, $sp, 96

	move	$t0, $a0
	move	$t1, $a1

.Lravx0:
	move	$a0, $t0
	move	$a1, $t1

.Lravx1:
	# 8 x 256-bit loads = 256 bytes
	xvld	$xr0, $a0, 0
	xvld	$xr1, $a0, 32
	xvld	$xr2, $a0, 64
	xvld	$xr3, $a0, 96
	xvld	$xr0, $a0, 128
	xvld	$xr1, $a0, 160
	xvld	$xr2, $a0, 192
	xvld	$xr3, $a0, 224
	addi.d	$a0, $a0, 256

	addi.d	$a1, $a1, -256
	bnez	$a1, .Lravx1

	dec	$a2
	bnez	$a2, .Lravx0

	xvld	$xr3, $sp, 96
	xvld	$xr2, $sp, 64
	xvld	$xr1, $sp, 32
	xvld	$xr0, $sp, 0
	addi.d	$sp, $sp, 128

	dbar 0
	ret

#-----------------------------------------------------------------------------
# Name: 	RandomWriterVector256 (LASX 256-bit random write)
# Params: 	$a0 = ptr to chunk pointers array
# 		$a1 = # of 256-byte chunks
# 		$a2 = # loops
# 		$a3 = value to write
#-----------------------------------------------------------------------------
.align 4
RandomWriterVector256:
	addi.d	$sp, $sp, -32
	st.d	$a2, $sp, 0
	st.d	$a3, $sp, 8
	st.d	$a4, $sp, 16
	st.d	$a5, $sp, 24
	xvreplgr2vr.d $xr0, $a3

.Lrwavx0:
	li.d	$a5, 0

.Lrwavx1:
	slli.d	$t0, $a5, 3
	add.d	$t0, $t0, $a0
	ld.d	$a4, $t0, 0

	# 8 x 256-bit stores = 256 bytes
	xvst	$xr0, $a4, 0
	xvst	$xr0, $a4, 192
	xvst	$xr0, $a4, 64
	xvst	$xr0, $a4, 128
	xvst	$xr0, $a4, 224
	xvst	$xr0, $a4, 96
	xvst	$xr0, $a4, 160
	xvst	$xr0, $a4, 32

	inc	$a5
	bltu	$a5, $a1, .Lrwavx1

	dec	$a2
	bnez	$a2, .Lrwavx0

	ld.d	$a2, $sp, 0
	ld.d	$a3, $sp, 8
	ld.d	$a4, $sp, 16
	ld.d	$a5, $sp, 24
	addi.d	$sp, $sp, 32

	dbar 0
	ret

#-----------------------------------------------------------------------------
# Name: 	RandomReaderVector256 (LASX 256-bit random read)
# Params: 	$a0 = ptr to chunk pointers array
# 		$a1 = # of 256-byte chunks
# 		$a2 = # loops
#-----------------------------------------------------------------------------
.align 4
RandomReaderVector256:
	addi.d	$sp, $sp, -128
	xvst	$xr0, $sp, 0
	xvst	$xr1, $sp, 32
	xvst	$xr2, $sp, 64
	xvst	$xr3, $sp, 96

.Lrravx0:
	zero	$a5

.Lrravx1:
	slli.d	$t0, $a5, 3
	add.d	$t0, $t0, $a0
	ld.d	$a4, $t0, 0

	# 8 x 256-bit loads = 256 bytes
	xvld	$xr0, $a4, 0
	xvld	$xr1, $a4, 192
	xvld	$xr2, $a4, 64
	xvld	$xr3, $a4, 128
	xvld	$xr0, $a4, 224
	xvld	$xr1, $a4, 96
	xvld	$xr2, $a4, 160
	xvld	$xr3, $a4, 32

	inc	$a5
	bne	$a5, $a1, .Lrravx1

	dec	$a2
	bnez	$a2, .Lrravx0

	xvld	$xr3, $sp, 96
	xvld	$xr2, $sp, 64
	xvld	$xr1, $sp, 32
	xvld	$xr0, $sp, 0
	addi.d	$sp, $sp, 128

	dbar 0
	ret

#-----------------------------------------------------------------------------
# Name: 	CopyVector256 (LASX 256-bit sequential copy)
# Params: 	$a0 = dst
# 		$a1 = src
# 		$a2 = size, multiple of 256
# 		$a3 = loops
#-----------------------------------------------------------------------------
.align 4
CopyVector256:
_CopyVector256:
	move	$a4, $a0
	move	$a5, $a1
	move	$a6, $a2

.Lcavx0:
	move	$a0, $a4
	move	$a1, $a5
	move	$a2, $a6

.Lcavx1:
	# 4 transfers by 256 bits = 128 bytes
	xvld	$xr0, $a1, 0
	xvld	$xr1, $a1, 32
	xvld	$xr2, $a1, 64
	xvld	$xr3, $a1, 96
	addi.d	$a1, $a1, 128

	xvst	$xr0, $a0, 0
	xvst	$xr1, $a0, 32
	xvst	$xr2, $a0, 64
	xvst	$xr3, $a0, 96
	addi.d	$a0, $a0, 128

	addi.d	$a2, $a2, -128
	bnez	$a2, .Lcavx1

	dec	$a3
	bnez	$a3, .Lcavx0

	dbar 0
	ret

#-----------------------------------------------------------------------------
# Name: 	VectorToVector128 (LSX 128-bit vector reg to vector reg)
# Params:	$a0 = loops
#-----------------------------------------------------------------------------
.align 4
VectorToVector128:

.Lvtv128:
	# 32 transfers x 16 bytes = 512 bytes per outer loop
	vor.v	$vr1, $vr0, $vr0
	vor.v	$vr2, $vr1, $vr1
	vor.v	$vr3, $vr2, $vr2
	vor.v	$vr4, $vr3, $vr3
	vor.v	$vr5, $vr4, $vr4
	vor.v	$vr6, $vr5, $vr5
	vor.v	$vr7, $vr6, $vr6
	vor.v	$vr8, $vr7, $vr7
	vor.v	$vr0, $vr8, $vr8
	vor.v	$vr1, $vr0, $vr0
	vor.v	$vr2, $vr1, $vr1
	vor.v	$vr3, $vr2, $vr2
	vor.v	$vr4, $vr3, $vr3
	vor.v	$vr5, $vr4, $vr4
	vor.v	$vr6, $vr5, $vr5
	vor.v	$vr7, $vr6, $vr6
	vor.v	$vr8, $vr7, $vr7
	vor.v	$vr0, $vr8, $vr8
	vor.v	$vr1, $vr0, $vr0
	vor.v	$vr2, $vr1, $vr1
	vor.v	$vr3, $vr2, $vr2
	vor.v	$vr4, $vr3, $vr3
	vor.v	$vr5, $vr4, $vr4
	vor.v	$vr6, $vr5, $vr5
	vor.v	$vr7, $vr6, $vr6
	vor.v	$vr8, $vr7, $vr7
	vor.v	$vr0, $vr8, $vr8
	vor.v	$vr1, $vr0, $vr0
	vor.v	$vr2, $vr1, $vr1
	vor.v	$vr3, $vr2, $vr2
	vor.v	$vr4, $vr3, $vr3
	vor.v	$vr5, $vr4, $vr4

	dec	$a0
	bnez	$a0, .Lvtv128

	ret

#-----------------------------------------------------------------------------
# Name: 	VectorToVector256 (LASX 256-bit vector reg to vector reg)
# Params:	$a0 = loops
#-----------------------------------------------------------------------------
.align 4
VectorToVector256:

.Lvtv256:
	# 32 transfers x 32 bytes = 1024 bytes per outer loop
	xvor.v	$xr1, $xr0, $xr0
	xvor.v	$xr2, $xr1, $xr1
	xvor.v	$xr3, $xr2, $xr2
	xvor.v	$xr4, $xr3, $xr3
	xvor.v	$xr5, $xr4, $xr4
	xvor.v	$xr6, $xr5, $xr5
	xvor.v	$xr7, $xr6, $xr6
	xvor.v	$xr8, $xr7, $xr7
	xvor.v	$xr0, $xr8, $xr8
	xvor.v	$xr1, $xr0, $xr0
	xvor.v	$xr2, $xr1, $xr1
	xvor.v	$xr3, $xr2, $xr2
	xvor.v	$xr4, $xr3, $xr3
	xvor.v	$xr5, $xr4, $xr4
	xvor.v	$xr6, $xr5, $xr5
	xvor.v	$xr7, $xr6, $xr6
	xvor.v	$xr8, $xr7, $xr7
	xvor.v	$xr0, $xr8, $xr8
	xvor.v	$xr1, $xr0, $xr0
	xvor.v	$xr2, $xr1, $xr1
	xvor.v	$xr3, $xr2, $xr2
	xvor.v	$xr4, $xr3, $xr3
	xvor.v	$xr5, $xr4, $xr4
	xvor.v	$xr6, $xr5, $xr5
	xvor.v	$xr7, $xr6, $xr6
	xvor.v	$xr8, $xr7, $xr7
	xvor.v	$xr0, $xr8, $xr8
	xvor.v	$xr1, $xr0, $xr0
	xvor.v	$xr2, $xr1, $xr1
	xvor.v	$xr3, $xr2, $xr2
	xvor.v	$xr4, $xr3, $xr3
	xvor.v	$xr5, $xr4, $xr4

	dec	$a0
	bnez	$a0, .Lvtv256

	ret

#-----------------------------------------------------------------------------
# Name: 	RegisterToVector128 (64-bit GPR to LSX VR 128-bit)
# Params:	$a0 = loops
#-----------------------------------------------------------------------------
.align 4
Register64ToVector128:

.Lr2v:
	# 32 inserts x 8 bytes = 256 bytes
	vinsgr2vr.d $vr0, $a1, 0
	vinsgr2vr.d $vr1, $a2, 0
	vinsgr2vr.d $vr2, $a3, 1
	vinsgr2vr.d $vr3, $a4, 0
	vinsgr2vr.d $vr4, $a5, 1
	vinsgr2vr.d $vr5, $a6, 0
	vinsgr2vr.d $vr6, $a7, 1
	vinsgr2vr.d $vr7, $t0, 0
	vinsgr2vr.d $vr0, $t1, 0
	vinsgr2vr.d $vr1, $t2, 1
	vinsgr2vr.d $vr2, $t3, 0
	vinsgr2vr.d $vr3, $a1, 1
	vinsgr2vr.d $vr4, $a2, 0
	vinsgr2vr.d $vr5, $a3, 1
	vinsgr2vr.d $vr6, $a4, 0
	vinsgr2vr.d $vr7, $a5, 1
	vinsgr2vr.d $vr0, $a6, 0
	vinsgr2vr.d $vr1, $a7, 1
	vinsgr2vr.d $vr2, $t0, 0
	vinsgr2vr.d $vr3, $t1, 1
	vinsgr2vr.d $vr4, $t2, 0
	vinsgr2vr.d $vr5, $t3, 1
	vinsgr2vr.d $vr6, $a1, 0
	vinsgr2vr.d $vr7, $a2, 1
	vinsgr2vr.d $vr0, $a3, 0
	vinsgr2vr.d $vr1, $a4, 1
	vinsgr2vr.d $vr2, $a5, 0
	vinsgr2vr.d $vr3, $a6, 1
	vinsgr2vr.d $vr4, $a7, 0
	vinsgr2vr.d $vr5, $t0, 1
	vinsgr2vr.d $vr6, $t1, 0
	vinsgr2vr.d $vr7, $t2, 1

	dec	$a0
	bnez	$a0, .Lr2v

	ret

#-----------------------------------------------------------------------------
# Name: 	Register8ToVector128 (8-bit GPR to LSX VR)
# Params:	$a0 = loops
#-----------------------------------------------------------------------------
.align 4
Register8ToVector128:

.Lr8v:
	vinsgr2vr.b $vr0, $a1, 0
	vinsgr2vr.b $vr1, $a2, 1
	vinsgr2vr.b $vr2, $a3, 2
	vinsgr2vr.b $vr3, $a4, 3
	vinsgr2vr.b $vr0, $a5, 4
	vinsgr2vr.b $vr1, $a6, 5
	vinsgr2vr.b $vr2, $a7, 6
	vinsgr2vr.b $vr3, $t0, 7
	vinsgr2vr.b $vr0, $t1, 8
	vinsgr2vr.b $vr1, $t2, 9
	vinsgr2vr.b $vr2, $t3, 10
	vinsgr2vr.b $vr3, $a1, 11
	vinsgr2vr.b $vr0, $a2, 12
	vinsgr2vr.b $vr1, $a3, 13
	vinsgr2vr.b $vr2, $a4, 14
	vinsgr2vr.b $vr3, $a5, 15
	vinsgr2vr.b $vr0, $a6, 0
	vinsgr2vr.b $vr1, $a7, 1
	vinsgr2vr.b $vr2, $t0, 2
	vinsgr2vr.b $vr3, $t1, 3
	vinsgr2vr.b $vr0, $t2, 4
	vinsgr2vr.b $vr1, $t3, 5
	vinsgr2vr.b $vr2, $a1, 6
	vinsgr2vr.b $vr3, $a2, 7
	vinsgr2vr.b $vr0, $a3, 8
	vinsgr2vr.b $vr1, $a4, 9
	vinsgr2vr.b $vr2, $a5, 10
	vinsgr2vr.b $vr3, $a6, 11
	vinsgr2vr.b $vr0, $a7, 12
	vinsgr2vr.b $vr1, $t0, 13
	vinsgr2vr.b $vr2, $t1, 14
	vinsgr2vr.b $vr3, $t2, 15

	dec	$a0
	bnez	$a0, .Lr8v

	ret

#-----------------------------------------------------------------------------
# Name: 	Register16ToVector128 (16-bit GPR to LSX VR)
# Params:	$a0 = loops
#-----------------------------------------------------------------------------
.align 4
Register16ToVector128:

.Lr16v:
	vinsgr2vr.h $vr0, $a1, 0
	vinsgr2vr.h $vr1, $a2, 1
	vinsgr2vr.h $vr2, $a3, 2
	vinsgr2vr.h $vr3, $a4, 3
	vinsgr2vr.h $vr0, $a5, 4
	vinsgr2vr.h $vr1, $a6, 5
	vinsgr2vr.h $vr2, $a7, 6
	vinsgr2vr.h $vr3, $t0, 7
	vinsgr2vr.h $vr0, $t1, 0
	vinsgr2vr.h $vr1, $t2, 1
	vinsgr2vr.h $vr2, $t3, 2
	vinsgr2vr.h $vr3, $a1, 3
	vinsgr2vr.h $vr0, $a2, 4
	vinsgr2vr.h $vr1, $a3, 5
	vinsgr2vr.h $vr2, $a4, 6
	vinsgr2vr.h $vr3, $a5, 7
	vinsgr2vr.h $vr0, $a6, 0
	vinsgr2vr.h $vr1, $a7, 1
	vinsgr2vr.h $vr2, $t0, 2
	vinsgr2vr.h $vr3, $t1, 3
	vinsgr2vr.h $vr0, $t2, 4
	vinsgr2vr.h $vr1, $t3, 5
	vinsgr2vr.h $vr2, $a1, 6
	vinsgr2vr.h $vr3, $a2, 7
	vinsgr2vr.h $vr0, $a3, 0
	vinsgr2vr.h $vr1, $a4, 1
	vinsgr2vr.h $vr2, $a5, 2
	vinsgr2vr.h $vr3, $a6, 3
	vinsgr2vr.h $vr0, $a7, 4
	vinsgr2vr.h $vr1, $t0, 5
	vinsgr2vr.h $vr2, $t1, 6
	vinsgr2vr.h $vr3, $t2, 7

	dec	$a0
	bnez	$a0, .Lr16v

	ret

#-----------------------------------------------------------------------------
# Name: 	Register32ToVector128 (32-bit GPR to LSX VR)
# Params:	$a0 = loops
#-----------------------------------------------------------------------------
.align 4
Register32ToVector128:

.Lr32v:
	vinsgr2vr.w $vr0, $a1, 0
	vinsgr2vr.w $vr1, $a2, 1
	vinsgr2vr.w $vr2, $a3, 2
	vinsgr2vr.w $vr3, $a4, 3
	vinsgr2vr.w $vr0, $a5, 0
	vinsgr2vr.w $vr1, $a6, 1
	vinsgr2vr.w $vr2, $a7, 2
	vinsgr2vr.w $vr3, $t0, 3
	vinsgr2vr.w $vr0, $t1, 0
	vinsgr2vr.w $vr1, $t2, 1
	vinsgr2vr.w $vr2, $t3, 2
	vinsgr2vr.w $vr3, $a1, 3
	vinsgr2vr.w $vr0, $a2, 0
	vinsgr2vr.w $vr1, $a3, 1
	vinsgr2vr.w $vr2, $a4, 2
	vinsgr2vr.w $vr3, $a5, 3
	vinsgr2vr.w $vr0, $a6, 0
	vinsgr2vr.w $vr1, $a7, 1
	vinsgr2vr.w $vr2, $t0, 2
	vinsgr2vr.w $vr3, $t1, 3
	vinsgr2vr.w $vr0, $t2, 0
	vinsgr2vr.w $vr1, $t3, 1
	vinsgr2vr.w $vr2, $a1, 2
	vinsgr2vr.w $vr3, $a2, 3
	vinsgr2vr.w $vr0, $a3, 0
	vinsgr2vr.w $vr1, $a4, 1
	vinsgr2vr.w $vr2, $a5, 2
	vinsgr2vr.w $vr3, $a6, 3
	vinsgr2vr.w $vr0, $a7, 0
	vinsgr2vr.w $vr1, $t0, 1
	vinsgr2vr.w $vr2, $t1, 2
	vinsgr2vr.w $vr3, $t2, 3

	dec	$a0
	bnez	$a0, .Lr32v

	ret

#-----------------------------------------------------------------------------
# Name: 	Vector128ToRegister8 (LSX VR 8-bit element to GPR)
# Params:	$a0 = loops
#-----------------------------------------------------------------------------
.align 4
Vector128ToRegister8:

.Lv82r:
	vpickve2gr.b $a1, $vr0, 0
	vpickve2gr.b $a2, $vr1, 1
	vpickve2gr.b $a3, $vr2, 2
	vpickve2gr.b $a4, $vr3, 3
	vpickve2gr.b $a5, $vr0, 4
	vpickve2gr.b $a6, $vr1, 5
	vpickve2gr.b $a7, $vr2, 6
	vpickve2gr.b $t0, $vr3, 7
	vpickve2gr.b $a1, $vr0, 8
	vpickve2gr.b $a2, $vr1, 9
	vpickve2gr.b $a3, $vr2, 10
	vpickve2gr.b $a4, $vr3, 11
	vpickve2gr.b $a5, $vr0, 12
	vpickve2gr.b $a6, $vr1, 13
	vpickve2gr.b $a7, $vr2, 14
	vpickve2gr.b $t0, $vr3, 15
	vpickve2gr.b $a1, $vr0, 7
	vpickve2gr.b $a2, $vr1, 6
	vpickve2gr.b $a3, $vr2, 5
	vpickve2gr.b $a4, $vr3, 4
	vpickve2gr.b $a5, $vr0, 3
	vpickve2gr.b $a6, $vr1, 2
	vpickve2gr.b $a7, $vr2, 1
	vpickve2gr.b $t0, $vr3, 0
	vpickve2gr.b $a1, $vr0, 10
	vpickve2gr.b $a2, $vr1, 11
	vpickve2gr.b $a3, $vr2, 12
	vpickve2gr.b $a4, $vr3, 13
	vpickve2gr.b $a5, $vr0, 14
	vpickve2gr.b $a6, $vr1, 15
	vpickve2gr.b $a7, $vr2, 8
	vpickve2gr.b $t0, $vr3, 9

	dec	$a0
	bnez	$a0, .Lv82r

	ret

#-----------------------------------------------------------------------------
# Name: 	Vector128ToRegister16 (LSX VR 16-bit element to GPR)
# Params:	$a0 = loops
#-----------------------------------------------------------------------------
.align 4
Vector128ToRegister16:

.Lv162r:
	vpickve2gr.h $a1, $vr0, 0
	vpickve2gr.h $a2, $vr1, 1
	vpickve2gr.h $a3, $vr2, 2
	vpickve2gr.h $a4, $vr3, 3
	vpickve2gr.h $a5, $vr0, 4
	vpickve2gr.h $a6, $vr1, 5
	vpickve2gr.h $a7, $vr2, 6
	vpickve2gr.h $t0, $vr3, 7
	vpickve2gr.h $a1, $vr0, 1
	vpickve2gr.h $a2, $vr1, 2
	vpickve2gr.h $a3, $vr2, 3
	vpickve2gr.h $a4, $vr3, 4
	vpickve2gr.h $a5, $vr0, 5
	vpickve2gr.h $a6, $vr1, 6
	vpickve2gr.h $a7, $vr2, 7
	vpickve2gr.h $t0, $vr3, 0
	vpickve2gr.h $a1, $vr0, 3
	vpickve2gr.h $a2, $vr1, 4
	vpickve2gr.h $a3, $vr2, 5
	vpickve2gr.h $a4, $vr3, 6
	vpickve2gr.h $a5, $vr0, 7
	vpickve2gr.h $a6, $vr1, 0
	vpickve2gr.h $a7, $vr2, 1
	vpickve2gr.h $t0, $vr3, 2
	vpickve2gr.h $a1, $vr0, 6
	vpickve2gr.h $a2, $vr1, 7
	vpickve2gr.h $a3, $vr2, 0
	vpickve2gr.h $a4, $vr3, 1
	vpickve2gr.h $a5, $vr0, 2
	vpickve2gr.h $a6, $vr1, 3
	vpickve2gr.h $a7, $vr2, 4
	vpickve2gr.h $t0, $vr3, 5

	dec	$a0
	bnez	$a0, .Lv162r

	ret

#-----------------------------------------------------------------------------
# Name: 	Vector128ToRegister32 (LSX VR 32-bit element to GPR)
# Params:	$a0 = loops
#-----------------------------------------------------------------------------
.align 4
Vector128ToRegister32:

.Lv322r:
	vpickve2gr.w $a1, $vr0, 0
	vpickve2gr.w $a2, $vr1, 1
	vpickve2gr.w $a3, $vr2, 2
	vpickve2gr.w $a4, $vr3, 3
	vpickve2gr.w $a5, $vr0, 1
	vpickve2gr.w $a6, $vr1, 2
	vpickve2gr.w $a7, $vr2, 3
	vpickve2gr.w $t0, $vr3, 0
	vpickve2gr.w $a1, $vr0, 2
	vpickve2gr.w $a2, $vr1, 3
	vpickve2gr.w $a3, $vr2, 0
	vpickve2gr.w $a4, $vr3, 1
	vpickve2gr.w $a5, $vr0, 3
	vpickve2gr.w $a6, $vr1, 0
	vpickve2gr.w $a7, $vr2, 1
	vpickve2gr.w $t0, $vr3, 2
	vpickve2gr.w $a1, $vr0, 0
	vpickve2gr.w $a2, $vr1, 1
	vpickve2gr.w $a3, $vr2, 2
	vpickve2gr.w $a4, $vr3, 3
	vpickve2gr.w $a5, $vr0, 0
	vpickve2gr.w $a6, $vr1, 1
	vpickve2gr.w $a7, $vr2, 2
	vpickve2gr.w $t0, $vr3, 3
	vpickve2gr.w $a1, $vr0, 1
	vpickve2gr.w $a2, $vr1, 2
	vpickve2gr.w $a3, $vr2, 3
	vpickve2gr.w $a4, $vr3, 0
	vpickve2gr.w $a5, $vr0, 2
	vpickve2gr.w $a6, $vr1, 3
	vpickve2gr.w $a7, $vr2, 0
	vpickve2gr.w $t0, $vr3, 1

	dec	$a0
	bnez	$a0, .Lv322r

	ret

#-----------------------------------------------------------------------------
# Name: 	Vector128ToRegister64 (LSX VR 64-bit element to GPR)
# Params:	$a0 = loops
#-----------------------------------------------------------------------------
.align 4
Vector128ToRegister64:

.Lv642r:
	vpickve2gr.d $a1, $vr0, 0
	vpickve2gr.d $a2, $vr1, 0
	vpickve2gr.d $a3, $vr2, 1
	vpickve2gr.d $a4, $vr3, 0
	vpickve2gr.d $a5, $vr0, 1
	vpickve2gr.d $a6, $vr1, 0
	vpickve2gr.d $a7, $vr2, 1
	vpickve2gr.d $t0, $vr3, 1
	vpickve2gr.d $a1, $vr0, 0
	vpickve2gr.d $a2, $vr1, 1
	vpickve2gr.d $a3, $vr2, 0
	vpickve2gr.d $a4, $vr3, 1
	vpickve2gr.d $a5, $vr0, 0
	vpickve2gr.d $a6, $vr1, 1
	vpickve2gr.d $a7, $vr2, 0
	vpickve2gr.d $t0, $vr3, 0
	vpickve2gr.d $a1, $vr0, 1
	vpickve2gr.d $a2, $vr1, 0
	vpickve2gr.d $a3, $vr2, 1
	vpickve2gr.d $a4, $vr3, 0
	vpickve2gr.d $a5, $vr0, 0
	vpickve2gr.d $a6, $vr1, 1
	vpickve2gr.d $a7, $vr2, 0
	vpickve2gr.d $t0, $vr3, 1
	vpickve2gr.d $a1, $vr0, 0
	vpickve2gr.d $a2, $vr1, 0
	vpickve2gr.d $a3, $vr2, 0
	vpickve2gr.d $a4, $vr3, 0
	vpickve2gr.d $a5, $vr0, 0
	vpickve2gr.d $a6, $vr1, 0
	vpickve2gr.d $a7, $vr2, 0
	vpickve2gr.d $t0, $vr3, 0

	dec	$a0
	bnez	$a0, .Lv642r

	ret

# Unused:
VectorToVector512:
	ret
