/*
 * delay.c
 *
 *  Created on: Apr 19, 2021
 *      Author: grantweiss
 *      Reference: https://github.com/LonelyWolf/stm32/blob/master/SSD1306/delay.c
 */
#include "delay.h"

// SysTick interrupt handler
void SysTick_Handler(void) {
	delay_count++;
}

void delay_ms(uint32_t ms) {
	// Enable the SysTick timer
	SysTick->CTRL |= SysTick_CTRL_ENABLE_Msk;

	// Wait for a specified number of milliseconds
	delay_count = 0;
	while (delay_count < ms);

	// Disable the SysTick timer
	SysTick->CTRL &= ~SysTick_CTRL_ENABLE_Msk;
}

void delay_lapse_start() {
	delay_count = 0;

	// Enable the SysTick timer
	SysTick->CTRL |= SysTick_CTRL_ENABLE_Msk;
}

void delay_lapse_end() {
	delay_count = 0;

	// Disable the SysTick timer
	SysTick->CTRL &= ~SysTick_CTRL_ENABLE_Msk;
}

void delay_init(void) {
	// Set reload register to generate IRQ every millisecond
	SysTick->LOAD = (uint32_t)(SystemCoreClock / (1000UL - 1UL));

	// Set priority for SysTick IRQ
	NVIC_SetPriority(SysTick_IRQn,(1 << __NVIC_PRIO_BITS) - 1);

	// Set the SysTick counter value
	SysTick->VAL = 0UL;

	// Set SysTick source and IRQ
	SysTick->CTRL = (SysTick_CTRL_CLKSOURCE_Msk | SysTick_CTRL_TICKINT_Msk);
}
