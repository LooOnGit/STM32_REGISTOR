# STM32 Register Programming Learning Project

Repository này được tạo ra để học và thực hành lập trình STM32 sử dụng thanh ghi (Register Programming).

## Mục tiêu

- Hiểu sâu về kiến trúc và cách hoạt động của vi điều khiển STM32
- Thực hành lập trình trực tiếp với thanh ghi (không sử dụng HAL/LL libraries)
- Xây dựng các ví dụ từ cơ bản đến nâng cao
- Tạo tài liệu tham khảo cho việc học STM32

## Nội dung khóa học

### Bài 1: Cài đặt và Cấu hình Môi trường Phát triển
1. Cài đặt công cụ cần thiết
   - GNU ARM Embedded Toolchain
     * Tải từ ARM Developer website
     * Thêm vào PATH system
     * Kiểm tra bằng lệnh `arm-none-eabi-gcc --version`
   
   - Build Tools
     * MinGW và MSYS2 cho Windows
     * Make utility
     * Kiểm tra bằng lệnh `make --version`
   
   - OpenOCD cho debug và flash
     * Tải từ GIT repository hoặc binary release
     * Thêm vào PATH system
     * Kiểm tra bằng lệnh `openocd --version`
   
   - STM32CubeProgrammer (tùy chọn)
     * Dùng để nạp chương trình
     * Công cụ hỗ trợ debug

2. Cấu trúc project cơ bản
   ```
   project/
   ├── Makefile
   ├── src/
   │   └── main.c
   ├── startup/
   │   └── startup_stm32f103c8tx.s
   └── linker/
       └── STM32F103C8TX_FLASH.ld
   ```

3. Tạo Makefile cơ bản
   ```makefile
   # Compiler settings
   CC = arm-none-eabi-gcc
   OBJCOPY = arm-none-eabi-objcopy
   OBJDUMP = arm-none-eabi-objdump
   
   # MCU settings
   CPU = -mcpu=cortex-m3
   FPU = -mfpu=softvfp
   FLOAT-ABI = -mfloat-abi=soft
   MCU = $(CPU) -mthumb $(FPU) $(FLOAT-ABI)
   
   # Compiler flags
   CFLAGS = $(MCU)
   CFLAGS += -Wall -Wextra
   CFLAGS += -g3 -O0
   CFLAGS += -ffunction-sections -fdata-sections
   
   # Linker settings
   LDSCRIPT = linker/STM32F103C8TX_FLASH.ld
   LIBS = -lc -lm -lnosys
   LDFLAGS = $(MCU) -specs=nano.specs -T$(LDSCRIPT) $(LIBS)
   LDFLAGS += -Wl,-Map=build/$(PROJECT).map,--cref -Wl,--gc-sections
   ```

4. Startup file và Vector Table
   - Cấu trúc của startup file
   - Vector table cho interrupts
   - Reset handler và system initialization

5. Linker Script
   - Memory regions (FLASH, RAM)
   - Section definitions
   - Entry point configuration

6. Hello World Project
   - Cấu trúc main.c cơ bản
   - System clock configuration
   - GPIO initialization
   - LED blinking example

7. Build và Flash
   - Compile project: `make`
   - Flash sử dụng OpenOCD
   - Debug với GDB

## Tham khảo

- [GNU ARM Embedded Toolchain](https://developer.arm.com/tools-and-software/open-source-software/developer-tools/gnu-toolchain/gnu-rm)
- [OpenOCD Documentation](http://openocd.org/documentation/)
- [STM32F103 Reference Manual](https://www.st.com/resource/en/reference_manual/cd00171190-stm32f101xx-stm32f102xx-stm32f103xx-stm32f105xx-and-stm32f107xx-advanced-armbased-32bit-mcus-stmicroelectronics.pdf)

## Cấu trúc Repository

```