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

#include "ButtonPresser.h"

ButtonPresser::ButtonPresser(String name_in, int control_pin_in, int post_move_delay_ms_in, int starting_angle_in, int target_angle_in) {
  _last_press = 0L;
  _verby = false;
  _name = name_in;
  _control_pin = control_pin_in;
  _post_move_delay_ms = post_move_delay_ms_in;
  _starting_angle = starting_angle_in;
  _target_angle = target_angle_in;
}

void ButtonPresser::set_verbose(boolean verb) {
  _verby = verb;
}

void ButtonPresser::setup() {
  _servo.attach(_control_pin);
  _servo.write(_starting_angle);
  delay(_post_move_delay_ms); // don't let anything else happen until this servo has come to rest; servos draw a lot of power for an Arduino
}

// TODO: Make this add to a list.
boolean ButtonPresser::add_blocking_buttonpresser(ButtonPresser* blocking_button, unsigned long lockout_time_ms) {
  _blocking_button = blocking_button;
  _lockout_time = lockout_time_ms;
  return true;
}

// TODO: Make this clear the list (see add_blocking_buttonpresser()).
boolean ButtonPresser::clear_blocking_buttonpressers() {
  _blocking_button = NULL;
  return true;
}

unsigned long ButtonPresser::get_remaining_lockout_time() {
  if (_blocking_button == NULL) {
    if (_verby) {
      Serial.print(" no blocker for button named ");
      Serial.println(_name);
    }
    return 0L;
  } else {
    if (_verby) {
      Serial.print(" blocker button ");
      Serial.print((*_blocking_button)._name);
      Serial.print(" last pressed: ");
      Serial.print((*_blocking_button)._last_press);
      Serial.print(", lockout time: ");
      Serial.print(_lockout_time);
      Serial.print(", millis: ");
      Serial.println(millis());
    }

    if (
	((*_blocking_button)._last_press == 0) // Arduino program just started; the blocking button hasn't been pressed yet.
	||
	(millis() - (*_blocking_button)._last_press >= _lockout_time) // enough time has passed between pressing the blocking button and now
	) {
      if (_verby) {
	Serial.println(" no active lockout for this button");
      }
      return 0L;
    } else {
      if (_verby) {
	Serial.print("  ");
	Serial.print((*_blocking_button)._name);
	Serial.print(" requires lockout of ");
	Serial.print(_name);
	Serial.print(" for ");
	Serial.print(_lockout_time - (millis() - (*_blocking_button)._last_press));
	Serial.println(" ms");
      }
      return (_lockout_time - (millis() - (*_blocking_button)._last_press));
    }
  }
}

// TODO: Use additional sensors to determine if the servo action actually had any effect, and only return true if there was
// no lockout in effect AND the button press had the desired effect. This additional layer of checking isn't needed for
// the ideal operational case, where servo arms are aligned and mounted firmly in relation to their target buttons, but any
// shortcomings in mounting could cause servo actions to fail to press their target buttons.
boolean ButtonPresser::toggle_button() {
  if (get_remaining_lockout_time() <= 0L) {
    _servo.write(_target_angle);
    delay(_post_move_delay_ms);
    _last_press=millis();
    if (_verby) {
      Serial.print(" * pressed ");
      Serial.println(_name);
    }
    _servo.write(_starting_angle);
    delay(_post_move_delay_ms);
    return true;
  } else {
    return false;
  }
}
