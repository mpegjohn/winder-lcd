#ifndef LcdMenu_h
#define LcdMenu_h

#include <Arduino.h>

// include the library code:
#include <Button.h>
#include <JobMenu.h>
#include <LiquidCrystal.h>
#include <RotaryEncoderAcelleration.h>
#include <TicksPerSecond.h>
#include <ValueMenu.h>
#include <Wire.h>

enum mainMenuMode { setupJobMode, reviewJobMode, startJobMode };

struct stackup {
  double numberWholeLayers;
  double turnsWholeLayer;
  double turnsLastLayer;
};


/*
* This is the first function to get called.
* It displays the main menu of
* Setup new job
* Review
* Start job
*/
void lcdMainMenu();

/*
* Decide which cursor position to clear based on the current menu
* set by lcdMainMenu
*/
void lcdPrintCursor();

/*
* Go through the series of menus to setup a new job
* order is Wire Size, turns and spool size
* defaults are set.
* If cancel is pressed, thn we just return
* If all data is OK'd we set the relevent globals
* It then calls calculateStackup() to calculate the 
* number of layers and turns per layer.
*/
void newJob();

/*
* Function to calculate the number of turns per layer, the number of layers
* and the number of turns for the last layer.
*/
StackFloatBytes calculateStackup(double wireSize, double bobbinLength,
                                 double turns);


/*
* Allow the user to look at the job paramters
*/
void lcdReview(StackFloatBytes stack);

/*
* Do a test on the I2C.
* Return the number of failing test patterns
* Data sent to other Arduino is:
* [0x00] -- Mode 0, the I2C test mode
* [0xAA, 0x55, 0xFF] -- The data sent
*
* It expects the same data back from the other side.
*/
int DataStreamTest();

#endif
