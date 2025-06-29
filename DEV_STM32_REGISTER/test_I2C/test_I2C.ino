#include <Wire.h>

uint8_t active_command = 0xff, led_status = 0;
char name_msg[15] = "Hello ThoNV12\n";

#define SLAVE_ADDR 0x68

void setup()
{
  //start the i2c bus as slave on address 9
  Wire.begin(SLAVE_ADDR);

  //Attach a function to trigger when something is received
  Wire.onReceive(receiveEvent);
  Wire.onRequest(requestEvent);
}

void receiveEvent(int bytes)
{
  active_command = Wire.read(); //read one character from the I2C
}

void requestEvent()
{
  if(active_command == 0x01)
  {
    uint8_t data = 15;
    Wire.write(&data, 1);
  }
  
  if(active_command == 0x02)
  {
    Wire.write((const uint8_t*)name_msg, 15);
  }
}

void loop()
{
  
}