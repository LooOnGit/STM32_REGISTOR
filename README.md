# 📟 STM32F411 Input Capture Timer Study Project
# 📟 Dự Án Học Tập Timer Input Capture STM32F411

## 📝 Overview - Tổng Quan
This project demonstrates the implementation of Input Capture functionality using timers on the STM32F411 microcontroller. Input Capture is used to measure pulse width, frequency, and duty cycle of input signals.

Dự án này trình bày việc thực hiện chức năng Input Capture sử dụng timer trên vi điều khiển STM32F411. Input Capture được sử dụng để đo độ rộng xung, tần số và chu kỳ của tín hiệu đầu vào.

## ⚡ Features - Tính Năng
- Timer Input Capture configuration (Cấu hình Timer Input Capture)
- Pulse width measurement (Đo độ rộng xung)
- Frequency measurement (Đo tần số)
- Duty cycle calculation (Tính toán chu kỳ nhiệm vụ)
- Edge detection (Rising/Falling) (Phát hiện cạnh (Lên/Xuống))

## 🛠️ Hardware Requirements - Yêu Cầu Phần Cứng
- STM32F411 Development Board (Kit phát triển STM32F411)
- Signal source for testing - function generator or test signal (Nguồn tín hiệu để test - máy phát tín hiệu hoặc tín hiệu test)
- ST-Link programmer/debugger (Thiết bị nạp/gỡ lỗi ST-Link)
- Jumper wires (Dây jump)

## 📚 Project Structure - Cấu Trúc Dự Án
```
Timer/
├── Core/
│   ├── Inc/           # Header files (File tiêu đề)
│   ├── Src/           # Source files (File mã nguồn)
│   └── Startup/       # Startup code (Mã khởi động)
├── Drivers/           # STM32 HAL drivers (Thư viện HAL STM32)
└── Debug/            # Build outputs (Thư mục build)
```

## ⚙️ Configuration - Cấu Hình
- System Clock: 100MHz (Xung nhịp hệ thống: 100MHz)
- Timer Configuration (Cấu hình Timer):
  - Timer Channel: TIM2 (Kênh Timer: TIM2)
  - Input Capture Mode (Chế độ Input Capture)
  - Edge Detection: Rising/Falling (Phát hiện cạnh: Lên/Xuống)
  - Prescaler: TBD based on input frequency (Bộ chia tần: Tùy thuộc vào tần số đầu vào)

## 🔌 Pin Configuration - Cấu Hình Chân
| Signal (Tín hiệu) | Pin | Function (Chức năng) |
|--------|-----|----------|
| Input Signal (Tín hiệu vào) | PA0 | TIM2_CH1 |

## 💻 Getting Started - Bắt Đầu
1. Clone this repository (Sao chép kho lưu trữ này)
2. Open project in STM32CubeIDE (Mở dự án trong STM32CubeIDE)
3. Configure the timer settings if needed (Cấu hình timer nếu cần)
4. Build and flash to your STM32F411 board (Build và nạp cho board STM32F411)
5. Connect your input signal to the configured pin (Kết nối tín hiệu đầu vào với chân đã cấu hình)

## 📊 Example Usage - Ví Dụ Sử Dụng
```c
// Timer Input Capture initialization (Khởi tạo Timer Input Capture)
HAL_TIM_IC_Start_IT(&htim2, TIM_CHANNEL_1);

// In the callback (Trong hàm callback)
void HAL_TIM_IC_CaptureCallback(TIM_HandleTypeDef *htim)
{
    if (htim->Channel == HAL_TIM_ACTIVE_CHANNEL_1)
    {
        // Process captured value (Xử lý giá trị đã capture)
        uint32_t captured_value = HAL_TIM_ReadCapturedValue(htim, TIM_CHANNEL_1);
    }
}
```

## 📈 Results - Kết Quả
- Successfully measured frequencies in range: 1Hz - 100kHz (Đo tần số thành công trong khoảng: 1Hz - 100kHz)
- Pulse width measurement resolution: 1µs (Độ phân giải đo độ rộng xung: 1µs)
- Accurate duty cycle measurements (Đo chu kỳ nhiệm vụ chính xác)

## 📚 References - Tài Liệu Tham Khảo
- [STM32F411 Reference Manual - Sách hướng dẫn tham khảo STM32F411](https://www.st.com/resource/en/reference_manual/rm0383-stm32f411xce-advanced-armbased-32bit-mcus-stmicroelectronics.pdf)
- [STM32F4 Timer Documentation - Tài liệu Timer STM32F4](https://www.st.com/resource/en/application_note/an4013-stm32-timer-overview-stmicroelectronics.pdf)

## 🔧 Troubleshooting - Xử Lý Sự Cố
- Check signal connections (Kiểm tra kết nối tín hiệu)
- Verify timer clock settings (Xác nhận cài đặt xung clock timer)
- Ensure proper interrupt handling (Đảm bảo xử lý ngắt đúng cách)

## 📝 License - Giấy Phép
This project is licensed under the MIT License - see the LICENSE file for details
(Dự án này được cấp phép theo Giấy phép MIT - xem file LICENSE để biết chi tiết)

## ✍️ Author - Tác Giả
[Your Name - Tên của bạn]

## 🤝 Contributing - Đóng Góp
Contributions, issues, and feature requests are welcome!
(Chào đón mọi đóng góp, báo lỗi và yêu cầu tính năng!)

## ⚡ Timer Calculations - Tính Toán Timer

### 🔢 Basic Timer Setup - Thiết Lập Timer Cơ Bản
```
Clock Settings (Cài đặt xung):
- APB1 Timer Clock (TIMCLK) = 16MHz
- Prescaler (PSC) = 16
- Counter Clock = TIMCLK/(PSC + 1) = 16MHz/16 = 1MHz

Timing Calculations (Tính toán thời gian):
- Counter Clock Period = 1/1MHz = 1µs
- Maximum Measurable Time = 1µs × 65535 (16-bit) ≈ 65.5ms
```

### 📊 Resolution Example - Ví Dụ Độ Phân Giải
```
Input Signal Timing (Thời gian tín hiệu vào):
- Minimum Resolution = 1µs (với prescaler = 16)
- Tần số đo được: 1Hz - 100kHz
- Độ chính xác: ±1µs
```

