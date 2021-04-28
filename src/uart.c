/*
 * uart.c
 *
 *  Created on: Apr 19, 2021
 *      Author: grantweiss
 */

#include "uart.h"
#include "main.h"
#include <string.h>

Big_Buffer _uart1_tx_buffer = {.head=0, .tail=0, .len=BIG_BUFFER_SIZE};
Big_Buffer _uart1_rx_buffer = {.head=0, .tail=0, .len=BIG_BUFFER_SIZE};

Big_Buffer *uart1_tx_buffer = &_uart1_tx_buffer;
Big_Buffer *uart1_rx_buffer = &_uart1_rx_buffer;

Buffer _uart2_tx_buffer = {.head=0, .tail=0, .len=BUFFER_SIZE};
Buffer _uart2_rx_buffer = {.head=0, .tail=0, .len=BUFFER_SIZE};

Buffer *uart2_tx_buffer = &_uart2_tx_buffer;
Buffer *uart2_rx_buffer = &_uart2_rx_buffer;

volatile uint16_t uart2Flag = 0;
volatile uint16_t uart1Flag = 0;

void uart2_receive() {
	// Enable the UART Error Interrupt: (Frame error, noise error, overrun error)
	SET_BIT(USART2->CR3, USART_CR3_EIE);

	// Enable the UART Parity Error interrupt and Data Register Not Empty interrupt
	SET_BIT(USART2->CR1, USART_CR1_PEIE | USART_CR1_RXNEIE);
}

void uart1_receive() {
	// Enable the UART Error Interrupt: (Frame error, noise error, overrun error)
	SET_BIT(LPUART1->CR3, USART_CR3_EIE);

	// Enable the UART Parity Error interrupt and Data Register Not Empty interrupt
	SET_BIT(LPUART1->CR1, USART_CR1_PEIE | USART_CR1_RXNEIE);
}

void uart2_transmit(const char * str) {
	buf_writeStr(str, uart2_tx_buffer);

	// Enable the Transmit Data Register Empty interrupt
	SET_BIT(USART2->CR1, USART_CR1_TXEIE);
}

void uart1_transmit(const char * str) {
	buf_writeStr_var(str, (Buffer *)uart1_tx_buffer);

	// Enable the Transmit Data Register Empty interrupt
	SET_BIT(LPUART1->CR1, USART_CR1_TXEIE);
}

void uart1_transmit_len(const char * str, uint16_t size) {
	buf_writeChars_var((Buffer *)uart1_tx_buffer, str, size);

	// Enable the Transmit Data Register Empty interrupt
	SET_BIT(LPUART1->CR1, USART_CR1_TXEIE);
}

void uart2_init(void) {
	// Enable Clock
	SET_BIT(RCC->IOPENR, RCC_IOPENR_GPIOAEN);
	SET_BIT(RCC->APB1ENR, RCC_APB1ENR_USART2EN);

	// Set PA0,1,2,3 to Alternate Function
	MODIFY_REG(GPIOA->MODER, (GPIO_MODER_MODE0 | GPIO_MODER_MODE1 | GPIO_MODER_MODE2 | GPIO_MODER_MODE3), (GPIO_MODER_MODE0_1 | GPIO_MODER_MODE1_1 | GPIO_MODER_MODE2_1 | GPIO_MODER_MODE3_1));

	// no pulls-up or pull-down
	CLEAR_BIT(GPIOA->PUPDR, (GPIO_PUPDR_PUPD0 | GPIO_PUPDR_PUPD1 | GPIO_PUPDR_PUPD2 | GPIO_PUPDR_PUPD3));

	// push-pull
	CLEAR_BIT(GPIOA->OTYPER, (GPIO_OTYPER_OT_0 | GPIO_OTYPER_OT_1 | GPIO_OTYPER_OT_2 | GPIO_OTYPER_OT_3));

	// very high speed
	SET_BIT(GPIOA->OSPEEDR, (GPIO_OSPEEDER_OSPEED0 | GPIO_OSPEEDER_OSPEED1 | GPIO_OSPEEDER_OSPEED2 | GPIO_OSPEEDER_OSPEED3));

	// AF4 = uart2
	MODIFY_REG(	GPIOA->AFR[0],
				(GPIO_AFRL_AFSEL0 | GPIO_AFRL_AFSEL1 | GPIO_AFRL_AFSEL2 | GPIO_AFRL_AFSEL3),
				(0x4 << GPIO_AFRL_AFSEL0_Pos) | (0x4 << GPIO_AFRL_AFSEL1_Pos) | (0x4 << GPIO_AFRL_AFSEL2_Pos) | (0x4 << GPIO_AFRL_AFSEL3_Pos)
				);

	// Disable UART peripheral for configuration
	CLEAR_BIT(USART2->CR1, USART_CR1_UE);

	// Configure CR1
	MODIFY_REG(	USART2->CR1,
				(USART_CR1_M | USART_CR1_PCE | USART_CR1_PS | USART_CR1_TE | USART_CR1_RE | USART_CR1_OVER8),
				(USART_CR1_TE | USART_CR1_RE)
			);

	// Configure CR2
	MODIFY_REG(	USART2->CR2,
				(USART_CR2_STOP | USART_CR2_LINEN | USART_CR2_CLKEN | USART_CR2_ADD),
				(((uint8_t)'\r') << USART_CR2_ADD_Pos)
			);

	// Configure CR3
	MODIFY_REG(	USART2->CR3,
				(USART_CR3_RTSE | USART_CR3_CTSE | USART_CR3_ONEBIT | USART_CR3_SCEN | USART_CR3_HDSEL | USART_CR3_IREN),
				(USART_CR3_RTSE | USART_CR3_CTSE)
//				(0x0) // hardware flow control?
			);

	// Set Baud-rate
	#define UART_DIV_SAMPLING16(__PCLK__, __BAUD__)  (((__PCLK__) + ((__BAUD__)/2U)) / (__BAUD__))
	uint32_t PCLK2Freq = SystemCoreClock >> APBPrescTable[(RCC->CFGR & RCC_CFGR_PPRE2) >> RCC_CFGR_PPRE2_Pos];
	USART2->BRR = (uint16_t)(UART_DIV_SAMPLING16(PCLK2Freq, UART2_BAUD));

	// Enable UART peripheral
	SET_BIT(USART2->CR1, USART_CR1_UE);

	NVIC_SetPriority(USART2_IRQn, 0);
	NVIC_EnableIRQ(USART2_IRQn);
}

void uart2_update_match(uint8_t match) {
	CLEAR_BIT(USART2->CR1, USART_CR1_UE);
	MODIFY_REG(USART2->CR2, USART_CR2_ADD, (match << USART_CR2_ADD_Pos));
	SET_BIT(USART2->CR1, USART_CR1_UE);
}

void uart1_update_match(uint8_t match) {
	CLEAR_BIT(LPUART1->CR1, USART_CR1_UE);
	MODIFY_REG(LPUART1->CR2, USART_CR2_ADD, (match << USART_CR2_ADD_Pos));
	SET_BIT(LPUART1->CR1, USART_CR1_UE);
}

void USART2_IRQHandler(void) {
	uint32_t isrflags   = READ_REG(USART2->ISR);
	uint32_t cr1its     = READ_REG(USART2->CR1);

	uint32_t errorflags = (isrflags & (uint32_t)(USART_ISR_PE | USART_ISR_FE | USART_ISR_ORE | USART_ISR_NE));

	// No errors
	if (errorflags == 0U) {
			// Receiving
			if (IS_RECEIVING) {
				char receiveData = (char) READ_REG(USART2->RDR);

				int bufPos = (unsigned int)(uart2_rx_buffer->head + 1) % BUFFER_SIZE;

				if(bufPos != uart2_rx_buffer->tail) {
					uart2_rx_buffer->buffer[uart2_rx_buffer->head] = receiveData;
					uart2_rx_buffer->head = bufPos;
				} else {	// buffer overflow
					error(__LINE__); // TODO
				}

//				if(receiveData == '\r') {
//					uartFlag = 1;
//				}
				if(READ_BIT(isrflags, USART_ISR_CMF) != RESET) {
					WRITE_REG(USART2->ICR, USART_ICR_CMCF);
					uart2Flag = 1;
				}
			}
			// transmitting
			if (IS_TRANSMITTING) {
				if(BUF_USED(uart2_tx_buffer) == 0) { // done transferring
					// Disable the UART Transmit Data Register Empty Interrupt
					CLEAR_BIT(USART2->CR1, USART_CR1_TXEIE);

					// Enable the UART Transmit Complete Interrupt
					SET_BIT(USART2->CR1, USART_CR1_TCIE);
				} else {
					// load next char into transmit data register
					USART2->TDR = (uint8_t)(buf_consumeByte(uart2_tx_buffer) & (uint8_t)0xFF);
				}
			}
			// Transmit complete
			if (IS_TX_COMPLETE) {
				CLEAR_BIT(USART2->CR1, (USART_CR1_TXEIE | USART_CR1_TCIE));
			}
		} else {
			// Error occurred
			error(__LINE__);
		}
}

void RNG_LPUART1_IRQHandler(void) {
	uint32_t isrflags   = READ_REG(LPUART1->ISR);
	uint32_t cr1its     = READ_REG(LPUART1->CR1);

	uint32_t errorflags = (isrflags & (uint32_t)(USART_ISR_PE | USART_ISR_FE | USART_ISR_ORE | USART_ISR_NE));

	// No errors
	if (errorflags == 0U) {
			// Receiving
			if (IS_RECEIVING) {
				char receiveData = (char) READ_REG(LPUART1->RDR);

				int bufPos = (unsigned int)(uart1_rx_buffer->head + 1) % BIG_BUFFER_SIZE;

				if(bufPos != uart1_rx_buffer->tail) {
					uart1_rx_buffer->buffer[uart1_rx_buffer->head] = receiveData;
					uart1_rx_buffer->head = bufPos;
				} else {	// buffer overflow
					error(__LINE__); // TODO
				}

//				if(receiveData == '\n') {
//					uart1Flag = 1;
//				}
				if(READ_BIT(isrflags, USART_ISR_CMF) != RESET) {
					WRITE_REG(LPUART1->ICR, USART_ICR_CMCF);
					uart1Flag++;
				}
			}
			// transmitting
			if (IS_TRANSMITTING) {
				if(BUF_USED_VAR(uart1_tx_buffer) == 0) { // done transferring
					// Disable the UART Transmit Data Register Empty Interrupt
					CLEAR_BIT(LPUART1->CR1, USART_CR1_TXEIE);

					// Enable the UART Transmit Complete Interrupt
					SET_BIT(LPUART1->CR1, USART_CR1_TCIE);
				} else {
					// load next char into transmit data register
					LPUART1->TDR = (uint8_t)(buf_consumeByte_var((Buffer *)uart1_tx_buffer) & (uint8_t)0xFF);
				}
			}
			// Transmit complete
			if (IS_TX_COMPLETE) {
				CLEAR_BIT(LPUART1->CR1, (USART_CR1_TXEIE | USART_CR1_TCIE));
			}
		} else {
			// Error occurred
			error(__LINE__);
		}
}


void lpuart_init(uint32_t baudrate) {
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
