
#include <stdint.h>
#define IWDG_ADDRESS_BASE 0x40003000 //fix me
void goto_low_power_mode()
{
    uint32_t* SCR = (uint32_t*)(0xe000ed10);
    *SCR |= (1 << 2);       // enable DEEPSLEEP
    __asm("WFI");           
}

void enable_iwdg()
{
    // set watchdog 3sec wakeup MCU
    uint32_t* IWDG_KR = (uint32_t*)(IWDG_ADDRESS_BASE + 0x00);
    uint32_t* IWDG_PR = (uint32_t*)(IWDG_ADDRESS_BASE + 0x04);
    uint32_t* IWDG_RLR = (uint32_t*)(IWDG_ADDRESS_BASE + 0x08);
    *IWDG_KR = 0x5555;
    *IWDG_PR = 0b011;
    *IWDG_RLR = 3000;
    *IWDG_KR = 0xCCCC;
}

void feed_iwdg()
{
    uint32_t* IWDG_KR = (uint32_t*)(IWDG_ADDRESS_BASE + 0x00);
    *IWDG_KR = 0xAAAA;
}