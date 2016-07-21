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

void lcdMainMenu();
StackFloatBytes calculateStackup(double wireSize, double bobbinLength,
                                 double turns);
void lcdPrintCursor();

int DataStreamTest();

#endif
