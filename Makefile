#============================================================================
# bandwidth, a benchmark to measure memory transfer bandwidth.
# Copyright (C) 2005-2024,2026 by Zack T Smith.
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
#============================================================================

UNAMESYS := $(shell uname -s | tr '[:upper:]' '[:lower:]')
UNAMEMACHINE := $(shell uname -m | tr '[:upper:]' '[:lower:]')

IS_CYGWIN := $(shell uname -a | grep -q Cygwin && echo 1 || echo 0)
IS_32BIT := $(shell file /bin/ls | grep -q 32-bit && echo 1 || echo 0)

CC=gcc 

LIB=-L./OOC -lOOC -lm

X86_SYSTEMS_USING_SYSV_CALLING_CONVENTION := freebsd openbsd netbsd haiku gnu 

ifneq ($(filter clean,$(MAKECMDGOALS)),)
	include Makefile-clean
else ifeq (${UNAMESYS},darwin)
	ifeq (${UNAMEMACHINE},arm64)
		include Makefile-${UNAMESYS}-aarch64
	else ifeq (${UNAMEMACHINE},arm)
		include Makefile-${UNAMESYS}-aarch64
	else ifeq (${UNAMEMACHINE},x86_64)
		include Makefile-${UNAMESYS}-${UNAMEMACHINE}
	else
		$(info "i386 is no longer supported by Mac OS.")
	endif
else ifeq (${UNAMESYS},linux)
	ifeq (${UNAMEMACHINE},ppc64le)
		include Makefile-${UNAMESYS}-${UNAMEMACHINE}
	else ifeq (${UNAMEMACHINE},armv6l)
		include Makefile-${UNAMESYS}-aarch32
	else ifeq (${UNAMEMACHINE},armv7l)
		include Makefile-${UNAMESYS}-aarch32
	else ifeq (${UNAMEMACHINE},aarch64)
		ifeq (${IS_32BIT},1)
			include Makefile-linux-aarch32
		else
			include Makefile-linux-aarch64
		endif
	else ifeq (${UNAMEMACHINE},i386)
		include Makefile-${UNAMESYS}-${UNAMEMACHINE}
	else ifeq (${UNAMEMACHINE},i486)
		include Makefile-${UNAMESYS}-i386
	else ifeq (${UNAMEMACHINE},i586) # JSLinux
		include Makefile-${UNAMESYS}-i386
	else ifeq (${UNAMEMACHINE},i686)
		include Makefile-${UNAMESYS}-i386
	else ifeq (${UNAMEMACHINE},x86_64)
		ifeq (${IS_32BIT},1)
			include Makefile-${UNAMESYS}-i386
		else
			include Makefile-${UNAMESYS}-${UNAMEMACHINE}
		endif
	else ifeq (${UNAMEMACHINE},riscv64)
		include Makefile-${UNAMESYS}-${UNAMEMACHINE}
	else ifeq (${UNAMEMACHINE},mips64)
		include Makefile-unsupported
	else ifeq (${UNAMEMACHINE},ppc64)	# Big-endian.
		include Makefile-linux-ppc64le	# In theory, could work.
	else 
		include Makefile-unsupported
	endif
else ifneq ($(filter $(UNAMESYS),$(X86_SYSTEMS_USING_SYSV_CALLING_CONVENTION)),)
	include Makefile-linux-x86_64
else ifeq (${UNAMESYS},windows)
	include Makefile-windows64
else ifeq (${IS_CYGWIN},1)
	include Makefile-windows64
else 
	include Makefile-unsupported
endif

Makefile-config:
	./configure

libOOC.a:
	( cd OOC; make )

