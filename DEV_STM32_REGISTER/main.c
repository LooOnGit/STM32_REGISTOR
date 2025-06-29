#include <stdint.h>
#include "led.h"
#include "delay.h"
#include "clock.h"
#include "pwm.h"
#include "SPI.h"
#include "i2c.h"

void SystemInit ();
void setup()
{
    led_init();
    delay_init();
    clock_init();
    pwm_init();
    SPI_init();
    uint8_t id = LSM303_Read_ID();
    LSM303_Init();
    I2C1_LSM303_init();
    I2C1_LSM303_ReadID();
}

int main(){
    setup();
    int cnt = 0;
    while(1){
        led_control(ORANGE, ON);
        // delay(1000);
        // led_control(ORANGE, OFF);
        // delay(1000);    
        if(++cnt > 100)
            cnt = 0;
        pwm_pulse_crtl(cnt);
        delay(10);
    }
    return 0;
}

void SystemInit (){
    
}