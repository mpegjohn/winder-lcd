

#include <Arduino.h>

// include the library code:
#include <LiquidCrystal.h>
#include <Button.h>
#include <TicksPerSecond.h>
#include <RotaryEncoderAcelleration.h>
#include <ValueMenu.h>


#define LCD_D7 9
#define LCD_D6 10
#define LCD_D5 4
#define LCD_D4 5
#define LCD_EN 11
#define LCD_RS 12

#define LCD_COLS 20
#define LCD_ROWS 4

#define encoderPinA 2
#define encoderPinB 3


int print_centered(int, char *);


// initialize the lcd library with the numbers of the interface pins
LiquidCrystal lcd(LCD_RS, LCD_EN, LCD_D4, LCD_D5, LCD_D6, LCD_D7);


// interrupt service routine vars
boolean A_set = false;
boolean B_set = false;

static const int buttonPin = A0;	// the number of the pushbutton pin
static const int rotorPinA = 2;	// One quadrature pin
static const int rotorPinB = 3;	// the other quadrature pin

RotaryEncoderAcelleration rotor;
Button pushButton;

void UpdateRotor() {
  rotor.update();
}


void setup()
{
  lcd.begin(LCD_COLS, LCD_ROWS);
  rotor.initialize(rotorPinA, rotorPinB);
  pushButton.initialize(buttonPin);
  attachInterrupt(0, UpdateRotor, CHANGE);

//  showMenu(2.00, 0.01, 0.3, 2, "mm", "Set wire size");
  showMenu(20000.0, 1.0, 5.0, 1, "turns", "Set number of turns");


}

void loop()
{

}
