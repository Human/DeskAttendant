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

#ifndef Treadmill_h
#define Treadmill_h

// A control class for treadmills. It can pause a running treadmill or start a paused treadmill.
class Treadmill {
 public:
  // set up the treadmill object
  virtual void setup() {};
  // if state is true, start the treadmill; otherwise, pause the treadmill
  virtual boolean set_state(boolean on) {};
};

#endif
