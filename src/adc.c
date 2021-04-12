/*
 * adc.c
 *
 *  Created on: Apr 12, 2021
 *      Author: grantweiss
 */

#include <adc.h>

// This function should enable the clock to ADC,
// turn on the clocks, wait for ADC to be ready.
void adc_setup() {
    RCC->APB2ENR |= RCC_APB2ENR_ADC1EN;     // enable the clock to ADC
    RCC->CR |= RCC_CR_HSION;     			// turn on clock
    while(!(RCC->CR & RCC_CR_HSIRDY));  	// wait for clock to be ready
    ADC1->CR |= ADC_CR_ADEN;   				// enable ADC
    while(!(ADC1->ISR & ADC_ISR_ADRDY));    // wait for ADC to be ready
    while((ADC1->CR & ADC_CR_ADSTART));   	// wait for ADCstart to be 0.
}

uint32_t adc_oneshot(uint32_t channel) {
	ADC1->CFGR1 |= ADC_CFGR1_AUTOFF;
	ADC1->CHSELR = channel;
	while(!(ADC1->ISR & ADC_ISR_ADRDY));
	ADC1->CR |= ADC_CR_ADSTART;

	while(!(ADC1->ISR & ADC_ISR_EOC));
	return ADC1->DR;
}

