# NUCLEO-U5A5JZ-Q_QSPI
 USB VCP UART for NUCLEO-U575 and NUCLEO-U5A5

## USB VCP UART on NUCLEO-U5A5 and NUCLEO-U575
This project is used to access the MCU via an USB VCP UART. In addition to the ST-Link UART, the USR USB port
provides also a USB VCP UART (on USB-C).
So, you can access the MCU with two parallel running UARTs: one can be use for Python scripts whereby the other remains available
as "backdoor" to monitor the system, to "interfere" with user commands.

## Compile Options
In order to toggle between a NUCLEO-U575 and NUCLEO-U5A5 board, see the macros set on project settings as:
* XXXSTM32U5A5xx
* STM32U575xx
Toggle between both variant by activate one or the other (remove and set XXX on names, including the underline, when it is there).
You get it: one or the other macro should be valid.

** ATTENTION:
Set these macros on C-compiler as well as for Assembler.

The macro:
* NUCLEO_BOARD
should be set if you use a NUCLEO board. If it is not set: it will generate code for my own MCU board (with different schematics and package).

## FW Features
The MCU FW provides a command shell. You can use command "help" to see the list of all implemented commands (and how to use their parameters).
* provides a command shell on both UARTs (in parallel - but not all commands are thread-safe)
* it provides commands to do transactions on the QSPI interface (read and write an external QSPI chip)
* it uses ADC1 in order to measure and display the voltages (VREF, VBAT) as well as the chip temperature: use command "sysinfo"
* you can easily extend the list of commands, e.g. add I2C transactions, regular SPI transactions, toggle LEDs...

## Baudrate Settings
* ST-LINK UART (Micro-USB needed for power): 1843200
* USR USB VCP UART (on USB-C): any baudrate (it is just USB based and limited by USB)

## Differences U575 vs. U5A5
The project handles the differences between both chips. These are:
* U575 has (just) an integrated USB OTG FS PHY (and limited to run all in FS)
* U5A5 has (just) an integrated USB OTH HS PHY - it runs the USB VCP UART in HS mode (480 Mbps) - my preference, also because of larger memories
The macros handle the difference, related to a different USB initialization.

## Project is based on AZURE RTOS
The project is intended to be compiled with the STM32CubeIDE: Other environments and compilers (e.g. IAR, AC6) are not supported (STM and GNU based only).
I have deleted all not needed files (to make the project smaller in size for ZIP, download and GitHub).

It uses the (new) AZURE RTOS (not the old STM Middleware), the USBX stack, threadx RTOS kernel etc.

