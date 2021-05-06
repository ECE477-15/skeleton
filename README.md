# To-Do
- [x] External interrupt (hat presence detection & digital sensors)
- [x] Dynamic Hat setup
- [x] Hat configuration parameters
- [x] Function to fetch hat ID from ADC reading (lookup)
- [x] EEPROM (most recent configuration)
- [x] Sleep and Wakeup - STOP working, SLEEP needs work
- [x] Comparator (analog sensors)
- [x] WiFi Firmware Driver (uart)
- [x] XBee Firmware Driver (uart)
- [x] Battery Babysitter Firmware Driver (i2c)
- [x] UART Peripheral Interface
- [x] ADC Peripheral Interface
- [x] I2C Peripheral Interface
- [x] Switch Pid reading pin (PA5) to PA4; switch pull-down from PA7 to PA8 [this is to avoid the discovery board LED connected to PA5, messes with ADC readings]

# Hat Pinout:
| Pin   | I2C       | UART        | ADC       |
| ----- | --------- | ----------- | --------- |
| PB10  | I2C2_SCL  | LPUART1_TX  |
| PB11  | I2C2_SDA  | LPUART1_RX  |
| PA6   |           | LPUART1_CTS | ADC1_Ch6  |
| PB1   |           | LPUART1_RTS | ADC1_Ch9  |

| Pin | Function            |
| --- | ------------------- |
| PA4 | Hat_Pid (ADC1_Ch4)  |

# XBee
| Pin | Function  |
| --- | --------- |
| PA0 | UART2_CTS |
| PA1 | UART2_RTS |
| PA2 | UART2_TX  |
| PA3 | UART2_RX  |

# Battery Babysitter
| Pin   | Function  |
| ----- | --------- |
| PB13  | I2C2_SCL  |
| PB14  | I2C2_SDA |

# Project Structure
'''
.
├── Debug
├── LinkerScript.ld
├── README.md
├── STM32L0538-DISCO.xml
├── inc
│   ├── adc.h             ADC function declarations
│   ├── batBaby.h         Battery Babysitter addresses and function decl.
│   ├── core/...
│   ├── delay.h           Delay function decl.
│   ├── device/...
│   ├── eeprom.h          EEPROM function decl., EEPROM storage memory structure
│   ├── hats.h            Hat-specific function decl.
│   ├── i2c.h             I2C driver function decl.
│   ├── main.h            Type of hat and settings enums; hat configuration struct; global variables & arrays; main function decl.; ref. resistor defined
│   ├── mqtt.h            MQTT function decl.
│   ├── ringBuf.h         Size defined; Macros and structs for buffer; function decl.
│   ├── rtc.h             RTC function decl.
│   ├── sleep.h           Sleep and Stop mode function decl.
│   ├── uart.h            Baud rate defined; Buffer decl.; uart action flag decl.; function decl;
│   ├── wifi.h            WiFi driver function decl.
│   └── xbee.h            XBee driver function decl.
├── skeleton2\ Run.cfg
├── src
│   ├── adc.c
│   ├── batBaby.c
│   ├── delay.c
│   ├── eeprom.c
│   ├── globals.c
│   ├── hats.c
│   ├── i2c.c
│   ├── main.c
│   ├── mqtt.c
│   ├── ringBuf.c
│   ├── rtc.c
│   ├── sleep.c
│   ├── system_stm32l0xx.c
│   ├── testers.c
│   ├── uart.c
│   ├── wifi.c
│   └── xbee.c
└── startup/...
'''
