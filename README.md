# Timer trong STM32F4

## 📋 Tổng quan về Timer

Timer là một phần cứng quan trọng trong STM32, được sử dụng để:
- ⏱️ Đo thời gian chính xác
- 🔄 Tạo độ trễ microsecond
- 📊 Tạo xung PWM
- 📈 Đo tần số/chu kỳ tín hiệu
- ⚡ Tạo ngắt định kỳ

## 🔄 Các loại Timer

### 1. Basic Timer (TIM6, TIM7)
- Chỉ đếm lên/xuống
- Không có đầu vào/ra
- Ứng dụng:
  + Tạo ngắt định kỳ
  + Trigger cho DAC

### 2. General Purpose Timer (TIM2-TIM5)
- Đếm lên/xuống/hai chiều
- 4 kênh Capture/Compare
- Hỗ trợ Encoder và DMA
- Ứng dụng:
  + PWM output
  + Input capture
  + Output compare

### 3. Advanced Timer (TIM1, TIM8)
- Tất cả tính năng của GP Timer
- Thêm:
  + Dead-time insertion
  + Break input
  + Complementary outputs
- Chuyên điều khiển động cơ

## 🔧 Cấu trúc Timer và Thanh ghi

### Cách tính thời gian với Prescaler:

Timer sử dụng 2 thành phần chính để tạo thời gian:
1. CNT (set trong TIMx_ARR - auto-reload)
2. Tcnt (set trong TIMx_PSC - pre-scaler)

Công thức tính:
```
CNT * Tcnt = 1 sec ~ 1000 msec
1000 * 1ms = 1000ms ~ 1s
```

Ví dụ với RCC 16MHz:
```
RCC (16MHz) -> Timer PSC (16000) -> 1ms
                      |
                      v
                    1000 = 1ms
```

Kết quả:
- Input: RCC Clock 16MHz
- Timer PSC: 16000 (tạo ra xung 1ms)
- Counter: 1000
- Output: 1000Hz (chu kỳ 1ms)

### Các thanh ghi chính:

| Thanh ghi | Offset | Mô tả |
|-----------|--------|--------|
| TIMx_CR1 | 0x00 | Control Register 1 |
| TIMx_CR2 | 0x04 | Control Register 2 |
| TIMx_SMCR | 0x08 | Slave Mode Control |
| TIMx_DIER | 0x0C | DMA/Interrupt Enable |
| TIMx_SR | 0x10 | Status Register |
| TIMx_EGR | 0x14 | Event Generation |
| TIMx_CCMR1 | 0x18 | Capture/Compare Mode 1 |
| TIMx_CCMR2 | 0x1C | Capture/Compare Mode 2 |
| TIMx_CCER | 0x20 | Capture/Compare Enable |
| TIMx_CNT | 0x24 | Counter Value |
| TIMx_PSC | 0x28 | Prescaler Value |
| TIMx_ARR | 0x2C | Auto-Reload Value |
| TIMx_CCR1 | 0x34 | Capture/Compare 1 Value |
| TIMx_CCR2 | 0x38 | Capture/Compare 2 Value |
| TIMx_CCR3 | 0x3C | Capture/Compare 3 Value |
| TIMx_CCR4 | 0x40 | Capture/Compare 4 Value |

### Bit Fields trong CR1:
```
CEN  - Counter Enable (Bit 0)
UDIS - Update Disable (Bit 1)
URS  - Update Request Source (Bit 2)
OPM  - One Pulse Mode (Bit 3)
DIR  - Direction (Bit 4)
CMS  - Center-aligned Mode (Bits 5-6)
ARPE - Auto-reload Preload Enable (Bit 7)
CKD  - Clock Division (Bits 8-9)
```

## ⚙️ Cấu hình Timer (Register Level)

### 1. Khởi tạo Timer cơ bản
```c
// Enable Timer2 clock in RCC
RCC->APB1ENR |= RCC_APB1ENR_TIM2EN;

// Configure Timer2
TIM2->PSC = 83;        // Prescaler = 84MHz/84 = 1MHz
TIM2->ARR = 999;       // Auto-reload = 1MHz/1000 = 1kHz
TIM2->CR1 = 0;         // Reset CR1
TIM2->CR1 |= TIM_CR1_CEN;  // Enable counter
```

### 2. Cấu hình ngắt Timer
```c
void Timer2_Init_IT(void)
{
    // Enable Timer2 clock
    RCC->APB1ENR |= RCC_APB1ENR_TIM2EN;
    
    // Configure Timer
    TIM2->PSC = 83;    // 1MHz
    TIM2->ARR = 999;   // 1kHz
    
    // Enable update interrupt
    TIM2->DIER |= TIM_DIER_UIE;
    
    // Enable Timer2 interrupt in NVIC
    NVIC_EnableIRQ(TIM2_IRQn);
    
    // Start timer
    TIM2->CR1 |= TIM_CR1_CEN;
}

void TIM2_IRQHandler(void)
{
    if(TIM2->SR & TIM_SR_UIF)
    {
        TIM2->SR &= ~TIM_SR_UIF;  // Clear flag
        // Xử lý ngắt ở đây
    }
}
```

### 3. Cấu hình PWM
```c
void PWM_Init(void)
{
    // Enable Timer2 and GPIOA clocks
    RCC->APB1ENR |= RCC_APB1ENR_TIM2EN;
    RCC->AHB1ENR |= RCC_AHB1ENR_GPIOAEN;
    
    // Configure PA5 as alternate function (Timer2 CH1)
    GPIOA->MODER &= ~GPIO_MODER_MODER5;
    GPIOA->MODER |= GPIO_MODER_MODER5_1;  // Alternate function
    GPIOA->AFR[0] |= (1 << 20);           // AF1 for Timer2
    
    // Configure Timer2 for PWM
    TIM2->PSC = 83;    // 1MHz
    TIM2->ARR = 999;   // 1kHz PWM frequency
    
    // Configure Channel 1 as PWM
    TIM2->CCMR1 &= ~TIM_CCMR1_OC1M;
    TIM2->CCMR1 |= TIM_CCMR1_OC1M_1 | TIM_CCMR1_OC1M_2;  // PWM Mode 1
    TIM2->CCMR1 |= TIM_CCMR1_OC1PE;  // Preload enable
    
    TIM2->CCR1 = 499;  // 50% duty cycle
    
    TIM2->CCER |= TIM_CCER_CC1E;  // Enable channel 1 output
    TIM2->CR1 |= TIM_CR1_CEN;     // Enable counter
}

// Thay đổi Duty Cycle (0-100%)
void PWM_SetDuty(uint8_t duty)
{
    if(duty <= 100)
    {
        TIM2->CCR1 = (TIM2->ARR + 1) * duty / 100;
    }
}
```

### 4. Input Capture
```c
void InputCapture_Init(void)
{
    // Enable Timer2 and GPIOA clocks
    RCC->APB1ENR |= RCC_APB1ENR_TIM2EN;
    RCC->AHB1ENR |= RCC_AHB1ENR_GPIOAEN;
    
    // Configure PA0 as alternate function
    GPIOA->MODER &= ~GPIO_MODER_MODER0;
    GPIOA->MODER |= GPIO_MODER_MODER0_1;
    GPIOA->AFR[0] |= (1 << 0);
    
    // Configure Timer2 Channel 1 for input capture
    TIM2->PSC = 83;    // 1MHz
    
    TIM2->CCMR1 &= ~TIM_CCMR1_CC1S;
    TIM2->CCMR1 |= TIM_CCMR1_CC1S_0;  // CC1 channel as input
    
    TIM2->CCER &= ~(TIM_CCER_CC1P | TIM_CCER_CC1NP);  // Rising edge
    TIM2->CCER |= TIM_CCER_CC1E;  // Enable capture
    
    TIM2->CR1 |= TIM_CR1_CEN;     // Enable counter
}

uint32_t IC_GetValue(void)
{
    return TIM2->CCR1;
}
```

## 🔍 Các chế độ đếm

### 1. Up-counting mode
```c
TIM2->CR1 &= ~(TIM_CR1_DIR | TIM_CR1_CMS);  // Up counting
```

### 2. Down-counting mode
```c
TIM2->CR1 |= TIM_CR1_DIR;
TIM2->CR1 &= ~TIM_CR1_CMS;
```

### 3. Center-aligned mode
```c
TIM2->CR1 &= ~TIM_CR1_DIR;
TIM2->CR1 |= TIM_CR1_CMS_0;  // Center-aligned mode 1
```

## 📊 Công thức tính toán

```
Timer_Clock = CPU_Clock / (APB1_Prescaler * Timer_Prescaler)
Timer_Period = (ARR + 1) / Timer_Clock
PWM_Frequency = Timer_Clock / (ARR + 1)
PWM_DutyCycle = (CCRx / (ARR + 1)) * 100%
```

## 🎯 Tips và Debug

### Các lỗi thường gặp:
1. Timer không hoạt động
   - ✔️ Kiểm tra RCC clock enable
   - ✔️ Xác nhận bit CEN trong CR1
   - ✔️ Kiểm tra giá trị PSC và ARR

2. PWM không hoạt động
   - ✔️ Kiểm tra GPIO alternate function
   - ✔️ Xác nhận CCMR1 configuration
   - ✔️ Kiểm tra CCER enable bit

## 📚 Tài liệu tham khảo
- STM32F4 Reference Manual (RM0090)
- STM32F4 Register Description
- STM32F4 Datasheet

---
<div align="center">
  <i>Developed with ❤️ for STM32 Community</i>
</div>

