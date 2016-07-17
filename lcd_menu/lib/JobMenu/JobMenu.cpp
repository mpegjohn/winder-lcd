
#include <JobMenu.h>

static long grotorPosition = 0;

void startJob(Floatbyte wireSize, Floatbyte turnsTotal, Floatbyte spoolLength,
              StackFloatBytes stackUp) {

  if (confirm() == 0)
    return;

  lcd.clear();

  lcd.print("L:      T:");
  lcd.setCursor(0, 1);
  lcd.print("TT%:      LT%:");
  lcd.setCursor(0, 2);
  lcd.print("TPS:");

  while (1) {
  }
  // Send job parameters to the other UNO
  // [0x1] -- comand job paremeters
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

  menuSelection selection;

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
        lcd.setCursor(2, 3);
        selection = cancelSelected;
      } else {
        lcd.setCursor(2, 3);
        lcd.print(" ");
        lcd.setCursor(14, 3);
        selection = okSelected;
      }
      lcd.print(">");
    }

    grotorPosition = pos;
  }
}
