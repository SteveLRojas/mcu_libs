################################################################################
# Automatically-generated file. Do not edit!
################################################################################

# Add inputs and outputs from these tool invocations to the build variables 
C_SRCS += \
../user_libs/ch32v307_core.c \
../user_libs/ch32v307_gpio.c \
../user_libs/ch32v307_rcc.c 

OBJS += \
./user_libs/ch32v307_core.o \
./user_libs/ch32v307_gpio.o \
./user_libs/ch32v307_rcc.o 

C_DEPS += \
./user_libs/ch32v307_core.d \
./user_libs/ch32v307_gpio.d \
./user_libs/ch32v307_rcc.d 


# Each subdirectory must supply rules for building sources it contributes
user_libs/%.o: ../user_libs/%.c
	@echo 'Building file: $<'
	@echo 'Invoking: GNU RISC-V Cross C Compiler'
	riscv-none-embed-gcc -march=rv32imac -mabi=ilp32 -msmall-data-limit=8 -msave-restore -Os -fmessage-length=0 -fsigned-char -ffunction-sections -fdata-sections -fno-common -Wunused -Wuninitialized  -g -I"C:\Users\Steve\mrs_community_workspace\ch32v307_rcc_gpio\user_libs" -I"C:\Users\Steve\mrs_community_workspace\ch32v307_rcc_gpio\Debug" -I"C:\Users\Steve\mrs_community_workspace\ch32v307_rcc_gpio\User" -std=gnu99 -MMD -MP -MF"$(@:%.o=%.d)" -MT"$(@)" -c -o "$@" "$<"
	@echo 'Finished building: $<'
	@echo ' '


