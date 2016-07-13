
#include <JobMenu.h>

static long grotorPosition = 0;

void startJob(Floatbyte wireSize, Floatbyte turnsTotal, Floatbyte spoolLength, stackup stack) {

	if(confirm() == 0)
		return;


	lcd.clear();

	lcd.print("L:      T:");
        lcd.setCursor(0,1);
	lcd.print("TT%:      LT%:");
	lcd.setCursor(0,2);
	lcd.print("TPS:");

	Wire.beginTransmission(8); // transmit to device #8

	# Send job parameters to the other UNO
	# [0x1] -- comand job paremeters
	# [4 bytes] -- wire size
	# [4 bytes] -- Total turns
	# [4 bytes ] -- spool length
	# [4 bytes ] -- Turns per layer
	# [4 bytes ] -- Number of whole layers
	# [4 bytes ] -- Turns last layer
	



	Wire.beginTransmission(8); // transmit to device #8





}




int confirm() {
  lcd.clear();
  lcd.print("Start Job?");
  lcd.setCursor(0, 3);

  lcd.print("   >Cancel         OK");

  rotor.setMinMax(0, 1);
  rotor.setPosition(0);

  menuSelection selection = cancelSelected;

 while(1) {
  pushButton.update();
  if(pushButton.isPressed()) {
	if(grotorPosition == cancelSelected)
	{
		returni 0;
	} else {
		return 1;
	}
  }


  long pos = rotor.getPosition();
  if (pos != grotorPosition) {
    if (pos == 0) {
      lcd.setCursor(18, 3);
      lcd.print(" ");
      lcd.setCursor(4, 3);
      selection = cancelSelected;
    } else {
      lcd.setCursor(4, 3);
      lcd.print(" ");
      lcd.setCursor(18, 3);
      selection = okSelected;
    }
	lcd.print(">")
  }

  grotorPosition = pos;
}
}


