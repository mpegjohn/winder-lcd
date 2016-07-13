#ifndef JobMenu_h
#define JobMenu_h

#include <Arduino.h>
#include <Button.h>
#include <LiquidCrystal.h>
#include <RotaryEncoderAcelleration.h>
#include <TicksPerSecond.h>
#include <Wire.h>
#include <lcd_menu.h>

#define LCD_COLS 20
#define LCD_ROWS 4

enum menuSelection {
	cancelSelected,
	okSelected
};

typedef union floatbytes {
  double value;
  byte bytes[4];
}
Floatbyte;

struct stackupFloatbyte {
        Floatbyte numberWholeLayers;
        Floatbyte turnsWholeLayer;
        Floatbyte turnsLastLayer;
};

extern LiquidCrystal lcd;
extern Button pushButton;
extern RotaryEncoderAcelleration rotor;

void startJob(double wireSize, double turnsTotal, double spoolLength, stackupFloatbyte stack);

#endif
