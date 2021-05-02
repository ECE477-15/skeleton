# To-Do
- [x] External interrupt (hat presence detection & digital sensors)
- [ ] Dynamic Hat setup
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
