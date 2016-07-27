
#include <JobMenu.h>

static long grotorPosition = 0;

void startJob(Floatbyte_t wireSize, Floatbyte_t turnsTotal,
              Floatbyte_t spoolLength, StackFloatBytes_t stackUp) {

  if (confirm() == 0)
    return;

  // Send job parameters to the other UNO
  // [0x1] -- Mode 1 comand job paremeters
  // [4 bytes] -- wire size
  // [4 bytes] -- Total turns
  // [4 bytes ] -- spool length
  // [4 bytes ] -- Turns per layer
  // [4 bytes ] -- Number of whole layers
  // [4 bytes ] -- Turns last layer

  uint8_t parameterData[25] = {};

  uint8_t *pparameterData;

  pparameterData = parameterData;

  *pparameterData++ = 0x1;
  pparameterData = doubleToData(wireSize.bytes, pparameterData);

  pparameterData = doubleToData(turnsTotal.bytes, pparameterData);
  pparameterData = doubleToData(spoolLength.bytes, pparameterData);
  pparameterData = doubleToData(stackUp.turnsWholeLayer.bytes, pparameterData);
  pparameterData =
      doubleToData(stackUp.numberWholeLayers.bytes, pparameterData);
  pparameterData = doubleToData(stackUp.turnsLastLayer.bytes, pparameterData);

  Wire.beginTransmission(8); // transmit to device #8

  Wire.write(parameterData, 25);

  Wire.endTransmission();

  delay(1000);

  // send start

  Wire.beginTransmission(8);

  Wire.write(0x2);

  Wire.endTransmission();

  updateDisplay(turnsTotal.value);
}

void updateDisplay(double total_turns) {

  Floatbyte_t current_layer;
  Floatbyte_t current_turns;
  Floatbyte_t current_layer_turns;
  Floatbyte_t current_speed;
  uint8_t direction;
  uint8_t running;

  do {
    // send get status

    Wire.beginTransmission(8);

    Wire.write(0x3);

    Wire.endTransmission();

    delay(100);

    // Listen for data from other side
    Wire.requestFrom(8, 18);

    uint8_t statusArray[18];

    int i = 0;
    while (Wire.available()) {
      statusArray[i++] = Wire.read();
    }

    uint8_t *status_pointer;
    status_pointer = statusArray;

    status_pointer = get_float_from_array(current_layer.bytes, status_pointer);
    status_pointer = get_float_from_array(current_turns.bytes, status_pointer);
    status_pointer =
        get_float_from_array(current_layer_turns.bytes, status_pointer);
    status_pointer = get_float_from_array(current_speed.bytes, status_pointer);

    direction = *status_pointer++;
    running = *status_pointer;

    lcd.clear();
    lcd.print("Lyr:");
    lcd.print(current_layer.value, 1);
    lcd.print(" Tps:");
    lcd.print(current_speed.value, 1);

    lcd.setCursor(0, 1);
    lcd.print("Turns:");
    lcd.print(current_turns.value, 1);
    lcd.print(" %:");
    // lcd.print("tgt:");

    double percent_turns = (current_turns.value / total_turns) * 100.0;

    lcd.print(percent_turns, 1);
    // lcd.print(total_turns, 1);

    lcd.setCursor(0, 2);
    lcd.print("Running");

    if (direction) {
      lcd.print("  R to L");
    } else {
      lcd.print("  L to R");
    }

    delay(500);
  } while (running);
  lcd.setCursor(0, 2);
  lcd.print("Idle                ");
  lcd.setCursor(0, 3);
  lcd.print(" >OK");
  do {
    pushButton.update();
  } while (!pushButton.isPressed());
}

uint8_t *get_float_from_array(uint8_t *out_array, uint8_t *current_index) {

  int i = 0;
  for (i = 0; i < 4; i++) {
    out_array[i] = *current_index++;
  }
  return current_index;
}

uint8_t *doubleToData(uint8_t *dataArray, uint8_t *pparameterData) {
  for (int i = 0; i < 4; i++) {
    *pparameterData++ = dataArray[i];
  }
  return pparameterData;
}

int confirm() {
  lcd.clear();
  lcd.print("Start Job?");
  lcd.setCursor(0, 3);

  lcd.print(" >Cancel       OK");

  rotor.setMinMax(0, 1);
  rotor.setPosition(0);

  while (1) {
    pushButton.update();
    if (pushButton.isPressed()) {
      if (grotorPosition == cancelSelected) {
        return 0;
      } else {
        return 1;
      }
    }

    long pos = rotor.getPosition();
    if (pos != grotorPosition) {
      if (pos == 0) {
        lcd.setCursor(14, 3);
        lcd.print(" ");
        lcd.setCursor(1, 3);
      } else {
        lcd.setCursor(1, 3);
        lcd.print(" ");
        lcd.setCursor(14, 3);
      }
      lcd.print(">");
    }

    grotorPosition = pos;
  }
}
