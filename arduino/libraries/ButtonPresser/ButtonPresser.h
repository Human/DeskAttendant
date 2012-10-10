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

#ifndef ButtonPresser_h
#define ButtonPresser_h

#include <Arduino.h> // defines 'boolean'
#include <Servo.h>

class ButtonPresser {
public:
  // This constructor fully configures the ButtonPresser.
  ButtonPresser(String name_in, int control_pin_in, int post_move_delay_ms_in, int starting_angle_in, int target_angle_in);

  // Sets verbosity for debugging via Serial.print(). NOTE: If you turn on verbosity, you must call
  // Serial.begin() before using the ButtonPresser object.
  void set_verbose(boolean verb);

  // Call this from Arduino's setup() function so that your ButtonPresser is ready to be used.
  void setup();

  // If blocking_button fires, it will block this ButtonPresser for lockout_time ms.
  boolean add_blocking_buttonpresser(ButtonPresser* blocking_button, unsigned long lockout_time_ms);

  // Removes all blocking ButtonPressers that were added.
  boolean clear_blocking_buttonpressers();

  // Gets remaining time before this ButtonPresser's corresponding button may be pressed, based on configured lockouts.
  // If there are no lockouts, return 0L. If there are lockouts, return a time based on the current time, the time at which
  // the paired lockout button was pressed, and the lockout period.
  unsigned long get_remaining_lockout_time();

  // Toggles the _servo to target_angle for post_move_delay_ms, then back to starting_angle indefinitely; blocks for
  // 2 * post_move_delay_ms ms; returns true if button was toggled; false if not
  boolean toggle_button(); 

 private:
  // The "driver" used to interface with the servo hardware.
  Servo _servo;

  // If this is non-null, when _blocking_button fires, it can prevent this ButtonPresser from operating for a time.
  ButtonPresser* _blocking_button;

  // If _blocking_button is non-null, this is how long _blocking_button can lock out this ButtonPresser from operating.
  unsigned long _lockout_time;

  // If true, print ongoing status via Serial.print().
  boolean _verby;

  // milliseconds to wait after moving the _servo; servos use lots of power for an Arduino
  int _post_move_delay_ms;

  // "resting" position for the _servo
  int _starting_angle;

  // target position for the _servo
  int _target_angle;

  // Arduino pin where we send control pulses to the _servo
  int _control_pin;

  // human-readable name of this button
  String _name;

  // TODO: unused for now; angle which the servo last reported
  int _angle;
  
  // uptime-based timestamp, in ms, at which this ButtonPresser last pressed a button
  unsigned long _last_press;

};

#endif
