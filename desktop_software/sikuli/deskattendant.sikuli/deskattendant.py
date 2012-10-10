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

import sys
import os

imageDir = getBundlePath() # the script path
imageSubDir = "imagery" # corresponds to scriptname/imagery
addImagePath(os.path.join(imageDir, imageSubDir))

mode = "none"

def print_usage():
    progname = sys.argv[0]
    print "USAGE: " + progname + " [--pause|--unpause]"
    sys.exit

if len(sys.argv) < 2:
    print 'missing parameter'
    print_usage()
elif sys.argv[1].startswith('--'):
    mode = sys.argv[1][2:]
    # fetch sys.argv[1] but without the first two characters
    if mode != 'pause' and mode != 'unpause':
        print 'bad parameter'
        print_usage()

# Fast mouse
Settings.MoveMouseDelay = 0

#
# put these apps into the right mode
#

for app in ["amarok", "pithos", "hamster"]:
    print "is " + app + " running?"
    try:
        # If app is running, pause it

        # The app icons live in the tray at the top of the window.
        setROI(1045,0,550,32)
        icon = wait(app+"trayicon.png", 1)
        print mode + " " + app
        button = Pattern(app+mode+"button.png").similar(0.86)
        rightClick(icon)
        try:
            # The popup menu goes lower than the top tray.
            setROI(1045,0,550,326)
            target = wait(button, 1)
            click(target)
        except FindFailed:
            print "found no " + mode + " button"
            rightClick(icon)
    except FindFailed:
        pass
