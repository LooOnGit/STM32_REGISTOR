/*
 * led.c
 *
 *  Created on: Aug 7, 2023
 *      Author: maitr
 */
#include "led.h"
#include "clock.h"
#include <stdint.h>
void leds_init()
{
	//set PD12, PD13, PD14, PD15 in output push-pull
	clock_enable_AHB1(GPIOD_peripheral);
	uint32_t* GPIOD_MODER = (uint32_t*)(GPIOD_BASE_ARR + 0x00);
	*GPIOD_MODER |= (0b01 << 24); //set PD12 in OUTPUT
	*GPIOD_MODER |= (0b01 << 26); //set PD13 in OUTPUT
	*GPIOD_MODER |= (0b01 << 28); //set PD14 in OUTPUT
	*GPIOD_MODER |= (0b01 << 30); //set PD15 in OUTPUT

//	uint32_t* GPIOD_OTYPER = (uint32_t*)(GPIOD_BASE_ARR + 0x04);
	*GPIOD_MODER &= ~(0b1111 << 12); //set PD12, PD13, PD14, PD15 in push-pull
}


void led_control(led_num_t led, led_state_t state)
{
	uint32_t* GPIOD_ODR = (uint32_t*)(GPIOD_BASE_ARR + 0x14);
	if(state == LED_ON)
		*GPIOD_ODR |= 1<<led;
	else
		*GPIOD_ODR &= ~(1<<led);
}