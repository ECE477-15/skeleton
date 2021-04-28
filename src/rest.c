/*
 * rest.c
 *
 *  Created on: Apr 28, 2021
 *      Author: eto
 */

#include "hats.h"
#include "main.h"
#include "delay.h"


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




