################################################################################
# Automatically-generated file. Do not edit!
# Toolchain: GNU Tools for STM32 (12.3.rel1)
################################################################################

# Add inputs and outputs from these tool invocations to the build variables 
C_SRCS += \
../Custom_Driver/Src/led.c 

OBJS += \
./Custom_Driver/Src/led.o 

C_DEPS += \
./Custom_Driver/Src/led.d 


# Each subdirectory must supply rules for building sources it contributes
Custom_Driver/Src/%.o Custom_Driver/Src/%.su Custom_Driver/Src/%.cyclo: ../Custom_Driver/Src/%.c Custom_Driver/Src/subdir.mk
	arm-none-eabi-gcc "$<" -mcpu=cortex-m4 -std=gnu11 -g3 -DDEBUG -DUSE_HAL_DRIVER -DSTM32F411xE -c -I../Core/Inc -I../Drivers/STM32F4xx_HAL_Driver/Inc -I../Drivers/STM32F4xx_HAL_Driver/Inc/Legacy -I../Drivers/CMSIS/Device/ST/STM32F4xx/Include -I../Drivers/CMSIS/Include -O0 -ffunction-sections -fdata-sections -Wall -fstack-usage -fcyclomatic-complexity -MMD -MP -MF"$(@:%.o=%.d)" -MT"$@" --specs=nano.specs -mfpu=fpv4-sp-d16 -mfloat-abi=hard -mthumb -o "$@"

clean: clean-Custom_Driver-2f-Src

clean-Custom_Driver-2f-Src:
	-$(RM) ./Custom_Driver/Src/led.cyclo ./Custom_Driver/Src/led.d ./Custom_Driver/Src/led.o ./Custom_Driver/Src/led.su

.PHONY: clean-Custom_Driver-2f-Src

