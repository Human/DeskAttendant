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

// Constants used in DeskAttendant.ino

#ifndef DeskAttendant_h
#define DeskAttendant_h

// Arduino pins used: 2-12
// 
// User Inputs:
//    distance sensor controls on 2, 3
//    manual override inputs on 4, 5
// User Outputs:
//    status LEDs on 6, 7, 8
// Automation Actions:
//    Servos on 9, 10, 11
//    relay trigger on 12

// If your DeskAttendant uses NewPingDistanceSensor or SoleF85Treadmill,
// see NewPingDistanceSensor.h for distance sensor pin constants, or
// see SoleF85Treadmill.h for servo control pin constants
// to ensure you aren't trying to do multiple things with the same pin.
const int manual_override_button_pin = 4; // an input button; induces manual override mode when pressed
const int automatic_mode_button_pin = 5; // an input button; induces automatic mode when pressed
const int out_of_range_filtered_led_pin = 6; // red LED
const int status_led_pin = 7; // green LED
const int override_led_pin = 8; // blue LED
const int receiver_button_servo_pin = 11; // the control pin for the Servo inside the A/V receiver's controlling ButtonPresser
// all status LEDs in one array so we can show a startup sequence
const int leds[3] = {out_of_range_filtered_led_pin, status_led_pin, override_led_pin};
const int fan_relay_pin = 12;

// Variables related to walker status and feedback about that status. Sent over serial port.
const int unknown_status = 0; // no chance yet to read from our sensor
const int walker_absent = 1; // no object detected by sensor, or object detected too far away to count
const int walker_present = 2; // object detected by sensor and near enough to count

// Used to initialize serial port output.
const int baud = 9600; // I couldn't exceed 9600 when reading the port from the daemon.

// ButtonPresser for A/V receiver
const int receiver_servo_ms_per_degree_delay = 12;
const int receiver_servo_starting_angle = 0; // the servo that presses the A/V receiver button rests at this angle
const int receiver_servo_target_angle = 13; // the A/V receiver power button's servo moves to this angle and back to av_servo_starting_angle
const int receiver_servo_delay = receiver_servo_ms_per_degree_delay * (receiver_servo_target_angle - receiver_servo_starting_angle);

// relay for fan
const int relay_actuate_time = 15; // milliseconds (per the PowerSwitch Tail II spec)
const int relay_release_time = 10; // milliseconds (per the PowerSwitch Tail II spec)

// Distance sensor parameters (I'm using the HY-SRF05 sensor. If you use a different sensor, read the spec sheet
// to see if you can or should increase or decrease any of these values.)

const int max_walker_distance = 40; // in centimeters; keep this below the underlying range sensor's spec'ed max range;
// this is used to create/calibrate the DistanceSensor subclass we use to get distances.
const int out_of_range = max_walker_distance + 50; // in centimeters
const int read_delay_ms = 39; // milliseconds to wait between getting distances from a NewPing object

// Parameters for the SmoothedDistanceSensor.
const int out_of_range_ms = 500; // milliseconds of continuous out-of-range before we believe it
const int in_range_ms = 500; // milliseconds of continuous in-range before we believe it
const int latency_tolerance_ms = 150; // readings taken farther apart than this break continuity

#endif
