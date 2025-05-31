# STM32 Register Programming Learning Project

Repository này được tạo ra để học và thực hành lập trình STM32 sử dụng thanh ghi (Register Programming).

## Mục tiêu

- Hiểu sâu về kiến trúc và cách hoạt động của vi điều khiển STM32
- Thực hành lập trình trực tiếp với thanh ghi (không sử dụng HAL/LL libraries)
- Xây dựng các ví dụ từ cơ bản đến nâng cao
- Tạo tài liệu tham khảo cho việc học STM32

## Nội dung khóa học

### Bài 1: GPIO (General Purpose Input/Output)

1. Tổng quan về GPIO
   - GPIO là gì?
     * Các chân vào/ra đa năng
     * Có thể cấu hình là input hoặc output
     Input có các chể độ:
     ![alt text](image.png)
     Output có các chế độ:
     ![alt text](image-1.png)
     * Điều khiển/đọc tín hiệu số (0V hoặc 3.3V)
   
   - Các tính năng của GPIO
     * Cấu hình được input/output
     * Push-pull hoặc open-drain output
     * Pull-up/pull-down internal resistors
     * Tốc độ đóng mở có thể cấu hình
     * Alternate function (kết nối với các ngoại vi khác)

2. Cấu trúc GPIO
   - Mỗi PORT có 16 chân (PIN0 đến PIN15)
   - Mỗi PIN có các thanh ghi điều khiển:
     * MODE: Input/Output/Alternate/Analog
     * TYPE: Push-pull/Open-drain
     * SPEED: Low/Medium/High/Very high
     * PULL: No pull/Pull-up/Pull-down
     * IDR: Input Data Register
     * ODR: Output Data Register
     * BSRR: Bit Set/Reset Register

3. Các bước lập trình GPIO
   a. Enable clock cho GPIO port
      * Sử dụng thanh ghi RCC_AHB1ENR
      * Set bit tương ứng với PORT cần dùng
   
   b. Cấu hình MODE (Input/Output)
      * Sử dụng GPIOx_MODER
      * 00: Input mode
      * 01: Output mode
      * 10: Alternate function
      * 11: Analog mode
   
   c. Cấu hình Output Type (nếu là output)
      * Sử dụng GPIOx_OTYPER
      * 0: Push-pull
      * 1: Open-drain
   
   d. Cấu hình Pull-up/Pull-down (nếu cần)
      * Sử dụng GPIOx_PUPDR
      * 00: No pull-up/pull-down
      * 01: Pull-up
      * 10: Pull-down
   
   e. Điều khiển GPIO
      * Đọc input: GPIOx_IDR
      * Ghi output: GPIOx_ODR hoặc GPIOx_BSRR

4. Ví dụ thực hành
   - Điều khiển LED
     * Cấu hình GPIO là output
     * Sử dụng push-pull mode
     * Điều khiển bằng BSRR để bật/tắt LED
   
   - Đọc nút nhấn
     * Cấu hình GPIO là input
     * Sử dụng pull-up hoặc pull-down
     * Đọc trạng thái từ IDR

5. Lưu ý quan trọng
   - Luôn enable clock trước khi cấu hình
   - Cẩn thận với điện áp và dòng điện tối đa
   - Không để pin ở trạng thái floating
   - Sử dụng BSRR thay vì ODR để atomic access

**Note:** 
- Trong diện tử digital mức 0 và mức 1 theo tiêu chuẩn TTL (0: 0- 0.8V, 1: 2.4 - VCC).
- Điện chảy từ nói có điện áp cao về nơi có điện áp thấp.

### Bài 2: EXTI (External Interrupts)

1. Tổng quan về EXTI
   - EXTI là gì?
     * External Interrupt/Event Controller
     * Cho phép tạo ngắt từ các nguồn bên ngoài
     * 23 đường EXTI (EXTI0 đến EXTI22)
     * Mỗi GPIO pin có thể kết nối với một line EXTI tương ứng

   - Đặc điểm của EXTI
     * Có thể cấu hình cạnh kích hoạt (rising/falling)
     * Có thể hoạt động ở chế độ event hoặc interrupt
     * Độc lập với các chức năng GPIO khác
     * Có thể đánh thức MCU từ sleep mode

2. Cấu trúc EXTI
   - EXTI line selection
   - Edge trigger selection
   - Interrupt mask register
   - Event mask register
   - Pending register

3. Các bước cấu hình EXTI
   a. Cấu hình GPIO
      * Enable clock cho GPIO port
      * Cấu hình pin là input mode
      * Cấu hình pull-up/pull-down nếu cần
   
   b. Cấu hình SYSCFG
      * Enable clock cho SYSCFG
      * Chọn GPIO port cho EXTI line (SYSCFG_EXTICR)
   
   c. Cấu hình EXTI
      * Chọn cạnh kích hoạt (EXTI_RTSR/FTSR)
      * Enable interrupt hoặc event (EXTI_IMR/EMR)
      * Clear pending flag nếu có
   
   d. Cấu hình NVIC
      * Set priority cho interrupt
      * Enable interrupt trong NVIC

4. Các thanh ghi quan trọng
   - SYSCFG_EXTICR1-4: External interrupt configuration
   - EXTI_IMR: Interrupt mask register
   - EXTI_EMR: Event mask register
   - EXTI_RTSR: Rising trigger selection
   - EXTI_FTSR: Falling trigger selection
   - EXTI_PR: Pending register

5. Xử lý ngắt EXTI
   - Kiểm tra pending flag
   - Thực hiện công việc cần thiết
   - Clear pending flag
   - Debouncing nếu cần (với nút nhấn)

6. Lưu ý quan trọng
   - Mỗi EXTI line chỉ kết nối được với một GPIO pin
   - Cần xử lý ngắt nhanh để tránh miss event
   - Nên có debouncing với input cơ học
   - Clear pending flag trước khi enable interrupt

7. Luồng hoạt động của EXTI
   a. Khi có sự kiện xảy ra:
      * Tín hiệu từ GPIO pin được đưa vào EXTI line tương ứng
      * EXTI controller kiểm tra điều kiện kích hoạt (rising/falling edge)
      * Nếu match điều kiện, EXTI set pending flag (EXTI_PR)
      * Nếu interrupt được enable (trong EXTI_IMR), ngắt được gửi tới NVIC
      * NVIC kiểm tra priority và trạng thái enable
      * CPU nhảy tới vector table và thực thi ISR tương ứng
      

   Sơ đồ khối của EXTI:
   ```
   ┌──────────────────────────────────┐
   │              ARM                 │
   │                           ┌────┐ │
   │                      ├────┤NVIC│ │
   │                           └────┘ │
   │         Interrupt Event          │
   │              ▲                   │
   │              │                   │
   │         ┌────┴─────┐            │
   │         │   EXTI   │            │
   │         └────┬─────┘            │
   │              │                   │
   │         ┌────┴─────┐            │
   │         │  GPIOx   │            │
   │         └──────────┘            │
   │        STM32F411               │
   └──────────────────────────────────┘
   GPIO → EXTI → NVIC → ARM Core
   ```

   b. Trong Interrupt Service Routine (ISR):
      * Kiểm tra pending flag để xác định EXTI line nào gây ngắt
      * Thực hiện công việc cần thiết
      * Clear pending flag bằng cách ghi 1 vào bit tương ứng trong EXTI_PR
      * CPU trở về thực thi chương trình chính

   c. Ví dụ với nút nhấn:
      * Khi nút được nhấn, tạo ra falling edge
      * EXTI phát hiện falling edge và set pending flag
      * CPU nhảy vào ISR
      * Trong ISR: Toggle LED, clear flag
      * Quay lại chương trình chính

## Tài liệu tham khảo

- [STM32F411xC/E Reference Manual (RM0383)](https://www.st.com/resource/en/reference_manual/dm00119316-stm32f411xc-e-advanced-arm-based-32-bit-mcus-stmicroelectronics.pdf)  
  *Note: Tài liệu chính để tra cứu chi tiết về thanh ghi, cách cấu hình và hoạt động của các peripheral như GPIO, UART, Timer,...*

- [STM32F411xC/E Datasheet](https://www.st.com/resource/en/datasheet/stm32f411ce.pdf)  
  *Note: Chứa thông tin về đặc tính điện, sơ đồ chân, điều kiện hoạt động và giới hạn của chip*

- [ARM Cortex-M4 Technical Reference Manual](https://developer.arm.com/documentation/100166/0001)  
  *Note: Tài liệu về kiến trúc CPU, cách hoạt động của core, hệ thống ngắt và các chế độ hoạt động của Cortex-M4*

- [Programming Manual (PM0214)](https://www.st.com/resource/en/programming_manual/dm00046982-stm32-cortex-m4-mcus-and-mpus-programming-manual-stmicroelectronics.pdf)  
  *Note: Hướng dẫn lập trình, tập lệnh assembly và các kỹ thuật tối ưu cho Cortex-M4*

## Cấu trúc thư mục

```
project/
├── docs/
│   ├── datasheets/          # Chứa các datasheet và reference manuals
│   └── examples/            # Các ví dụ mã nguồn
├── src/
│   ├── 01_gpio_basic/      # Ví dụ GPIO cơ bản
│   ├── 02_clock_config/    # Cấu hình clock system
│   └── 03_interrupts/      # Xử lý ngắt
└── templates/              # Các template project
```

## License

MIT License