#include <Arduino.h>


typedef union floatbytes {
	float value;
	byte bytes[4];
} Floatbyte;

// 0x00 Command TWI test
typedef struct twi_test {
	byte byte0;   // 0xAA
	byte byte1;   // 0x55
	byte byte2;   // 0x00
	byte byte3;   // oxFF
}Twitest;

// 0x01 Command job parameters
struct new_job {
	Floatbyte wire_size;
	Floatbyte num_turns;
	Floatbyte spool_len;
	byte checksum;
};

// 0x04 Manual Motor command
struct motor_control {
	byte motor_select; // 0x00 = shuttle, 0x01 = spool
	byte state; //0x00 = off, 0x01 = on
};

typedef struct twi_data {
	byte bytes[10];
	int length;
}Twidata;






