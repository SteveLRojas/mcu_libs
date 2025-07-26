################################################################################
# Automatically-generated file. Do not edit!
################################################################################

# Add inputs and outputs from these tool invocations to the build variables 
C_SRCS += \
../User/main.c \
../User/support.c 

OBJS += \
./User/main.o \
./User/support.o 

C_DEPS += \
./User/main.d \
./User/support.d 


# Each subdirectory must supply rules for building sources it contributes
User/%.o: ../User/%.c
	@echo 'Building file: $<'
	@echo 'Invoking: GNU RISC-V Cross C Compiler'
	riscv-none-embed-gcc -march=rv32imac -mabi=ilp32 -msmall-data-limit=8 -msave-restore -Os -fmessage-length=0 -fsigned-char -ffunction-sections -fdata-sections -fno-common -Wunused -Wuninitialized  -g -I"C:\Users\Steve\repos\mcu_libs\CH32V203\ch32v203_dma\user_libs" -I"C:\Users\Steve\repos\mcu_libs\CH32V203\ch32v203_dma\Debug" -I"C:\Users\Steve\repos\mcu_libs\CH32V203\ch32v203_dma\User" -std=gnu99 -MMD -MP -MF"$(@:%.o=%.d)" -MT"$(@)" -c -o "$@" "$<"
	@echo 'Finished building: $<'
	@echo ' '


