# Copyright 2012,2014 Robert W Igo
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

import sys, os, datetime

imageDir = getBundlePath() # the script path
imageSubDir = "imagery" # corresponds to scriptname/imagery
addImagePath(os.path.join(imageDir, imageSubDir))

# NOTE: These must all match the Arduino sketch.
unknown_status=0
walker_absent=1
walker_present=2
last_status=unknown_status

#
# Check arguments.
#
port = "none"

def print_usage():
    print("USAGE: See desk_attendant.sh")
    sys.exit

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
    # First, make sure the desktop is unlocked by looking for the "power" icon.
    print("Is the desktop unlocked?")
    unlocked=False
    setRect(1812,0,148,48)
    for i in range(0, 30):
        try:
            power = wait("powerbutton.png", 1)
            unlocked=True
            print(" yes")
            break
        except FindFailed:
            pass
    if unlocked == False:
        print(" no; we can't toggle apps to " + mode)
        return
    
    for app in ["amarok", "pithos", "hamster"]:
        print("is " + app + " running?")
        try:
            # If app is running, pause it
            
            # The app icons live in the tray at the top of the window.
            setRect(1045,0,550,32)
            icon = wait(Pattern(app+"trayicon.png").similar(0.85), 1)
            print(" " + mode + " " + app)
            button = Pattern(app+mode+"button.png").similar(0.76)
            rightClick(icon)
            try:
                # The popup menu goes lower than the top tray.
                setRect(1045,0,700,200)
                target = wait(button, 1)
                click(target)
            except FindFailed:
                print(" found no " + mode + " button")
                rightClick(icon)
        except FindFailed:
            pass

#
# Open 'port'.
#
try:
    ser=os.open(port, os.O_RDONLY)
except:
    print("unable to read from serial port " + port)

print("Desk Attendant is ready on serial port " + port)

#
# Tell Sikuli to make the mouse fast.
#
Settings.MoveMouseDelay = 0

#
# Read from 'port' as long as it exists.
#
while os.path.exists(port):
    for line in os.read(ser,1):
        try:
            this_status = int(line)
        except ValueError:
            pass # sometimes, crap comes through on the serial port
        if this_status != last_status:
            this_status_name=get_name_for_state(this_status)
            print(unicode(datetime.datetime.now()) + ": " + this_status_name)
            if this_status != unknown_status:
                take_action(this_status_name)
            last_status=this_status

print("exited")
