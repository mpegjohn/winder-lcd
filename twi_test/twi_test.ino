#include <Arduino.h>
#include <Wire.h>

typedef union floatbytes {
  float value;
  byte bytes[4];
}
Floatbyte;

Floatbyte wire_size;
Floatbyte turns;
Floatbyte spool_length;

void setup()
{
  wire_size.value = 0.0;
  turns.value = 0.0;
  spool_length.value = 0.0;

  Serial.begin(9600);           // start serial for output
  Wire.begin(8);
  Wire.onRequest(requestEvent); // register a request
  Wire.onReceive(receiveEvent); // register event
}

void loop()
{
  Serial.print("Wire size: ");
  printDouble(wire_size.value,2);
  Serial.print("\n");

  Serial.print("turns: ");
  printDouble(turns.value,2);
  Serial.print("\n");
  Serial.print("spool length: ");
  printDouble(spool_length.value,2);
  Serial.print("\n");

  delay(1000);
}

byte i2c_test_data [3];

void requestEvent()
{
  //Wire.write(i2c_test_data, 3);

  Wire.write(i2c_test_data,3);


}

// function that executes whenever data is received from master
// this function is registered as an event, see setup()
void receiveEvent(int howMany)
{
  byte command = Wire.read();
  //  Serial.print("received ");
  //  Serial.print(command, HEX);
  //  Serial.print("\n");
  if(command == 0x01) // I2C test
  {
    //    Serial.print("I2C test received \n");
    int i = 0;
    while(Wire.available())
    {
      i2c_test_data[i++] = Wire.read();
    }
  }
  else if(command == 0x00) // Job perameters
  {
    //0x01            => Command job parameters
    //[4 bytes]      => wire size
    //[4 bytes]      => number of turns
    //[4 bytes]      => spool length
    //[checksum]
    Serial.print("New job perameters received \n");

    byte parameters[12];

    int i = 0;
    while(Wire.available())
    {
      parameters[i++] = Wire.read();
    }

    byte * offset_base;
    offset_base = parameters;

    for(i = 0; i<4; i++)
    {
      wire_size.bytes[i] = *offset_base++;
    }

    for(i = 0; i<4; i++)
    {
      turns.bytes[i] = *offset_base++;
    }

    for(i = 0; i<4; i++)
    {
      spool_length.bytes[i] = *offset_base++;
    }


  }
}


//CRC-8 - based on the CRC8 formulas by Dallas/Maxim
//http://www.leonardomiliani.com/en/2013/un-semplice-crc8-per-arduino/
//code released under the therms of the GNU GPL 3.0 license
byte CRC8(const byte *data, byte len)
{
  byte crc = 0x00;
  while (len--)
  {
    byte extract = *data++;
    for (byte tempI = 8; tempI; tempI--)
    {
      byte sum = (crc ^ extract) & 0x01;
      crc >>= 1;
      if (sum)
      {
        crc ^= 0x8C;
      }
      extract >>= 1;
    }
  }
  return crc;
}


void printDouble( double val, byte precision){
  // prints val with number of decimal places determine by precision
  // precision is a number from 0 to 6 indicating the desired decimial places
  // example: printDouble( 3.1415, 2); // prints 3.14 (two decimal places)

  Serial.print (int(val));  //prints the int part
  if( precision > 0) {
    Serial.print("."); // print the decimal point
    unsigned long frac;
    unsigned long mult = 1;
    byte padding = precision -1;
    while(precision--)
      mult *=10;

    if(val >= 0)
      frac = (val - int(val)) * mult;
    else
      frac = (int(val)- val ) * mult;
    unsigned long frac1 = frac;
    while( frac1 /= 10 )
      padding--;
    while(  padding--)
      Serial.print("0");
    Serial.print(frac,DEC) ;
  }
}
