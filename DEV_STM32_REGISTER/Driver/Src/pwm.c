#include "pwm.h"
#include "clock.h"
#include"stdint.h"

//16Mhz is 16000000 counter/sec

/**
* @brief generate pwm with frequency = 1Hz, pulse width = 50%
 */

 void pwm_init()
 {
    //set gpio alternate function
    clock_enable_AHB1(GPIOD_peripheral);
    uint32_t* MODER = (uint32_t*)0x40020c00;
    uint32_t* AFR = (uint32_t*)0x40020c24;

    *MODER &= ~(0b11 << 24);//clean bit
    *MODER |= (0b10 << 24);
    *AFR &= ~(0b1111 << 16);
    *AFR |= (0b0010 << 16);


    clock_enable_APB1(TIM4_peripheral);

    uint32_t* PSC = (uint32_t*)0x40000828;
    uint32_t* ARR = (uint32_t*)0x4000082c;
    uint32_t* CCR1 = (uint32_t*)0x40000834;
    uint32_t* CCMR1_Output = (uint32_t*)0x40000818;
    uint32_t* CCER = (uint32_t*)0x40000820;
    uint32_t* CR1 = (uint32_t*)0x40000800;

    *PSC = 1600 - 1;
    *ARR = 100;
    *CCR1 = 0;

    //select channel 1 work in compare (pwm mode 1) or capture mode
    *CCMR1_Output &= ~(0b11 << 0); //set channel 1 in output compare mode
    *CCMR1_Output &= ~(0b111 << 4);
    *CCMR1_Output |= (0b110 << 4);//set pwm mode1 for compare channel 1

    //enable channel 1 (compare/capture)
    *CCER |= (1 << 0);

    //enable counter
    *CR1 |= (1 << 0);
 }

void pwm_pulse_crtl(uint8_t pulse)
{
   if(pulse > 100)    // Chỉ cần kiểm tra điều kiện này vì uint16_t luôn >= 0
      return;

   uint32_t* CCR1 = (uint32_t*)0x40000834;
   *CCR1 = pulse;
}