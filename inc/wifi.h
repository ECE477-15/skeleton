/*
 * wifi.h
 *
 *  Created on: Apr 21, 2021
 *      Author: grantweiss
 */

#ifndef WIFI_H_
#define WIFI_H_

#include "stm32l0xx.h"
#include "stdbool.h"
#include "main.h"

void wifi_setup(void);
void wifi_send_mqtt(hat_t discoHat, char *uniqueID, char * payload);
void wifi_send_mqtt_disco(hat_t discoHat, char *uniqueID, device_class_t dev_class);


#endif /* WIFI_H_ */
