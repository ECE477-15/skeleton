/*
 * mqtt.c
 *
 *  Created on: Apr 19, 2021
 *      Author: grantweiss
 */
#include "mqtt.h"
#include "main.h"
#include "hats.h"
#include "String.h"
#include "stdio.h"

void floatToStr(char *toStr, float fl);

void mqtt_undiscover(uint32_t addrH, uint32_t addrL, char hatId) {
	if(global_state.connectedHat != wifi_gateway) {	// todo, check to see if connection to mqtt is good as well
		return;
	}
	if(hatId >= HAT_LIST_LEN || hatId == 0) {
		error(__LINE__);
	}

	char uniqueID[10];
	uniqueID[8] = '\0';
	uniqueID[9] = '\0';
	uint32_t uniqueInt = addrL;
	for(uint8_t i = 0; i < 8; i++) {
		uniqueID[i] = (char)(uniqueInt & 0xF) + 65;
		uniqueInt >>= 4;
	}

	if(hat_list[(hat_t)hatId].dev_class == class_tempHum) {
		uniqueID[8] = 'T';
		wifi_send_mqtt_undisco((hat_t)hatId, uniqueID);
		uniqueID[8] = 'H';
		wifi_send_mqtt_undisco((hat_t)hatId, uniqueID);
	} else {
		wifi_send_mqtt_undisco((hat_t)hatId, uniqueID);
	}
}

void mqtt_discover(uint32_t addrH, uint32_t addrL, char hatId) {
	if(global_state.connectedHat != wifi_gateway) {	// todo, check to see if connection to mqtt is good as well
		return;
	}
	if(hatId >= HAT_LIST_LEN || hatId == 0) {
		error(__LINE__);
	}

	char uniqueID[10];
	uniqueID[8] = '\0';
	uniqueID[9] = '\0';
	uint32_t uniqueInt = addrL;
	for(uint8_t i = 0; i < 8; i++) {
		uniqueID[i] = (char)(uniqueInt & 0xF) + 65;
		uniqueInt >>= 4;
	}

	if(hat_list[(hat_t)hatId].dev_class == class_tempHum) {
		uniqueID[8] = 'T';
		wifi_send_mqtt_disco((hat_t)hatId, uniqueID, class_temp);
		uniqueID[8] = 'H';
		wifi_send_mqtt_disco((hat_t)hatId, uniqueID, class_hum);
	} else {
		wifi_send_mqtt_disco((hat_t)hatId, uniqueID, hat_list[(hat_t)hatId].dev_class);
	}
}

void mqtt_value(uint32_t addrH, uint32_t addrL, char hatId, char* payload) {
	if(global_state.connectedHat != wifi_gateway) {
		return;
	}

	char uniqueID[10];
	uniqueID[8] = '\0';
	uniqueID[9] = '\0';
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
			if(hat_list[(hat_t)hatId].dev_class == class_tempHum) {
				union {
				    float myFloats[2];
				    uint8_t myChars[8];
				  } myData;
				memcpy(myData.myChars, payload, 8);
				uniqueID[8] = 'T';
				char floatStr[15];
				floatToStr(floatStr, myData.myFloats[0]);
				wifi_send_mqtt((hat_t)hatId, uniqueID, floatStr);
				uniqueID[8] = 'H';
				floatToStr(floatStr, myData.myFloats[1]);
				wifi_send_mqtt((hat_t)hatId, uniqueID, floatStr);
			}
			break;
		default:
			break;
	}
}

void floatToStr(char *toStr, float fl) {
	int flWhole = (int)fl;
	int flDec = (fl - (float)flWhole) * 100;

	snprintf(toStr, 15, "%d.%d", flWhole, flDec);
}
