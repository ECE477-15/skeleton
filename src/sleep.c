/*
 * sleep.c
 *
 *  Created on: Apr 13, 2021
 *      Author: grantweiss
 */
#include "sleep.h"

void enter_sleep() {
//	PWR->CR |= PWR_CR_PDDS; // enter standby mode when entering deepsleep
//	__WFI(); // Check global status (state machine) on wake-up

	// TODO doesnt work
}

// wake-up from any EXTI lines
void enter_stop() {
	// Turn off VREFint
	CLEAR_BIT(SYSCFG->CFGR3, SYSCFG_CFGR3_EN_VREFINT);
	CLEAR_BIT(PWR->CR, PWR_CR_ULP);

	// set regulator to low-power
	SET_BIT(PWR->CR, PWR_CR_LPSDSR);

	// enter stop mode on WFI
	CLEAR_BIT(PWR->CR, PWR_CR_PDDS);

	// do deep sleep on WFI
	SET_BIT(SCB->SCR, SCB_SCR_SLEEPDEEP_Msk);

	// WFI
	__WFI();

	// hey, woke up
	CLEAR_BIT(SCB->SCR, SCB_SCR_SLEEPDEEP_Msk);
}
