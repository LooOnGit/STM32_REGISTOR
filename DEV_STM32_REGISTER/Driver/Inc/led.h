/*
 * led.h
 *
 *  Created on: Aug 7, 2023
 *      Author: maitr
 */

#ifndef INC_LED_H_
#define INC_LED_H_

#define GPIOA_BASE_ARR 0x40020000
#define GPIOD_BASE_ARR 0x40020C00


void leds_init();
typedef enum
{
	GREEN_LED = 12,
	 ORANGE_LED,
	RED_LED,
	BLUE_LED
}led_num_t;
typedef enum
{
	LED_OFF,
	LED_ON
}led_state_t;
void led_control(led_num_t led, led_state_t state);

#endif /* INC_LED_H_ */