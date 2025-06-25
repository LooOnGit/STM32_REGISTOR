#include <stdint.h>
#include "E:\Tech\Embedded\STM32_REGISTOR\DEV_STM32_REGISTER\Driver\Inc\led.h"
#include "E:\Tech\Embedded\STM32_REGISTOR\DEV_STM32_REGISTER\Driver\Inc\delay.h"
#include "E:\Tech\Embedded\STM32_REGISTOR\DEV_STM32_REGISTER\Driver\Inc\clock.h"

void SystemInit ();
void setup()
{
    led_init();
    delay_init();
    clock_init();
}

int main(){
    setup();
    while(1){
        led_control(GREEN, ON);
        delay(1000);
        led_control(GREEN, OFF);
        delay(1000);
    }
    return 0;
}

void SystemInit (){
    
}