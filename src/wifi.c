/*
 * wifi.c
 *
 *  Created on: Apr 21, 2021
 *      Author: grantweiss
 */

#include "wifi.h"
#include "ringBuf.h"
#include "uart.h"

void wifi_setup() {
	buf_writeStr_var("AT\r\n", (Buffer *)uart1_tx_buffer);
	SET_BIT(LPUART1->CR1, USART_CR1_TXEIE);

	// Get connected to Wifi
	//	- provisioning

	// Make sure mqtt connection established
}
