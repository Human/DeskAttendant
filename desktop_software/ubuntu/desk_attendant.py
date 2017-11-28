# Copyright 2012-2017 Robert W Igo
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

import datetime
import os
import serial
import subprocess
import sys
import time
from subprocess import call
from igolibs.haupdater import HAUpdater

# NOTE: These must all match the Arduino sketch.
unknown_status = 0
walker_absent = 1
walker_present = 2
last_status = unknown_status

#
# Check arguments.
#
port = "none"


def print_usage():
    print("USAGE: See desk_attendant.sh")
    sys.exit()


if len(sys.argv) < 1:
    print('missing parameter')
    print_usage()
else:
    port = sys.argv[1]


#
# get a human-readable name for the numeric state code
#
def get_name_for_state(x):
    return {
        1: 'pause',
        2: 'unpause',
    }[x]


#
# put these apps into the right mode
#
def take_action(mode):
    cmd = None
    app2 = []
    for app in ["amarok", "pithos"]:  # , "hamster"]:
        cmd1 = "pgrep " + app
        p = subprocess.Popen(cmd1, shell=True, executable="/bin/bash", stdout=subprocess.PIPE)
        (out, err) = p.communicate()
        if out != "":
            # print app + " is running"
            app2.append(app)

    for app in app2:
        print("putting " + app + " into " + mode + " mode")
        # put 'app' in state 'mode'
        if app == "amarok":
            cmd1 = "pgrep amarok"
            p = subprocess.Popen(cmd1, shell=True, executable="/bin/bash", stdout=subprocess.PIPE)
            (out, err) = p.communicate()
            if out.decode() is not "":
                if mode == "pause":
                    cmd = "amarok --pause"
                else:
                    cmd = "amarok --play"
        elif app == "hamster":
            if mode == "pause":
                cmd = "hamster-cli stop"

                cmd2 = "notify-send \"pausing tasks\""
                p = subprocess.Popen(cmd2, shell=True, executable="/bin/bash", stdout=subprocess.PIPE)
                p.communicate()

            else:
                # get the last-used task
                cmd1 = "hamster-cli list | tail -1 | awk -F '|' '{print $2}' | awk -F '@' '{print $1}'"
                p = subprocess.Popen(cmd1, shell=True, executable="/bin/bash", stdout=subprocess.PIPE)
                (out, err) = p.communicate()
                task = out.decode().strip()

                if task is not "":
                    cmd2 = "notify-send \"resuming task: " + str(task) + "\""
                    p = subprocess.Popen(cmd2, shell=True, executable="/bin/bash", stdout=subprocess.PIPE)
                    p.communicate()

                    # start the last-used task
                    cmd = "hamster-cli start " + str(task)
                    subprocess.Popen(cmd, shell=True, executable="/bin/bash")

        elif app == "pithos":
            # figure out what pithos is doing
            cmd1 = "dbus-send --print-reply --dest=net.kevinmehall.Pithos /net/kevinmehall/Pithos net.kevinmehall.Pithos.IsPlaying | grep boolean | sed 's/boolean//'"
            p = subprocess.Popen(cmd1, shell=True, executable="/bin/bash", stdout=subprocess.PIPE)
            (out, err) = p.communicate()
            playing = out.decode().strip()
            if (playing == "true" and mode == "pause") or (playing == "false" and mode == "unpause"):
                cmd = "dbus-send --print-reply --dest=net.kevinmehall.Pithos /net/kevinmehall/Pithos net.kevinmehall.Pithos.PlayPause"

        if cmd is not None:
            # print "   INVOKING: " + cmd
            # set the state
            p = subprocess.Popen(cmd, shell=True, executable="/bin/bash", stdout=subprocess.PIPE,
                                 stderr=subprocess.PIPE)
            p.communicate()

    # finally, inform OpenHAB that the treadmill is becoming active or inactive (based on mode)
    if mode == "pause":
        updater.updateOpenhab("http://openhab.igo:8080/CMD", {"Treadmill_Desk_Active": "OFF"})
    else:
        updater.updateOpenhab("http://openhab.igo:8080/CMD", {"Treadmill_Desk_Active": "ON"})

    print("----")


updater = HAUpdater()
#
# Open 'port'.
#
print("Running Desk Attendant on serial port " + port)
try:
    while True:
        with serial.Serial(port, timeout=None) as ser:
            line = str(ser.read(1).decode().rstrip("\n"))
            try:
                this_status = int(line)
                if this_status != last_status:
                    try:
                        this_status_name = get_name_for_state(this_status)
                        print(str(datetime.datetime.now()) + ": " + this_status_name)
                        if this_status != unknown_status:
                            take_action(this_status_name)
                        last_status = this_status
                    except KeyError:
                        print("damn")
            except ValueError:
                pass  # sometimes, crap comes through on the serial port
except FileNotFoundError as e:
    print("unable to read from serial port " + port)
    print(str(e))

print("exited")
