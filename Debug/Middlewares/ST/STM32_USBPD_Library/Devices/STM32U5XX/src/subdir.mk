################################################################################
# Automatically-generated file. Do not edit!
# Toolchain: GNU Tools for STM32 (12.3.rel1)
################################################################################

# Add inputs and outputs from these tool invocations to the build variables 
C_SRCS += \
../Middlewares/ST/STM32_USBPD_Library/Devices/STM32U5XX/src/usbpd_cad_hw_if.c \
../Middlewares/ST/STM32_USBPD_Library/Devices/STM32U5XX/src/usbpd_hw.c \
../Middlewares/ST/STM32_USBPD_Library/Devices/STM32U5XX/src/usbpd_hw_if_it.c \
../Middlewares/ST/STM32_USBPD_Library/Devices/STM32U5XX/src/usbpd_phy_hw_if.c \
../Middlewares/ST/STM32_USBPD_Library/Devices/STM32U5XX/src/usbpd_pwr_hw_if.c 

OBJS += \
./Middlewares/ST/STM32_USBPD_Library/Devices/STM32U5XX/src/usbpd_cad_hw_if.o \
./Middlewares/ST/STM32_USBPD_Library/Devices/STM32U5XX/src/usbpd_hw.o \
./Middlewares/ST/STM32_USBPD_Library/Devices/STM32U5XX/src/usbpd_hw_if_it.o \
./Middlewares/ST/STM32_USBPD_Library/Devices/STM32U5XX/src/usbpd_phy_hw_if.o \
./Middlewares/ST/STM32_USBPD_Library/Devices/STM32U5XX/src/usbpd_pwr_hw_if.o 

C_DEPS += \
./Middlewares/ST/STM32_USBPD_Library/Devices/STM32U5XX/src/usbpd_cad_hw_if.d \
./Middlewares/ST/STM32_USBPD_Library/Devices/STM32U5XX/src/usbpd_hw.d \
./Middlewares/ST/STM32_USBPD_Library/Devices/STM32U5XX/src/usbpd_hw_if_it.d \
./Middlewares/ST/STM32_USBPD_Library/Devices/STM32U5XX/src/usbpd_phy_hw_if.d \
./Middlewares/ST/STM32_USBPD_Library/Devices/STM32U5XX/src/usbpd_pwr_hw_if.d 


# Each subdirectory must supply rules for building sources it contributes
Middlewares/ST/STM32_USBPD_Library/Devices/STM32U5XX/src/%.o Middlewares/ST/STM32_USBPD_Library/Devices/STM32U5XX/src/%.su Middlewares/ST/STM32_USBPD_Library/Devices/STM32U5XX/src/%.cyclo: ../Middlewares/ST/STM32_USBPD_Library/Devices/STM32U5XX/src/%.c Middlewares/ST/STM32_USBPD_Library/Devices/STM32U5XX/src/subdir.mk
	arm-none-eabi-gcc "$<" -mcpu=cortex-m33 -std=gnu11 -g3 -DDEBUG -DUSE_FULL_LL_DRIVER -DTX_INCLUDE_USER_DEFINE_FILE -DTX_SINGLE_MODE_NON_SECURE=1 -DUSBPD_PORT_COUNT=1 -DUSBPD_THREADX -D_SNK -DUSBPDCORE_LIB_NO_PD -DUX_INCLUDE_USER_DEFINE_FILE -DUSE_HAL_DRIVER -DSTM32U5A5xx -DXXX_STM32U575xx -DNUCLEO_BOARD -DUSE_NUCLEO_144 -DLEVEL_SHIFT -DUX_PERIODIC_RATE=1000 -DSPI3_DMA -DQSPI_DMA -DXXX_PDM_MCU -DENABLE_DCACHE -DXXX_CODEC_SAI -DXXX_SPDIF_TEST -DXXX_SPDIF_TEST_A -c -I../Core/Inc -I../AZURE_RTOS/App -I../USBPD/App -I../USBPD/Target -I../USBX/App -I../USBX/Target -I../Drivers/STM32U5xx_HAL_Driver/Inc -I../Drivers/STM32U5xx_HAL_Driver/Inc/Legacy -I../Middlewares/ST/threadx/common/inc -I../Middlewares/ST/STM32_USBPD_Library/Core/inc -I../Middlewares/ST/STM32_USBPD_Library/Devices/STM32U5XX/inc -I../Drivers/CMSIS/Device/ST/STM32U5xx/Include -I../Middlewares/ST/threadx/ports/cortex_m33/gnu/inc -I../Middlewares/ST/usbx/common/core/inc -I../Middlewares/ST/usbx/ports/generic/inc -I../Middlewares/ST/usbx/common/usbx_stm32_device_controllers -I../Middlewares/ST/usbx/common/usbx_device_classes/inc -I../Drivers/CMSIS/Include -I../Drivers/BSP/STM32U5xx_Nucleo -Og -ffunction-sections -fdata-sections -Wall -Wextra -fstack-usage -fcyclomatic-complexity -MMD -MP -MF"$(@:%.o=%.d)" -MT"$@" --specs=nano.specs -mfpu=fpv5-sp-d16 -mfloat-abi=hard -mthumb -o "$@"

clean: clean-Middlewares-2f-ST-2f-STM32_USBPD_Library-2f-Devices-2f-STM32U5XX-2f-src

clean-Middlewares-2f-ST-2f-STM32_USBPD_Library-2f-Devices-2f-STM32U5XX-2f-src:
	-$(RM) ./Middlewares/ST/STM32_USBPD_Library/Devices/STM32U5XX/src/usbpd_cad_hw_if.cyclo ./Middlewares/ST/STM32_USBPD_Library/Devices/STM32U5XX/src/usbpd_cad_hw_if.d ./Middlewares/ST/STM32_USBPD_Library/Devices/STM32U5XX/src/usbpd_cad_hw_if.o ./Middlewares/ST/STM32_USBPD_Library/Devices/STM32U5XX/src/usbpd_cad_hw_if.su ./Middlewares/ST/STM32_USBPD_Library/Devices/STM32U5XX/src/usbpd_hw.cyclo ./Middlewares/ST/STM32_USBPD_Library/Devices/STM32U5XX/src/usbpd_hw.d ./Middlewares/ST/STM32_USBPD_Library/Devices/STM32U5XX/src/usbpd_hw.o ./Middlewares/ST/STM32_USBPD_Library/Devices/STM32U5XX/src/usbpd_hw.su ./Middlewares/ST/STM32_USBPD_Library/Devices/STM32U5XX/src/usbpd_hw_if_it.cyclo ./Middlewares/ST/STM32_USBPD_Library/Devices/STM32U5XX/src/usbpd_hw_if_it.d ./Middlewares/ST/STM32_USBPD_Library/Devices/STM32U5XX/src/usbpd_hw_if_it.o ./Middlewares/ST/STM32_USBPD_Library/Devices/STM32U5XX/src/usbpd_hw_if_it.su ./Middlewares/ST/STM32_USBPD_Library/Devices/STM32U5XX/src/usbpd_phy_hw_if.cyclo ./Middlewares/ST/STM32_USBPD_Library/Devices/STM32U5XX/src/usbpd_phy_hw_if.d ./Middlewares/ST/STM32_USBPD_Library/Devices/STM32U5XX/src/usbpd_phy_hw_if.o ./Middlewares/ST/STM32_USBPD_Library/Devices/STM32U5XX/src/usbpd_phy_hw_if.su ./Middlewares/ST/STM32_USBPD_Library/Devices/STM32U5XX/src/usbpd_pwr_hw_if.cyclo ./Middlewares/ST/STM32_USBPD_Library/Devices/STM32U5XX/src/usbpd_pwr_hw_if.d ./Middlewares/ST/STM32_USBPD_Library/Devices/STM32U5XX/src/usbpd_pwr_hw_if.o ./Middlewares/ST/STM32_USBPD_Library/Devices/STM32U5XX/src/usbpd_pwr_hw_if.su

.PHONY: clean-Middlewares-2f-ST-2f-STM32_USBPD_Library-2f-Devices-2f-STM32U5XX-2f-src

