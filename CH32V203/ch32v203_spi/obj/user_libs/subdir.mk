################################################################################
# Automatically-generated file. Do not edit!
################################################################################

# Add inputs and outputs from these tool invocations to the build variables 
C_SRCS += \
../user_libs/ch32v203_core.c \
../user_libs/ch32v203_gpio.c \
../user_libs/ch32v203_rcc.c \
../user_libs/ch32v203_spi.c \
../user_libs/ch32v203_uart.c \
../user_libs/fifo.c 

OBJS += \
./user_libs/ch32v203_core.o \
./user_libs/ch32v203_gpio.o \
./user_libs/ch32v203_rcc.o \
./user_libs/ch32v203_spi.o \
./user_libs/ch32v203_uart.o \
./user_libs/fifo.o 

C_DEPS += \
./user_libs/ch32v203_core.d \
./user_libs/ch32v203_gpio.d \
./user_libs/ch32v203_rcc.d \
./user_libs/ch32v203_spi.d \
./user_libs/ch32v203_uart.d \
./user_libs/fifo.d 


# Each subdirectory must supply rules for building sources it contributes
user_libs/%.o: ../user_libs/%.c
	@echo 'Building file: $<'
	@echo 'Invoking: GNU RISC-V Cross C Compiler'
	riscv-none-embed-gcc -march=rv32imac -mabi=ilp32 -msmall-data-limit=8 -msave-restore -Os -fmessage-length=0 -fsigned-char -ffunction-sections -fdata-sections -fno-common -Wunused -Wuninitialized  -g -I"C:\Users\Steve\repos\mcu_libs\CH32V203\ch32v203_spi\Debug" -I"C:\Users\Steve\repos\mcu_libs\CH32V203\ch32v203_spi\user_libs" -I"C:\Users\Steve\repos\mcu_libs\CH32V203\ch32v203_spi\User" -std=gnu99 -MMD -MP -MF"$(@:%.o=%.d)" -MT"$(@)" -c -o "$@" "$<"
	@echo 'Finished building: $<'
	@echo ' '


