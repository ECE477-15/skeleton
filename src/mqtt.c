/*
 * mqtt.c
 *
 *  Created on: Apr 19, 2021
 *      Author: grantweiss
 */
#include "mqtt.h"
#include "main.h"
#include "hats.h"

void mqtt_discover(uint32_t addrH, uint32_t addrL, char hatId) {
	if(global_state.connectedHat != wifi_gateway) {	// todo, check to see if connection to mqtt is good as well
		error(__LINE__);
	}
	if(hatId >= HAT_LIST_LEN || hatId == 0) {
		error(__LINE__);
	}

	char uniqueID[8];
	*((uint32_t *)uniqueID) = addrH;
	*(((uint32_t *)uniqueID) + 1) = addrL;

	wifi_send_mqtt_disco((hat_t)hatId, uniqueID);
}

void mqtt_value(uint32_t addrH, uint32_t addrL, char val1, char val2) {
	if(global_state.connectedHat != wifi_gateway) {
		error(__LINE__);
	}

	// TODO
	wifi_send_mqtt("homeassistant/binary_sensor/UNIQUE_ID/state", "ON");
}
