/*
 * eeprom.c
 *
 *  Created on: Apr 12, 2021
 *      Author: grantweiss
 */

#include "eeprom.h"

void Eeprom_Write_Obj(uint8_t *wAddr, uint8_t *obj, uint16_t size) {
	if((FLASH->PECR & FLASH_PECR_PELOCK) != RESET) {
		/* Disable interrupts to avoid any interruption during unlock sequence */
		  uint32_t primask_bit = __get_PRIMASK();
		__disable_irq();

		/* Unlocking the Data memory and FLASH_PECR register access*/
		FLASH->PEKEYR = ((uint32_t)0x89ABCDEFU);
		FLASH->PEKEYR = ((uint32_t)0x02030405U);

		/* Re-enable the interrupts: restore previous priority mask */
		__set_PRIMASK(primask_bit);

		if((FLASH->PECR & FLASH_PECR_PELOCK) != RESET) {
			error(__LINE__); // TODO
		}
	}

    uint8_t *ptr = (uint8_t *)obj;

    for (uint8_t i=0;i<size;i++)
    	*(wAddr++) = *(ptr++);

    SET_BIT(FLASH->PECR, FLASH_PECR_PELOCK);
}

void Eeprom_Read_Obj(uint8_t *wAddr, uint8_t *obj, uint16_t size) {
	while(size--) {
		*obj++ = *wAddr++;
	}
}
