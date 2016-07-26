#ifndef ManualMenu_h
#define ManualMenu_h

#include <Arduino.h>
#include <Button.h>
#include <LiquidCrystal.h>
#include <RotaryEncoderAcelleration.h>
#include <TicksPerSecond.h>
#include <Wire.h>

#define LCD_COLS 20
#define LCD_ROWS 4

extern LiquidCrystal lcd;
extern Button pushButton;
extern RotaryEncoderAcelleration rotor;


// This menu will allow the user to manually enable or disable either motor
void menualMenu();

// This function writes to the other nano with the set motor command, and the
// motor_status byte;
void setMotorStatus(unit8_t motor_status);

// Ensures the > cursor is printed in the correct place
void lcdPrintCursor(long pos);

// Reflects the motor_status byte to the LCD
void printMotorStatus(uint8_t motor_status);

#endif
