################################################################################
# Automatically-generated file. Do not edit!
# Toolchain: GNU Tools for STM32 (12.3.rel1)
################################################################################

-include ../makefile.init

RM := rm -rf

# All of the sources participating in the build are defined here
-include sources.mk
-include USBX/App/subdir.mk
-include USBPD/Target/subdir.mk
-include USBPD/App/subdir.mk
-include Middlewares/ST/usbx/common/usbx_stm32_device_controllers/subdir.mk
-include Middlewares/ST/usbx/common/usbx_device_classes/src/subdir.mk
-include Middlewares/ST/usbx/common/core/src/subdir.mk
-include Middlewares/ST/threadx/ports/cortex_m33/gnu/src/subdir.mk
-include Middlewares/ST/threadx/common/src/subdir.mk
-include Middlewares/ST/STM32_USBPD_Library/Devices/STM32U5XX/src/subdir.mk
-include Middlewares/ST/STM32_USBPD_Library/Core/src/subdir.mk
-include Drivers/STM32U5xx_HAL_Driver/Src/subdir.mk
-include Drivers/BSP/STM32U5xx_Nucleo/subdir.mk
-include Core/Startup/subdir.mk
-include Core/Src/subdir.mk
-include AZURE_RTOS/App/subdir.mk
-include objects.mk

ifneq ($(MAKECMDGOALS),clean)
ifneq ($(strip $(S_DEPS)),)
-include $(S_DEPS)
endif
ifneq ($(strip $(S_UPPER_DEPS)),)
-include $(S_UPPER_DEPS)
endif
ifneq ($(strip $(C_DEPS)),)
-include $(C_DEPS)
endif
endif

-include ../makefile.defs

OPTIONAL_TOOL_DEPS := \
$(wildcard ../makefile.defs) \
$(wildcard ../makefile.init) \
$(wildcard ../makefile.targets) \


BUILD_ARTIFACT_NAME := NUCLEO-U5A5JZ-Q_QSPI
BUILD_ARTIFACT_EXTENSION := elf
BUILD_ARTIFACT_PREFIX :=
BUILD_ARTIFACT := $(BUILD_ARTIFACT_PREFIX)$(BUILD_ARTIFACT_NAME)$(if $(BUILD_ARTIFACT_EXTENSION),.$(BUILD_ARTIFACT_EXTENSION),)

# Add inputs and outputs from these tool invocations to the build variables 
EXECUTABLES += \
NUCLEO-U5A5JZ-Q_QSPI.elf \

MAP_FILES += \
NUCLEO-U5A5JZ-Q_QSPI.map \

SIZE_OUTPUT += \
default.size.stdout \

OBJDUMP_LIST += \
NUCLEO-U5A5JZ-Q_QSPI.list \

OBJCOPY_BIN += \
NUCLEO-U5A5JZ-Q_QSPI.bin \


# All Target
all: main-build

# Main-build Target
main-build: NUCLEO-U5A5JZ-Q_QSPI.elf secondary-outputs

# Tool invocations
NUCLEO-U5A5JZ-Q_QSPI.elf NUCLEO-U5A5JZ-Q_QSPI.map: $(OBJS) $(USER_OBJS) C:\Users\tj\Documents\CubeIDE_workspace\NUCLEO-U5A5JZ-Q_QSPI\STM32U5A5ZJTXQ_FLASH.ld makefile objects.list $(OPTIONAL_TOOL_DEPS)
	arm-none-eabi-gcc -o "NUCLEO-U5A5JZ-Q_QSPI.elf" @"objects.list" $(USER_OBJS) $(LIBS) -mcpu=cortex-m33 -T"C:\Users\tj\Documents\CubeIDE_workspace\NUCLEO-U5A5JZ-Q_QSPI\STM32U5A5ZJTXQ_FLASH.ld" -Wl,-Map="NUCLEO-U5A5JZ-Q_QSPI.map" -Wl,--cref -Wl,--gc-sections -nodefaultlibs -static -L../Middlewares/ST/STM32_USBPD_Library/Core/lib -Wl,--no-warn-rwx-segment --specs=nano.specs -mfpu=fpv5-sp-d16 -mfloat-abi=hard -mthumb -Wl,--start-group -lc -lm -Wl,--end-group
	@echo 'Finished building target: $@'
	@echo ' '

default.size.stdout: $(EXECUTABLES) makefile objects.list $(OPTIONAL_TOOL_DEPS)
	arm-none-eabi-size  $(EXECUTABLES)
	@echo 'Finished building: $@'
	@echo ' '

NUCLEO-U5A5JZ-Q_QSPI.list: $(EXECUTABLES) makefile objects.list $(OPTIONAL_TOOL_DEPS)
	arm-none-eabi-objdump -h -S $(EXECUTABLES) > "NUCLEO-U5A5JZ-Q_QSPI.list"
	@echo 'Finished building: $@'
	@echo ' '

NUCLEO-U5A5JZ-Q_QSPI.bin: $(EXECUTABLES) makefile objects.list $(OPTIONAL_TOOL_DEPS)
	arm-none-eabi-objcopy  -O binary $(EXECUTABLES) "NUCLEO-U5A5JZ-Q_QSPI.bin"
	@echo 'Finished building: $@'
	@echo ' '

# Other Targets
clean:
	-$(RM) NUCLEO-U5A5JZ-Q_QSPI.bin NUCLEO-U5A5JZ-Q_QSPI.elf NUCLEO-U5A5JZ-Q_QSPI.list NUCLEO-U5A5JZ-Q_QSPI.map default.size.stdout
	-@echo ' '

secondary-outputs: $(SIZE_OUTPUT) $(OBJDUMP_LIST) $(OBJCOPY_BIN)

fail-specified-linker-script-missing:
	@echo 'Error: Cannot find the specified linker script. Check the linker settings in the build configuration.'
	@exit 2

warn-no-linker-script-specified:
	@echo 'Warning: No linker script specified. Check the linker settings in the build configuration.'

.PHONY: all clean dependents main-build fail-specified-linker-script-missing warn-no-linker-script-specified

-include ../makefile.targets
