/*
 * adc.c
 *
 *  Created on: Apr 12, 2021
 *      Author: grantweiss
 */

#include "stm32l0xx.h"
#include <adc.h>

// TODO convert all register manipulation to use macros

void adc_calibrate() {
	if ((ADC1->CR & ADC_CR_ADEN) != 0) {
		ADC1->CR |= ADC_CR_ADDIS;
	}
	ADC1->CR |= ADC_CR_ADCAL;
	while ((ADC1->ISR & ADC_ISR_EOCAL) == 0);
	ADC1->ISR |= ADC_ISR_EOCAL;
}

void adc_enable() {
	CLEAR_BIT(ADC1->CR, ADC_CR_ADEN);
	MODIFY_REG(ADC->CCR, ADC_CCR_PRESC, (ADC_CCR_PRESC_0 | ADC_CCR_PRESC_1 | ADC_CCR_PRESC_3));

	ADC1->ISR |= ADC_ISR_ADRDY;
	ADC1->CR |= ADC_CR_ADEN;

//	for(uint32_t wait = 0; wait < 1000; ++wait);

	if((ADC1->CFGR1 & ADC_CFGR1_AUTOFF) == 0) {
		while ((ADC1->ISR & ADC_ISR_ADRDY) == 0);
	}
}

void adc_disable() {
	if ((ADC1->CR & ADC_CR_ADSTART) != 0) {
	 ADC1->CR |= ADC_CR_ADSTP;
	}
	while ((ADC1->CR & ADC_CR_ADSTP) != 0);

	ADC1->CR |= ADC_CR_ADDIS;
	while ((ADC1->CR & ADC_CR_ADEN) != 0);
}

// This function should enable the clock to ADC,
// turn on the clocks, wait for ADC to be ready.
void adc_setup() {
    RCC->APB2ENR |= RCC_APB2ENR_ADC1EN;     // enable the clock to ADC
    RCC->CR |= RCC_CR_HSION;     			// turn on clock
    while(!(RCC->CR & RCC_CR_HSIRDY));  	// wait for clock to be ready
}

uint32_t adc_oneshot(uint32_t channel) {
	ADC1->CFGR2 &= ~ADC_CFGR2_CKMODE;
	ADC1->CFGR1 &= ~ADC_CFGR1_AUTOFF;

	ADC1->SMPR |= ADC_SMPR_SMP; // sampling time, amount of time to charge adc capacitor
	ADC1->CHSELR = channel;
	while(!(ADC1->ISR & ADC_ISR_ADRDY));

	ADC1->CR |= ADC_CR_ADSTART;

	while(!(ADC1->ISR & ADC_ISR_EOC));
	return ADC1->DR;
}


