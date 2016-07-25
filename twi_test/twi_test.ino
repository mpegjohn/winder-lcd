#include <Arduino.h>
#include <Wire.h>

uint8_t *get_float_from_array(uint8_t *out_array, uint8_t *current_index);
uint8_t *doubleToData(uint8_t *dataArray, uint8_t *pparameterData);

typedef union floatbytes {
  float value;
  uint8_t bytes[4];
} Floatbyte_t;

Floatbyte_t wire_size;
Floatbyte_t turns;
Floatbyte_t spool_length;
Floatbyte_t tuns_per_layer;
Floatbyte_t whole_layers;
Floatbyte_t last_layer_turns;

Floatbyte_t current_layer;
Floatbyte_t current_turns;
Floatbyte_t current_layer_turns;
Floatbyte_t current_speed;

uint8_t direction = 0;
uint8_t running = 0;

enum modes { idleMode, testMode, parameterMode, runningMode, getStatusMode };

modes current_mode = idleMode;
modes request_mode = idleMode;

uint8_t i2c_test_data[3];

void setup() {
  wire_size.value = 0.0;
  turns.value = 0.0;
  spool_length.value = 0.0;

  Serial.begin(9600); // start serial for output
  Wire.begin(8);
  Wire.onRequest(requestEvent); // register a request
  Wire.onReceive(receiveEvent); // register event
}

void loop() {

  if (current_mode == parameterMode) {
    Serial.print("Wire size: ");
    printDouble(wire_size.value, 2);
    Serial.print("\n");

    Serial.print("turns: ");
    printDouble(turns.value, 2);
    Serial.print("\n");
    Serial.print("spool length: ");
    printDouble(spool_length.value, 2);
    Serial.print("\n");
    running = 1;
    current_layer.value = 1.0;
  }
  if (current_mode == runningMode) {

    current_turns.value = current_turns.value + 1.0;
    current_layer_turns.value = current_layer_turns.value + 1.0;
    current_speed.value = 1.2;
    if (current_turns.value == turns.value) {
      current_mode = idleMode;
      running = 0;
    }
  }
  delay(100);
}

void requestEvent() {
  if (current_mode == testMode) {
    Wire.write(i2c_test_data, 3);
    current_mode = idleMode;
  } else if (request_mode == getStatusMode) {

    //[4 bytes layer]
    //[4 bytes turns]
    //[4 bytes layer turns]
    //[4 bytes speed]
    //[1 byte] direction 1 = L to R, 0 = R to L
    //[1 byte] running 1 = running, 0 - stopped

    uint8_t status_data[18];

    uint8_t *status_data_pointer;
    status_data_pointer = status_data;

    status_data_pointer =
        doubleToData(current_layer.bytes, status_data_pointer);
    status_data_pointer =
        doubleToData(current_turns.bytes, status_data_pointer);
    status_data_pointer =
        doubleToData(current_layer_turns.bytes, status_data_pointer);
    status_data_pointer =
        doubleToData(current_speed.bytes, status_data_pointer);
    status_data[16] = direction;
    status_data[17] = running;

    Wire.write(status_data, 18);
    request_mode = idleMode;
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
  } else if (command == 0x01) // Job perameters
  {
    // [0x1] -- Mode 1 comand job paremeters
    // [4 bytes] -- wire size
    // [4 bytes] -- Total turns
    // [4 bytes ] -- spool length
    // [4 bytes ] -- Turns per layer
    // [4 bytes ] -- Number of whole layers
    // [4 bytes ] -- Turns last layer

    Serial.print("New job perameters received \n");

    uint8_t parameters[24];

    uint8_t *parameters_pointer;

    parameters_pointer = parameters;

    int i = 0;
    while (Wire.available()) {
      parameters[i++] = Wire.read();
    }

    parameters_pointer =
        get_float_from_array(wire_size.bytes, parameters_pointer);
    parameters_pointer = get_float_from_array(turns.bytes, parameters_pointer);
    parameters_pointer =
        get_float_from_array(spool_length.bytes, parameters_pointer);
    parameters_pointer =
        get_float_from_array(tuns_per_layer.bytes, parameters_pointer);
    parameters_pointer =
        get_float_from_array(whole_layers.bytes, parameters_pointer);
    parameters_pointer =
        get_float_from_array(last_layer_turns.bytes, parameters_pointer);

    current_mode = parameterMode;
  } else if ((command == 0x02) && (current_mode == parameterMode)) // start
  {
    current_mode = runningMode;
  } else if (command == 0x03) // status
  {
    request_mode = getStatusMode;
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
