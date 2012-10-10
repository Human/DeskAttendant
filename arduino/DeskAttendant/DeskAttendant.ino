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

// 1) Detects proximity of a person using a distance sensor.
// 
// 2) If a person was not in range but enters range, tell the treadmill to start, and
// tell the receiver's ButtonPresser to toggle the POWER button.
//
// 3) If a person was present but leaves range, tell the treadmill to pause, and tell
// the receiver's ButtonPresser to toggle the POWER button.
// 
// 4) In the main loop, print a status code to the serial port so that an attached
// computer can fire secondary logic based on changes in the presence of a person.

#include "DeskAttendant.h" // most of the constants used in this program

// See http://provideyourown.com/2011/advanced-arduino-including-multiple-libraries/ to learn
// why all these messy high-level includes are here.

// pressing buttons and controlling a treadmill
#include <Servo.h> // servo class, used by ButtonPresser class
#include "ButtonPresser.h" // wrapper for Servo that presses buttons; we'll use it to toggle the receiver's power button
#include "Treadmill.h" // lets us control a treadmill
#include "ButtonTreadmill.h" // lets us control a treadmill with button presses
#include "SoleF85Treadmill.h" // NOTE: Edit the constants there to customize for your treadmill

// measuring distance to a target (the walker)
#include "NewPing.h" // sonic distance sensor "driver"
#include "DistanceSensor.h" // generic distance sensor
#include "NewPingDistanceSensor.h" // NewPing-based distance sensor
#include "SmoothedDistanceSensor.h" // distance sensor wrapper that filters results for consistency

/***********\
* VARIABLES *
\***********/

/*
 debugging flags and variables
 */
// flags
const boolean debug_pings = false; // serial port: report pings?
const boolean debug_actions = false; // serial port: report whether or not actions were successful?

// variables: range history
int out_of_range_counter = 0; // keeps track of how many sequential out-of-range values we read
int in_range_counter = 0; // keeps track of how many sequential in-range values we read

/*
 walker status (current and new)
 */
int status = unknown_status;
int new_status = unknown_status;
unsigned long turned_on_at = 0L;

/*
 Button pressers press the momentary switches on the treadmill and the A/V receiver.
 */
boolean take_action = true; // affect the treadmill, press buttons, and fire relays if true; otherwise just change status LEDs
ButtonPresser receiver_power("RECEIVER POWER", receiver_button_servo_pin, receiver_servo_delay, receiver_servo_starting_angle, receiver_servo_target_angle);

/*
 Treadmill and subclasses let you control a treadmill by turning it on or off.
 */
Treadmill* treadmill;
SoleF85Treadmill tm;

/*
 Filtered distance sensor object (SmoothedDistanceSensor with an internal
 NewPingDistanceSensor) gives us more reliable distance data.
*/
DistanceSensor* ruler;
NewPingDistanceSensor sonar(max_walker_distance);
SmoothedDistanceSensor sds(&sonar, out_of_range_ms, in_range_ms, latency_tolerance_ms);
int distance = -1; // default distance, in cm

/*
 Manual override mode turns off the system's "eyes" and tells it to act as if the walker is absent.
 The system starts in automatic mode (override off).
 */
boolean manual_override = false;


/***********\
* FUNCTIONS *
\***********/

/*---*\
 SETUP
\*---*/

// Prepare the manual override control buttons for input. It's probably important to do this first because
// this function enables onboard pull-up resistors, and the user might press an override control button early
// in the startup sequence.
void override_buttons_setup() {
  // set up INPUT pins for manual override
  pinMode(manual_override_button_pin, INPUT);
  digitalWrite(manual_override_button_pin, HIGH); // enable internal pull-up resistor

  // set up INPUT pins for automatic mode button
  pinMode(automatic_mode_button_pin, INPUT);
  digitalWrite(automatic_mode_button_pin, HIGH); // enable internal pull-up resistor
}

// A little eye candy for showing that the program is starting.
void rgb_blink() {
  for(int i = 0; i < 3; i++) {
    // turn on
    digitalWrite(leds[i], HIGH);
    delay(167);
  }

  for(int i = 0; i < 3; i++) {
    // turn off
    digitalWrite(leds[i], LOW);
    delay(167);
  }

  //Serial.println("REBOOTED");
}

// Facilitate communication of status with the human (LEDs) and the daemon (serial output).
void status_communication_setup() {
  // status communication over serial
  Serial.begin(baud);

  // we'll light this LED if the system thinks the walker is present
  pinMode(status_led_pin, OUTPUT);

  // we'll light this LED if the system thinks the walker is absent
  pinMode(out_of_range_filtered_led_pin, OUTPUT);

  // we'll light this LED if manual override is in effect
  pinMode(override_led_pin, OUTPUT);

  // do a little 1960s style light blink to show we're starting up
  rgb_blink();
}

// Initialize the ButtonPresser for the A/V receiver.
void button_presser_setup() {
  receiver_power.setup();
}

// Prepare the fan relay to be controlled by pin fan_relay_pin.
void fan_relay_setup() {
  // this pin can throw the relay that lets power go to the fan
  pinMode(fan_relay_pin, OUTPUT);
  // explicitly start with the relay OFF (it may be this by default, but it helps to be sure about starting state)
  set_fan_relay(false);
}

// Initializes the system inputs (distance sensor, override control buttons), outputs (LEDs, serial port), and
// actions (servos and relay).
void setup() {
  //Serial.println("setup() called");

  // Prepare the manual override control buttons for input.
  override_buttons_setup();

  // Facilitate communication of status with the human (LEDs) and the daemon (serial output).
  status_communication_setup();

  ruler = &sds;

  // If we're configured to act on changes in walker status, initialize the A/V receiver's button presser
  // and set up the Treadmill object for use.
  if (take_action) {
    button_presser_setup();

    // Prepare the fan relay to be controlled by pin fan_relay_pin.
    fan_relay_setup();

    tm.setup();
    treadmill = &tm; // work with a SoleF85Treadmill object using the API of Treadmill
  }
}


/*--*\
 LOOP
\*--*/

// Get a stable distance from the sensor to the target (the walker). It is guaranteed to have been *either*
// consistently in range for in_range_ms milliseconds, *or* consistently out of range for out_of_range_ms
// milliseconds. The last sensor-provided distance is used.
void distance_loop() {
  // returns a "vetted" distance which is reliably in range or out of range
  distance = (*ruler).get_distance_cm();

  if (distance > max_walker_distance || distance <= 0) { // out-of-range value
    distance = out_of_range;
  } 

  // The DistanceSensor object (or subclass) may also delay as needed between distance readings, but a small
  // delay here can be useful in case something goes wrong.
  delay(10);
}

// Infer walker status (present/absent) from the reported distance.
void get_status_from_distance() {
  // Keep track of how many times in a row the target has been out of range.
  if (distance == out_of_range) {
    if ((debug_pings) && (in_range_counter > 0)) {
      Serial.print("walker was in range for ");
      Serial.print(in_range_counter);
      Serial.println(" pings before going out of range");
    }
    out_of_range_counter++;
    in_range_counter = 0;
    new_status = walker_absent;
    //Serial.println("new status: walker absent");
  } 
  else {
    if ((debug_pings) && (out_of_range_counter > 0)) {
      Serial.print("walker was out of range for ");
      Serial.print(out_of_range_counter);
      Serial.println(" pings before going in range");
    }
    in_range_counter++;
    out_of_range_counter = 0;
    new_status = walker_present;
    //Serial.println("new status: walker present");
  }
}

// Turn the fan relay either OFF or ON, based on the value of 'on'. Wait a small amount of time
// for the relay to complete its operation, as per the relay's spec sheet.
void set_fan_relay(boolean on) {
  if (on) {
    digitalWrite(fan_relay_pin, HIGH);
    delay(relay_actuate_time);
  } 
  else {
    digitalWrite(fan_relay_pin, LOW);
    delay(relay_release_time);
  }
}

// If there's a changed status, act on it. Turn status LEDs on or off, and perform physical actions with
// servos and relays.
void process_status_loop() {
  boolean on = false;

  // report the new status using the built-in test LED (status_led_pin)
  if (new_status == walker_absent) { // indicate absent walker state with LEDs
    digitalWrite(out_of_range_filtered_led_pin, HIGH);
    digitalWrite(status_led_pin, LOW);
  }
  if (new_status == walker_present) { // indicate present walker state with LEDs
    digitalWrite(out_of_range_filtered_led_pin, LOW);
    digitalWrite(status_led_pin, HIGH);
  }

  if (new_status != status) { // status changed

    if (take_action) { // take the appropriate actions based on walker status

      if (new_status == walker_present) {
        // we will turn on the fan and start the treadmill
        on = true;
      } 
      else {
        // we will turn off the fan and pause the treadmill
        on = false;
      }

      // On *first* run, don't act on a walker_absent state. The starting conditions
      // are mandated to be: treadmill awake, desired workout chosen, treadmill not running, A/V receiver
      // off. Pausing the treadmill on the first run is never appropriate.
      if ((new_status == walker_absent) && (turned_on_at == 0L)) {
        if (debug_pings == true) { // TODO: Only print this once.
          Serial.println("ignoring first-run walker absent state");
        }
      } 
      else {
        // Setting the treadmill's state may or may not work the first time. The Treadmill subclass may refuse
        // to act, due to a button lockout relationship, or there may be some unreliable control hardware involved.
        // Keep trying to set state until it works.
        if (debug_actions) {
          Serial.print("Setting treadmill status to ");
          Serial.println(on);
        }
        if ((*treadmill).set_state(on)) { // this may or may not succeed due to the underlying hardware
          receiver_power.toggle_button();
          set_fan_relay(on);
          status = new_status;
          turned_on_at = millis();
          Serial.println(status); // report the new status over serial
        } else {
          if (debug_actions) {
             Serial.print("treadmill state could not be changed to ");
             Serial.print(new_status);
             Serial.println("; will try again");
          }
        }
      }
    } 
    else { // status LEDs only; not rotating servos or firing relays
      status = new_status;
    }
  }
}

// See if the user pressed either physical input button (override on; override off).
// If either override button is being pressed, set manual_override accordingly and set the LED that
// tells the user whether or not they're in override mode.
void override_button_loop() {
  if ((manual_override == false) && (digitalRead(manual_override_button_pin) == LOW)) {
    manual_override = true;
    // light up a special status LED to show override mode is on
    digitalWrite(override_led_pin, HIGH);
  } 
  else if ((manual_override == true) && (digitalRead(automatic_mode_button_pin) == LOW)) {
    manual_override = false;
    // turn off a special status LED to show override mode is off
    digitalWrite(override_led_pin, LOW);
  }
}

// Main loop. Gets distance to walker, determines what this means about the walker's presence,
// then acts upon any changes in status. Checks to see if either override button is being pressed.
void loop() {
  if (!manual_override) { // normal operational mode (automatic)
    distance_loop(); // Get a stable distance from the sensor to the target (the walker).
    get_status_from_distance(); // Infers walker status (present/absent) from the reported distance.
  } 
  else { // user has forced manual override, which acts as if the walker is absent
    new_status = walker_absent;
  }

  process_status_loop(); // If there's a changed status, act on it.
  override_button_loop(); // See if the user pressed either physical input button (override on; override off).
}

