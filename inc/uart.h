/*
 * uart.h
 *
 *  Created on: Apr 19, 2021
 *      Author: grantweiss
 */

#ifndef UART_H_
#define UART_H_

#define UART2_BAUD 9600

#include "stm32l0xx.h"
#include "ringBuf.h"

void uart2_init(void);
void uart2_transmit(char * str);
void uart2_receive(void);

Big_Buffer *uart1_tx_buffer;
Big_Buffer *uart1_rx_buffer;
Buffer *uart2_tx_buffer;
Buffer *uart2_rx_buffer;
volatile uint16_t uart2Flag;
void uart2_update_match(uint8_t);

#endif /* UART_H_ */
