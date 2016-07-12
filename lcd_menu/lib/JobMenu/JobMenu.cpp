
#include <JobMenu.h>

long grotorPosition = 0;

void startJob() {
  lcd.clear();
  lcd.print("Start Job?");
  lcd.setCursor(0, 3);

  lcd.print("   >Cancel         OK");

  rotor.setMinMax(0, 1);
  rotor.setPosition(0);

  long pos = rotor.getPosition();
  if (pos != grotorPosition) {
    if (pos == 0) {
      lcd.setCursor(4, 3);
    } else {
    }
  }
  grotorPosition = pos;
}
