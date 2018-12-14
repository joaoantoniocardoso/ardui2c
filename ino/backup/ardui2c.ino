/* ARDUINO UNO I2C PINS:
 *        SDA: A4
 *        SCL: A5
 * ARDUINO UNO ADC PINS:
 *        ADC0: A0
 *        ADC1: A1
 */ 
#include <Wire.h>

#define ARDUI2C_DEVICE_ADDR  0x11
#define CMD_DEVICE_ID        0x0A
#define CMD_GET_SCALE        0x0B
#define CMD_SINGLE_SHOT_A0   0x0C
#define CMD_SINGLE_SHOT_A1   0x0D

uint16_t raw_a0 = 0;
uint16_t raw_a1 = 0;
uint16_t scale = 1;


void receiveData(int byteCount);

void setup()
{
	Wire.begin(ARDUI2C_DEVICE_ADDR);
	Wire.onReceive(receiveData);
	Serial.begin(9600);
	Serial.print("Started.\n");
	analogRead(A0);
	analogRead(A1);
}

void loop()
{
	//raw_a0 = analogRead(A0);
	//raw_a1 = analogRead(A1);
	raw_a0 = 567;
	raw_a1 = 1011;
}

// callback for received data
void receiveData(int byteCount)
{
	byte cmd;
	Serial.print("count: ");
	Serial.print(byteCount);
	Serial.print(" ");
	while(Wire.available()){
		cmd = Wire.read();
		switch(cmd){
		case CMD_DEVICE_ID:
			Wire.write(ARDUI2C_DEVICE_ADDR);
			Serial.print("CMD_DEVICE_ID");
			break;
		case CMD_SINGLE_SHOT_A0:
			Wire.write(567);
			//Wire.write((char *)&raw_a0, 2);
			Serial.print("CMD_SINGLE_SHOT_A0");
			break;
		case CMD_SINGLE_SHOT_A1:
			Wire.write(666);
			//Wire.write((char *)&raw_a1, 2);
			Serial.print("CMD_SINGLE_SHOT_A1");
			break;
		case CMD_GET_SCALE:
			Wire.write(121);
			//Wire.write((char *)&scale, 2);
			Serial.print("CMD_GET_SCALE_A0");
			break;
		default:
			Serial.print("CMD_UNKOWN");
			break;
		}
	Serial.println(" received");
	}
}

int main(void)
{
	for(;;) loop();
	return 0;
}

