# To-Do
- [ ] External interrupt (hat presence detection & digital sensors) (Ethan)
- [ ] Dynamic Hat setup
- [ ] Hat configuration parameters
- [ ] Function to fetch hat ID from ADC reading (lookup)
- [x] EEPROM (most recent configuration)
- [ ] Sleep and Wakeup (Ethan)
- [x] Comparator (analog sensors)
- [ ] WiFi Firmware Driver (uart)
- [ ] XBee Firmware Driver (uart)
- [ ] Battery Babysitter Firmware Driver (i2c)
- [x] UART Peripheral Interface
- [x] ADC Peripheral Interface
- [x] I2C Peripheral Interface

# Hat Pinout:
| Pin   | I2C       | UART        | ADC       |
| ----- | --------- | ----------- | --------- |
| PB10  | I2C2_SCL  | LPUART1_TX  |
| PB11  | I2C2_SDA  | LPUART1_RX  |
| PA6   |           | LPUART1_CTS | ADC1_Ch6  |
| PB1   |           | LPUART1_RTS | ADC1_Ch9  |

| Pin | Function            |
| --- | ------------------- |
| PA5 | Hat_Pid (ADC1_Ch5)  |

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
