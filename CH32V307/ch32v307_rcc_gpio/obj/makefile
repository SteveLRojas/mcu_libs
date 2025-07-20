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
ch32v307_rcc_gpio.hex \

SECONDARY_LIST += \
ch32v307_rcc_gpio.lst \

SECONDARY_SIZE += \
ch32v307_rcc_gpio.siz \


# All Target
all: ch32v307_rcc_gpio.elf secondary-outputs

# Tool invocations
ch32v307_rcc_gpio.elf: $(OBJS) $(USER_OBJS)
	@echo 'Building target: $@'
	@echo 'Invoking: GNU RISC-V Cross C Linker'
	riscv-none-embed-gcc -march=rv32imac -mabi=ilp32 -msmall-data-limit=8 -msave-restore -Os -fmessage-length=0 -fsigned-char -ffunction-sections -fdata-sections -fno-common -Wunused -Wuninitialized  -g -T "C:\Users\Steve\mrs_community_workspace\ch32v307_rcc_gpio\Ld\Link.ld" -nostartfiles -Xlinker --gc-sections -Wl,-Map,"ch32v307_rcc_gpio.map" --specs=nano.specs --specs=nosys.specs -o "ch32v307_rcc_gpio.elf" $(OBJS) $(USER_OBJS) $(LIBS)
	@echo 'Finished building target: $@'
	@echo ' '

ch32v307_rcc_gpio.hex: ch32v307_rcc_gpio.elf
	@echo 'Invoking: GNU RISC-V Cross Create Flash Image'
	riscv-none-embed-objcopy -O ihex "ch32v307_rcc_gpio.elf"  "ch32v307_rcc_gpio.hex"
	@echo 'Finished building: $@'
	@echo ' '

ch32v307_rcc_gpio.lst: ch32v307_rcc_gpio.elf
	@echo 'Invoking: GNU RISC-V Cross Create Listing'
	riscv-none-embed-objdump --all-headers --demangle --disassemble "ch32v307_rcc_gpio.elf" > "ch32v307_rcc_gpio.lst"
	@echo 'Finished building: $@'
	@echo ' '

ch32v307_rcc_gpio.siz: ch32v307_rcc_gpio.elf
	@echo 'Invoking: GNU RISC-V Cross Print Size'
	riscv-none-embed-size --format=berkeley "ch32v307_rcc_gpio.elf"
	@echo 'Finished building: $@'
	@echo ' '

# Other Targets
clean:
	-$(RM) $(OBJS)$(SECONDARY_FLASH)$(SECONDARY_LIST)$(SECONDARY_SIZE)$(ASM_DEPS)$(S_UPPER_DEPS)$(C_DEPS) ch32v307_rcc_gpio.elf
	-@echo ' '

secondary-outputs: $(SECONDARY_FLASH) $(SECONDARY_LIST) $(SECONDARY_SIZE)

.PHONY: all clean dependents

-include ../makefile.targets
