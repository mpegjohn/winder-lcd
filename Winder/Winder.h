#ifndef Winder_H
#define Winder_H

#include <AccelStepper.h>
#include <Arduino.h>
#include <Winder.h>
#include <Wire.h>
#include <pins.h>

// Define our maximum and minimum speed in steps per second (scale pot to these)
#define MAX_SPEED 500
#define MIN_SPEED 1

#define DEBUG true

typedef union floatbytes {
  float value;
  uint8_t bytes[4];
} Floatbyte_t;

Floatbyte_t wire_size;
Floatbyte_t turns;
Floatbyte_t spool_length;
Floatbyte_t turns_per_layer;
Floatbyte_t last_layer_turns;

Floatbyte_t current_turns;
Floatbyte_t current_layer_turns;
Floatbyte_t current_speed;

int this_layer;
int num_layers;

// Motor status byte
// bit 0 = spool 1 = on 0 = off
// bit 1 = sguttle 1 = on 0 = off

uint8_t motor_status = 0x03; // both motors on

uint8_t direction = 0;
uint8_t running = 0;

enum modes {
  idleMode,
  testMode,
  parameterMode,
  runningMode,
  getStatusMode,
  getMotorStatusMode
};

uint8_t *get_float_from_array(uint8_t *out_array, uint8_t *current_index);
uint8_t *doubleToData(uint8_t *dataArray, uint8_t *pparameterData);
float calculateSpoolSpeed();
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
