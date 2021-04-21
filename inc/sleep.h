/*
 * sleep.h
 *
 *  Created on: Apr 13, 2021
 *      Author: grantweiss
 */

#ifndef SLEEP_H_
#define SLEEP_H_

#include <stm32l0xx.h>

typedef uint16_t (*stop_check_fn)(void);

void enter_sleep(void);
void enter_stop(void);
void enter_stop_cond(stop_check_fn check);

#endif /* SLEEP_H_ */
