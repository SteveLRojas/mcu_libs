################################################################################
# Automatically-generated file. Do not edit!
################################################################################

# Add inputs and outputs from these tool invocations to the build variables 
C_SRCS += \
../User/main.c 

OBJS += \
./User/main.o 

C_DEPS += \
./User/main.d 


# Each subdirectory must supply rules for building sources it contributes
User/%.o: ../User/%.c
	@echo 'Building file: $<'
	@echo 'Invoking: GNU RISC-V Cross C Compiler'
	riscv-none-embed-gcc -march=rv32imac -mabi=ilp32 -msmall-data-limit=8 -msave-restore -Os -fmessage-length=0 -fsigned-char -ffunction-sections -fdata-sections -fno-common -Wunused -Wuninitialized  -g -I"/media/dragomir/lnx_hdd/lnx_home/mrs_community-workspace/ch32v203_usbd_cdc_stress_test/Debug" -I"/media/dragomir/lnx_hdd/lnx_home/mrs_community-workspace/ch32v203_usbd_cdc_stress_test/User" -I"/media/dragomir/lnx_hdd/lnx_home/mrs_community-workspace/ch32v203_usbd_cdc_stress_test/user_libs" -std=gnu99 -MMD -MP -MF"$(@:%.o=%.d)" -MT"$(@)" -c -o "$@" "$<"
	@echo 'Finished building: $<'
	@echo ' '


