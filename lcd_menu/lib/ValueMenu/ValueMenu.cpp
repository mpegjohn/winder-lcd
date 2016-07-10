#include <ValueMenu.h>

// Global variables
static double gcurrent_value; // the current set value
long gcurrentRotor; // the current rotor position
static long glastRotor = 0; // The laet rotor position
static char const * gunit; // The unit of meaure
int gdecimalPoints; // number of decimal points.

menuResult showMenu(double max, double min, double initialValue, int decimalPoints, char const * unit, char const* text) {


  gunit = unit;
  gdecimalPoints = decimalPoints;

  items currentSelectedItem = value; // the current selecte menu item
  double divisor = pow(10.0, (double)decimalPoints );

  long rotorMax = (long) (max * divisor);
  long rotorMin = (long) (min * divisor);
  long rotorInit = (long) (initialValue * divisor);

  gcurrentRotor = rotorInit;

  setupMenu(text, initialValue);

  while (1) {
    pushButton.update();

    if (pushButton.isPressed()) {
      pushButton.update();
      switch (currentSelectedItem) {
        case value: {

          // We have selected to change the value
          // print [] around the value to indicate
          // selection

          printFloat(true);

          rotor.setMinMax(rotorMin, rotorMax);
          rotor.setPosition(gcurrentRotor);

          setTheValue(divisor);

          gcurrentRotor = rotor.getPosition(); // store the rotor position
          rotor.setMinMax(0, 2);
          rotor.setPosition(0);
          // set the rotor for item selection again
          // print > at start of value

          printFloat(false);

          break;
        };
        case cancel:
        {
          menuResult result;
          result.status = false;
          result.value = gcurrent_value;
          return (result);
        };
        case OK:
        {
          menuResult result;
          result.status = true;
          result.value = gcurrent_value;
          return (result);
        };
        default: { break; };
      }
    }

    long pos = rotor.getPosition();

    if (glastRotor != pos) {

      // erase all > prompts
      lcd.setCursor(5, 1);
      lcd.print(" ");
      lcd.setCursor(2, 4);
      lcd.print(" ");
      lcd.setCursor(14, 4);
      lcd.print(" ");

      // Set the cursor to the correct item
      // then print a >
      if (pos == 0) {
        lcd.setCursor(5, 1);
        currentSelectedItem = value;
      } else if (pos == 1) {
        lcd.setCursor(2, 4);
        currentSelectedItem = cancel;
      } else {
        lcd.setCursor(14, 4);
        currentSelectedItem = OK;
      }
      lcd.print(">");

      glastRotor = pos;
    }
  }
}

void setupMenu(char const* text, double initialValue) {

  lcd.clear();
  print_centered(0, text);

  gcurrent_value = initialValue;

  printFloat(false);

  lcd.setCursor(3, 4);
  lcd.print("Cancel      OK");

  rotor.setMinMax(0, 2);
  rotor.setPosition(0);
}

void setTheValue(double divisor) {
  while (1) {
    pushButton.update();
    if (!pushButton.isPressed()) {
      long pos = rotor.getPosition();

      if (glastRotor != pos) {
        gcurrent_value = (double)pos / divisor;
        printFloat(true);
      }

      glastRotor = pos;
    } else {
      return;
    }
  }
}

void printFloat(boolean isSelected) {
  const int startColumn = 5;

  lcd.setCursor(startColumn, 1);
  lcd.print("             "); // clear all values on the line
  lcd.setCursor(startColumn, 1);

  (isSelected) ? lcd.print("[") : lcd.print(">");

  lcd.print(gcurrent_value, gdecimalPoints);

  lcd.print(" ");
  lcd.print(gunit);

  if (isSelected)
    lcd.print("]");
  return;
}

int print_centered(int row, char const* text) {
  int str_len = strlen(text);

  int offset = (LCD_COLS - str_len) / 2;

  lcd.setCursor(offset, row);
  lcd.print(text);

  return offset;
}
