
#include <Winder.h>

boolean print_data = true;

modes current_mode = idleMode;
modes request_mode = idleMode;

AccelStepper spool(AccelStepper::DRIVER, SPOOL_STEP, SPOOL_DIR);
AccelStepper shuttle(AccelStepper::DRIVER, SHUTTLE_STEP, SHUTTLE_DIR);

float spoolSpeed;
float shuttleSpeed;

uint8_t i2c_test_data[3];

ISR(TIMER1_COMPA_vect) { // timer1 interrupt

  if (running) {
    spool.runSpeedToPosition();
    shuttle.runSpeedToPosition();
  }
}

void setup() {
  wire_size = 0.0;
  turns = 0;
  spool_length = 0.0;

  pinMode(SPOOL_ENABLE, OUTPUT);
  pinMode(SHUTTLE_ENABLE, OUTPUT);

  digitalWrite(SPOOL_ENABLE, LOW);
  digitalWrite(SHUTTLE_ENABLE, LOW);

  spool.setMaxSpeed(MAX_SPEED);
  shuttle.setMaxSpeed(MAX_SPEED);

  Serial.begin(9600); // start serial for output
  Wire.begin(8);
  Wire.onRequest(requestEvent); // register a request
  Wire.onReceive(receiveEvent); // register event

  SetUpInterrupts(100);
}

void loop() {

while(Serial.available() == 0){} // Wait for something on the serial bus

while (Serial.available()) {

  if(Serial.read() == 0x01) {// set parameters

  // [0x1] -- Mode 1 comand job paremeters


  // WS[float] -- wire size
  // TT[int] -- Total turns
  // SL[float ] -- spool length
  // TL[int ] -- Turns per layer
  // NL[int ] -- Number of whole layers
  // LL[int] -- Turns last layer

    char identifier[3];

    Serial.readBytes(identifier, 2);

    if(strcmp(identifier, "WS")== 0) {
        // Wire size
        wire_size = Serial.parseFloat();
    }

    else if(strcmp(identifier, "TT")== 0) {
        // Total turns
        turns = Serial.parseInt();
    }

    else if(strcmp(identifier, "SL")== 0) {
        // Spool length
        spool_length = Serial.parseFloat();
    }

    else if(strcmp(identifier, "TL")== 0) {
        // Turns per layer
        turns_per_layer = Serial.parseInt();
    }

    else if(strcmp(identifier, "NL")== 0) {
        // Number of layers
        num_layers = Serial.parseInt();
    }

    else if(strcmp(identifier, "LL")== 0) {
        // turns last layer
        last_layer_turns = Serial.parseInt();

    }


    else {

      Serial.print("Unable to decypher ");
      Serial.print(identifier);
      Serial.print("\n");

    }


    current_mode = parameterMode;

  }
}







#ifdef SERIAL_DEBUG
  if ((current_mode == parameterMode) && print_data) {
    Serial.print("Wire size: ");
    printDouble(wire_size, 2);
    Serial.print("\n");

    Serial.print("turns: ");
    printDouble(turns, 2);
    Serial.print("\n");
    Serial.print("spool length: ");
    printDouble(spool_length, 2);
    Serial.print("\n");
    print_data = false;
  }
#endif

  if (current_mode == runningMode) {

    direction = 0;

    // turn on the enables for the motors
    digitalWrite(SPOOL_ENABLE, LOW);
    digitalWrite(SHUTTLE_ENABLE, LOW);
    motor_status = 0x03; // both motors on

    running = 1;

    int layer_count;
    this_layer = 0;

    // Do all the whole layers
    for (layer_count = 0; layer_count < num_layers; layer_count++) {

      this_layer++;

#ifdef SERIAL_DEBUG
      Serial.print("In multiple layers\n");
      Serial.print("this layer = ");
      Serial.print(this_layer);
      Serial.print("\n");
#endif

      do_a_layer(turns_per_layer);
      direction = direction ^ 1;
    }

    // do the last layer
    if (last_layer_turns > 0) {
      this_layer++;
#ifdef SERIAL_DEBUG
      Serial.print("In last layer\n");
      Serial.print("this layer = ");
      Serial.print(this_layer);
      Serial.print("\n");
#endif
      do_a_layer(last_layer_turns);
    }

    current_mode = idleMode;
    running = 0;
  }

  if (motor_status & 0x01) {
    digitalWrite(SPOOL_ENABLE, LOW);
  } else {
    digitalWrite(SPOOL_ENABLE, HIGH);
  }
  if (motor_status & 0x02) {
    digitalWrite(SHUTTLE_ENABLE, LOW);
  } else {
    digitalWrite(SHUTTLE_ENABLE, HIGH);
  }
}

void do_a_layer(int num_turns) {

  long spoolSteps;
  long shuttleSteps;

  current_layer_turns = 0.0;

  running = 0;
  spool.setCurrentPosition(0);
  shuttle.setCurrentPosition(0);

  spoolSpeed = calculateSpoolSpeed();
  spoolSteps = calculateSpoolSteps(num_turns);

  shuttleSpeed = calculateShuttleSpeed(spoolSpeed, wire_size);
  shuttleSteps = calculateShuttleSteps(wire_size, num_turns);

#ifdef SERIAL_DEBUG
  Serial.print("spool steps = ");
  Serial.print(spoolSteps, DEC);
  Serial.print("\n");
  Serial.print("shuttle steps = ");
  Serial.print(shuttleSteps, DEC);
  Serial.print("\n");
  Serial.print("Number of turns for this layer\n");
  Serial.print(num_turns);
  Serial.print("\n");
#endif

  if (direction) {
    // If we change direction reset the position to 0
    // shuttle.moveTo(0);
    shuttle.moveTo(shuttleSteps);
  } else {
    // Set the shuttle to move to the calculated position
    shuttle.moveTo(-1 * shuttleSteps);
  }

  spool.moveTo(spoolSteps);

  spool.setSpeed(spoolSpeed);
  shuttle.setSpeed(shuttleSpeed);
  running = 1;

  unsigned long start = millis();

  do {
    // every 150 mS
    if ((millis() - start) >= 250) {
      spoolSpeed = calculateSpoolSpeed();
      shuttleSpeed = calculateShuttleSpeed(spoolSpeed, wire_size);

      spool.setSpeed(spoolSpeed);
      shuttle.setSpeed(shuttleSpeed);
      updateTps();
      updateTurns();
      start = millis();
    }

  } while ((spool.distanceToGo() != 0) || (shuttle.distanceToGo() != 0));
  updateTps();
  updateTurns();
}

void updateTps() { current_speed = spoolSpeed / 200.0; }

void updateTurns() {
  long pos = spool.currentPosition();

  if (pos < 0)
    pos = pos * -1;

  float temp_turns = ((float)pos) / 200.0;

  float delta_turns = temp_turns - current_layer_turns;

  current_layer_turns = temp_turns;

  current_turns += delta_turns;
  /*
  #ifdef SERIAL_DEBUG
    Serial.print("temp_turns = ");
    printDouble(temp_turns, 1);
    Serial.print("\n");
    Serial.print("delta_turns = ");
    printDouble(delta_turns, 1);
    Serial.print("\n");
    Serial.print("current_turns = ");
    printDouble(current_turns.value, 1);
    Serial.print("\n");
  #endif
  */
}

void requestEvent() {

  if (current_mode == testMode) {
    Wire.write(i2c_test_data, 3);
    current_mode = idleMode;
  } else if (request_mode == getStatusMode) {

      } else if (request_mode == getMotorStatusMode) {
    Wire.write(motor_status);
  } else if (request_mode == getVersion) {
    Wire.write(git_sha, sizeof(git_sha));
  } else if (request_mode == getDate) {
    Wire.write(build_date, sizeof(build_date));
  }
}

// function that executes whenever data is received from master
// this function is registered as an event, see setup()
void receiveEvent(int howMany) {

  uint8_t command = Wire.read();
  if (command == 0x00) // I2C test
  {
    // Serial.print("I2C test received \n");
    current_mode = testMode;
    int i = 0;
    while (Wire.available()) {
      i2c_test_data[i++] = Wire.read();
    }
  } else if ((command == 0x02) && (current_mode == parameterMode)) // start
  {
    current_mode = runningMode;
  } else if (command == 0x03) // status
  {
    request_mode = getStatusMode;
  } else if (command == 0x04) // motor status
  {
    request_mode = getMotorStatusMode;
  } else if (command == 0x05) // set motor status
  {
    motor_status = Wire.read();
  } else if (command == 0x06) // pause state
  {
    current_mode = pauseMode;
    running = 0;
  } else if (command == 0x07) // continue state
  {
    current_mode = runningMode;
    running = 1;
  } else if (command == 0x08) { // get version
    request_mode = getVersion;
  } else if (command == 0x09) { // get DATE
    request_mode = getDate;
  }
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

// CRC-8 - based on the CRC8 formulas by Dallas/Maxim
// http://www.leonardomiliani.com/en/2013/un-semplice-crc8-per-arduino/
// code released under the therms of the GNU GPL 3.0 license
uint8_t CRC8(const uint8_t *data, uint8_t len) {
  uint8_t crc = 0x00;
  while (len--) {
    uint8_t extract = *data++;
    for (uint8_t tempI = 8; tempI; tempI--) {
      uint8_t sum = (crc ^ extract) & 0x01;
      crc >>= 1;
      if (sum) {
        crc ^= 0x8C;
      }
      extract >>= 1;
    }
  }
  return crc;
}

float calculateSpoolSpeed() {
  static int analog_value;
  // Now read the pot (from 0 to 1023)
  analog_value = analogRead(SPEED_PIN);
  //  And scale the pot's value from min to max speeds
  float spoolSpeed =
      ((analog_value / 1023.0) * (MAX_SPEED - MIN_SPEED)) + MIN_SPEED;
  return spoolSpeed;
}

float calculateShuttleSpeed(float spoolSpeed, float wireSize) {
  float shuttleSpeed = wireSize * spoolSpeed;

  return shuttleSpeed;
}

long calculateShuttleSteps(float wireSize, int numberTurns) {

  long shuttleSteps = (long)(wireSize * 200 * numberTurns);
  return shuttleSteps;
}

long calculateSpoolSteps(int numberTurns) {

  long spoolSteps = (long)(200 * numberTurns);
  return spoolSteps;
}

/*****************************************************************************
 ** SetUpInterrupts
 ** ===============
 * Set up interrupt routine to service stepper motor run() function.
 */
bool SetUpInterrupts(const int usecs) {
  // initialize Timer1
  cli();      // disable global interrupts
  TCCR1A = 0; // set entire TCCR1A register to 0
  TCCR1B = 0; // same for TCCR1B

  // set compare match register to desired timer count (1ms):
  // ATmega328 with a 16MHz clock, clk/8
  // (# timer counts + 1) = (target time) / (timer resolution)
  //                      =     .0001s      /   6.25e-8 s  * 8
  //                      =   200
  const float targetSecs = ((float)usecs) / 1e6;
  const float timerSteps = 6.25e-8; //    1/16MHz
  int count = 0;
  int prescale = 1; // valid values: 1, 8, 64, 256, 1024
  do {
    count = targetSecs / (timerSteps * prescale);
    if (count < 65535) // Timer 1 is 16-bits wide
      break;
    prescale *= 8;
  } while (prescale <= 1024);
  if (prescale > 1024) // time too long
    return false;
  if (prescale == 1 && count < 100) // time too short
    return false;

  OCR1A = count; // Eg, 200 = 0.1ms - I found 1ms gives rough acceleration
  // turn on CTC mode (Clear Timer on Compare Match):
  TCCR1B |= (1 << WGM12);
  // Set CS10 and CS12 bits for 1024 prescaler:
  // CS12   CS11   CS10
  //   0      0      0   no clock source, Timer/counter stopped
  //   0      0      1   clk/1  no prescaling
  //   0      1      0   clk/8
  //   0      1      1   clk/64
  //   1      0      0   clk/256
  //   1      0      1   clk/1024
  //   1      1      0   external clock on T1 pin, falling edge
  //   1      1      1   external clock on T1 pin, rising edge
  switch (prescale) {
  case 1:
    TCCR1B |= (1 << CS10); // 0 0 1
    break;
  case 8:
    TCCR1B |= (1 << CS11); // 0 1 0
    break;
  case 64:
    TCCR1B |= (1 << CS11) & (1 << CS10); // 0 1 1
    break;
  case 256:
    TCCR1B |= (1 << CS12); // 1 0 0
    break;
  case 1024:
    TCCR1B |= (1 << CS12) & (1 << CS10); // 1 0 1
    break;
  }
  // enable timer compare interrupt:
  TIMSK1 |= (1 << OCIE1A);
  // enable global interrupts:
  sei();

  return true;
}

void printDouble(double val, uint8_t precision) {
  // prints val with number of decimal places determine by precision
  // precision is a number from 0 to 6 indicating the desired decimial places
  // example: printDouble( 3.1415, 2); // prints 3.14 (two decimal places)

  Serial.print(int(val)); // prints the int part
  if (precision > 0) {
    Serial.print("."); // print the decimal point
    unsigned long frac;
    unsigned long mult = 1;
    uint8_t padding = precision - 1;
    while (precision--)
      mult *= 10;

    if (val >= 0)
      frac = (val - int(val)) * mult;
    else
      frac = (int(val) - val) * mult;
    unsigned long frac1 = frac;
    while (frac1 /= 10)
      padding--;
    while (padding--)
      Serial.print("0");
    Serial.print(frac, DEC);
  }
}
