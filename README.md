# 📚 Hướng dẫn Timer PWM trên STM32F411

<div align="center">
  <img src="https://www.st.com/content/ccc/fragment/product_related/rpn_information/product_circuit_diagram/group0/0c/36/dc/ae/ea/c2/45/7d/stm32f411ccu6-pinout/files/stm32f411ccu6-pinout.jpg/jcr:content/translations/en.stm32f411ccu6-pinout.jpg" width="400">
</div>

## 📋 Mục lục
1. [Tổng quan](#tổng-quan)
2. [PWM là gì?](#pwm-là-gì)
3. [Lý thuyết Timer PWM](#lý-thuyết-timer-pwm)
4. [Thực hiện Timer PWM trên STM32](#thực-hiện-timer-pwm-trên-stm32)
5. [Ví dụ mã nguồn](#ví-dụ-mã-nguồn)
6. [Ứng dụng](#ứng-dụng)
7. [Cấu trúc dự án](#cấu-trúc-dự-án)
8. [Yêu cầu](#yêu-cầu-phần-cứng)
9. [Tài liệu tham khảo](#tài-liệu-tham-khảo)

## 🎯 Tổng quan
Repository này chứa các ví dụ và tài liệu hướng dẫn thực hiện Timer PWM (Pulse Width Modulation) trên vi điều khiển STM32F411. Dự án này được xây dựng dựa trên bài giảng từ [Video Timer PWM STM32](https://www.youtube.com/watch?v=fZgGG5vrTno&list=PLeF_iec1JSb6FLu07L6uAleGYWszlG1rY&index=12).

## ⚡ PWM là gì?
Pulse Width Modulation (PWM) là kỹ thuật điều chế độ rộng xung, được sử dụng để điều khiển công suất trung bình cung cấp cho tải bằng cách chuyển đổi nhanh tín hiệu giữa trạng thái cao và thấp.

### Các tham số chính của PWM:
| Tham số | Mô tả |
|---------|--------|
| **Tần số** | Tốc độ chuyển đổi tín hiệu (Hz) |
| **Chu kỳ làm việc (Duty Cycle)** | Phần trăm thời gian tín hiệu ở mức cao |
| **Chu kỳ (Period)** | Tổng thời gian cho một chu kỳ hoàn chỉnh |
| **Độ rộng xung (Pulse Width)** | Thời gian tín hiệu duy trì ở mức cao |

## 📐 Lý thuyết Timer PWM

### 🔍 Khái niệm cơ bản
1. **Bộ đếm Timer**: 
   - Đếm từ 0 đến giá trị Auto-Reload Register (ARR)
   - Tạo cơ sở thời gian cho PWM

2. **Thanh ghi so sánh (CCR)**:
   - Xác định thời điểm chuyển đổi đầu ra
   - Điều khiển độ rộng xung

3. **Chế độ PWM**:
   - Timer tự động điều khiển đầu ra
   - Dựa trên so sánh giá trị CNT và CCR

### 📊 Các công thức PWM
```
Chu kỳ = (ARR + 1) / Tần số đồng hồ Timer
Chu kỳ làm việc = (CCR / (ARR + 1)) × 100%
Tần số = Tần số đồng hồ Timer / (ARR + 1)
```

## 🛠 Thực hiện Timer PWM trên STM32

### Cấu hình Capture/Compare
1. **Enable channel (CCER)**
   ```c
   TIMx->CCER |= TIM_CCER_CC1E;  // Kích hoạt kênh 1
   ```

2. **Chọn chế độ Compare (CCMR)**
   ```c
   TIMx->CCMR1 |= TIM_CCMR1_OC1M_1 | TIM_CCMR1_OC1M_2;  // PWM Mode 1
   ```

3. **Set giá trị Compare (CCR)**
   ```c
   TIMx->CCR1 = value;  // Đặt duty cycle
   ```

### 🔧 Cấu hình phần cứng
| Thành phần | Mô tả |
|------------|--------|
| **Timer** | TIM2, TIM3, TIM4, TIM5 (Timer đa năng) |
| **Kênh** | 4 kênh độc lập trên mỗi timer |
| **Chế độ** | PWM Mode 1 hoặc PWM Mode 2 |
| **Đầu ra** | GPIO với chức năng thay thế |

### 📝 Các thanh ghi quan trọng
| Thanh ghi | Chức năng |
|-----------|-----------|
| TIMx_CR1 | Điều khiển Timer |
| TIMx_ARR | Tự động tải lại (Chu kỳ) |
| TIMx_CCRx | Bắt/so sánh (Duty Cycle) |
| TIMx_CCMRx | Chế độ bắt/so sánh |
| TIMx_CCER | Bật/tắt bắt/so sánh |

## 💻 Ví dụ mã nguồn

### Khởi tạo PWM cơ bản
```c
void Timer_PWM_Init(void)
{
    // Bật đồng hồ timer
    __HAL_RCC_TIM2_CLK_ENABLE();
    
    // Cấu hình timer
    htim2.Instance = TIM2;
    htim2.Init.Prescaler = 83;        // 84MHz / 84 = 1MHz
    htim2.Init.Period = 999;          // 1MHz / 1000 = 1kHz
    htim2.Init.ClockDivision = TIM_CLOCKDIVISION_DIV1;
    htim2.Init.CounterMode = TIM_COUNTERMODE_UP;
    
    HAL_TIM_Base_Init(&htim2);
    
    // Cấu hình PWM
    sConfigOC.OCMode = TIM_OCMODE_PWM1;
    sConfigOC.Pulse = 500;            // 50% duty cycle
    sConfigOC.OCPolarity = TIM_OCPOLARITY_HIGH;
    
    HAL_TIM_PWM_ConfigChannel(&htim2, &sConfigOC, TIM_CHANNEL_1);
    HAL_TIM_PWM_Start(&htim2, TIM_CHANNEL_1);
}
```

### Điều chỉnh Duty Cycle
```c
void PWM_SetDutyCycle(uint16_t duty_cycle)
{
    uint16_t ccr_value = (duty_cycle * (htim2.Init.Period + 1)) / 100;
    __HAL_TIM_SET_COMPARE(&htim2, TIM_CHANNEL_1, ccr_value);
}
```

## 🎯 Ứng dụng

### 💡 Điều khiển độ sáng LED
- PWM điều chỉnh độ sáng LED
- Duty cycle ↔ độ sáng
- Tần số > 100Hz (tránh nhấp nháy)

### ⚙️ Điều khiển động cơ
- PWM điều khiển tốc độ
- Duty cycle ↔ tốc độ quay
- Tần số phù hợp với động cơ

### 🤖 Điều khiển servo
- Độ rộng xung: 1-2ms
- Tần số: 50Hz (20ms)
- Vị trí tương ứng: 0-180°

## 📁 Cấu trúc dự án
```
STM32_REGISTOR/
├── 📂 STM32F4/                 # Dự án STM32F4 chính
├── 📂 STM32F411_Workspace1/    # Workspace với nhiều dự án
│   ├── 📂 FLASH/              # Dự án dựa trên Flash
│   ├── 📂 FuntionInRam/       # Dự án dựa trên RAM
│   ├── 📂 FW_test_led/        # Firmware test LED
│   ├── 📂 FW1_Bootloader/     # Firmware bootloader
│   ├── 📂 FW2_App1/           # Ứng dụng 1
│   └── 📂 FW3_App2/           # Ứng dụng 2
└── 📂 Documents/               # Tài liệu STM32
```

## 🔧 Yêu cầu phần cứng
- ✅ Board phát triển STM32F411VET6
- 💡 LED để test (tùy chọn)
- 📊 Dao động kế để phân tích dạng sóng (tùy chọn)
- 🔄 Động cơ hoặc servo cho ứng dụng thực tế (tùy chọn)

## 💻 Yêu cầu phần mềm
- ✅ STM32CubeIDE hoặc IDE tương tự
- ✅ Thư viện STM32 HAL
- ✅ STM32CubeMX để cấu hình (tùy chọn)

## 📚 Tài liệu tham khảo
- 🎥 [Video hướng dẫn Timer PWM STM32](https://www.youtube.com/watch?v=fZgGG5vrTno&list=PLeF_iec1JSb6FLu07L6uAleGYWszlG1rY&index=12)
- 📖 Sổ tay tham khảo STM32F411 (RM0383)
- 📖 Sổ tay lập trình STM32F4 (PM0214)

## 📝 Giấy phép
Dự án này được phát triển cho mục đích giáo dục. Bạn có thể tự do sử dụng và chỉnh sửa mã nguồn cho các dự án của riêng mình.

---
<div align="center">
  <i>Developed with ❤️ for STM32 Community</i>
</div>
