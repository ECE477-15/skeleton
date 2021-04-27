/*
 * wifi.h
 *
 *  Created on: Apr 21, 2021
 *      Author: grantweiss
 */

#ifndef WIFI_H_
#define WIFI_H_

#include "stm32l0xx.h"

void wifi_setup(void);
void wifi_send_mqtt(char * topic, char * payload);
void wifi_send_mqtt_disco(char * friendlyName, char * identifier, char *type, char *class, char *more_opts);

#endif /* WIFI_H_ */
