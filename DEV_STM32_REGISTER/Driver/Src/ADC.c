#include "ADC.h"
#include "clock.h"
#include <stdint.h>
#define ADC1_ADDRESS_BASE 0x40012000
#define GPIOA_ADDRESS_BASE 0x40012000
#define ADC1_IN1 1
#define ADC1_IN16 16

void adc_init()
{
    clock_enable_AHB1(GPIOA_peripheral);
    uint32_t *GPIOA_MODER = (uint32_t *)(GPIOA_ADDRESS_BASE + 0x00);
    *GPIOA_MODER |= (0b11 << 2);

    clock_enable_APB2(ADC1_peripheral);
    // __HAL_RCC_ADC1_CLK_ENABLE();
    uint32_t *ADC_CR2 = (uint32_t *)(ADC1_ADDRESS_BASE + 0x08);
    uint32_t *ADC_JSQR = (uint32_t *)(ADC1_ADDRESS_BASE + 0x38);
    uint32_t *ADC_CCR = (uint32_t *)(ADC1_ADDRESS_BASE + 0x300 + 0x04);
    uint32_t *ADC_SMPR1 = (uint32_t *)(ADC1_ADDRESS_BASE + 0x0C);

    *ADC_SMPR1 |= (0b110 << 18);    // using 12bit resolution (request 15 cycles for conversion)
    *ADC_JSQR &= ~(0b11 << 20);     
    *ADC_JSQR |= (0b00 << 20);     // Injected sequence length = 00: 1 conversion

    *ADC_JSQR |= (ADC1_IN16 << 15); // set source for JSQ4 is temp sensor(IN16)
    *ADC_JSQR |= (ADC1_IN1 << 10); // set source for JSQ3 is PA1(IN1)
    *ADC_CCR |= 1 << 23;            // enable temperature sensor
    *ADC_CR2 |= 1 << 0;             // enable ADC1 
}

float adc_get_temp_ss()
{
    float temp = 0;
    float vin = 0;
    uint16_t raw_data = 0;
    uint32_t *ADC_SR = (uint32_t *)(ADC1_ADDRESS_BASE + 0x00);
    uint32_t *ADC_CR2 = (uint32_t *)(ADC1_ADDRESS_BASE + 0x08);
    uint32_t *ADC_JDR1 = (uint32_t *)(ADC1_ADDRESS_BASE + 0x3C);

    // trigger ADC start convert
    *ADC_CR2 |= 1 << 22;

    // wait until end of conversion
    while(((*ADC_SR >> 2) & 1) == 0);
    *ADC_SR &= ~(1<<2); //clear JEOC flag
    // read ADC data from JDR1 (data of JSQ4)
    raw_data = *ADC_JDR1;

    vin = (raw_data * 3000) / 4095.0;
    temp = ((vin - 760) / 2.5) + 25;
    return temp;
}

float adc_get_vin_pa1()
{
    float vin = 0;
    uint16_t raw_data = 0;
    uint32_t *ADC_SR = (uint32_t *)(ADC1_ADDRESS_BASE + 0x00);
    uint32_t *ADC_CR2 = (uint32_t *)(ADC1_ADDRESS_BASE + 0x08);
    uint32_t *ADC_JDR2 = (uint32_t *)(ADC1_ADDRESS_BASE + 0x0C);


    // trigger ADC start convert
    *ADC_CR2 |= 1 << 22;

    // wait until end of conversion
    while(((*ADC_SR >> 2) & 1) == 0);
    *ADC_SR &= ~(1<<2); //clear JEOC flag
    // read ADC data from JDR1 (data of JSQ4)
    raw_data = *ADC_JDR2;

    vin = (raw_data * 3000) / 4095.0;
    return vin;
}