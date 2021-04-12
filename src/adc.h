/*
 * adc.h
 *
 *  Created on: Apr 12, 2021
 *      Author: grantweiss
 */

#ifndef ADC_H_
#define ADC_H_

#include <stm32l0xx.h>

#define VREFINT_CAL_ADDR (uint16_t *)(0x1FF80078UL)
#define VREFINT_CAL (*VREFINT_CAL_ADDR)

void adc_setup();
uint32_t adc_oneshot(uint32_t channel);

static __inline uint32_t adc_get_vref() {
	ADC->CCR |= ADC_CCR_VREFEN;
	uint32_t VREFINT_DATA = adc_oneshot(ADC_CHSELR_CHSEL17);
	ADC->CCR &= ~(ADC_CCR_VREFEN);

	return VREFINT_DATA;
}

#endif /* ADC_H_ */
