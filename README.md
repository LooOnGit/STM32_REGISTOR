# Hướng dẫn Timer PWM trên STM32

## Tổng quan
Repository này chứa các ví dụ và tài liệu hướng dẫn thực hiện Timer PWM (Pulse Width Modulation) trên vi điều khiển STM32F411 dựa trên bài giảng từ [Video Timer PWM STM32](https://www.youtube.com/watch?v=fZgGG5vrTno&list=PLeF_iec1JSb6FLu07L6uAleGYWszlG1rY&index=12).

## PWM là gì?
Pulse Width Modulation (PWM) là kỹ thuật được sử dụng để điều khiển công suất trung bình cung cấp cho tải bằng cách chuyển đổi nhanh tín hiệu giữa trạng thái cao và thấp. Các tham số chính của PWM bao gồm:

- **Tần số**: Tốc độ chuyển đổi tín hiệu (Hz)
- **Chu kỳ làm việc (Duty Cycle)**: Phần trăm thời gian tín hiệu ở mức cao
- **Chu kỳ (Period)**: Tổng thời gian cho một chu kỳ hoàn chỉnh
- **Độ rộng xung (Pulse Width)**: Thời gian tín hiệu duy trì ở mức cao

## Lý thuyết Timer PWM

### Khái niệm cơ bản
1. **Bộ đếm Timer**: Đếm từ 0 đến giá trị Auto-Reload Register (ARR)
2. **Thanh ghi so sánh (CCR)**: Xác định khi nào đầu ra nên chuyển đổi
3. **Chế độ PWM**: Timer tự động chuyển đổi đầu ra dựa trên giá trị bộ đếm

### Các tham số PWM
- **Chu kỳ** = (ARR + 1) / Tần số đồng hồ Timer
- **Chu kỳ làm việc** = (CCR / (ARR + 1)) × 100%
- **Tần số** = Tần số đồng hồ Timer / (ARR + 1)

## Thực hiện Timer PWM trên STM32

### Cấu hình phần cứng
- **Timer**: TIM2, TIM3, TIM4, hoặc TIM5 (Timer đa năng)
- **Kênh**: Bất kỳ 1 trong 4 kênh của mỗi timer
- **Chế độ**: PWM Mode 1 hoặc PWM Mode 2
- **Đầu ra**: Chân GPIO được cấu hình làm chức năng thay thế

### Các thanh ghi quan trọng
1. **TIMx_CR1**: Thanh ghi điều khiển Timer
2. **TIMx_ARR**: Thanh ghi tự động tải lại (Chu kỳ)
3. **TIMx_CCRx**: Thanh ghi bắt/so sánh (Chu kỳ làm việc)
4. **TIMx_CCMRx**: Thanh ghi chế độ bắt/so sánh
5. **TIMx_CCER**: Thanh ghi bật bắt/so sánh

### Các bước cấu hình
1. Bật đồng hồ timer
2. Cấu hình chân GPIO làm chức năng thay thế
3. Đặt prescaler và ARR cho tần số mong muốn
4. Cấu hình chế độ PWM trong thanh ghi CCMR
5. Đặt giá trị CCR cho chu kỳ làm việc mong muốn
6. Bật timer và đầu ra PWM

## Ví dụ mã nguồn

### Thiết lập PWM cơ bản
```c
// Cấu hình Timer PWM
void Timer_PWM_Init(void)
{
    // Bật đồng hồ timer
    __HAL_RCC_TIM2_CLK_ENABLE();
    
    // Cấu hình timer
    htim2.Instance = TIM2;
    htim2.Init.Prescaler = 83;  // 84MHz / 84 = 1MHz
    htim2.Init.Period = 999;    // 1MHz / 1000 = 1kHz
    htim2.Init.ClockDivision = TIM_CLOCKDIVISION_DIV1;
    htim2.Init.CounterMode = TIM_COUNTERMODE_UP;
    htim2.Init.AutoReloadPreload = TIM_AUTORELOAD_PRELOAD_DISABLE;
    
    HAL_TIM_Base_Init(&htim2);
    
    // Cấu hình PWM
    sConfigOC.OCMode = TIM_OCMODE_PWM1;
    sConfigOC.Pulse = 500;  // 50% chu kỳ làm việc
    sConfigOC.OCPolarity = TIM_OCPOLARITY_HIGH;
    sConfigOC.OCFastMode = TIM_OCFAST_DISABLE;
    
    HAL_TIM_PWM_ConfigChannel(&htim2, &sConfigOC, TIM_CHANNEL_1);
    
    // Khởi động PWM
    HAL_TIM_PWM_Start(&htim2, TIM_CHANNEL_1);
}
```

### Điều khiển chu kỳ làm việc thay đổi
```c
// Thay đổi chu kỳ làm việc động
void PWM_SetDutyCycle(uint16_t duty_cycle)
{
    uint16_t ccr_value = (duty_cycle * (htim2.Init.Period + 1)) / 100;
    __HAL_TIM_SET_COMPARE(&htim2, TIM_CHANNEL_1, ccr_value);
}
```

## Ứng dụng

### Điều khiển độ sáng LED
- Sử dụng PWM để điều khiển độ sáng LED
- Chu kỳ làm việc quyết định độ sáng cảm nhận
- Tần số nên > 100Hz để tránh nhấp nháy

### Điều khiển tốc độ động cơ
- PWM điều khiển tốc độ động cơ
- Chu kỳ làm việc cao hơn = động cơ nhanh hơn
- Tần số ảnh hưởng đến hành vi động cơ

### Điều khiển servo
- Độ rộng xung PWM điều khiển vị trí servo
- Phạm vi điển hình: 1ms đến 2ms độ rộng xung
- Tần số: 50Hz (chu kỳ 20ms)

## Cấu trúc dự án
```
STM32_REGISTOR/
├── STM32F4/                    # Dự án STM32F4 chính
├── STM32F411_Workspace1/       # Workspace với nhiều dự án
│   ├── FLASH/                  # Dự án dựa trên Flash
│   ├── FuntionInRam/           # Dự án dựa trên RAM
│   ├── FW_test_led/            # Firmware test LED
│   ├── FW1_Bootloader/         # Firmware bootloader
│   ├── FW2_App1/               # Ứng dụng 1
│   └── FW3_App2/               # Ứng dụng 2
└── Documents/                  # Tài liệu STM32
```

## Yêu cầu phần cứng
- Board phát triển STM32F411VET6
- LED để test (tùy chọn)
- Dao động kế để phân tích dạng sóng (tùy chọn)
- Động cơ hoặc servo cho ứng dụng thực tế (tùy chọn)

## Yêu cầu phần mềm
- STM32CubeIDE hoặc IDE tương tự
- Thư viện STM32 HAL
- STM32CubeMX để cấu hình (tùy chọn)

## Tài liệu tham khảo
- [Video hướng dẫn Timer PWM STM32](https://www.youtube.com/watch?v=fZgGG5vrTno&list=PLeF_iec1JSb6FLu07L6uAleGYWszlG1rY&index=12)
- Sổ tay tham khảo STM32F411 (RM0383)
- Sổ tay lập trình STM32F4 (PM0214)

## Giấy phép
Dự án này dành cho mục đích giáo dục. Hãy tự do sử dụng và chỉnh sửa mã nguồn cho các dự án của riêng bạn.
