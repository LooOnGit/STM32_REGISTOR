#ifndef __SPI_H__
#define __SPI_H__

#include <stdint.h> 

void SPI_init();
void SPI_Write(uint8_t data);
uint8_t SPI_Read();
void LSM303_Active();
void LSM303_Inactive();
void LSM303_Init();
uint8_t LSM303_Read_ID();

#endif