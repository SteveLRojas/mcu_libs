################################################################################
# Automatically-generated file. Do not edit!
################################################################################

# Add inputs and outputs from these tool invocations to the build variables 
C_SRCS += \
../user_libs/ch32v203_adc.c \
../user_libs/ch32v203_core.c \
../user_libs/ch32v203_dma.c \
../user_libs/ch32v203_exti.c \
../user_libs/ch32v203_gpio.c \
../user_libs/ch32v203_rcc.c \
../user_libs/ch32v203_spi.c \
../user_libs/ch32v203_st7735s.c \
../user_libs/ch32v203_timer.c \
../user_libs/ch32v203_uart.c \
../user_libs/ch32v203_usbd.c \
../user_libs/ch32v203_usbd_cdc.c \
../user_libs/ch32v203_usbfsd.c \
../user_libs/ch32v203_usbfsh.c \
../user_libs/fifo.c \
../user_libs/pseudo_random.c 

OBJS += \
./user_libs/ch32v203_adc.o \
./user_libs/ch32v203_core.o \
./user_libs/ch32v203_dma.o \
./user_libs/ch32v203_exti.o \
./user_libs/ch32v203_gpio.o \
./user_libs/ch32v203_rcc.o \
./user_libs/ch32v203_spi.o \
./user_libs/ch32v203_st7735s.o \
./user_libs/ch32v203_timer.o \
./user_libs/ch32v203_uart.o \
./user_libs/ch32v203_usbd.o \
./user_libs/ch32v203_usbd_cdc.o \
./user_libs/ch32v203_usbfsd.o \
./user_libs/ch32v203_usbfsh.o \
./user_libs/fifo.o \
./user_libs/pseudo_random.o 

C_DEPS += \
./user_libs/ch32v203_adc.d \
./user_libs/ch32v203_core.d \
./user_libs/ch32v203_dma.d \
./user_libs/ch32v203_exti.d \
./user_libs/ch32v203_gpio.d \
./user_libs/ch32v203_rcc.d \
./user_libs/ch32v203_spi.d \
./user_libs/ch32v203_st7735s.d \
./user_libs/ch32v203_timer.d \
./user_libs/ch32v203_uart.d \
./user_libs/ch32v203_usbd.d \
./user_libs/ch32v203_usbd_cdc.d \
./user_libs/ch32v203_usbfsd.d \
./user_libs/ch32v203_usbfsh.d \
./user_libs/fifo.d \
./user_libs/pseudo_random.d 


# Each subdirectory must supply rules for building sources it contributes
user_libs/%.o: ../user_libs/%.c
	@echo 'Building file: $<'
	@echo 'Invoking: GNU RISC-V Cross C Compiler'
	riscv-none-embed-gcc -march=rv32imac -mabi=ilp32 -msmall-data-limit=8 -msave-restore -Os -fmessage-length=0 -fsigned-char -ffunction-sections -fdata-sections -fno-common -Wunused -Wuninitialized  -g -I"/home/dragomir/mrs_community-workspace/ch32v203_st7735s/Debug" -I"/home/dragomir/mrs_community-workspace/ch32v203_st7735s/User" -I"/home/dragomir/mrs_community-workspace/ch32v203_st7735s/user_libs" -std=gnu99 -MMD -MP -MF"$(@:%.o=%.d)" -MT"$(@)" -c -o "$@" "$<"
	@echo 'Finished building: $<'
	@echo ' '


