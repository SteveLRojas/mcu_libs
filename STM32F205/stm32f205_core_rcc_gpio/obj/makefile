################################################################################
# Automatically-generated file. Do not edit!
################################################################################

-include ../makefile.init

RM := rm -rf

# All of the sources participating in the build are defined here
-include sources.mk
-include user_libs/subdir.mk
-include User/subdir.mk
-include Startup/subdir.mk
-include subdir.mk
-include objects.mk

ifneq ($(MAKECMDGOALS),clean)
ifneq ($(strip $(ASM_DEPS)),)
-include $(ASM_DEPS)
endif
ifneq ($(strip $(S_UPPER_DEPS)),)
-include $(S_UPPER_DEPS)
endif
ifneq ($(strip $(C_DEPS)),)
-include $(C_DEPS)
endif
endif

-include ../makefile.defs

# Add inputs and outputs from these tool invocations to the build variables 
SECONDARY_FLASH += \
stm32f205_core_rcc_gpio.hex \

SECONDARY_LIST += \
stm32f205_core_rcc_gpio.lst \

SECONDARY_SIZE += \
stm32f205_core_rcc_gpio.siz \


# All Target
all: stm32f205_core_rcc_gpio.elf secondary-outputs

# Tool invocations
stm32f205_core_rcc_gpio.elf: $(OBJS) $(USER_OBJS)
	@echo 'Building target: $@'
	@echo 'Invoking: GNU ARM Cross C Linker'
	arm-none-eabi-gcc -mcpu=cortex-m3 -mthumb -mthumb-interwork -Os -fmessage-length=0 -fsigned-char -ffunction-sections -fdata-sections -fno-common  -g -T "C:\Users\Steve\mrs_community_workspace\stm32f205_core_rcc_gpio\LD\STM32F205VCTX_FLASH.ld" -nostartfiles -Xlinker --gc-sections -Wl,-Map,"stm32f205_core_rcc_gpio.map" --specs=nano.specs --specs=nosys.specs -o "stm32f205_core_rcc_gpio.elf" $(OBJS) $(USER_OBJS) $(LIBS)
	@echo 'Finished building target: $@'
	@echo ' '

stm32f205_core_rcc_gpio.hex: stm32f205_core_rcc_gpio.elf
	@echo 'Invoking: GNU ARM Cross Create Flash Image'
	arm-none-eabi-objcopy -O ihex "stm32f205_core_rcc_gpio.elf"  "stm32f205_core_rcc_gpio.hex"
	@echo 'Finished building: $@'
	@echo ' '

stm32f205_core_rcc_gpio.lst: stm32f205_core_rcc_gpio.elf
	@echo 'Invoking: GNU ARM Cross Create Listing'
	arm-none-eabi-objdump --source --all-headers --demangle --disassemble "stm32f205_core_rcc_gpio.elf" > "stm32f205_core_rcc_gpio.lst"
	@echo 'Finished building: $@'
	@echo ' '

stm32f205_core_rcc_gpio.siz: stm32f205_core_rcc_gpio.elf
	@echo 'Invoking: GNU ARM Cross Print Size'
	arm-none-eabi-size --format=berkeley "stm32f205_core_rcc_gpio.elf"
	@echo 'Finished building: $@'
	@echo ' '

# Other Targets
clean:
	-$(RM) $(OBJS)$(SECONDARY_FLASH)$(SECONDARY_LIST)$(SECONDARY_SIZE)$(ASM_DEPS)$(S_UPPER_DEPS)$(C_DEPS) stm32f205_core_rcc_gpio.elf
	-@echo ' '

secondary-outputs: $(SECONDARY_FLASH) $(SECONDARY_LIST) $(SECONDARY_SIZE)

.PHONY: all clean dependents

-include ../makefile.targets
