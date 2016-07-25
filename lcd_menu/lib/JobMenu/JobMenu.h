#ifndef JobMenu_h
#define JobMenu_h

#include <Arduino.h>
#include <Button.h>
#include <LiquidCrystal.h>
#include <RotaryEncoderAcelleration.h>
#include <TicksPerSecond.h>
#include <Wire.h>

#define LCD_COLS 20
#define LCD_ROWS 4

enum menuSelection { cancelSelected, okSelected };

typedef union floatbytes {
  uint8_t bytes[4];
  float value;
} Floatbyte_t;

typedef struct stackupFloats {
  Floatbyte_t numberWholeLayers;
  Floatbyte_t turnsWholeLayer;
  Floatbyte_t turnsLastLayer;
} StackFloatBytes_t;

extern LiquidCrystal lcd;
extern Button pushButton;
extern RotaryEncoderAcelleration rotor;

void startJob(Floatbyte_t wireSize, Floatbyte_t turnsTotal,
              Floatbyte_t spoolLength, StackFloatBytes_t stackUp);

int confirm();
uint8_t *doubleToData(uint8_t *dataArray, uint8_t *pparameterData);

uint8_t *get_float_from_array(uint8_t *out_array, uint8_t *current_index);

void updateDisplay(double total_turns);
#endif
