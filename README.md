# 🔧 STM32F411 RCC Programming Guide

## 📚 Table of Contents
- [Introduction](#introduction)
- [System Reset](#system-reset)
- [Hardware Requirements](#hardware-requirements)
- [Clock System Overview](#clock-system-overview)
- [Register Programming Guide](#register-programming-guide)
- [Examples](#examples)
- [References](#references)

## 🎯 Introduction
RCC là Reset and clock control có 2 chức năng quản lý sự kiện, nguyên nhân tạo ra reset và chức năng config 
clock cho hệ thống.

## 🔄 System Reset
System reset là quá trình đặt lại tất cả các thanh ghi về giá trị mặc định của chúng, ngoại trừ:
- Các cờ reset trong thanh ghi CSR của bộ điều khiển clock
- Các thanh ghi trong vùng Backup domain

### Các Nguyên Nhân Gây Reset
1. **NRST Pin Reset (External Reset)**
   - Xảy ra khi chân NRST được kéo xuống mức thấp
   - Reset từ bên ngoài chip

2. **Window Watchdog Reset (WWDG)**
   - Reset khi bộ đếm watchdog đạt điều kiện kết thúc
   - Dùng để phát hiện lỗi phần mềm

3. **Independent Watchdog Reset (IWDG)**
   - Tương tự WWDG nhưng hoạt động độc lập
   - Có thể hoạt động ngay cả khi clock chính gặp sự cố

4. **Software Reset**
   - Reset được kích hoạt bằng phần mềm
   - Thực hiện bằng cách ghi vào thanh ghi AIRCR

5. **Low-power Management Reset**
   - Reset khi điện áp nguồn không đủ
   - Bảo vệ chip khỏi hoạt động không ổn định

## 🛠️ Hardware Requirements
- Board phát triển STM32F411
- Programmer ST-Link V2
- Cáp USB
- IDE (khuyến nghị sử dụng STM32CubeIDE)

## ⚡ Clock System Overview
STM32F411 có hệ thống clock linh hoạt với nhiều nguồn:
- HSI (High-Speed Internal) - Dao động RC 16 MHz
- HSE (High-Speed External) - Thạch anh 4-26 MHz
- PLL (Phase-Locked Loop) - Lên đến 100 MHz
- LSI (Low-Speed Internal) - Dao động RC 32 kHz
- LSE (Low-Speed External) - Thạch anh 32.768 kHz

lowspeed để vào chế độ ngủ tắt xung clock highspeed để tiêu tốn điện năng ít, còn highspeed dùng cho run mode.
lowspeed dùng cho rtc và watchdog.

### Clock Tree (Cây Clock)
```
                   ┌─────────┐
                   │   HSI   │
                   │(16 MHz) │
                   └────┬────┘
                        │
┌─────────┐      ┌─────┴─────┐      ┌─────────┐
│   HSE   ├──────┤    PLL    ├──────┤  SYSCLK │
│(8-26MHz)│      │           │      │         │
└─────────┘      └───────────┘      └────┬────┘
                                         │
                                    ┌────┴────┐
                                    │  AHB    │
                                    │         │
                                    └────┬────┘
                                         │
                                    ┌────┴────┐
                                    │  APB1   │
                                    │  APB2   │
                                    └─────────┘
```

## 📝 Register Programming Guide

### Các Thanh Ghi RCC Chính
1. **RCC_CR (Thanh Ghi Điều Khiển Clock)**
   - Địa chỉ: `0x40023800`
   - Các bit quan trọng:
     - HSION: Bật clock tốc độ cao nội
     - HSEON: Bật clock tốc độ cao ngoại
     - PLLON: Bật PLL chính

2. **RCC_PLLCFGR (Thanh Ghi Cấu Hình PLL)**
   - Địa chỉ: `0x40023804`
   - Các bit quan trọng:
     - PLLM: Hệ số chia cho đầu vào PLL
     - PLLN: Hệ số nhân cho VCO
     - PLLP: Hệ số chia cho clock hệ thống chính

3. **RCC_CFGR (Thanh Ghi Cấu Hình Clock)**
   - Địa chỉ: `0x40023808`
   - Các bit quan trọng:
     - SW: Chuyển đổi clock hệ thống
     - HPRE: Bộ chia tần AHB
     - PPRE1: Bộ chia tần APB1
     - PPRE2: Bộ chia tần APB2

## ⚡ Flash Latency Configuration
Khi tăng tần số clock hệ thống, cần phải cấu hình Flash latency (wait states) phù hợp để đảm bảo Flash memory có thể hoạt động ổn định với tần số mới. Nếu không cấu hình đúng, có thể dẫn đến lỗi đọc Flash và hệ thống hoạt động không ổn định.

### Thanh Ghi FLASH_ACR (Access Control Register)
- Địa chỉ: `0x40023C00`
- Các bit quan trọng:
  - LATENCY[2:0]: Số wait states
    - 0 WS (000): 0 < HCLK ≤ 30MHz
    - 1 WS (001): 30MHz < HCLK ≤ 64MHz
    - 2 WS (010): 64MHz < HCLK ≤ 90MHz
    - 3 WS (011): 90MHz < HCLK ≤ 100MHz

### Quy Trình Cấu Hình An Toàn
1. Cấu hình Flash latency trước khi tăng tần số
2. Đợi bit LATENCY được cập nhật
3. Thực hiện cấu hình clock
4. Kiểm tra tần số hoạt động mới

### Ví Dụ Cấu Hình
```c
// Cấu hình Flash latency cho tần số 100MHz
#define FLASH_ACR (*(volatile uint32_t *)(0x40023C00))
FLASH_ACR |= (3 << 0);  // Set 3 wait states
while((FLASH_ACR & (3 << 0)) != (3 << 0));  // Đợi cập nhật
```

## 💻 Examples

### Cấu Hình Clock Cơ Bản
```c
// Cấu hình Flash latency trước
RCC->CR |= RCC_CR_HSEON;
#define FLASH_ACR (*(volatile uint32_t *)(0x40023C00))
FLASH_ACR |= (3 << 0);  // Set 3 wait states cho 100MHz
while((FLASH_ACR & (3 << 0)) != (3 << 0));  // Đợi cập nhật

// Bật HSE và đợi sẵn sàng
while(!(RCC->CR & RCC_CR_HSERDY));  // Đợi HSE sẵn sàng

// Cấu hình PLL
RCC->PLLCFGR = (
    (8 << RCC_PLLCFGR_PLLM_Pos) |    // PLLM = 8
    (336 << RCC_PLLCFGR_PLLN_Pos) |  // PLLN = 336
    (0 << RCC_PLLCFGR_PLLP_Pos)      // PLLP = 2
);

// Bật PLL
RCC->CR |= RCC_CR_PLLON;
while(!(RCC->CR & RCC_CR_PLLRDY));  // Đợi PLL sẵn sàng

// Chuyển sang PLL
RCC->CFGR |= 0x02;  // Chọn PLL làm clock hệ thống
while((RCC->CFGR & 0x0C) != 0x08);  // Đợi chuyển đổi hoàn tất
```

## 📚 References
1. [STM32F411 Reference Manual (RM0383)](https://www.st.com/resource/en/reference_manual/rm0383-stm32f411xce-advanced-armbased-32bit-mcus-stmicroelectronics.pdf)
2. [STM32F411 Datasheet](https://www.st.com/resource/en/datasheet/stm32f411ce.pdf)
3. [Clock Configuration Application Note (AN4621)](https://www.st.com/resource/en/application_note/an4621-getting-started-with-stm32f4xxxx-mcu-hardware-development-stmicroelectronics.pdf)

## 🔄 Clock Configuration Tips
1. Luôn kiểm tra cờ ready trước khi chuyển sang bước tiếp theo
2. Cấu hình flash wait states trước khi tăng tần số clock
3. Giữ tần số APB1 ≤ 50 MHz
4. Giữ tần số APB2 ≤ 100 MHz

## ⚠️ Important Notes
- Luôn tham khảo datasheet để biết thông số timing chính xác
- Đảm bảo nguồn cấp phù hợp cho hoạt động tần số cao
- Cấu hình flash wait states một cách thích hợp
- Tuân thủ nghiêm ngặt trình tự cấu hình clock

## 📝 License
Dự án này được cấp phép theo giấy phép MIT - xem file LICENSE để biết thêm chi tiết.

---
⭐ Nếu hướng dẫn này hữu ích cho bạn, hãy cho một ngôi sao nhé!
