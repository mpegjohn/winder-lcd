#ifndef Winder_H
#define Winder_H

#include "version.h"
#include <AccelStepper.h>
#include <Arduino.h>
#include <Winder.h>
#include <Wire.h>
#include <pins.h>

// Define our maximum and minimum speed in steps per second (scale pot to these)
#define MAX_SPEED 500
#define MIN_SPEED 1

#define SERIAL_DEBUG

/*
* These Floatbyte_t variables are set by entering the paameterMode
*/

float wire_size;        // The wire size in mm
int turns;            // The total number of turs
float spool_length;     // Spool length in mm
int turns_per_layer;  // Number of turns per layer
int last_layer_turns; // Number of turns for the last layer

/*
* These Floatbyte_t variables are used during execution of the winding
*/

float current_turns;
int current_layer_turns;
float current_speed; // In Turns per second (TPS)

uint8_t this_layer; // Counter for current layer
int num_layers; // Integer form of the number of layers.

// Motor status byte
// bit 0 = spool 1 = on 0 = off
// bit 1 = shuttle 1 = on 0 = off

uint8_t motor_status = 0x03; // both motors on

uint8_t direction = 0;
uint8_t running = 0; // The flag that controlls the motor updates

enum modes {
  idleMode,
  testMode,
  parameterMode,
  runningMode,
  getStatusMode,
  getMotorStatusMode,
  getVersion,
  getDate,
  pauseMode
};

/* get_float_from_array
* given an array of float variables extract these into seperate float variables.
* [ float 1  ][  float 2 ]
* [0][1][2][3][4][5][6][7]
* Floats are considered to be 4 consecutive bytes bytes as above
* Inputs
*		- uint8_t *out_array Pointer to the location of the 4 receiving
*bytes
*		- uint8_t *current_index Pointer to the start of the float in the
*input array
* Output
*		- uint8_t * Pointer to the last location + 1 of the float in the
*input array
*/
uint8_t *get_float_from_array(uint8_t *out_array, uint8_t *current_index);

/* doubleToData
* The opposite of get_float_from_array.
* given a float (4 bytes) put them in an array
*/
uint8_t *doubleToData(uint8_t *dataArray, uint8_t *pparameterData);

/* calculateSpoolSpeed
* Reads the analogue spped setting from the pot and sets a speed in
* steps per second, between MAX_SPEED and MIN_SPEED
*/
float calculateSpoolSpeed();

/* calculateShuttleSpeed
* based on the spool speed and the wire size, adjust the shuttle speed.
*/
float calculateShuttleSpeed(float spoolSpeed, float wireSize);
long calculateShuttleSteps(float wireSize, int numberTurns);
long calculateSpoolSteps(int numberTurns);

void requestEvent();
void receiveEvent(int howMany);

void do_a_layer(double num_turns);
void updateTps();
void updateTurns();

void printDouble(double val, uint8_t precision);

bool SetUpInterrupts(const int usecs);

#endif
