FREERTOS_DIR = FreeRTOS-LTS/FreeRTOS/FreeRTOS-Kernel
HEADER = -mfpu=fpv4-sp-d16 -mthumb -mfloat-abi=hard -mcpu=cortex-m4 -std=gnu11 -IDriver\Inc -I$(FREERTOS_DIR)/include/ -I. -I$(FREERTOS_DIR)/portable/GCC/ARM_CM4F

All:
	arm-none-eabi-gcc -c $(FREERTOS_DIR)/croutine.c $(HEADER) -o build/croutine.o
	arm-none-eabi-gcc -c $(FREERTOS_DIR)/event_groups.c $(HEADER) -o build/event_groups.o
	arm-none-eabi-gcc -c $(FREERTOS_DIR)/list.c $(HEADER) -o build/list.o
	arm-none-eabi-gcc -c $(FREERTOS_DIR)/queue.c $(HEADER) -o build/queue.o
	arm-none-eabi-gcc -c $(FREERTOS_DIR)/stream_buffer.c $(HEADER) -o build/stream_buffer.o
	arm-none-eabi-gcc -c $(FREERTOS_DIR)/tasks.c $(HEADER) -o build/tasks.o
	arm-none-eabi-gcc -c $(FREERTOS_DIR)/timers.c $(HEADER) -o build/timers.o
	arm-none-eabi-gcc -c $(FREERTOS_DIR)/portable/GCC/ARM_CM4F/port.c $(HEADER) -o build/port.o
	arm-none-eabi-gcc -c $(FREERTOS_DIR)/portable/MemMang/heap_4.c $(HEADER) -o build/heap_4.o

	arm-none-eabi-gcc -c main.c $(HEADER) -o build/main.o
	arm-none-eabi-gcc -c Driver\Src\Led.c $(HEADER) -o build/led.o
	arm-none-eabi-gcc -c Driver\Src\ADC.c $(HEADER) -o build/ADC.o
	arm-none-eabi-gcc -c Driver\Src\clock.c $(HEADER) -o build/clock.o
	arm-none-eabi-gcc -c Driver\Src\delay.c $(HEADER) -o build/delay.o
	arm-none-eabi-gcc -c Driver\Src\Usart.c $(HEADER) -o build/Usart.o
	arm-none-eabi-gcc -c Driver\Src\capture.c $(HEADER) -o build/capture.o
	arm-none-eabi-gcc -c Driver\Src\pwm.c $(HEADER) -o build/pwm.o
	arm-none-eabi-gcc -c Driver\Src\low_power.c $(HEADER) -o build/low_power.o
	arm-none-eabi-gcc -c -x assembler-with-cpp startup_stm32f411vetx.s -mcpu=cortex-m4 -std=gnu11 -o build/startup.o
	arm-none-eabi-gcc build\ADC.o build\capture.o build\clock.o build\Delay.o build\Led.o build\pwm.o build\main.o build\startup.o build\low_power.o build/croutine.o build/event_groups.o build/list.o build/queue.o build/stream_buffer.o build/tasks.o build/timers.o build/port.o build/heap_4.o $(HEADER) -T"STM32F411VETX_FLASH.ld" -Wl,-Map="file.map" -Wl,--gc-sections -static -o build/blink_led.elf
	arm-none-eabi-objcopy -O ihex build/blink_led.elf build/blink_led.hex
	arm-none-eabi-objcopy -O binary build/blink_led.elf build/blink_led.bin
Clean:
	rmdir /q /s build
	mkdir build
Flash:
	ST-LINK_CLI -c SWD -P build/blink_led.hex -V -Run