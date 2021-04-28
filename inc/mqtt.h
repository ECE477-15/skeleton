/*
 * mqtt.h
 *
 *  Created on: Apr 19, 2021
 *      Author: grantweiss
 */

#ifndef MQTT_H_
#define MQTT_H_

#include "stm32l0xx.h"
#include "wifi.h"

void mqtt_discover(uint32_t addrH, uint32_t addrL, char hatId);
void mqtt_value(uint32_t addrH, uint32_t addrL, char val1, char val2);

#endif /* MQTT_H_ */
