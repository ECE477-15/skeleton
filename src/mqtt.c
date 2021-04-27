/*
 * mqtt.c
 *
 *  Created on: Apr 19, 2021
 *      Author: grantweiss
 */
#include "mqtt.h"
#include "main.h"
#include "hats.h"

static const char* const discovery[] = {
		"{'dev_cla':'",							// 'device_class'	abbreviations
		// temperature
		"','name':'",
		// Temperature
		"','stat_t':'homeassistant/binary_sensor/",	// 'state_topic'
		// bedroomSensor
		"/state',",
		"'unit_of_meas':'°F',"					// 'unit_of_measurement'
		"'val_tpl':'",							// 'value_template'
		// {{value}}
		"'}"

};

void mqtt_discover(uint64_t addr, char hatId) {
	if(global_state.connectedHat != wifi_gateway) {	// todo, check to see if connection to mqtt is good as well
		error(__LINE__);
	}

	switch( (hat_t)hatId ) {
		case temp_hum:
			wifi_send_mqtt("homeassistant/binary_sensor/sensorBedroom/state", "3");
//			buf_writeStr(discovery[0], uart1_tx_buffer);
//			buf_writeStr("temperature", uart1_tx_buffer);
//			buf_writeStr(discovery[1], uart1_tx_buffer);
//			buf_writeStr("Temperature", uart1_tx_buffer);
//			buf_writeStr(discovery[2], uart1_tx_buffer);
//			buf_writeChars(uart1_tx_buffer, (char *)addr, 8);
//			buf_writeStr("bedroomSensor", uart1_tx_buffer);
//			buf_writeStr(discovery[3], uart1_tx_buffer);
//			buf_writeStr(discovery[4], uart1_tx_buffer);
//			buf_writeStr(discovery[5], uart1_tx_buffer);
//			buf_writeStr("{{value}}", uart1_tx_buffer);
//			buf_writeStr(discovery[6], uart1_tx_buffer);
			break;
		case PIR_motion:
			wifi_send_mqtt_disco("FRIENDLY", "bedroom", "binary_sensor", "motion", ",'off_dly':30");
			break;
		default:
			wifi_send_mqtt("homeassistant/binary_sensor/pirmotion/state", "7");
			error(__LINE__);
			break;
	}
}

void mqtt_value(uint64_t addr, char val1, char val2) {
	if(global_state.connectedHat != wifi_gateway) {
		error(__LINE__);
	}

	// TODO
	wifi_send_mqtt("homeassistant/binary_sensor/UNIQUE_ID/state", "ON");
}
