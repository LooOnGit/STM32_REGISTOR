# 🔍 Learning Watchdog Timer on STM32F411

## 📚 Giới thiệu về Watchdog
Watchdog (WDG) là một bộ đếm thời gian được sử dụng để phát hiện và phục hồi từ các sự cố phần mềm. STM32F411 có 2 loại Watchdog:

### 🛡️ Independent Watchdog (IWDG)
- 🔄 Sử dụng bộ dao động LSI (40 kHz)
- 🔓 Hoạt động độc lập với clock chính
- ⚡ Phù hợp cho việc phục hồi từ lỗi phần mềm

### 🪟 Window Watchdog (WWDG)
- ⏰ Sử dụng clock từ APB1
- 📊 Có thể cấu hình cửa sổ thời gian
- 🎯 Phát hiện lỗi thời gian thực chính xác hơn

---

## 💤 Low Power Modes

STM32F411 cung cấp 3 chế độ tiết kiệm điện:

### 1. 😴 Sleep Mode
- CPU và Cortex®-M4F core bị dừng
- Các ngoại vi vẫn hoạt động
- Thích hợp cho việc tiết kiệm điện nhẹ
- Thời gian wake-up nhanh

### 2. 🛑 Stop Mode
- Tất cả clock đều bị dừng
- Giữ nội dung SRAM và thanh ghi
- Tiết kiệm điện đáng kể
- Thời gian wake-up trung bình

### 3. 🔌 Standby Mode
- Tắt nguồn domain 1.2V
- Mất nội dung SRAM và thanh ghi
- Tiết kiệm điện tối đa
- Thời gian wake-up lâu nhất

### ⚡ Watchdog trong Low Power Mode
1. **IWDG**:
   - Tiếp tục hoạt động trong mọi chế độ
   - Sử dụng LSI nên độc lập với system clock
   - Có thể đánh thức hệ thống từ Sleep/Stop

2. **WWDG**:
   - Dừng trong Stop và Standby mode
   - Hoạt động trong Sleep mode nếu APB1 clock được bật
   - Không thể đánh thức hệ thống từ Stop/Standby

## ⚖️ So sánh IWDG và WWDG

| Đặc điểm | 🛡️ IWDG | 🪟 WWDG |
|----------|----------|---------|
| Clock | LSI (40 kHz) | APB1 (max 42 MHz) |
| Counter | 12-bit | 7-bit |
| Prescaler | 4 → 256 | 1, 2, 4, 8 |
| Timeout | 0.1ms → 26.2s | Theo APB1 clock |
| Cửa sổ thời gian | ❌ | ✅ |
| Ngắt cảnh báo | ❌ | ✅ |
| Low Power Mode | Hoạt động tất cả mode | Chỉ Sleep mode |
| Ứng dụng | Bảo vệ treo hệ thống | Giám sát timing |

## 🚀 Ứng dụng thực tế
1. **🔄 Phục hồi từ treo phần mềm**
   - 🛡️ Sử dụng IWDG để reset hệ thống
   - ⏱️ Thời gian timeout phù hợp với chu kỳ task

2. **📊 Giám sát thời gian thực**
   - 🪟 Sử dụng WWDG để đảm bảo timing
   - 🎯 Cấu hình cửa sổ cho phép refresh

3. **💪 Kết hợp cả hai**
   - 🛡️ IWDG cho bảo vệ tổng thể
   - 🔍 WWDG cho giám sát chi tiết

## 📝 Notes
- ✅ Luôn test watchdog trước khi triển khai
- ⚠️ Tính toán timeout dựa trên worst-case scenario
- 🎯 Đặt refresh point ở vị trí thích hợp
- 🌡️ Xem xét điều kiện môi trường ảnh hưởng tới LSI
- 💾 Backup dữ liệu quan trọng trước khi reset
- ⚡ Xử lý ngắt WWDG một cách phù hợp
