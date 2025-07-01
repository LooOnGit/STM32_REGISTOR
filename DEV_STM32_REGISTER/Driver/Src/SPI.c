#include "SPI.h"
#include "clock.h"

void SPI_init()
{
	clock_enable_AHB1(GPIOA_peripheral);

	uint32_t* GPIOA_MODER = (uint32_t*)(0x40020000);
	uint32_t* GPIOA_AFRL  = (uint32_t*)(0x40020020);
	*GPIOA_MODER &= ~(0b111111 << 10);
	*GPIOA_MODER |= (0b10 << 10) | (0b10 << 12) | (0b10 << 14);
	*GPIOA_AFRL &= ~(0xfff << 20);
	*GPIOA_AFRL |= (5<<20) | (5 << 24) | (5<<28);

	clock_enable_AHB1(GPIOE_peripheral);
	uint32_t* GPIOE_MODER = (uint32_t*)(0x40021000);
	*GPIOE_MODER |= (0b01 << 6);
	uint32_t* GPIOE_ODR = (uint32_t*)(0x40021014);
	*GPIOE_ODR |= 1<<3;

	clock_enable_APB2(SPI1_peripheral);
	uint32_t* CR1 = (uint32_t*)(0x40013000);
	*CR1 |= (0b11<<3);			//set baundrate as Fpclk/16 = 1MHz
	*CR1 |= (1<<8) | (1<<9); 	//enable software management
	*CR1 |= (0b1 <<6) | (1<<2); //enable SPI in master mode
}

void SPI_Write(uint8_t data)
{
	uint8_t temp = 0;
	uint32_t* DR = (uint32_t*)(0x4001300c);
	uint32_t* SR = (uint32_t*)(0x40013008);

	while(((*SR >> 1)&1)!=1);
	*DR = data;
	while(((*SR >> 7)&1)==1);

	while(((*SR >> 0)&1)!=1);	//if Rx buffer has data
	temp = *DR;					//clear Rx buffer
	while(((*SR >> 7)&1)==1);

}

uint8_t SPI_Read()
{
	uint8_t temp = 0;
	uint32_t* DR = (uint32_t*)(0x4001300c);
	uint32_t* SR = (uint32_t*)(0x40013008);

	while(((*SR >> 1)&1)!=1);
	*DR = 0xff;
	while(((*SR >> 7)&1)==1);

	while(((*SR >> 0)&1)!=1);	//if Rx buffer has data
	temp = *DR;					//clear Rx buffer
	while(((*SR >> 7)&1)==1);

	return temp;
}

void LSM303_Active()
{
	uint32_t* GPIOE_ODR = (uint32_t*)(0x40021014);
	*GPIOE_ODR &= ~(1<<3);		//active slave
}

void LSM303_Inactive()
{
	uint32_t* GPIOE_ODR = (uint32_t*)(0x40021014);
	*GPIOE_ODR |= (1<<3);	//in-active slave
}

void LSM303_Init()
{
	LSM303_Active();
	/* enable x, y, z axis
	 * Power mode: 10Hz
	 */
	SPI_Write(0x20);
	SPI_Write(0x27);

	/* data resolution is 12bit (set HR to 1)
	 * Power mode: 10Hz
	 * Enable SPI
	 */
	SPI_Write(0x23 );
	SPI_Write((1<<3) | 1);

	LSM303_Inactive();
}

int16_t LSM303_Read_X()
{
	LSM303_Active();
	SPI_Write(0x28 | (1<<7));
	uint8_t low = SPI_Read();

	SPI_Write(0x29| (1<<7));
	uint8_t high = SPI_Read();
	LSM303_Inactive();

	int16_t result = low | (high << 8);
	if(result > 0x8000)
		result -= (0xffff+1);
	result /= 16;
	return result;
}

uint16_t LSM303_Read_Y()
{
	LSM303_Active();
	SPI_Write(0x2A | (1<<7));
	uint8_t low = SPI_Read();

	SPI_Write(0x2B| (1<<7));
	uint8_t high = SPI_Read();
	LSM303_Inactive();
	int16_t result = low | (high << 8);
	if(result > 0x8000)
		result -= (0xffff+1);
	result /= 16;
	return result;
}
uint16_t LSM303_Read_Z()
{
	LSM303_Active();
	SPI_Write(0x2C | (1<<7));
	uint8_t low = SPI_Read();

	SPI_Write(0x2D| (1<<7));
	uint8_t high = SPI_Read();
	LSM303_Inactive();

	int16_t result = low | (high << 8);

	if(result > 0x8000)
		result -= (0xffff+1);
	result /= 16;

	return result;
}

uint8_t LSM303_Read_ID()
{
	uint8_t WHO_AM_I = 0x0f | (1<<7);	//read ID
	LSM303_Active();
	SPI_Write(WHO_AM_I);
	uint8_t ID = SPI_Read();
	LSM303_Inactive();
	return ID;
}