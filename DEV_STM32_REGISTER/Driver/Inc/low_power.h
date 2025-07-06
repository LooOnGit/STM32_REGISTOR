#ifndef __LOW_POWER_H
#define __LOW_POWER_H

#include <stdint.h>

#define IWDG_ADDRESS_BASE 0x40003000

void goto_low_power_mode();
//apply sleep mode to MCU
void enable_iwdg();

//
void feed_iwdg();


#endif