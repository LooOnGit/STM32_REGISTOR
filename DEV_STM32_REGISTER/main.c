#include <stdint.h>
#include "led.h"
#include "delay.h"
#include "clock.h"

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
        led_control(ORANGE, ON);
        delay(1000);
        led_control(ORANGE, OFF);
        delay(1000);
    }
    return 0;
}

void SystemInit (){
    
}