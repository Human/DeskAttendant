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

#ifndef ButtonTreadmill_h
#define ButtonTreadmill_h

#include <Arduino.h> // defines 'boolean'

#include "Treadmill.h"
#include "ButtonPresser.h"

// A ButtonTreadmill lets us control a treadmill with ButtonPresser objects that physically press toggle buttons using servos.
class ButtonTreadmill : public Treadmill {
 public:
  // A number of setup steps specific to button-controlled treadmills.
  void setup() {
    button_setup();
    (*start_button).setup();
    (*pause_button).setup();
  }

  // Implement this in the subclass to set up start_button and pause_button.
  virtual void button_setup() {};

  // Sets the treadmill's state to running (true) or paused (false). Return true if the relevant ButtonPresser reports success.
  // Return false if the relevant ButtonPresser reports failure. The intent is that callers of this method will keep calling it
  // until they get a true return value.
  boolean set_state(boolean on) {
    if (on) {
      return (*start_button).toggle_button();
    } else {
      return (*pause_button).toggle_button();
    }
  }
 protected:
  // an uninitialized ButtonPresser assigned to the treadmill's START button; button_setup() inits it
  ButtonPresser* start_button;

  // an uninitialized ButtonPresser assigned to the treadmill's STOP button; button_setup() inits it
  ButtonPresser* pause_button;
};

#endif
