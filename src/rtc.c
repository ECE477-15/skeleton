/*
 * rtc.c
 *
 *  Created on: Apr 28, 2021
 *      Author: eto
 */
#include "rtc.h"



void rtc_init() {
	//	To wake up from the Stop mode with an RTC Wakeup event, it is necessary to:

	// enable rtc
	SET_BIT(RCC->APB1ENR, RCC_APB1ENR_PWREN); // enable power interface clock
	SET_BIT(PWR -> CR, PWR_CR_DBP); // enable write access
	SET_BIT(RCC -> CSR, RCC_CSR_RTCEN); //Enable RTC
	SET_BIT(RCC -> CSR, RCC_CSR_RTCSEL_LSE);

	// LSE (Low Speed External) Clock initialization
	SET_BIT(RCC -> CSR, RCC_CSR_LSEON); // Enable LSE clock to act as RTC clock source
	while((RCC -> CSR & RCC_CSR_LSERDY) != RCC_CSR_LSERDY)
	{
		//Wait for LSE clock to be ready before moving on
	}
	SET_BIT(RCC->CIER, RCC_CIER_LSERDYIE); // lse ready interrupt enabled


	//	a) Configure the EXTI Line 20 to be sensitive to rising edges (Interrupt or Eventmodes)

	SET_BIT(EXTI->RTSR, EXTI_RTSR_RT20);	// rising edge
	WRITE_REG(EXTI->PR, EXTI_PR_PIF20);		// Clear pending bit (if any)
//	SET_BIT(EXTI->EMR, EXTI_EMR_EM20);		// Enable Event
	SET_BIT(EXTI->IMR, EXTI_IMR_IM20);		// Enable interrupt

	NVIC_SetPriority(RTC_IRQn,0);
	NVIC_EnableIRQ(RTC_IRQn);

	//	b) Enable the RTC Wakeup Interrupt in the RTC_CR register

	//	c) Configure the RTC to generate the RTC Wakeup event
}

void rtc_setTimeout(int minutes){

}


