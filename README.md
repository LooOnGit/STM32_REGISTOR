# STM32 Register Programming Project
# Dự án Lập trình Thanh ghi STM32

## Quy trình Biên dịch và Nạp chương trình
```
[File .c] --> [Compile (.o)] --> [Link (.elf)] --> [Binary (.bin)] --> [ST-Link] --> [STM32F4]
```

### Chi tiết các bước:

1. **File .c (Source Code)**
   - File mã nguồn chứa code C
   - Viết các hàm điều khiển thanh ghi
   - Cấu hình các chân GPIO, Timer, etc.

2. **Compile (.o)**
   - Biên dịch file .c thành file object (.o)
   - Kiểm tra cú pháp
   - Tối ưu hóa code
   - Tạo mã máy cho từng file riêng biệt

3. **Link (.elf - Executable and Linkable Format)**
   - Liên kết các file .o thành một file thực thi
   - Gán địa chỉ cụ thể cho code và data
   - Sắp xếp các section:
     + .text: chứa mã máy của các hàm
     + .data: chứa biến toàn cục đã khởi tạo
     + .bss: chứa biến toàn cục chưa khởi tạo
   - Tuân theo file linker script (.ld)

4. **Binary (.bin)**
   - Chuyển đổi file .elf thành binary
   - Tạo file nhị phân thuần túy
   - Sẵn sàng để nạp vào STM32

5. **ST-Link**
   - Công cụ nạp chương trình
   - Kết nối qua cổng SWD hoặc JTAG
   - Hỗ trợ debug trực tiếp

6. **STM32F4**
   - Vi điều khiển đích
   - Nhận và thực thi chương trình
   - Chạy từ bộ nhớ Flash

### Lệnh biên dịch cơ bản:
```bash
# Kiểm tra version gcc 
arm-none-eabi-gcc --version

# Biên dịch file .c thành .o
arm-none-eabi-gcc -c main.c -mcpu=cortex-m4 -mthumb -std=gnu11 -o main.o
- std(standard tùy công ty lựa chọn nên hỏi)
- tùy thêm cpu nào mà chọn cho đúng vì đang dùng stm32f411 nên là cortex-m4

# Liên kết thành file .elf
arm-none-eabi-gcc main.o -mcpu=cortex-m4 -mthumb -T STM32F411VETX_FLASH.ld -o program.elf

# Tạo file binary
arm-none-eabi-objcopy -O binary program.elf program.bin

# Nạp chương trình (sử dụng ST-Link)
st-flash write program.bin 0x08000000
```

### Giải thích các tham số biên dịch:
- `-mcpu=cortex-m4`: Chỉ định CPU là ARM Cortex-M4
- `-mthumb`: Sử dụng bộ lệnh Thumb (16/32-bit)
- `-std=gnu11`: Sử dụng chuẩn C GNU11
- `-T STM32F411VETX_FLASH.ld`: File linker script định nghĩa bố trí bộ nhớ
- `-c`: Chỉ biên dịch, không liên kết
- `-o`: Chỉ định tên file đầu ra

### Note
- Gcc là trình biên dịch cho máy tính (x86/x64)
- arm-none-eabi-gcc là trình biên dịch cho vi điều khiển ARM
- Cần thêm các tham số phù hợp với kiến trúc ARM Cortex-M4