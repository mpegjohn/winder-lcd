
#include <ManualMenu.h>

void manualMenu() {

  long lastRotor = 0;
  // Get the motor status
  Wire.beginTransmission(8);

  Wire.write(0x4);

  Wire.endTransmission();

  delay(100);

  // Listen for data from other side
  Wire.requestFrom(8, 1);
  // Motor status byte
  // bit 0 = spool 1 = on 0 = off
  // bit 1 = sguttle 1 = on 0 = off
  uint8_t motor_status = Wire.read();

  lcd.clear();
  lcd.print("Manual motor control");

  printMotorStatus(motor_status);

  lcd.setCursor(0, 3);
  lcd.print(" OK");

  rotor.setMinMax(0, 2);
  rotor.setPosition(0);

  long pos;

  while (1) {

    pushButton.update();
    if (pushButton.isPressed()) {
      if (pos == 0) {
        motor_status = motor_status ^ 1;
      } else if (pos == 1) {
        motor_status = motor_status ^ 2;
      } else {
        return;
      }
      printMotorStatus(motor_status);
      setMotorStatus(motor_status);
    }

    pos = rotor.getPosition();

    if (pos != lastRotor) {
      lcdPrintCursor(pos);
      lastRotor = pos;
    }
  }
}

// Send the current motor status byte to the other nano
void setMotorStatus(uint8_t motor_status) {

  uint8_t motor_byte[2];
  motor_byte[0] = 0x5; // set the motor status command

  motor_byte[1] = motor_status;

  Wire.beginTransmission(8);

  Wire.write(motor_byte, 2);

  Wire.endTransmission();
}

// Decide which cursor position to clear based on the current menu
void lcdPrintCursor(long pos) {

  // Clear all >
  for (int i = 0; i < 4; i++) {
    lcd.setCursor(0, i);
    lcd.print(" ");
  }

  // Display the > at the correct line
  switch (pos) {
  case 0: { // spool
    lcd.setCursor(0, 0);
    break;
  }
  case 1: { // shuttle
    lcd.setCursor(0, 1);
    break;
  }
  case 2: { // OK
    lcd.setCursor(0, 2);
    break;
  }
  }
  lcd.print(">");
}

void printMotorStatus(uint8_t motor_status) {
  lcd.setCursor(0, 1);

  if (motor_status & 0x01) {
    lcd.print(" Spool ON ");
  } else {
    lcd.print(" Spool OFF");
  }

  lcd.setCursor(0, 2);

  if (motor_status & 0x02) {
    lcd.print(" Shuttle ON ");
  } else {
    lcd.print(" Shuttle OFF");
  }
}
