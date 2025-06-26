# All:
# 	arm-none-eabi-gcc -c main.c $(HEADER) -o build/main.o
# 	arm-none-eabi-gcc -c Custom/Src/Led.c $(HEADER) -o build/Led.o
# 	arm-none-eabi-gcc -c Custom/Src/clock.c $(HEADER) -o build/clock.o
# 	arm-none-eabi-gcc -c Custom/Src/Delay.c $(HEADER) -o build/Delay.o
# 	arm-none-eabi-gcc -c Custom/Src/capture.c $(HEADER) -o build/capture.o
# 	arm-none-eabi-gcc -c Custom/Src/ADC.c $(HEADER) -o build/ADC.o
# 	arm-none-eabi-gcc -c Custom/Src/Usart.c $(HEADER) -o build/Usart.o
# 	# arm-none-eabi-gcc -c -x assembler-with-cpp startup_stm32f411vetx.s -mcpu=cortex-m4 -std=gnu11 -o build/startup.o
# 	arm-none-eabi-gcc D:\Dev_STM32\stm32_vs\build\ADC.o D:\Dev_STM32\stm32_vs\build\capture.o D:\Dev_STM32\stm32_vs\build\clock.o D:\Dev_STM32\stm32_vs\build\Delay.o D:\Dev_STM32\stm32_vs\build\Led.o D:\Dev_STM32\stm32_vs\build\main.o D:\Dev_STM32\stm32_vs\build\startup.o $(HEADER) -T"STM32F411VETX_FLASH.ld" -Wl,-Map="file.map" -Wl,--gc-sections -static -o build/blink_led.elf
# 	arm-none-eabi-objcopy -O ihex build/blink_led.elf build/blink_led.hex
# 	arm-none-eabi-objcopy -O binary build/blink_led.elf build/blink_led.bin
# clean:
# 	rmdir /q /s build
# 	mkdir build


# Flash:
# 	ST-LINK_CLI -c SWD -P build/blink_led.hex -V -Run

All:
	arm-none-eabi-gcc -c main.c -mcpu=cortex-m4 -std=gnu11 -IDriver\Inc -o build/main.o
	arm-none-eabi-gcc -c Driver\Src\Led.c -mcpu=cortex-m4 -std=gnu11 -IDriver\Inc -o build/led.o
	arm-none-eabi-gcc -c Driver\Src\ADC.c -mcpu=cortex-m4 -std=gnu11 -IDriver\Inc -o build/ADC.o
	arm-none-eabi-gcc -c Driver\Src\clock.c -mcpu=cortex-m4 -std=gnu11 -IDriver\Inc -o build/clock.o
	arm-none-eabi-gcc -c Driver\Src\delay.c -mcpu=cortex-m4 -std=gnu11 -IDriver\Inc -o build/delay.o
	arm-none-eabi-gcc -c Driver\Src\Usart.c -mcpu=cortex-m4 -std=gnu11 -IDriver\Inc -o build/Usart.o
	arm-none-eabi-gcc -c Driver\Src\capture.c -mcpu=cortex-m4 -std=gnu11 -IDriver\Inc -o build/capture.o
	arm-none-eabi-gcc -c -x assembler-with-cpp startup_stm32f411vetx.s -mcpu=cortex-m4 -std=gnu11 -o build/startup.o
	arm-none-eabi-gcc build\ADC.o build\capture.o build\clock.o build\Delay.o build\Led.o build\main.o build\startup.o -T"STM32F411VETX_FLASH.ld" -Wl,-Map="file.map" -Wl,--gc-sections -static -o build/blink_led.elf
	arm-none-eabi-objcopy -O ihex build/blink_led.elf build/blink_led.hex
	arm-none-eabi-objcopy -O binary build/blink_led.elf build/blink_led.bin
Clean:
	rmdir /q /s build
	mkdir build