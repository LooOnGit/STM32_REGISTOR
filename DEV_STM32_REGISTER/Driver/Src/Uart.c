/*
 * uart.c
 *
 *  Created on: Sep 15, 2023
 *      Author: maitr
 */
#include "clock.h"
#include <stdarg.h>
#include <stdio.h>
#include <string.h>
#include <stdint.h>
void UART1_Init()
{
	clock_enable_APB2(USART1_peripheral);
	clock_enable_AHB1(GPIOB_peripheral);

	uint32_t* MODER = (uint32_t*)(0x40020400);
	*MODER |= (0b10 << 12) | (0b10 << 14);		//set PB6 (U1Tx), PB7(U1Rx)

	uint32_t* AFRL = (uint32_t*)(0x40020420);
	*AFRL  |= (0b0111 << 24) | (0b0111 << 28);

	uint32_t* BRR = (uint32_t*)(0x40011008);
	*BRR = (104<<4) | 3;

	uint32_t* CR1 = (uint32_t*)(0x4001100c);

//	uint32_t* CR3 = (uint32_t*)(0x40011014);
//	*CR3 |= (1<<6);

	*CR1 |= (1<< 3)|(1<<2)|(1<<13);
}

static void UART1_Send(char data)
{
	uint32_t* SR = (uint32_t*)(0x40011000);
	uint32_t* DR = (uint32_t*)(0x40011004);
	while(((*SR >> 7) & 1) != 1);
	*DR	= data;
	while(((*SR >> 6) & 1) != 1);
}

void printlog(char* format, ...)
{
	char buf[1024] = {0};
	char buf_len = 0;
	va_list ap;
    va_start(ap, format);
	vsprintf(buf, format, ap);
	va_end(ap);
	buf_len = strlen(buf);
	for(int i = 0; i < buf_len; i++)
	{
		UART1_Send(buf[i]);
	}
	vTaskDelay(100);

}