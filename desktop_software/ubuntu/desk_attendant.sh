#!/bin/bash

# Copyright 2012 Robert W Igo
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
    echo `basename $0` "-p [port]"
    echo "-p: Serial port where your Arduino is attached"
    echo "e.g."
    echo "$0 -p /dev/ttyUSB0"
    exit 4
}

while getopts "p:" FLAG ; do
    case "$FLAG" in
	p) port="$OPTARG";;
	*) Usage;;
    esac
done

if [ "$port" == "" ]; then
    Usage
fi

# NOTE: The paths used (or omitted) here must be compatible with your Sikuli installation and
# with the location where you installed the deskattendant.sikuli directory.
sikuli_base_cmd="sikuli-ide -r /repos/desk_attendant/desktop_software/sikuli/deskattendant.sikuli -args"

# NOTE: These must all match the Arduino sketch.
baud=9600
unknown_status=0
walker_absent=1
walker_present=2
last_status=$unknown_status # unknown status

date=`date +%s`
echo "$date: $0 starting..."

# First, set the appropriate serial port to the appropriate baud rate.
stty -F $port raw ispeed $baud ospeed $baud time 3 min 1

take_action() {
    new_state=$1
    arg=""

    # If the state transitions to walker_present, unpause programs on the desktop
    if [ "$this_status" == "$walker_absent" ]; then
	arg="--pause"
    else
	# If the state transitions to walker_absent, pause programs on the desktop.
	if [ "$this_status" == "$walker_present" ]; then
	    arg="--unpause"
	fi
    fi

    date=`date +%s`
    echo "$date: acting on new state '$new_state' with $arg"

    if [ "$arg" != "" ]; then
	$sikuli_base_cmd $arg
    fi

    date=`date +%s`
    echo "$date: acted on state '$new_state' with $arg"
}

# Now, read from it forever.
while true
do
    this_status=`head -c 1 $port`
    retval=$?
    if [ $retval -eq 1 ]; then
	date=`date +%s`
	echo "$date: unable to read $port - waiting 5s"
	sleep 5
    fi
    # I ended up having to check directly for the expected values
    # because some unprintable characters come through when there's nothing to read.
    if [[ "$this_status" == "$unknown_status" ]] ||
	[[ "$this_status" == "$walker_absent" ]] ||
	[[ "$this_status" == "$walker_present" ]] ; then
	#date=`date +%s`
	#echo "$date: status is '$this_status'"
	if [ "$this_status" != "$last_status" ]; then
	    # it's a change, but is it an actionable change?
	    if [ "$this_status" != "$unknown_status" ]; then
		# If there's an actionable status change, act on it. Any status
		# change involving transitions to a _known_ state is
		# actionable.
		take_action $this_status	    
	    fi
	    last_status=$this_status
	fi
    fi
    sleep 0.5
done

