/*
 * hats.c
 *
 *  Created on: Apr 12, 2021
 *      Author: grantweiss
 */

#include "hats.h"
#include "main.h"

// Setup hat for PB11 generating interrupts on LO and HI (falling and rising)
void hat_interrupt_PB11(void) {
	SET_BIT(RCC->IOPENR, RCC_IOPENR_GPIOBEN);	// Enable GPIOB Clock

	CLEAR_BIT(GPIOB->MODER, GPIO_MODER_MODE11); // input mode
	CLEAR_BIT(GPIOB->PUPDR, GPIO_PUPDR_PUPD11); // no pull resistors

	// set EXTI 11 to trigger from Port B
	MODIFY_REG(SYSCFG->EXTICR[2], SYSCFG_EXTICR3_EXTI11, SYSCFG_EXTICR3_EXTI11_PB);

	SET_BIT(EXTI->RTSR, EXTI_RTSR_RT11);	// rising edge
	SET_BIT(EXTI->FTSR, EXTI_FTSR_FT11); 	// falling edge
	WRITE_REG(EXTI->PR, EXTI_PR_PIF11);		// Clear pending bit (if any)
	SET_BIT(EXTI->EMR, EXTI_EMR_EM11);		// Enable Event
	SET_BIT(EXTI->IMR, EXTI_IMR_IM11);		// Enable interrupt

	NVIC_SetPriority(EXTI4_15_IRQn,0);
	NVIC_EnableIRQ(EXTI4_15_IRQn);
}

void hat_detect_interrupt() {
	SET_BIT(RCC->IOPENR, RCC_IOPENR_GPIOAEN);	// Enable GPIOA Clock

	CLEAR_BIT(GPIOA->MODER, GPIO_MODER_MODE5); // input mode
	CLEAR_BIT(GPIOA->PUPDR, GPIO_PUPDR_PUPD5); // no pull resistors

	// make sure pull-down is not activated
	SET_BIT(GPIOA->MODER, GPIO_MODER_MODE7); // analog mode

	// set EXTI 5 to trigger from Port A
	MODIFY_REG(SYSCFG->EXTICR[1], SYSCFG_EXTICR2_EXTI5, SYSCFG_EXTICR2_EXTI5_PA);

	if(global_state.hatDetectTrig == hat_connect) {
		SET_BIT(EXTI->RTSR, EXTI_RTSR_RT5);		// rising edge
		CLEAR_BIT(EXTI->FTSR, EXTI_FTSR_FT5); 	// not falling edge
	} else { // hat_disconnect
		CLEAR_BIT(EXTI->RTSR, EXTI_RTSR_RT5);	// not rising edge
		SET_BIT(EXTI->FTSR, EXTI_FTSR_FT5); 	// falling edge
	}
	WRITE_REG(EXTI->PR, EXTI_PR_PIF5);		// Clear pending bit (if any)
	SET_BIT(EXTI->EMR, EXTI_EMR_EM5);		// Enable Event
	SET_BIT(EXTI->IMR, EXTI_IMR_IM5);		// Enable interrupt

	NVIC_SetPriority(EXTI4_15_IRQn,0);
	NVIC_EnableIRQ(EXTI4_15_IRQn);
}

void send_homeassistant_boolean_PB11() {
	uint16_t value = ((GPIOB->IDR & GPIO_IDR_ID11) != 0x0);

	// adjustment for active-low?

	// send homeassistant an update on a boolean sensor value
	// TODO
}

void EXTI11_IRQHandler() {
	hat_flag = 0x1;
}

void EXTI5_IRQHandler() {
	// HAT CONNECTED!
	hat_conn_flag = 0x1;
}

void EXTI4_15_IRQHandler() {
	if(READ_BIT(EXTI->PR, EXTI_PR_PIF11) != RESET) {	// check for pending event
		WRITE_REG(EXTI->PR, EXTI_PR_PIF11);				// clear event

		EXTI11_IRQHandler();
	}
	if(READ_BIT(EXTI->PR, EXTI_PR_PIF5) != RESET) {	// check for pending event
		WRITE_REG(EXTI->PR, EXTI_PR_PIF5);				// clear event

		EXTI5_IRQHandler();
	}

}
