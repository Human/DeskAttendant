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

#ifndef NewPingDistanceSensor_h
#define NewPingDistanceSensor_h

#include "DistanceSensor.h"

// See http://provideyourown.com/2011/advanced-arduino-including-multiple-libraries/ to learn why this horrible include is here.
#include "../NewPing/NewPing.h"

// Trigger and echo pins for the NewPing-supported distance sensor.
// NOTE: Redefine these if you use a different pin assignment.
const int trigger_pin = 2;
const int echo_pin = 3; // NOTE: Pin 13 won't work as the echo pin on the Duemilanove but it will on Uno R3

// A NewPing-based DistanceSensor object. Uses a NewPing object as the
// hardware driver.

class NewPingDistanceSensor : public DistanceSensor {
 public:
  // This object will return distances from get_distance_cm() up to and including
  // max_cm_distance, even if the underlying hardware is capable of more range.
  // Any distance out of range is returned as get_out_of_range_value_cm().
  NewPingDistanceSensor(int max_cm_distance);
 
  // Gets the distance to a target as reported by NewPing::ping_cm()
  int get_distance_cm();

  // NewPing returns a 0 for out of range readings, so we will, too.
  int get_out_of_range_value_cm() {
    return 0;
  };

  // Implements a very basic algorithm to comply with DistanceSensor::get_direction_trend().
  // History is only two readings deep, and no temporal filters are applied.
  short get_direction_trend(int tolerance_cm);

 private:
  // An internal NewPing object used for direct hardware access to the sensor.
  NewPing* _sonar;
  // the last distance we measured (used by get_direction_trend)
  int _distance;
  // the next-to-last distance we measured (used by get_direction_trend)
  int _old_distance;
};

#endif
