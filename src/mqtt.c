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
		return;
	}
	if(hatId >= HAT_LIST_LEN || hatId == 0) {
		error(__LINE__);
	}

	char uniqueID[8];
	uint32_t uniqueInt = addrL;
	for(uint8_t i = 0; i < 8; i++) {
		uniqueID[i] = (char)(uniqueInt & 0xF) + 65;
		uniqueInt >>= 4;
	}

	wifi_send_mqtt_disco((hat_t)hatId, uniqueID);
}

void mqtt_value(uint32_t addrH, uint32_t addrL, char hatId, char* payload) {
	if(global_state.connectedHat != wifi_gateway) {
		return;
	}

	char uniqueID[8];
	uint32_t uniqueInt = addrL;
	for(uint8_t i = 0; i < 8; i++) {
		uniqueID[i] = (char)(uniqueInt & 0xF) + 65;
		uniqueInt >>= 4;
	}

	// TODO
	switch(hat_list[(hat_t)hatId].dev_type) {
		case binary_sensor:
			if(payload[0]) {
				wifi_send_mqtt((hat_t)hatId, uniqueID, "ON");
			} else {
				wifi_send_mqtt((hat_t)hatId, uniqueID, "OFF");
			}
			break;
		case sensor:

			break;
		default:
			break;
	}
}
