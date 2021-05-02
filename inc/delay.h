/*
 * delay.h
 *
 *  Created on: Apr 19, 2021
 *      Author: grantweiss
 */

#ifndef DELAY_H_
#define DELAY_H_

#include "stm32l0xx.h"

void delay_ms(uint32_t ms);
void delay_init(void);
void delay_lapse_start();
void delay_lapse_end();

// Delay counter
static volatile uint32_t delay_count;

#endif /* DELAY_H_ */
