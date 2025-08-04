#include <stdint.h>
#include "led.h"
#include "delay.h"
#include "clock.h"
#include "pwm.h"
#include "Usart.h"
// #include "low_power.h"
#include "FreeRTOS.h"
#include "task.h"
#include "ADC.h"
#include "queue.h"
#include "event_groups.h"
#include "semphr.h"

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
float common_memory;
QueueHandle_t tempQueue;
EventGroupHandle_t tempEvent;
SemaphoreHandle_t uartLock;

void func_3(void* param)
{
	UART1_Init();
	for(;;)
	{
        //wait until tempEvent is set
         xEventGroupSync(tempEvent, 0, 1, portMAX_DELAY); // Wait for the event to be set before proceeding
        // float temp = common_memory; // Read the temperature from common memory
        float temp = 0; // Read the temperature from common memory
        xSemaphoreTake(uartLock, 0xffffffff);
        usart_printf("\033[0;31m[task 3]\033[0m: temperature: [");
        while (uxQueueMessagesWaiting(tempQueue) > 0)
        {
            xQueueReceive(tempQueue, &temp, 0); // Receive temperature data from the queue
            usart_printf("%0.2f, ", temp);
        }
        usart_printf("\b\b]\r\n");
        xSemaphoreGive(uartLock);
        // vTaskDelay(1000);        
	}
}

void func_4(void* param)
{
	adc_init();
    int measureCnt = 0;
	for(;;)
	{
       float temp = adc_get_temp_ss();
        // common_memory = temp; // Store the temperature in common memory
        xQueueGenericSend(tempQueue, &temp, 10000, queueSEND_TO_BACK);
        vTaskDelay(100);   
        if(++measureCnt > 10)
        {
            measureCnt = 0;
            //set event
            xEventGroupSetBits(tempEvent, 1); // Set an event bit to indicate a new measurement
        }
	}
}


void func_5(void* param)
{
	
	while(1)
	{
		xSemaphoreTake(uartLock, 0xffffffff);
		usart_printf("\033[0;32m[task 5]\033[0m: hello world\r\n");
		xSemaphoreGive(uartLock);
		vTaskDelay(500);
	}
}

int main(){
    setup();
    int cnt = 0;
    // enable_iwdg();
    TaskHandle_t task_1 = NULL;
	TaskHandle_t task_2 = NULL;
    TaskHandle_t task_3 = NULL;
    TaskHandle_t task_4 = NULL;
    TaskHandle_t task_5 = NULL;
    xTaskCreate(func_1, "task 1", 512, NULL, 0, &task_1);
	xTaskCreate(func_2, "task 2", 512, NULL, 0, &task_2);
    xTaskCreate(func_3, "task 3", 512, NULL, 0, &task_3);
    xTaskCreate(func_4, "task 4", 512, NULL, 0, &task_4);
    xTaskCreate(func_5, "task 5", 512, NULL, 0, &task_5);
    tempQueue = xQueueCreate(20,sizeof(float)); // Create a queue to hold temperature data
    tempEvent = xEventGroupCreate(); // Create an event group for synchronization
    uartLock = xSemaphoreCreateMutex();
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