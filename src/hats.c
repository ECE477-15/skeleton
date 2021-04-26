/*
 * hats.c
 *
 *  Created on: Apr 12, 2021
 *      Author: grantweiss
 */

#include "hats.h"
#include "main.h"
#include "uart.h"
#include "xbee.h"

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

	CLEAR_BIT(GPIOA->MODER, GPIO_MODER_MODE4); // input mode
	CLEAR_BIT(GPIOA->PUPDR, GPIO_PUPDR_PUPD4); // no pull resistors

	// make sure pull-down is not activated
	SET_BIT(GPIOA->MODER, GPIO_MODER_MODE8); // analog mode

	// set EXTI 5 to trigger from Port A
	MODIFY_REG(SYSCFG->EXTICR[1], SYSCFG_EXTICR2_EXTI4, SYSCFG_EXTICR2_EXTI4_PA);

	if(global_state.hatDetectTrig == hat_connect) {
		SET_BIT(EXTI->RTSR, EXTI_RTSR_RT4);		// rising edge
		CLEAR_BIT(EXTI->FTSR, EXTI_FTSR_FT4); 	// not falling edge
	} else { // hat_disconnect
		CLEAR_BIT(EXTI->RTSR, EXTI_RTSR_RT4);	// not rising edge
		SET_BIT(EXTI->FTSR, EXTI_FTSR_FT4); 	// falling edge
	}
	WRITE_REG(EXTI->PR, EXTI_PR_PIF4);		// Clear pending bit (if any)
	SET_BIT(EXTI->EMR, EXTI_EMR_EM4);		// Enable Event
	SET_BIT(EXTI->IMR, EXTI_IMR_IM4);		// Enable interrupt

	NVIC_SetPriority(EXTI4_15_IRQn,0);
	NVIC_EnableIRQ(EXTI4_15_IRQn);
}

void send_homeassistant_boolean_PB11() {
	uint16_t value = ((GPIOB->IDR & GPIO_IDR_ID11) != 0x0);

	// adjustment for active-low?
//	if(GET_HAT_CONFIG(global_state->connectedHat)->activeLow == true) {
//		value = !value;
//	}

	// send homeassistant an update on a boolean sensor value
	// TODO
	tx_req_frame_t txReq = {
			.addrH = ENDIAN_SWAP32(0x0),
			.addrL = ENDIAN_SWAP32(0xFFFF),
	};
	uint8_t payload[3] = {(char)send_value, (value & 0xFF), 0x0};
	xbee_msg->payload = payload;
	xbee_msg->payloadLen = 3;
	xbee_send_message(&txReq);

}

__attribute__( ( always_inline ) ) __STATIC_INLINE void EXTI11_IRQHandler() {
	hat_flag = 0x1;
}

__attribute__( ( always_inline ) ) __STATIC_INLINE void EXTI4_IRQHandler() {
	// HAT CONNECTED!
	hat_conn_flag = 0x1;
}

void EXTI4_15_IRQHandler() {
	if(READ_BIT(EXTI->PR, EXTI_PR_PIF11) != RESET) {	// check for pending event
		WRITE_REG(EXTI->PR, EXTI_PR_PIF11);				// clear event

		EXTI11_IRQHandler();
	}
	if(READ_BIT(EXTI->PR, EXTI_PR_PIF4) != RESET) {	// check for pending event
		WRITE_REG(EXTI->PR, EXTI_PR_PIF4);				// clear event

		EXTI4_IRQHandler();
	}

}

void blank_setup() {

}

void hat_gpio_setup_uart() {
	uint32_t baudrate = 115200; // TODO need to dynamically set this

	// Enable Clock
	SET_BIT(RCC->IOPENR, RCC_IOPENR_GPIOAEN | RCC_IOPENR_GPIOBEN);
	SET_BIT(RCC->APB1ENR, RCC_APB1ENR_LPUART1EN);

	// Set PB1,10,11; PA6 to Alternate Function
	MODIFY_REG(GPIOB->MODER, (GPIO_MODER_MODE1 | GPIO_MODER_MODE10 | GPIO_MODER_MODE11), (GPIO_MODER_MODE1_1 | GPIO_MODER_MODE10_1 | GPIO_MODER_MODE11_1));
	MODIFY_REG(GPIOA->MODER, GPIO_MODER_MODE6, GPIO_MODER_MODE6_1);

	// no pulls-up or pull-down
	CLEAR_BIT(GPIOB->PUPDR, (GPIO_PUPDR_PUPD1 | GPIO_PUPDR_PUPD10 | GPIO_PUPDR_PUPD11));
	CLEAR_BIT(GPIOA->PUPDR, GPIO_PUPDR_PUPD6);

	// push-pull
	CLEAR_BIT(GPIOB->OTYPER, (GPIO_OTYPER_OT_1 | GPIO_OTYPER_OT_10 | GPIO_OTYPER_OT_11));
	CLEAR_BIT(GPIOA->OTYPER, GPIO_OTYPER_OT_6);

	// very high speed
	SET_BIT(GPIOB->OSPEEDR, (GPIO_OSPEEDER_OSPEED1 | GPIO_OSPEEDER_OSPEED10 | GPIO_OSPEEDER_OSPEED11));
	SET_BIT(GPIOA->OSPEEDR, GPIO_OSPEEDER_OSPEED6);

	// AF4 = lpuart1
	MODIFY_REG(GPIOB->AFR[0], GPIO_AFRL_AFSEL1, (0x4 << GPIO_AFRL_AFSEL1_Pos));	// PB1
	MODIFY_REG(GPIOB->AFR[1], (GPIO_AFRH_AFSEL10 | GPIO_AFRH_AFSEL11), (0x4 << GPIO_AFRH_AFSEL10_Pos) | (0x4 << GPIO_AFRH_AFSEL11_Pos));	// PB10, 11
	MODIFY_REG(GPIOA->AFR[0], GPIO_AFRL_AFSEL6, (0x4 << GPIO_AFRL_AFSEL6_Pos));	// PA6

	NVIC_SetPriority(RNG_LPUART1_IRQn, 0);
	NVIC_EnableIRQ(RNG_LPUART1_IRQn);

	// Disable UART peripheral for configuration
	CLEAR_BIT(LPUART1->CR1, USART_CR1_UE);

	// Set clock source
	CLEAR_BIT(RCC->CCIPR, RCC_CCIPR_LPUART1SEL);	// 00:APB clock, 01:System clock, 10:HSI16 clock, 11:LSE clock
//	MODIFY_REG(RCC->CCIPR, RCC_CCIPR_LPUART1SEL, RCC_CCIPR_LPUART1SEL_0);

	// Configure CR1
	MODIFY_REG(LPUART1->CR1, (USART_CR1_M | USART_CR1_PCE | USART_CR1_PS | USART_CR1_TE | USART_CR1_RE | USART_CR1_OVER8), (USART_CR1_TE | USART_CR1_RE));

	// Configure CR2
	MODIFY_REG(LPUART1->CR2, (USART_CR2_ADD | USART_CR2_STOP | USART_CR2_LINEN | USART_CR2_CLKEN), (((uint8_t)'\n') << USART_CR2_ADD_Pos));

	// Configure CR3
	MODIFY_REG(LPUART1->CR3,
			(USART_CR3_RTSE | USART_CR3_CTSE | USART_CR3_ONEBIT | USART_CR3_SCEN | USART_CR3_HDSEL | USART_CR3_IREN),
					(USART_CR3_RTSE | USART_CR3_CTSE)
//					(0x0)	// hardware flow control?
			);

	// Set Baud-rate
	#define UART_DIV_LPUART(__PCLK__, __BAUD__)      (((((uint64_t)(__PCLK__)*256U)) + ((__BAUD__)/2U)) / (__BAUD__))
	uint32_t pclk = (SystemCoreClock >> APBPrescTable[(RCC->CFGR & RCC_CFGR_PPRE1) >> RCC_CFGR_PPRE1_Pos]);				// only if RCC->CCIPR = 0b00
	LPUART1->BRR = (uint32_t)(UART_DIV_LPUART(pclk, baudrate));

	// Enable UART peripheral
	SET_BIT(LPUART1->CR1, USART_CR1_UE);
}
