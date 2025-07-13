#include <stdint.h>
#include "led.h"
#include "delay.h"
#include "clock.h"
#include "pwm.h"
// #include "low_power.h"
#include "FreeRTOS.h"
#include "task.h"

void vApplicationMallocFailedHook()
{

}

void vApplicationTickHook()
{

}

void vApplicationStackOverflowHook( TaskHandle_t xTask,
                                        char * pcTaskName )
{
	
}

void vApplicationIdleHook()
{

}

void SystemInit ();
void setup()
{
    led_init();
    delay_init();
    // clock_init();
    // pwm_init();
}

void func_1(void* param)
{
	while(1)
	{
		led_control(BLUE, ON);
		vTaskDelay(1000);
		led_control(BLUE, OFF);
		vTaskDelay(1000);
	}
}

void func_2(void* param)
{
	while(1)
	{
		led_control(RED, ON);
		vTaskDelay(1500);
		led_control(RED, OFF);
		vTaskDelay(1000);
	}
}

int main(){
    setup();
    int cnt = 0;
    // enable_iwdg();
    TaskHandle_t task_1 = NULL;
	TaskHandle_t task_2 = NULL;
    xTaskCreate(func_1, "task 1", 512, NULL, 0, &task_1);
	xTaskCreate(func_2, "task 2", 512, NULL, 0, &task_2);

    vTaskStartScheduler();
    while(1){
        // led_control(ORANGE, ON);
        // delay(1000);
        // led_control(ORANGE, OFF);
        // delay(1000);    
        // if(++cnt > 100)
        //     cnt = 0;
        // pwm_pulse_crtl(cnt);
        // delay(10);

        // delay(5000);
        // goto_low_power_mode();
    }
    return 0;
}

void SystemInit (){
    
}