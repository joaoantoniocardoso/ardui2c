/* ARDUINO UNO I2C PINS:
 *        SDA: A4
 *        SCL: A5
 * ARDUINO UNO ADC PINS:
 *        ADC0: A0
 *        ADC1: A1
 */ 
#include <Wire.h>

#define ARDUI2C_DEVICE_ADDR  0x11
#define SERIAL_ON
#define FAKE_ANALOG_ON

// FAKE_ANALOG!!!
#ifdef FAKE_ANALOG_ON
	uint8_t i = 0;
	#define fake_analogRead()	(324+i++)
#endif

struct{
	uint16_t voltage0 = 0;
	uint16_t voltage1 = 0;
	uint32_t scale = 1;
}__attribute__((__packed__))data;

void receiveData();

void setup()
{
	Wire.begin(ARDUI2C_DEVICE_ADDR);
	Wire.onRequest(receiveData);
#ifdef SERIAL_ON
	Serial.begin(9600);
	Serial.print("Started.\n");
#endif
#ifndef FAKE_ANALOG_ON
	analogRead(A0);
	analogRead(A1);
#endif
}

void loop()
{
#ifdef FAKE_ANALOG_ON
	data.voltage0 = fake_analogRead();
	data.voltage1 = fake_analogRead();
#else
	data.voltage0 = analogRead(A0);
	data.voltage1 = analogRead(A1);
#endif
}

// callback for received data
void receiveData()
{
	Wire.write((unsigned char *)&data, sizeof(data));
#ifdef SERIAL_ON
	Serial.println("DATA REQUEST");
#endif
}
