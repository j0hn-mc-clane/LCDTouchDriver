################################################################################
# Automatically-generated file. Do not edit!
################################################################################

-include ../makefile.init

RM := rm -rf

# All of the sources participating in the build are defined here
-include sources.mk
-include Drivers/STM32F1xx_HAL_Driver/Src/subdir.mk
-include Core/Startup/subdir.mk
-include Core/Src/subdir.mk
-include Core/Inc/subdir.mk
-include subdir.mk
-include objects.mk

ifneq ($(MAKECMDGOALS),clean)
ifneq ($(strip $(C_DEPS)),)
-include $(C_DEPS)
endif
endif

-include ../makefile.defs

# Add inputs and outputs from these tool invocations to the build variables 
EXECUTABLES += \
lab.elf \

SIZE_OUTPUT += \
default.size.stdout \

OBJDUMP_LIST += \
lab.list \

OBJCOPY_BIN += \
lab.bin \


# All Target
all: lab.elf secondary-outputs

# Tool invocations
lab.elf: $(OBJS) $(USER_OBJS) C:\Users\JoachimCardoen\Google\ Drive\UGent\Schakelprogramma\Semester\ 2\Computerarchitectuur\Labs\lab3_comp_arch_joachimcardoen\STM32F103RBTX_FLASH.ld
	arm-none-eabi-gcc -o "lab.elf" @"objects.list" $(USER_OBJS) $(LIBS) -mcpu=cortex-m3 -T"C:\Users\JoachimCardoen\Google Drive\UGent\Schakelprogramma\Semester 2\Computerarchitectuur\Labs\lab3_comp_arch_joachimcardoen\STM32F103RBTX_FLASH.ld" --specs=nosys.specs -Wl,-Map="lab.map" -Wl,--gc-sections -static --specs=nano.specs -mfloat-abi=soft -mthumb -Wl,--start-group -lc -lm -Wl,--end-group
	@echo 'Finished building target: $@'
	@echo ' '

default.size.stdout: $(EXECUTABLES)
	arm-none-eabi-size  $(EXECUTABLES)
	@echo 'Finished building: $@'
	@echo ' '

lab.list: $(EXECUTABLES)
	arm-none-eabi-objdump -h -S $(EXECUTABLES) > "lab.list"
	@echo 'Finished building: $@'
	@echo ' '

lab.bin: $(EXECUTABLES)
	arm-none-eabi-objcopy  -O binary $(EXECUTABLES) "lab.bin"
	@echo 'Finished building: $@'
	@echo ' '

# Other Targets
clean:
	-$(RM) *
	-@echo ' '

secondary-outputs: $(SIZE_OUTPUT) $(OBJDUMP_LIST) $(OBJCOPY_BIN)

.PHONY: all clean dependents
.SECONDARY:

-include ../makefile.targets
