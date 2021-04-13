/*
 * eeprom.h
 *
 *  Created on: Apr 12, 2021
 *      Author: grantweiss
 */

#ifndef EEPROM_H_
#define EEPROM_H_

#include <stm32l0xx.h>

void Eeprom_Write_Obj(uint8_t *wAddr, uint8_t *obj, uint16_t size);
void Eeprom_Read_Obj (uint8_t *wAddr, uint8_t *obj, uint16_t size);

typedef struct
{
  __IO uint16_t declaredHat;
} eeprom_config_t;

#define eeprom_config ((eeprom_config_t *)DATA_EEPROM_BASE)

#define EEPROM_READ(addr, dest) Eeprom_Read_Obj((uint8_t *) &(addr), (uint8_t *)&(dest), (uint16_t) sizeof(dest))
#define EEPROM_WRITE(addr, dest) Eeprom_Write_Obj((uint8_t *) &(addr), (uint8_t *)&(dest), (uint16_t) sizeof(dest))

#define EEPROM_READ_OBJ(addr, dest) Eeprom_Read_Obj((uint8_t *)(addr), (uint8_t *)&dest, (uint16_t) sizeof(dest))
#define EEPROM_WRITE_OBJ(addr, dest) Eeprom_Write_Obj((uint8_t *)(addr), (uint8_t *)&dest, (uint16_t) sizeof(dest))

#endif /* EEPROM_H_ */
