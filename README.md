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
1. CNT (set trong TIMx_ARR - auto-reload) = 1000 (< 65535)
2. Tcnt (set trong TIMx_PSC - pre-scaler) = 16000 (< 65535)

Lưu ý: Cả CNT và Tcnt đều là thanh ghi 16-bit nên giá trị tối đa là 65535.

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

## 📊 Công thức tính toán

```
Timer_Clock = CPU_Clock / (APB1_Prescaler * Timer_Prescaler)
Timer_Period = (ARR + 1) / Timer_Clock
PWM_Frequency = Timer_Clock / (ARR + 1)
PWM_DutyCycle = (CCRx / (ARR + 1)) * 100%
```

## ⏰ SysTick Timer

SysTick là một timer đặc biệt 24-bit được tích hợp trong tất cả các vi điều khiển ARM Cortex-M. Nó thường được sử dụng để tạo ra system tick cho hệ điều hành hoặc tạo độ trễ chính xác.

### Đặc điểm của SysTick:
- Timer 24-bit đếm xuống
- 3 nguồn clock có thể chọn:
  + Processor clock (HCLK)
  + Processor clock/8
  + External clock
- Ngắt tự động khi đếm về 0
- Tự động nạp lại giá trị từ RELOAD register

### Các thanh ghi SysTick:

| Thanh ghi | Offset | Mô tả |
|-----------|--------|-------|
| CTRL | 0xE000E010 | Control and Status Register |
| LOAD | 0xE000E014 | Reload Value Register |
| VAL | 0xE000E018 | Current Value Register |
| CALIB | 0xE000E01C | Calibration Value Register |

### Cấu hình SysTick cơ bản:
```c
// Cấu hình SysTick để ngắt mỗi 1ms với HCLK = 16MHz
void SysTick_Init(void)
{
    // Tắt SysTick
    SysTick->CTRL = 0;
    
    // Nạp giá trị RELOAD (16000 - 1 cho 1ms với HCLK = 16MHz)
    SysTick->LOAD = 16000 - 1;
    
    // Reset giá trị hiện tại
    SysTick->VAL = 0;
    
    // Chọn source clock (processor clock)
    // Enable SysTick interrupt
    // Enable SysTick counter
    SysTick->CTRL = SysTick_CTRL_CLKSOURCE_Msk |
                    SysTick_CTRL_TICKINT_Msk |
                    SysTick_CTRL_ENABLE_Msk;
}

// Handler cho SysTick interrupt
void SysTick_Handler(void)
{
    // Xử lý ngắt ở đây
    // Được gọi mỗi 1ms
}
```

### Tạo độ trễ với SysTick:
```c
void Delay_ms(uint32_t ms)
{
    uint32_t start = SysTick->VAL;
    uint32_t ticks = ms * (SystemCoreClock / 1000);
    uint32_t elapsed = 0;
    
    do {
        elapsed = start - SysTick->VAL;
        if (elapsed > ticks)
            break;
    } while (1);
}
```

### Các bit trong CTRL Register:
```
ENABLE    - Enable counter (Bit 0)
TICKINT   - Enable SysTick interrupt (Bit 1)
CLKSOURCE - Clock source selection (Bit 2)
COUNTFLAG - Timer counted to 0 (Bit 16)
```

### Công thức tính:
```
Tick_Period = (RELOAD + 1) / HCLK_Frequency
Interrupt_Frequency = HCLK_Frequency / (RELOAD + 1)
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

