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

#include "SoleF85Treadmill.h"
#include "ButtonTreadmill.h"
#include "ButtonPresser.h"

void SoleF85Treadmill::button_setup() {
  start_button = new ButtonPresser("START", START_button_servo_pin, START_servo_delay, 0, START_servo_target_angle);
  pause_button = new ButtonPresser("STOP", STOP_button_servo_pin, STOP_servo_delay, 0, STOP_servo_target_angle);
  
  // Pressing STOP locks out the start button; the treadmill needs a while to come to rest before it
  // will accept a new START.
  (*start_button).add_blocking_buttonpresser(pause_button, 11500UL);
  
  // Pressing START locks out the stop button; the treadmill will ignore STOP in the first few seconds
  // after pressing START.
  (*pause_button).add_blocking_buttonpresser(start_button, 5500UL);
}
