# Timer trong STM32

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

## 🔧 Cấu trúc Timer

### Các thanh ghi cơ bản:

| Thanh ghi | Chức năng | Mô tả |
|-----------|-----------|--------|
| CNT | Counter | Đếm xung clock |
| PSC | Prescaler | Chia tần số đầu vào |
| ARR | Auto-Reload | Giá trị đếm tối đa |
| CCR | Capture/Compare | So sánh và capture |

### Công thức tính:
```
Timer_Clock = CPU_Clock / (PSC + 1)
Timer_Period = (ARR + 1) / Timer_Clock
Frequency = Timer_Clock / (ARR + 1)
```

## ⚙️ Cấu hình và Lập trình

### 1. Khởi tạo Timer cơ bản
```c
void Timer_Init(void)
{
    // Cấu hình timer
    htim2.Instance = TIM2;
    htim2.Init.Prescaler = 83;        // 84MHz/84 = 1MHz
    htim2.Init.Period = 999;          // 1MHz/1000 = 1kHz
    htim2.Init.CounterMode = TIM_COUNTERMODE_UP;
    
    // Khởi tạo timer
    HAL_TIM_Base_Init(&htim2);
}
```

### 2. Tạo độ trễ microsecond
```c
void Timer_Delay_us(TIM_HandleTypeDef *htim, uint16_t us)
{
    __HAL_TIM_SET_COUNTER(htim, 0);
    while(__HAL_TIM_GET_COUNTER(htim) < us);
}
```

### 3. Cấu hình ngắt Timer
```c
void Timer_Init_IT(void)
{
    // Cấu hình cơ bản
    Timer_Init();
    
    // Cấu hình ngắt
    HAL_NVIC_SetPriority(TIM2_IRQn, 0, 0);
    HAL_NVIC_EnableIRQ(TIM2_IRQn);
    
    // Bật ngắt timer
    HAL_TIM_Base_Start_IT(&htim2);
}

void TIM2_IRQHandler(void)
{
    HAL_TIM_IRQHandler(&htim2);
    // Xử lý ngắt ở đây
}
```

## 🎯 Ứng dụng thực tế

### 1. Đo độ rộng xung
```c
uint32_t Measure_Pulse_Width(void)
{
    // Đợi cạnh lên
    while(HAL_GPIO_ReadPin(GPIOA, GPIO_PIN_0) == GPIO_PIN_RESET);
    __HAL_TIM_SET_COUNTER(&htim2, 0);
    
    // Đợi cạnh xuống
    while(HAL_GPIO_ReadPin(GPIOA, GPIO_PIN_0) == GPIO_PIN_SET);
    
    return __HAL_TIM_GET_COUNTER(&htim2);
}
```

### 2. Tạo xung PWM
```c
void PWM_Init(void)
{
    // Cấu hình PWM
    TIM_OC_InitTypeDef sConfigOC = {0};
    sConfigOC.OCMode = TIM_OCMODE_PWM1;
    sConfigOC.Pulse = 500;    // 50% duty cycle
    
    HAL_TIM_PWM_ConfigChannel(&htim2, &sConfigOC, TIM_CHANNEL_1);
    HAL_TIM_PWM_Start(&htim2, TIM_CHANNEL_1);
}
```

## 🔍 Tips và Debug

### Các lỗi thường gặp:
1. Timer không hoạt động
   - ✔️ Kiểm tra clock enable
   - ✔️ Xác nhận prescaler
   - ✔️ Kiểm tra timer start

2. Tần số không đúng
   - ✔️ Xem lại công thức tính
   - ✔️ Kiểm tra PSC và ARR
   - ✔️ Đo bằng oscilloscope

### Tối ưu hiệu năng:
- Sử dụng DMA khi cần
- Chọn prescaler phù hợp
- Tránh polling khi có thể

## 📚 Tài liệu tham khảo
- [STM32F4 Reference Manual](https://www.st.com)
- [STM32 Timer Programming Guide](https://www.st.com)
- [HAL Timer Documentation](https://www.st.com)

---
<div align="center">
  <i>Developed with ❤️ for STM32 Community</i>
</div>

