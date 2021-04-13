/*
 * hats.h
 *
 *  Created on: Apr 12, 2021
 *      Author: grantweiss
 */

#ifndef HATS_H_
#define HATS_H_

#include <stm32l0xx.h>

void hat_interrupt_PB11(void);
void hat_detect_interrupt();
void send_homeassistant_boolean_PB11();

#endif /* HATS_H_ */
