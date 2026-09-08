#!/bin/bash
#=============================================================================
# bandwidth, a benchmark to measure memory transfer bandwidth.
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
#============================================================================*/

reset
MACHINE=`uname -m`
if [[ "$MACHINE" == "x86_64" || "$MACHINE" == "i386" || "$MACHINE" == "i486" || "$MACHINE" == "i586" || "$MACHINE" == "i686" ]]; then
	echo CPU is compatible.
else
	echo CPU is not x86.
	exit 1
fi

CMD=false
if which podman >/dev/null; then
	CMD=podman
	echo We will use Podman.
elif which docker >/dev/null; then
	CMD=docker
	echo We will use Docker.
else
	echo You have neither Podman nor Docker installed.
	exit 2
fi

if $CMD build --platform=linux/386 -t debian-i386-image -f ./Dockerfile-i386 .; then
	if ! $CMD run -e TERM=xterm --platform=linux/386 -it debian-i386-image; then
		echo Run failed.
	fi
else
	echo Build failed.
fi

