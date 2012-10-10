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

#ifndef DistanceSensor_h
#define DistanceSensor_h

// An interface for a distance sensor. You can get the distance to
// a target in cm, you can ask what distance value is used when the
// distance to the target cannot be determined, and you can ask
// what the directional trend is based on past readings.
class DistanceSensor {
 public:
  // Get the distance in cm between the sensor and the targeted
  // object, or some sensor-specific constant if the distance could
  // not be measured.
  virtual int get_distance_cm() {};

  // Gets the number that this DistanceSensor will return from
  // get_distance_cm() if the targeted object was not in range
  // (actually too close, actually too far, or actually in range
  // but not detected). This will tend to depend on the specific
  // sensor you use, as well as whatever library you use to work
  // the sensor.
  virtual int get_out_of_range_value_cm() {};

  // Returns a value that shows whether the sensor
  // is getting consistently closer to (-1) or further from (1)
  // the targeted object. If no trend can be determined, or if
  // the targeted object is consistently the same distance, return 0.
  //
  // For a given value of tolerance_cm, changes in movement between
  // readings must be at least (tolerance_cm + 1) cm in order for them
  // to count as movement.
  //
  // This method can be useful if you want to operate a motorized pocket
  // door based on both proximity and direction of movement.
  //
  // TODO: Make this more powerful; report the speed of the movement, not
  // just the direction.
  virtual short get_direction_trend(int tolerance_cm) {};
};

#endif
