/*
 * wifi.c
 *
 *  Created on: Apr 21, 2021
 *      Author: grantweiss
 */


#include "uart.h"
#include "wifi.h"
#include "ringBuf.h"
#include "delay.h"
#include "stdbool.h"

void wifi_send_at(char * str, bool check);
uint16_t wifi_ok(Buffer *buffer);
void wifi_at_sender_checker(char * str);

void wifi_setup() {
	uart1_receive();

	wifi_send_at("AT+UART_DEF=115200,8,1,0,3\r\n", true);
	wifi_send_at("ATE0\r\n", true);
	wifi_send_at("AT\r\n", true);

	// Get connected to Wifi
	//	- provisioning
	wifi_send_at("AT+CWMODE=1\r\n", true);

//	wifi_send_at("AT+CWSTARTSMART=1\r\n", true);
//	// WAIT for connected string
//	wifi_send_at("AT+CWSTOPSMART\r\n", true);

	// cheating it for now
	wifi_at_sender_checker("AT+CWJAP=\"sauce pan\",\"youidleweed420\"\r\n");

	// Make sure mqtt connection established
	wifi_send_at("AT+MQTTUSERCFG=0,1,\"ESP32\",\"MuT\",\"MuTpass\",0,0,\"\"\r\n", true);
	wifi_send_at("AT+MQTTCONNCFG=0,120,0,\"\",\"\",0,0\r\n", true);
	wifi_send_at("AT+MQTTCONN=0,\"192.168.1.137\",1883,1\r\n", true);
	wifi_send_at("AT+MQTTPUB=0,\"homeassistant/sensor/sensorBedroom/state\",\"12\",0,0\r\n", true);
}


void wifi_at_sender_checker(char * str) {
	// Clear the buffer
	buf_clear((Buffer *)uart1_rx_buffer);

	// Make sure it's at 0, since ISR increments it
	uart1Flag = 0;

	// Set flag when received '\n'
	uart1_update_match('\n');

	// Make sure receiving is enabled
	uart1_receive();

	// Send the command
	uart1_transmit(str);

	bool alive = true;

	#define BUF_CHECK(BUF,DEC,CHK) (BUF_GET_TOP_VAR(uart1_rx_buffer, 4) == CHK)

	uint8_t calls = 0;
	uint8_t bufLenAt[5];
	uint16_t lastTail = uart1_rx_buffer->tail;

	while(uart1Flag < 5);

	while(alive) {
		while(uart1Flag == 0);
		uart1Flag--;

		bufLenAt[calls] = BUF_USED(uart1_rx_buffer);

		calls++;

		// ETHAN, check line below, should pass and set alive to false
		if(BUF_CHECK(uart1_rx_buffer, 4, 'O') && BUF_CHECK(uart1_rx_buffer, 3, 'K') && BUF_CHECK(uart1_rx_buffer, 2, '\r') && BUF_CHECK(uart1_rx_buffer, 1, '\n')) {
			alive = false;
		}

		uart1_rx_buffer->tail = uart1_rx_buffer->head;
	}

	// Clear the buffer
	buf_clear((Buffer *)uart1_rx_buffer);
}


void wifi_send_at(char * str, bool check) {
	// Clear the buffer
	buf_clear((Buffer *)uart1_rx_buffer);

	// Make sure it's at 0, since ISR increments it
	uart1Flag = 0;

	// Set flag when received '\n'
	uart1_update_match('\n');

	// Make sure receiving is enabled
	uart1_receive();

	// Send the command
	uart1_transmit(str);

	// wait for two carriage returns
	while(uart1Flag < 2);
	uart1Flag = 0;

	// check for good response
	if(check) {
		if(!wifi_ok((Buffer *)uart1_rx_buffer)) {
			error(__LINE__);	// error
		}
	}

	// Clear the buffer
	buf_clear((Buffer *)uart1_rx_buffer);
}

uint16_t wifi_ok(Buffer *buff) {
	if(BUF_GET_AT_VAR(buff, buff->head-4) == 'O' && BUF_GET_AT_VAR(buff, buff->head-3) == 'K' && BUF_GET_AT_VAR(buff, buff->head-2) == '\r' && BUF_GET_AT_VAR(buff, buff->head-1) == '\n') {
		return 1;
	}
	return 0;
}
