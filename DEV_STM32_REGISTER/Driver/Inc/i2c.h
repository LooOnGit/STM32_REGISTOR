#ifndef __I2C_H__
#define __I2C_H__

#include <stdint.h>

void I2C_Init(void);
void I2C1_LSM303_init();
void I2C1_LSM303_ReadID();

#endif