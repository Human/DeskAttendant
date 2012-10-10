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

#include "NewPingDistanceSensor.h"

NewPingDistanceSensor::NewPingDistanceSensor(int max_cm_distance) {
  _sonar = new NewPing(trigger_pin, echo_pin, max_cm_distance);
  _old_distance = get_out_of_range_value_cm();
  _distance = get_out_of_range_value_cm();
}

int NewPingDistanceSensor::get_distance_cm() {
  _old_distance = _distance;
  _distance = (*_sonar).ping_cm();
  return _distance;
}

short NewPingDistanceSensor::get_direction_trend(int tolerance_cm) {
  int diff;

  diff = _distance - _old_distance;

  if (
      (_old_distance == get_out_of_range_value_cm()) ||
      (_distance == get_out_of_range_value_cm()) || // no usable readings
      (abs(diff) <= tolerance_cm) // the difference between the last two distances is too small to count
      ) {
    diff = 0; // not enough info to determine a directional trend
  }

  // if the code got here, it means that the last two readings were in range and within tolerance

  if (diff == 0) {
    return (short)0; // no change
  } else if (diff < 0) {
    return (short)-1; // the difference between the last two distances is within tolerances
  } else {
    return (short)1; // target getting further from sensor
  }
}
