################################################################################
# Automatically-generated file. Do not edit!
################################################################################

# Add inputs and outputs from these tool invocations to the build variables 
C_SRCS += \
../user_libs/stm32f205_core.c \
../user_libs/stm32f205_gpio.c \
../user_libs/stm32f205_rcc.c 

OBJS += \
./user_libs/stm32f205_core.o \
./user_libs/stm32f205_gpio.o \
./user_libs/stm32f205_rcc.o 

C_DEPS += \
./user_libs/stm32f205_core.d \
./user_libs/stm32f205_gpio.d \
./user_libs/stm32f205_rcc.d 


# Each subdirectory must supply rules for building sources it contributes
user_libs/%.o: ../user_libs/%.c
	@echo 'Building file: $<'
	@echo 'Invoking: GNU ARM Cross C Compiler'
	arm-none-eabi-gcc -mcpu=cortex-m3 -mthumb -mthumb-interwork -Os -fmessage-length=0 -fsigned-char -ffunction-sections -fdata-sections -fno-common  -g -I"C:\Users\Steve\mrs_community_workspace\stm32f205_core_rcc_gpio\user_libs" -I"C:\Users\Steve\mrs_community_workspace\stm32f205_core_rcc_gpio\User" -std=gnu11 -MMD -MP -MF"$(@:%.o=%.d)" -MT"$(@)" -c -o "$@" "$<"
	@echo 'Finished building: $<'
	@echo ' '


