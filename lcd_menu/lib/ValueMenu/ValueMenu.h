#ifndef ValueMenu_h
#define ValueMenu_h

#include <Arduino.h>
#include <LiquidCrystal.h>
#include <Button.h>
#include <TicksPerSecond.h>
#include <RotaryEncoderAcelleration.h>

#define LCD_COLS 20
#define LCD_ROWS 4

extern LiquidCrystal lcd;
extern Button pushButton;
extern RotaryEncoderAcelleration rotor;

enum items {
  value,
  cancel,
  OK
};

struct menuResult {
  double value;
  boolean status;
};


/*
Displays a menu on the LCD to set a value using the rotary encoderPinB
Display looks like:
r1  set wire size
r2     >0.01 mm
r3
r4  Cancel         OK

Input:
  double max - Maximum value allowed
  double min - Minimum value allowed
  double initialValue - The value to show at the beginning
  int decimalPoints - the number of decimal points
  char * unit - The units for this value
  char * text - The text to Display at the top.
  e.g.
  showMenu(20000.0, 1.0, 5.0, 1, "turns", "Set number of turns");

Returns menuResult.
*/
menuResult showMenu(double max, double min, double initialValue, int decimalPoints, char const * unit, char const * text);


/*
 Prints the inital menu
*/
void setupMenu(char const* text, double initialValue);

/*
Print any text centered in a row.
Returns the column number of the first char.
*/
int print_centered(int row, char const * text);

/*
Creates a loop that allows a user to set the value
*/
void setTheValue(double divisor);

/*
Prints the value as a float.
Input:
 isSelected - true then print [] around value
              false print > at the start of the value

*/
void printFloat(boolean isSelected);

#endif
