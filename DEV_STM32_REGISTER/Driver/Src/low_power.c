#include "low_power.h"


void goto_low_power_mode()
{
    uint32_t* SCR = (uint32_t*)0xE000ED10;
    *SCR |= (1<<2);         //enable DEEPSLEEP
    __asm__("WFI");         //
}

void enable_iwdg()
{
    //set watchdog 3sec wakeup MCU
    

}