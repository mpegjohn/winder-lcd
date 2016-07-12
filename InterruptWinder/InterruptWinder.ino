// ConstantSpeed.pde
// -*- mode: C++ -*-
//
// Shows how to run AccelStepper in the simplest,
// fixed speed mode with no accelerations
/// \author  Mike McCauley (mikem@airspayce.com)
// Copyright (C) 2009 Mike McCauley
// $Id: ConstantSpeed.pde,v 1.1 2011/01/05 01:51:01 mikem Exp mikem $

#include <AccelStepper.h>
#include <Wire.h>

union cvt {
  float val;
  unsigned char b[4];
};


// Definition of digital pins used
#define SPOOL_STEP 2
#define SPOOL_DIR 3
#define SHUTTLE_STEP 4
#define SHUTTLE_DIR 5
#define SPOOL_ENABLE 6
#define SHUTTLE_ENABLE 7

// Define our analog pot input pin
#define  SPEED_PIN 0

// Define our maximum and minimum speed in steps per second (scale pot to these)
#define  MAX_SPEED 500
#define  MIN_SPEED 1

AccelStepper spool(AccelStepper::DRIVER, SPOOL_STEP, SPOOL_DIR);
AccelStepper shuttle(AccelStepper::DRIVER, SHUTTLE_STEP, SHUTTLE_DIR);

float wireSize;
long totalTurns;
float spoolSpeed;
float shuttleSpeed;
long spoolSteps;
long shuttleSteps;
int loop_count;

int run;

void setup()
{ 
  Serial.begin(9600); 

  pinMode(SPOOL_ENABLE, OUTPUT);
  pinMode(SHUTTLE_ENABLE, OUTPUT);

  digitalWrite(SPOOL_ENABLE, LOW);
  digitalWrite(SHUTTLE_ENABLE, LOW);

  spool.setMaxSpeed(MAX_SPEED);
  shuttle.setMaxSpeed(MAX_SPEED);

  run = 0;

  //setupJob();

  //spool.moveTo(spoolSteps);
  //shuttle.moveTo(shuttleSteps);

  //spool.setSpeed(spoolSpeed);
  //shuttle.setSpeed(shuttleSpeed);

  //loop_count = 10000;
  SetUpInterrupts(100);

  Wire.begin(8);                // join i2c bus with address #8
  Wire.onReceive(receiveEvent); // register event

}

// function that executes whenever data is received from master
// this function is registered as an event, see setup()
void receiveEvent(int howMany) {
  
  cvt bytes_in;
  
  int i = 0;
  while (1 < Wire.available()) { // loop through all but the last
    Serial.print(i);
    bytes_in.b[i] = Wire.read(); // receive byte as a character
    i++;
  }
  
  printDouble(bytes_in.val,2);
  
 // int x = Wire.read();    // receive byte as an integer
 // Serial.println(x);         // print the integer
}




ISR(TIMER1_COMPA_vect){//timer1 interrupt

    if(run) {
    spool.runSpeedToPosition();
    shuttle.runSpeedToPosition();
  }
}

void loop()
{  
}

/*
void loop()
 {  
 
 loop_count--;
 if(loop_count == 0)
 {
 loop_count = 10000;
 Serial.println(spool.currentPosition());
 spoolSpeed = calculateSpoolSpeed();
 shuttleSpeed = calculateShuttleSpeed(spoolSpeed, wireSize);
 
 spool.setSpeed(spoolSpeed);
 shuttle.setSpeed(shuttleSpeed);
 }
 if((spool.distanceToGo() == 0) && (shuttle.distanceToGo() == 0))
 {
 
 Serial.println("End of run");
 Serial.print("Spool count: ");
 Serial.println(spool.currentPosition());
 Serial.print("Shuttle count: ");
 Serial.println(shuttle.currentPosition());
 run = 0;
 setupJob();
 }
 }
 */

void printMenu() {

  Serial.println("Winder menu");
  Serial.println("---------------");
  Serial.println("Enable motors: e");
  Serial.println("Disable motors: d");

  Serial.println("Set wire diameter mm: w");
  Serial.println("Set total number of turns: n");
  Serial.println("Set number of layers: l");
  Serial.println("Start: s");
  Serial.println("Pause: p");
}

void setupJob() {

  while(1) {
    printMenu();

    waitForSerial();

    char option = Serial.read();

    if(option == 'w') {
      Serial.println("Enter wire size in mm: ");
      waitForSerial();
      wireSize = Serial.parseFloat();
    }

    else if(option == 'e') {
      Serial.println("Enter motor to enable - spool(s) or shuttle (h): s/h");
      waitForSerial();
      char subOption = Serial.read();
      if( subOption == 's') {
        digitalWrite(SPOOL_ENABLE,LOW);
      }
      else if( subOption == 'h') {
        digitalWrite(SHUTTLE_ENABLE,LOW);
      }
      else {
        continue;
      }
    }

    else if(option == 'd') {
      Serial.println("Enter motor to disable - spool(s) or shuttle (h): s/h");
      waitForSerial();
      char subOption = Serial.read();
      if( subOption == 's') {
        digitalWrite(SPOOL_ENABLE,HIGH);
      }
      else if(subOption == 'h') {
        digitalWrite(SHUTTLE_ENABLE,HIGH);
      }
      else {
        continue;
      }
    }

    else if(option == 'n') {
      Serial.println("Enter total number of turns: ");
      waitForSerial();
      totalTurns = Serial.parseInt();       
    }
    else if(option == 's') {

      spoolSpeed = calculateSpoolSpeed();
      spoolSteps = calculateSpoolSteps(totalTurns);

      shuttleSpeed = calculateShuttleSpeed(spoolSpeed, wireSize);
      shuttleSteps = calculateShuttleSteps(wireSize, totalTurns);
      shuttleSteps = shuttleSteps * -1;

      Serial.print("Spool speed is set to: ");
      printDouble(spoolSpeed, 2);
      Serial.print(" pps\n");
      Serial.print("Shuttle speed is set to: ");
      printDouble(shuttleSpeed, 2);
      Serial.print(" pps\n");

      Serial.print("Total length: ");
      float length = totalTurns * wireSize;
      printDouble(length,2);
      Serial.print(" mm\n");

      Serial.print("Shuttle Steps: ");
      Serial.print(shuttleSteps);
      Serial.print(" steps\n");

      Serial.print("Spool Steps: ");
      Serial.print(spoolSteps);
      Serial.print(" steps\n");

      Serial.println("GO? y");
      waitForSerial();

      option = Serial.read();
      if(option != 'y') {
        continue; 
      }
      // Enable the motors
      digitalWrite(SPOOL_ENABLE, LOW);
      digitalWrite(SHUTTLE_ENABLE, LOW);
      run = 1;
      return;
    }

  }


}

void waitForSerial() {
  while(!Serial.available()) {
  }
}

float calculateSpoolSpeed()
{
  static int analog_value;
  // Now read the pot (from 0 to 1023)
  analog_value = analogRead(SPEED_PIN);
  //  And scale the pot's value from min to max speeds
  float spoolSpeed = ((analog_value/1023.0) * (MAX_SPEED - MIN_SPEED)) + MIN_SPEED;
  return spoolSpeed;
}

float calculateShuttleSpeed(float spoolSpeed, float wireSize)
{
  float shuttleSpeed = wireSize * spoolSpeed;

  return shuttleSpeed;
}

long calculateShuttleSteps(float wireSize, int numberTurns) {

  long shuttleSteps = (long)(wireSize * 200.0 * (long)numberTurns);
  return shuttleSteps;
}

long calculateSpoolSteps(int numberTurns) {

  long spoolSteps = 200 * (long)numberTurns;
  return spoolSteps;
}

/*****************************************************************************
 ** SetUpInterrupts
 ** ===============
 * Set up interrupt routine to service stepper motor run() function.
 */
bool SetUpInterrupts(const int usecs)
{
  // initialize Timer1
  cli();          // disable global interrupts
  TCCR1A = 0;     // set entire TCCR1A register to 0
  TCCR1B = 0;     // same for TCCR1B

  // set compare match register to desired timer count (1ms):
  // ATmega328 with a 16MHz clock, clk/8
  // (# timer counts + 1) = (target time) / (timer resolution)
  //                      =     .0001s      /   6.25e-8 s  * 8
  //                      =   200
  const float targetSecs = ((float) usecs) / 1e6;
  const float timerSteps = 6.25e-8;                //    1/16MHz
  int count = 0;
  int prescale = 1;  // valid values: 1, 8, 64, 256, 1024
  do  {
    count = targetSecs / (timerSteps * prescale);
    if(count < 65535)  // Timer 1 is 16-bits wide
      break;
    prescale *= 8;
  } 
  while (prescale <= 1024);
  if(prescale > 1024)                 // time too long
    return false;
  if(prescale == 1 && count < 100)    // time too short
    return false;

  OCR1A = count;         // Eg, 200 = 0.1ms - I found 1ms gives rough acceleration
  // turn on CTC mode (Clear Timer on Compare Match):
  TCCR1B |= (1 << WGM12);
  // Set CS10 and CS12 bits for 1024 prescaler:
  // CS12   CS11   CS10
  //   0      0      0   no clock source, Timer/counter stopped
  //   0      0      1   clk/1  no prescaling
  //   0      1      0   clk/8
  //   0      1      1   clk/64
  //   1      0      0   clk/256
  //   1      0      1   clk/1024
  //   1      1      0   external clock on T1 pin, falling edge
  //   1      1      1   external clock on T1 pin, rising edge
  switch(prescale)  {
  case 1:
    TCCR1B |= (1 << CS10);                   // 0 0 1
    break;
  case 8:
    TCCR1B |= (1 << CS11);                   // 0 1 0
    break;
  case 64:
    TCCR1B |= (1 << CS11) & (1 << CS10);     // 0 1 1
    break;
  case 256:
    TCCR1B |= (1 << CS12);                   // 1 0 0
    break;
  case 1024:
    TCCR1B |= (1 << CS12) & (1 << CS10);     // 1 0 1
    break;
  }
  // enable timer compare interrupt:
  TIMSK1 |= (1 << OCIE1A);
  // enable global interrupts:
  sei();

  return true;
}

















