/*
 * hats.h
 *
 *  Created on: Apr 12, 2021
 *      Author: grantweiss
 */

#ifndef HATS_H_
#define HATS_H_

#include <stm32l0xx.h>
#include "wifi.h"

// HDC2010 Temperature and Humidity Sensor
#define HDC2010_Addr 0x40

#define HDC2010_TEMP_LOW 0x0
#define HDC2010_HUMID_LOW 0x2
#define HDC2010_CONFIG 0x0E
#define HDC2010_MEASUREMENT_CONFIG 0x0F

void hat_interrupt_PB11(void);
void hat_detect_interrupt();
void send_homeassistant_boolean_PB11();
void blank_setup(void);
void hat_uart_115200(void);
void hat_uart_9600(void);
void hdc2010_send(void);
void hdc2010_setup();
void lp5523_setup();

#endif /* HATS_H_ */
