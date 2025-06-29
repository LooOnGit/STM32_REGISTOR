#include "i2c.h"
#include "clock.h"
#include <stdint.h>
//SCL1: PB8
//SDA1: PB9

// void I2C_Init(void)
// {
//     clock_enable_AHB1(GPIOB_peripheral);
//     uint32_t* MODER = (uint32_t*)0x40020400;
//     uint32_t* AFR = (uint32_t*)0x40020424;

//     *MODER &= ~(0b11 << 16); //clean bit
//     *MODER &= ~(0b11 << 18); //clean bit
//     *MODER |= (0b10 << 16); //set bit
//     *MODER |= (0b10 << 18); //set bit

//     *AFR &= ~(0b1111 << 0); //clean bit
//     *AFR |= (0b0100 << 0); //set bit

//     *AFR &= ~(0b1111 << 4); //clean bit
//     *AFR |= (0b0100 << 4); //set bit

//     clock_enable_APB1(I2C1_peripheral);

//     uint32_t* CR1 = (uint32_t*)0x40005400;
//     uint32_t* CR2 = (uint32_t*)0x40005404
// }

void I2C1_LSM303_init()
{
	clock_enable_APB1(I2C1_peripheral);
	clock_enable_AHB1(GPIOB_peripheral);

	uint32_t* MODER = (uint32_t*)(0x40020400);
	*MODER |= (0b10 << 12) | (0b10 << 18);

	uint32_t* PUPDR = (uint32_t*)(0x4002040c);
	*PUPDR |= (0b01 << 12) | (0b01 << 18);

	uint32_t* AFRL = (uint32_t*)(0x40020420);
	*AFRL &= ~(0b1111 << 24);
	*AFRL |= (4 << 24);

	uint32_t* AFRH = (uint32_t*)(0x40020424);
	*AFRH &= ~(0b1111 << 4);
	*AFRH |= (4 << 4);

	uint32_t* CR1 = (uint32_t*)0x40005400;
	*CR1 &= ~1;

	uint32_t* CR2 = (uint32_t*)0x40005404;
	*CR2 |= 16;

	uint32_t* CCR = (uint32_t*)0x4000541c;
	*CCR |= 80;

	*CR1 |= 1;
}

void I2C1_LSM303_ReadID()
{
	uint32_t* CR1 = (uint32_t*)0x40005400;
	uint32_t* DR  = (uint32_t*)0x40005410;
	uint32_t* SR1  = (uint32_t*)0x40005414;
	uint32_t* SR2  = (uint32_t*)0x40005418;

	const unsigned char SLAVE_ADDR = 0b0011001;

	while(((*SR2 >> 1) &1) == 1);
	*CR1 |= 1<<8;				//generate start bit
	while(((*SR1 >> 0) &1) == 0);

	//send slave addr(0b0011001) + write bit (0)
	*DR = (SLAVE_ADDR << 1) | 0;
	while(((*SR1 >> 1) &1) == 0);
	uint32_t temp = *SR2;

	//send WHO_AM_I (0x0f)
	*DR = 0x0f;
	while(((*SR1 >> 2) &1) == 0);

	//wait ACK
	while(((*SR1 >> 10) &1) == 1);

	//generate start bit
	*CR1 |= 1<<8;
	while(((*SR1 >> 0) &1) == 0);
	//send slave addr(0b0011001) + read bit (1)
	*DR = (SLAVE_ADDR << 1) | 1;
	while(((*SR1 >> 1) &1) == 0);
	temp = *SR2;

	//read data from slave
	uint8_t data = *DR;
	//generate stop bit
	*CR1 |= 1<<9;

}