################################################################################
# Automatically-generated file. Do not edit!
# Toolchain: GNU Tools for STM32 (12.3.rel1)
################################################################################

# Add inputs and outputs from these tool invocations to the build variables 
C_SRCS += \
../Core/Src/ADF_PDM.c \
../Core/Src/I2C1_IMU.c \
../Core/Src/I2C3_flash.c \
../Core/Src/MEM_Pool.c \
../Core/Src/PSRAM.c \
../Core/Src/QSPI.c \
../Core/Src/SPI1_CODEC.c \
../Core/Src/SYS_config.c \
../Core/Src/app_threadx.c \
../Core/Src/cmd_dec.c \
../Core/Src/linked_list.c \
../Core/Src/main.c \
../Core/Src/stm32u5xx_hal_msp.c \
../Core/Src/stm32u5xx_hal_timebase_tim.c \
../Core/Src/stm32u5xx_it.c \
../Core/Src/syscalls.c \
../Core/Src/syserr.c \
../Core/Src/sysmem.c \
../Core/Src/system_stm32u5xx.c \
../Core/Src/temp_sensor.c 

S_UPPER_SRCS += \
../Core/Src/tx_initialize_low_level.S 

OBJS += \
./Core/Src/ADF_PDM.o \
./Core/Src/I2C1_IMU.o \
./Core/Src/I2C3_flash.o \
./Core/Src/MEM_Pool.o \
./Core/Src/PSRAM.o \
./Core/Src/QSPI.o \
./Core/Src/SPI1_CODEC.o \
./Core/Src/SYS_config.o \
./Core/Src/app_threadx.o \
./Core/Src/cmd_dec.o \
./Core/Src/linked_list.o \
./Core/Src/main.o \
./Core/Src/stm32u5xx_hal_msp.o \
./Core/Src/stm32u5xx_hal_timebase_tim.o \
./Core/Src/stm32u5xx_it.o \
./Core/Src/syscalls.o \
./Core/Src/syserr.o \
./Core/Src/sysmem.o \
./Core/Src/system_stm32u5xx.o \
./Core/Src/temp_sensor.o \
./Core/Src/tx_initialize_low_level.o 

S_UPPER_DEPS += \
./Core/Src/tx_initialize_low_level.d 

C_DEPS += \
./Core/Src/ADF_PDM.d \
./Core/Src/I2C1_IMU.d \
./Core/Src/I2C3_flash.d \
./Core/Src/MEM_Pool.d \
./Core/Src/PSRAM.d \
./Core/Src/QSPI.d \
./Core/Src/SPI1_CODEC.d \
./Core/Src/SYS_config.d \
./Core/Src/app_threadx.d \
./Core/Src/cmd_dec.d \
./Core/Src/linked_list.d \
./Core/Src/main.d \
./Core/Src/stm32u5xx_hal_msp.d \
./Core/Src/stm32u5xx_hal_timebase_tim.d \
./Core/Src/stm32u5xx_it.d \
./Core/Src/syscalls.d \
./Core/Src/syserr.d \
./Core/Src/sysmem.d \
./Core/Src/system_stm32u5xx.d \
./Core/Src/temp_sensor.d 


# Each subdirectory must supply rules for building sources it contributes
Core/Src/%.o Core/Src/%.su Core/Src/%.cyclo: ../Core/Src/%.c Core/Src/subdir.mk
	arm-none-eabi-gcc "$<" -mcpu=cortex-m33 -std=gnu11 -g3 -DDEBUG -DUSE_FULL_LL_DRIVER -DTX_INCLUDE_USER_DEFINE_FILE -DTX_SINGLE_MODE_NON_SECURE=1 -DUSBPD_PORT_COUNT=1 -DUSBPD_THREADX -D_SNK -DUSBPDCORE_LIB_NO_PD -DUX_INCLUDE_USER_DEFINE_FILE -DUSE_HAL_DRIVER -DSTM32U5A5xx -DXXX_STM32U575xx -DNUCLEO_BOARD -DUSE_NUCLEO_144 -DLEVEL_SHIFT -DUX_PERIODIC_RATE=1000 -DSPI3_DMA -DQSPI_DMA -DXXX_PDM_MCU -DENABLE_DCACHE -DXXX_CODEC_SAI -DXXX_SPDIF_TEST -DXXX_SPDIF_TEST_A -c -I../Core/Inc -I../AZURE_RTOS/App -I../USBPD/App -I../USBPD/Target -I../USBX/App -I../USBX/Target -I../Drivers/STM32U5xx_HAL_Driver/Inc -I../Drivers/STM32U5xx_HAL_Driver/Inc/Legacy -I../Middlewares/ST/threadx/common/inc -I../Middlewares/ST/STM32_USBPD_Library/Core/inc -I../Middlewares/ST/STM32_USBPD_Library/Devices/STM32U5XX/inc -I../Drivers/CMSIS/Device/ST/STM32U5xx/Include -I../Middlewares/ST/threadx/ports/cortex_m33/gnu/inc -I../Middlewares/ST/usbx/common/core/inc -I../Middlewares/ST/usbx/ports/generic/inc -I../Middlewares/ST/usbx/common/usbx_stm32_device_controllers -I../Middlewares/ST/usbx/common/usbx_device_classes/inc -I../Drivers/CMSIS/Include -I../Drivers/BSP/STM32U5xx_Nucleo -Og -ffunction-sections -fdata-sections -Wall -Wextra -fstack-usage -fcyclomatic-complexity -MMD -MP -MF"$(@:%.o=%.d)" -MT"$@" --specs=nano.specs -mfpu=fpv5-sp-d16 -mfloat-abi=hard -mthumb -o "$@"
Core/Src/%.o: ../Core/Src/%.S Core/Src/subdir.mk
	arm-none-eabi-gcc -mcpu=cortex-m33 -g3 -DDEBUG -DTX_SINGLE_MODE_NON_SECURE=1 -DXXX_STM32U575xx -DSTM32U5A5xx -UTX_LOW_POWER -c -x assembler-with-cpp -MMD -MP -MF"$(@:%.o=%.d)" -MT"$@" --specs=nano.specs -mfpu=fpv5-sp-d16 -mfloat-abi=hard -mthumb -o "$@" "$<"

clean: clean-Core-2f-Src

clean-Core-2f-Src:
	-$(RM) ./Core/Src/ADF_PDM.cyclo ./Core/Src/ADF_PDM.d ./Core/Src/ADF_PDM.o ./Core/Src/ADF_PDM.su ./Core/Src/I2C1_IMU.cyclo ./Core/Src/I2C1_IMU.d ./Core/Src/I2C1_IMU.o ./Core/Src/I2C1_IMU.su ./Core/Src/I2C3_flash.cyclo ./Core/Src/I2C3_flash.d ./Core/Src/I2C3_flash.o ./Core/Src/I2C3_flash.su ./Core/Src/MEM_Pool.cyclo ./Core/Src/MEM_Pool.d ./Core/Src/MEM_Pool.o ./Core/Src/MEM_Pool.su ./Core/Src/PSRAM.cyclo ./Core/Src/PSRAM.d ./Core/Src/PSRAM.o ./Core/Src/PSRAM.su ./Core/Src/QSPI.cyclo ./Core/Src/QSPI.d ./Core/Src/QSPI.o ./Core/Src/QSPI.su ./Core/Src/SPI1_CODEC.cyclo ./Core/Src/SPI1_CODEC.d ./Core/Src/SPI1_CODEC.o ./Core/Src/SPI1_CODEC.su ./Core/Src/SYS_config.cyclo ./Core/Src/SYS_config.d ./Core/Src/SYS_config.o ./Core/Src/SYS_config.su ./Core/Src/app_threadx.cyclo ./Core/Src/app_threadx.d ./Core/Src/app_threadx.o ./Core/Src/app_threadx.su ./Core/Src/cmd_dec.cyclo ./Core/Src/cmd_dec.d ./Core/Src/cmd_dec.o ./Core/Src/cmd_dec.su ./Core/Src/linked_list.cyclo ./Core/Src/linked_list.d ./Core/Src/linked_list.o ./Core/Src/linked_list.su ./Core/Src/main.cyclo ./Core/Src/main.d ./Core/Src/main.o ./Core/Src/main.su ./Core/Src/stm32u5xx_hal_msp.cyclo ./Core/Src/stm32u5xx_hal_msp.d ./Core/Src/stm32u5xx_hal_msp.o ./Core/Src/stm32u5xx_hal_msp.su ./Core/Src/stm32u5xx_hal_timebase_tim.cyclo ./Core/Src/stm32u5xx_hal_timebase_tim.d ./Core/Src/stm32u5xx_hal_timebase_tim.o ./Core/Src/stm32u5xx_hal_timebase_tim.su ./Core/Src/stm32u5xx_it.cyclo ./Core/Src/stm32u5xx_it.d ./Core/Src/stm32u5xx_it.o ./Core/Src/stm32u5xx_it.su ./Core/Src/syscalls.cyclo ./Core/Src/syscalls.d ./Core/Src/syscalls.o ./Core/Src/syscalls.su ./Core/Src/syserr.cyclo ./Core/Src/syserr.d ./Core/Src/syserr.o ./Core/Src/syserr.su ./Core/Src/sysmem.cyclo ./Core/Src/sysmem.d ./Core/Src/sysmem.o ./Core/Src/sysmem.su ./Core/Src/system_stm32u5xx.cyclo ./Core/Src/system_stm32u5xx.d ./Core/Src/system_stm32u5xx.o ./Core/Src/system_stm32u5xx.su ./Core/Src/temp_sensor.cyclo ./Core/Src/temp_sensor.d ./Core/Src/temp_sensor.o ./Core/Src/temp_sensor.su ./Core/Src/tx_initialize_low_level.d ./Core/Src/tx_initialize_low_level.o

.PHONY: clean-Core-2f-Src

