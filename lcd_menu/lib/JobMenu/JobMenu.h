#ifndef JobMenu_h
#define JobMenu_h

#include <Arduino.h>
#include <Button.h>
#include <LiquidCrystal.h>
#include <RotaryEncoderAcelleration.h>
#include <TicksPerSecond.h>

#define LCD_COLS 20
#define LCD_ROWS 4

extern LiquidCrystal lcd;
extern Button pushButton;
extern RotaryEncoderAcelleration rotor;

void startJob();

#endif
