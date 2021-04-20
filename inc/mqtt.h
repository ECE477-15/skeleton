/*
 * mqtt.h
 *
 *  Created on: Apr 19, 2021
 *      Author: grantweiss
 */

#ifndef MQTT_H_
#define MQTT_H_

#include "stm32l0xx.h"

void mqtt_discover(uint64_t addr, char hatId);
void mqtt_value(uint64_t addr, char val1, char val2);

#endif /* MQTT_H_ */
