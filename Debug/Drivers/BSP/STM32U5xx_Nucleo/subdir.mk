################################################################################
# Automatically-generated file. Do not edit!
# Toolchain: GNU Tools for STM32 (12.3.rel1)
################################################################################

# Add inputs and outputs from these tool invocations to the build variables 
C_SRCS += \
../Drivers/BSP/STM32U5xx_Nucleo/stm32u5xx_nucleo.c \
../Drivers/BSP/STM32U5xx_Nucleo/stm32u5xx_nucleo_usbpd_pwr.c 

OBJS += \
./Drivers/BSP/STM32U5xx_Nucleo/stm32u5xx_nucleo.o \
./Drivers/BSP/STM32U5xx_Nucleo/stm32u5xx_nucleo_usbpd_pwr.o 

C_DEPS += \
./Drivers/BSP/STM32U5xx_Nucleo/stm32u5xx_nucleo.d \
./Drivers/BSP/STM32U5xx_Nucleo/stm32u5xx_nucleo_usbpd_pwr.d 


# Each subdirectory must supply rules for building sources it contributes
Drivers/BSP/STM32U5xx_Nucleo/%.o Drivers/BSP/STM32U5xx_Nucleo/%.su Drivers/BSP/STM32U5xx_Nucleo/%.cyclo: ../Drivers/BSP/STM32U5xx_Nucleo/%.c Drivers/BSP/STM32U5xx_Nucleo/subdir.mk
	arm-none-eabi-gcc "$<" -mcpu=cortex-m33 -std=gnu11 -g3 -DDEBUG -DUSE_FULL_LL_DRIVER -DTX_INCLUDE_USER_DEFINE_FILE -DTX_SINGLE_MODE_NON_SECURE=1 -DUSBPD_PORT_COUNT=1 -DUSBPD_THREADX -D_SNK -DUSBPDCORE_LIB_NO_PD -DUX_INCLUDE_USER_DEFINE_FILE -DUSE_HAL_DRIVER -DSTM32U5A5xx -DXXX_STM32U575xx -DNUCLEO_BOARD -DUSE_NUCLEO_144 -DLEVEL_SHIFT -DUX_PERIODIC_RATE=1000 -DSPI3_DMA -DQSPI_DMA -DXXX_PDM_MCU -DENABLE_DCACHE -DXXX_CODEC_SAI -DXXX_SPDIF_TEST -DXXX_SPDIF_TEST_A -c -I../Core/Inc -I../AZURE_RTOS/App -I../USBPD/App -I../USBPD/Target -I../USBX/App -I../USBX/Target -I../Drivers/STM32U5xx_HAL_Driver/Inc -I../Drivers/STM32U5xx_HAL_Driver/Inc/Legacy -I../Middlewares/ST/threadx/common/inc -I../Middlewares/ST/STM32_USBPD_Library/Core/inc -I../Middlewares/ST/STM32_USBPD_Library/Devices/STM32U5XX/inc -I../Drivers/CMSIS/Device/ST/STM32U5xx/Include -I../Middlewares/ST/threadx/ports/cortex_m33/gnu/inc -I../Middlewares/ST/usbx/common/core/inc -I../Middlewares/ST/usbx/ports/generic/inc -I../Middlewares/ST/usbx/common/usbx_stm32_device_controllers -I../Middlewares/ST/usbx/common/usbx_device_classes/inc -I../Drivers/CMSIS/Include -I../Drivers/BSP/STM32U5xx_Nucleo -Og -ffunction-sections -fdata-sections -Wall -Wextra -fstack-usage -fcyclomatic-complexity -MMD -MP -MF"$(@:%.o=%.d)" -MT"$@" --specs=nano.specs -mfpu=fpv5-sp-d16 -mfloat-abi=hard -mthumb -o "$@"

clean: clean-Drivers-2f-BSP-2f-STM32U5xx_Nucleo

clean-Drivers-2f-BSP-2f-STM32U5xx_Nucleo:
	-$(RM) ./Drivers/BSP/STM32U5xx_Nucleo/stm32u5xx_nucleo.cyclo ./Drivers/BSP/STM32U5xx_Nucleo/stm32u5xx_nucleo.d ./Drivers/BSP/STM32U5xx_Nucleo/stm32u5xx_nucleo.o ./Drivers/BSP/STM32U5xx_Nucleo/stm32u5xx_nucleo.su ./Drivers/BSP/STM32U5xx_Nucleo/stm32u5xx_nucleo_usbpd_pwr.cyclo ./Drivers/BSP/STM32U5xx_Nucleo/stm32u5xx_nucleo_usbpd_pwr.d ./Drivers/BSP/STM32U5xx_Nucleo/stm32u5xx_nucleo_usbpd_pwr.o ./Drivers/BSP/STM32U5xx_Nucleo/stm32u5xx_nucleo_usbpd_pwr.su

.PHONY: clean-Drivers-2f-BSP-2f-STM32U5xx_Nucleo

