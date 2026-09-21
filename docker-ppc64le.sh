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
case "$(uname -m)" in
    ppc64le)
	echo "Your device is ppc64le."
	;;
    *) 
	echo Your device is not ppc64le-based, so ppc64le will be emulated.
	;;
esac

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

echo ________________________________________

IMG=bandwidth-ppc64le
ARCH=ppc64le
PLATFORM=linux/$ARCH
RELEASE=trixie
DOCKFILE=/tmp/.bandwidthDockerfile

echo "FROM --platform=$PLATFORM debian:$RELEASE" > $DOCKFILE
echo "RUN dpkg --add-architecture $ARCH " >> $DOCKFILE
cat ./Dockerfile-common >> $DOCKFILE

if $CMD build --platform=$PLATFORM -t $IMG -f $DOCKFILE .; then
	if ! $CMD run -e TERM=xterm --platform=$PLATFORM -it $IMG; then
		echo Run failed.
	fi
else
	echo Build failed.
fi

