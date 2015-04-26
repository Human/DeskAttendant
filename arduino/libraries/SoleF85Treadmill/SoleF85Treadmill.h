// Copyright 2012 Robert W Igo
//
// bob@igo.name
// http://bob.igo.name
// http://www.linkedin.com/profile/view?id=22294307
//
// This file is part of DeskAttendant.
// 
// DeskAttendant is free software: you can redistribute it and/or modify
// it under the terms of the GNU General Public License as published by
// the Free Software Foundation, either version 3 of the License, or
// (at your option) any later version.
// 
// DeskAttendant is distributed in the hope that it will be useful,
// but WITHOUT ANY WARRANTY; without even the implied warranty of
// MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
// GNU General Public License for more details.
// 
// You should have received a copy of the GNU General Public License
// along with DeskAttendant.  If not, see <http://www.gnu.org/licenses/>.

// By default, this class assumes that 1) you have a START and STOP button on your treadmill, and that pressing START
// once starts the treadmill, and pressing STOP once stops (pauses) the treadmill; 2) you use ButtonPressers to physically
// press the START and STOP buttons with servos.

#ifndef SoleF85Treadmill_h
#define SoleF85Treadmill_h

#include "ButtonTreadmill.h"

// NOTE: As written, this header works with the Sole F85 treadmill and Tower Pro SG-5010 servos controlled by pins
// START_button_servo_pin and STOP_button_servo_pin.
//
// You will absolutely need to change many, if not all, of these constants to work with your own treadmill

//
// The following constants define the pins where your corresponding servos' control wires are attached.
//

// NOTE: Redefine these if you use a different pin assignment to control the servos that press your treadmill's START and STOP buttons.
const int START_button_servo_pin = 9;
const int STOP_button_servo_pin = 10;

// The ButtonPresser class takes a delay value in milliseconds, which it uses after each rotation
// from starting angle to target angle, and from target angle back to starting angle.
//
// Per the specs on the ROB-09064 servo from Sparkfun, it takes 200ms to rotate 60deg at
// 4.8V of power. These servos should be getting 5V of power, but let's use the 4.8V timing
// data to be on the safe side and figure that each degree of rotation takes 5.56 ms (rounding
// to 6ms). In practise, it was necessary to double this value from 6ms to 12ms.
//
// NOTE: Tweak these as needed to match your servos and the position of your servo arms relative to their
// target buttons.

// All servos will use this angle as their "resting" angle. They will be initialized to it and return to it after
// moving to any target angle.
const int servo_starting_angle = 0;

// the START button's servo moves to this angle and back to servo_starting_angle
const int START_servo_target_angle = 20;

// the STOP button's servo moves to this angle and back to servo_starting_angle
const int STOP_servo_target_angle = 26;

// for each degree moved, wait this many ms to ensure that the servo actually reaches the target position
const int ms_per_degree_delay = 6;

const int START_servo_delay = ms_per_degree_delay * (START_servo_target_angle - servo_starting_angle);
const int STOP_servo_delay = ms_per_degree_delay * (STOP_servo_target_angle - servo_starting_angle);

// A subclass of Treadmill for button-controlled treadmills.
class SoleF85Treadmill : public ButtonTreadmill {
public:
  // Create the ButtonPresser objects we need to control the servos for the START and STOP buttons, and configure their
  // lockout relationship.
  void button_setup();
};

#endif
