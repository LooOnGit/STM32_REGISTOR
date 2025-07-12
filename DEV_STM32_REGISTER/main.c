#include <stdint.h>
#include "led.h"
#include "delay.h"
#include "clock.h"
#include "pwm.h"
#include "low_power.h"
#include "FreeRTOS.h"
#include "task.h"

void SystemInit ();
void setup()
{
    led_init();
    delay_init();
    // clock_init();
    // pwm_init();
}

int main(){
    setup();
    int cnt = 0;
    enable_iwdg();

    while(1){
        // led_control(ORANGE, ON);
        // // delay(1000);
        // // led_control(ORANGE, OFF);
        // // delay(1000);    
        // if(++cnt > 100)
        //     cnt = 0;
        // pwm_pulse_crtl(cnt);
        // delay(10);

        delay(5000);
        goto_low_power_mode();
    }
    return 0;
}

void SystemInit (){
    
}