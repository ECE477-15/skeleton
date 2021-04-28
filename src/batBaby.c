/*
 * batBaby.c
 *
 *  Created on: Apr 28, 2021
 *      Author: grantweiss
 */

#include "stm32l0xx.h"
#include "i2c.h"
#include "batBaby.h"

uint16_t BB_getSOC(void) {
	uint8_t data[2];
	I2C_Mem_Rx(BQ72441_I2C_ADDRESS, BQ27441_COMMAND_SOC, 1, data, 2);
	uint16_t Soc = (data[1]<<8) | data[0];
	return Soc;
}

uint16_t BB_getVolt(void) {
	uint8_t data[2];
	I2C_Mem_Rx(BQ72441_I2C_ADDRESS, BQ27441_COMMAND_VOLTAGE, 1, data, 2);
	uint16_t Volt = (data[1]<<8) | data[0];
	return Volt;

}

uint16_t BB_getSOH(void) {
	uint8_t data[2];
	I2C_Mem_Rx(BQ72441_I2C_ADDRESS, BQ27441_COMMAND_SOH, 1, data, 2);
	uint16_t sohPercent = data[0];
	return sohPercent;
}

uint16_t BB_getRemCap(void) {
	uint8_t data[2];
	I2C_Mem_Rx(BQ72441_I2C_ADDRESS, BQ27441_COMMAND_REM_CAPACITY, 1, data, 2);
	uint16_t Rcap = (data[1]<<8) | data[0];
	return Rcap;
}
