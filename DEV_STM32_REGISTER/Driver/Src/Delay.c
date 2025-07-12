/*
 * delay.c
 *
 *  Created on: Aug 7, 2023
 *      Author: maitr
 */
#include "delay.h"
#include "clock.h"
#define TIMER1	1
#define SYS_TICK 2

#define DELAY_SRC TIMER1

#if DELAY_SRC != TIMER1 && DELAY_SRC!= SYS_TICK
#error DELAY_SRC must be TIMER1 or SYS_TICK
#endif

uint32_t tim1_cnt = 0;
#if (DELAY_SRC == TIMER1)
void TIM1_UP_TIM10_IRQHandler()
{
	uint16_t* SR = (uint16_t*)(0x40010010);
	*SR &= ~(1<<0);				  //clean UIF flag
	tim1_cnt++;
}
#else
void SysTick_Handler()
{
	tim1_cnt++;
}
#endif
void delay_ms(uint32_t msec)
{
	uint32_t current_cnt = tim1_cnt;
	while((uint32_t)(tim1_cnt - current_cnt) < msec);
}

void delay_init()
{
#if (DELAY_SRC == TIMER1)
	//set 1 msec for timer
	// rcc --> 16Mhz -> psc (16) --> 1000000Hz | 1 cnt -> 1uS
	// ARR = 1000

	clock_enable_APB2(TIM1_peripheral);
	uint16_t* CR1 = (uint16_t*)(0x40010000);
	uint16_t* DIER = (uint16_t*)(0x4001000c);

	uint16_t* ARR = (uint16_t*)(0x4001002c);
	uint16_t* PSC = (uint16_t*)(0x40010028);
	*ARR = 1000;
	*PSC = 16 - 1;

	*DIER |= 1<< 0; //enable interrupt
	*CR1 |= 1 << 0; //enable counter

	uint32_t* ISER0 = (uint32_t*)(0xe000e100);
	*ISER0 |= (1<<25);
#else
	uint32_t* CSR = (uint32_t*)(0xE000E010);
	uint32_t* RVR = (uint32_t*)(0xE000E014);
	*RVR = 16000;
	*CSR |= (1<<0) | (1<<1) | (1<<2);
#endif
}


