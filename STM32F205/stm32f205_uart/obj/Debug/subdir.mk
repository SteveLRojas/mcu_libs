################################################################################
# Automatically-generated file. Do not edit!
################################################################################

# Add inputs and outputs from these tool invocations to the build variables 
C_SRCS += \
../Debug/debug.c 

OBJS += \
./Debug/debug.o 

C_DEPS += \
./Debug/debug.d 


# Each subdirectory must supply rules for building sources it contributes
Debug/%.o: ../Debug/%.c
	@echo 'Building file: $<'
	@echo 'Invoking: GNU ARM Cross C Compiler'
	arm-none-eabi-gcc -mcpu=cortex-m3 -mthumb -mthumb-interwork -Os -fmessage-length=0 -fsigned-char -ffunction-sections -fdata-sections -fno-common  -g -I"/media/dragomir/lnx_hdd/lnx_home/repos/mcu_libs/STM32F205/stm32f205_uart/user_libs" -I"/media/dragomir/lnx_hdd/lnx_home/repos/mcu_libs/STM32F205/stm32f205_uart/Debug" -I"/media/dragomir/lnx_hdd/lnx_home/repos/mcu_libs/STM32F205/stm32f205_uart/User" -std=gnu11 -MMD -MP -MF"$(@:%.o=%.d)" -MT"$(@)" -c -o "$@" "$<"
	@echo 'Finished building: $<'
	@echo ' '


