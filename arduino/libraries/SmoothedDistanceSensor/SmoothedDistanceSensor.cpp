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

#include "SmoothedDistanceSensor.h"

SmoothedDistanceSensor::SmoothedDistanceSensor(DistanceSensor* distance_sensor, int out_of_range_ms, int in_range_ms, int latency_tolerance_ms) {
  _distance_sensor = distance_sensor;
  _out_of_range_ms = out_of_range_ms;
  _in_range_ms = in_range_ms;
  _latency_tolerance_ms = latency_tolerance_ms;
  _old_distance = get_out_of_range_value_cm();
  _distance = get_out_of_range_value_cm();
}

int SmoothedDistanceSensor::get_distance_cm() {
  unsigned long streak_start_time = millis();
  boolean in_range = false; // true if the last distance read was "in range" (i.e. not get_out_of_range_value_cm() cm)
  int temp_distance = get_out_of_range_value_cm();

  // TODO: Remember the last N distances read for more advanced smoothing and trending operations.

  // TODO: Should this operate in a thread? I could have a method that returns the latest trusted distance,
  // based on continuous operation.
  
  // loop forever until we have a trusted value to return
  while(true) {
    // get another distance
    temp_distance = (*_distance_sensor).get_distance_cm();
    delay(39); // A Sensor object's ping_cm() should be called at most every 39ms.

    // If the previous streak was recent enough, see if this distance fits the previous streak.
    if (millis() - _last_returned_distance_ms <= _latency_tolerance_ms) { // last streak was recent enough
      if ((temp_distance != get_out_of_range_value_cm()) && _last_distance_was_in_range) {
	_last_returned_distance_ms = millis();
	_old_distance = _distance;
	_distance = temp_distance;
	return _distance;
      } else if ((temp_distance == get_out_of_range_value_cm()) && !_last_distance_was_in_range) {
	_last_returned_distance_ms = millis();
	_old_distance = _distance;
	_distance = temp_distance;
	return _distance;
      }
    }

    if (in_range) { // our last distance reading was in range
      if (temp_distance != get_out_of_range_value_cm()) {
	// Last reading was in range, and this one is, too.
	
	// See if our streak is long enough to count.
	if (millis() - streak_start_time >= _in_range_ms) {
	  // We had an unbroken streak of in-range values for in_range_ms or more ms
	  _last_distance_was_in_range = true;
	  _last_returned_distance_ms = millis();
	  _old_distance = _distance;
	  _distance = temp_distance;
	  return _distance;
	}
      } else {
	// Last reading was in range, but this one is not.  This breaks the streak, so start a new one.
	streak_start_time = millis();
	in_range = false;
      }
    } else { // our last distance reading was out of range
      if (temp_distance == get_out_of_range_value_cm()) {
	// Last reading was out of range, and this one is, too. 
	
	// See if our streak is long enough to count.
	if (millis() - streak_start_time >= _out_of_range_ms) {
	  _last_distance_was_in_range = false;
	  _last_returned_distance_ms = millis();
	  _old_distance = _distance;
	  _distance = temp_distance;
	  return _distance;
	}
      } else {
	// Last reading was out of range, but this one is not.  This breaks the streak, so start a new one.
	streak_start_time = millis();
	in_range = true;
      }
    }
  }
}

int SmoothedDistanceSensor::get_out_of_range_value_cm() {
  return (*_distance_sensor).get_out_of_range_value_cm();
}

short SmoothedDistanceSensor::get_direction_trend(int tolerance_cm) {
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
