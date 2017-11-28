#!/bin/bash

# Copyright 2012-2015 Robert W Igo
#
# bob@igo.name
# http://bob.igo.name
# http://www.linkedin.com/profile/view?id=22294307
#
# This file is part of DeskAttendant.
# 
# DeskAttendant is free software: you can redistribute it and/or modify
# it under the terms of the GNU General Public License as published by
# the Free Software Foundation, either version 3 of the License, or
# (at your option) any later version.
# 
# DeskAttendant is distributed in the hope that it will be useful,
# but WITHOUT ANY WARRANTY; without even the implied warranty of
# MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
# GNU General Public License for more details.
# 
# You should have received a copy of the GNU General Public License
# along with DeskAttendant.  If not, see <http:#www.gnu.org/licenses/>.

Usage() {
    echo "USAGE:"
    echo `basename $0` "-p PORT [-v]"
    echo "-p: Serial port where your Arduino is attached"
    echo "-v: interact with apps visually (using Sikuli); otherwise use CLI mode"
    echo "e.g."
    echo "$0 -p /dev/ttyUSB0"
    exit 4
}

while getopts "p:v:" FLAG ; do
    case "$FLAG" in
	p) port="$OPTARG";;
	v) mode="visual";;
	*) Usage;;
    esac
done

# Try to auto-determine the port. This makes a big assumption that only one FTDI serial device
# converter is attached via USB and that it's our Arduino.
if [ "$port" == "" ]; then
    port=`ls -1 /dev/serial/by-id/*FTDI* | head -1`
fi

if [ "$port" == "" ]; then
    port="/dev/"`grep -a 'FTDI USB Serial Device converter now attached to' /var/log/kern.log | tail -1 | awk -F 'now attached to ' '{print $2}'`
fi

if [ "$port" == "" ]; then
    Usage
fi

# NOTE: This must match the Arduino sketch.
baud=9600

# First, set the appropriate serial port to the appropriate baud rate.
# NOTE: This restarts the Arduino on the _first_ invocation. Subsequent invocations will not.
stty -F $port raw ispeed $baud ospeed $baud time 3 min 1 -hupcl

# NOTE: The paths used (or omitted) here must be compatible with your Sikuli installation and
# with the location where you installed the deskattendant.sikuli directory.
if [ "$mode" == "visual" ]; then
    sikuli-ide -r /repos/DeskAttendant/desktop_software/sikuli/deskattendant.sikuli -args $port
else
    python3 /repos/DeskAttendant/desktop_software/ubuntu/desk_attendant.py $port
fi
