################################################################################
# Automatically-generated file. Do not edit!
################################################################################

# Add inputs and outputs from these tool invocations to the build variables 
C_SRCS += \
../user_libs/fifo.c \
../user_libs/stm32f205_core.c \
../user_libs/stm32f205_gpio.c \
../user_libs/stm32f205_rcc.c \
../user_libs/stm32f205_uart.c 

OBJS += \
./user_libs/fifo.o \
./user_libs/stm32f205_core.o \
./user_libs/stm32f205_gpio.o \
./user_libs/stm32f205_rcc.o \
./user_libs/stm32f205_uart.o 

C_DEPS += \
./user_libs/fifo.d \
./user_libs/stm32f205_core.d \
./user_libs/stm32f205_gpio.d \
./user_libs/stm32f205_rcc.d \
./user_libs/stm32f205_uart.d 


# Each subdirectory must supply rules for building sources it contributes
user_libs/%.o: ../user_libs/%.c
	@echo 'Building file: $<'
	@echo 'Invoking: GNU ARM Cross C Compiler'
	arm-none-eabi-gcc -mcpu=cortex-m3 -mthumb -mthumb-interwork -Os -fmessage-length=0 -fsigned-char -ffunction-sections -fdata-sections -fno-common  -g -I"/media/dragomir/lnx_hdd/lnx_home/repos/mcu_libs/STM32F205/stm32f205_uart/user_libs" -I"/media/dragomir/lnx_hdd/lnx_home/repos/mcu_libs/STM32F205/stm32f205_uart/Debug" -I"/media/dragomir/lnx_hdd/lnx_home/repos/mcu_libs/STM32F205/stm32f205_uart/User" -std=gnu11 -MMD -MP -MF"$(@:%.o=%.d)" -MT"$(@)" -c -o "$@" "$<"
	@echo 'Finished building: $<'
	@echo ' '


