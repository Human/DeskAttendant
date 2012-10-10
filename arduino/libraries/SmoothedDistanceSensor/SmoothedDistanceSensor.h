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

#ifndef SmoothedDistanceSensor_h
#define SmoothedDistanceSensor_h

#include <Arduino.h> // defines 'boolean'

#include "DistanceSensor.h"
#include "NewPingDistanceSensor.h"

// This class provides the same API as DistanceSensor, but it applies
// a digital filter to the raw data read from the underlying sensor
// hardware, with the intent of producing more stable, reliable output.
//
// Some sensor hardware has occasional false positives or
// negatives, and these need to be smoothed out. Even 100% reliable
// sensor hardware may need this kind of filter so that a single
// "in range" value in a sea of "out of range" values is ignored.
// Otherwise, any actions taken based on sensor data may appear to
// "flail" as they go rapidly between states.
class SmoothedDistanceSensor : public DistanceSensor {
 public:
  // Builds a SmoothedDistanceSensor that 1) returns an out of range value when only out of range values have been
  // read for the past out_of_range_ms ms, OR 2) returns an in-range value when only in-range values have been read
  // for the past in_range_ms ms. In each case, two consecutive sensor readings are considered to be part of
  // the same "streak" if they occurred <= latency_tolerance_ms ms apart.
  //
  // See SmoothedDistanceSensor::get_distance_cm() for details.
  SmoothedDistanceSensor(DistanceSensor* distance_sensor, int out_of_range_ms, int in_range_ms, int latency_tolerance_ms);

  // A blocking wrapper for NewPing::ping_cm() that blocks until a trusted reading occurs.
  // In this case, trust is defined as receiving a sufficiently long, unbroken streak of either
  // in-range distance readings or out-of-range distance readings. Untrusted distances are not
  // reported by this class. When first called, this will block for max(out_of_range_ms, in_range_ms) ms.
  // Subsequent calls may return very quickly, but if a change in distance type occurs (i.e. last
  // contiguous block of distances was in range, but the next distance is out of range), it will
  // block as it does on the first call.
  //
  // The return unit is centimeters.
  //
  // The return value is get_out_of_range_value_cm() if we read out-of-range distances (based on _sonar's
  // configuration) for out_of_range_ms or more ms.
  //
  // The return value is the last-read (least latent) distance if we've read in-range distances for
  // in_range_ms or more ms.
  int get_distance_cm();

  // Calls _distance_sensor's get_out_of_range_value_cm() method and returns the result.
  int get_out_of_range_value_cm();

  // Returns the last-seen direction trend, based on the distances that are actually returned by
  // get_distance_cm(), not just the ones polled.
  short get_direction_trend(int tolerance_cm);

 private:
  // pointer to the DistanceSensor object we're using to get distances from the underlying hardware
  DistanceSensor* _distance_sensor;
  // Only trust that nothing is in range after reading out_of_range_ms ms of continuous out-of-range distances.
  int _out_of_range_ms;
  // Only trust that something is in range after reading in_range_ms ms of continuous in-range distances.
  int _in_range_ms;
  // If it's been latency_tolerance_ms or fewer ms since we last returned a distance, count the next distance as
  // part of the previous streak, if it's the same distance type (e.g. a new single in-range value is compatible
  // with a very recent in-range streak).
  int _latency_tolerance_ms;
  // remembers whether or not the last distance read was in or out of range
  boolean _last_distance_was_in_range;
  // the post-run time when we last read a distance from sonar
  unsigned long _last_returned_distance_ms;
  // the last distance we measured (used by get_direction_trend)
  int _distance;
  // the next-to-last distance we measured (used by get_direction_trend)
  int _old_distance;
};

#endif
