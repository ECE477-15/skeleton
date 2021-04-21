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

void hat_interrupt_PB11(void);
void hat_detect_interrupt();
void send_homeassistant_boolean_PB11();
void blank_setup(void);
void hat_gpio_setup_uart(void);

#endif /* HATS_H_ */
