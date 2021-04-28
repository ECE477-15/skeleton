/*
 * rest.c
 *
 *  Created on: Apr 28, 2021
 *      Author: eto
 */

#include "hats.h"
#include "main.h"
#include "String.h"
#include "delay.h"


void rest_enter_stop();


void rest_enter_stop() {
	// Turn off VREFint
	CLEAR_BIT(SYSCFG->CFGR3, SYSCFG_CFGR3_EN_VREFINT);
	CLEAR_BIT(PWR->CR, PWR_CR_ULP);

	// set regulator to low-power
	SET_BIT(PWR->CR, PWR_CR_LPSDSR);

	// enter stop mode on WFI
	CLEAR_BIT(PWR->CR, PWR_CR_PDDS);
//	SET_BIT(PWR->CR, PWR_CR_PDDS);

	// do deep sleep on WFI
	SET_BIT(SCB->SCR, SCB_SCR_SLEEPDEEP_Msk);

	// WFI
	__WFI();

	// hey, woke up
	CLEAR_BIT(SCB->SCR, SCB_SCR_SLEEPDEEP_Msk);
}

void rest_initGPIO()
{
	RCC->APB2ENR |= RCC_APB2ENR_SYSCFGEN;	// RCC_SYSCFG_CLK_ENABLE

	// set pa0 for input
	RCC->IOPENR |= RCC_IOPENR_GPIOAEN;   // enable clock to gpio A
	RCC->IOPSMENR |= RCC_IOPSMENR_GPIOASMEN;   // enable clock for port A during sleep
	GPIOA->MODER &= !(GPIO_MODER_MODE0);      // clearing mode value to (00) for input
	GPIOA->PUPDR &= !(GPIO_PUPDR_PUPD0);   // set pin to no pull up

}



