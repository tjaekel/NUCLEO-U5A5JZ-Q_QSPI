################################################################################
# Automatically-generated file. Do not edit!
# Toolchain: GNU Tools for STM32 (12.3.rel1)
################################################################################

# Add inputs and outputs from these tool invocations to the build variables 
S_SRCS += \
../Core/Startup/startup_stm32u575ritxq.s \
../Core/Startup/startup_stm32u5a5zjtxq.s 

OBJS += \
./Core/Startup/startup_stm32u575ritxq.o \
./Core/Startup/startup_stm32u5a5zjtxq.o 

S_DEPS += \
./Core/Startup/startup_stm32u575ritxq.d \
./Core/Startup/startup_stm32u5a5zjtxq.d 


# Each subdirectory must supply rules for building sources it contributes
Core/Startup/%.o: ../Core/Startup/%.s Core/Startup/subdir.mk
	arm-none-eabi-gcc -mcpu=cortex-m33 -g3 -DDEBUG -DTX_SINGLE_MODE_NON_SECURE=1 -DXXX_STM32U575xx -DSTM32U5A5xx -UTX_LOW_POWER -c -x assembler-with-cpp -MMD -MP -MF"$(@:%.o=%.d)" -MT"$@" --specs=nano.specs -mfpu=fpv5-sp-d16 -mfloat-abi=hard -mthumb -o "$@" "$<"

clean: clean-Core-2f-Startup

clean-Core-2f-Startup:
	-$(RM) ./Core/Startup/startup_stm32u575ritxq.d ./Core/Startup/startup_stm32u575ritxq.o ./Core/Startup/startup_stm32u5a5zjtxq.d ./Core/Startup/startup_stm32u5a5zjtxq.o

.PHONY: clean-Core-2f-Startup

