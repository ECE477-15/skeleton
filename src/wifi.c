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
#include "String.h"

typedef enum {
	check_none,
	check_OK,
	check_ready,
	check_conn_ip_ok,	// todo
	check_mqttconn_ok,	// todo
	check_smartconn
} check_t;

void wifi_send_AT(char * str, check_t check);
void check_ok_fn();
void check_ready_fn();
void check_smartconn_fn();

void wifi_setup() {
	delay_ms(5000);	// wait, might try to connect/disconnect from previous session
	wifi_send_AT("AT+RESTORE\r\n", check_ready);
	wifi_send_AT("ATE1\r\n", check_OK); // prefer 0, can use 1 for debugging (fills buffer, more processing)
	wifi_send_AT("AT+UART_DEF=115200,8,1,0,3\r\n", check_OK);
	wifi_send_AT("AT\r\n", check_OK);

	// Get connected to Wifi
	//	- provisioning
	wifi_send_AT("AT+CWMODE=1\r\n", check_OK);

	wifi_send_AT("AT+CWSTARTSMART=1\r\n", check_smartconn);
	wifi_send_AT("AT+CWSTOPSMART\r\n", check_OK);

	// cheating it for now
//	wifi_send_AT("AT+CWJAP=\"sauce pan\",\"youidleweed420\"\r\n", check_OK);

	// Make sure mqtt connection established
	wifi_send_AT("AT+MQTTUSERCFG=0,1,\"ESP32\",\"MuT\",\"MuTpass\",0,0,\"\"\r\n", check_OK);
	wifi_send_AT("AT+MQTTCONNCFG=0,120,0,\"\",\"\",0,0\r\n", check_OK);
	wifi_send_AT("AT+MQTTCONN=0,\"192.168.1.137\",1883,1\r\n", check_OK);
	wifi_send_AT("AT+MQTTPUB=0,\"homeassistant/sensor/sensorBedroom/state\",\"12\",0,0\r\n", check_OK);
}

void wifi_send_mqtt(char * topic, char * payload) {
	buf_writeStr_var("AT+MQTTPUB=0,\"", (Buffer *)uart1_tx_buffer);
	buf_writeStr_var(topic, (Buffer *)uart1_tx_buffer);
	buf_writeStr_var("\",\"", (Buffer *)uart1_tx_buffer);
	buf_writeStr_var(payload, (Buffer *)uart1_tx_buffer);
	wifi_send_AT("\",0,0\r\n", check_OK);
}

void wifi_send_AT(char * str, check_t check) {
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

	switch(check) {
		case check_OK:
			check_ok_fn();
			break;
		case check_ready:
			check_ready_fn();
			break;
		case check_smartconn:
			check_smartconn_fn();
			break;
		default:
			break;
	}

	// Clear the buffer
	buf_clear((Buffer *)uart1_rx_buffer);
}

void check_ok_fn() {
	bool alive = true;
	uint8_t prevHead = uart1_rx_buffer->head;
	uint8_t prevLineLen = 0;

	// Wait for an eventual "OK\r\n" message, even if other messages come before it
	while(alive) {
		while(uart1Flag == 0);
		uint8_t head = uart1_rx_buffer->head;
		uint8_t lineLen = head - prevHead;
		uart1Flag--;

		if(lineLen + prevLineLen == 6) {	// cheap - check for match of message length "\r\nOK\r\n"
			if(BUF_GET_AT_BIG(uart1_rx_buffer,head-4) == 'O' && BUF_GET_AT_BIG(uart1_rx_buffer,head-3) == 'K' && BUF_GET_AT_BIG(uart1_rx_buffer,head-2) == '\r' && BUF_GET_AT_BIG(uart1_rx_buffer,head-1) == '\n') {
				alive = false;	// received OK\r\n
			}
		}

		prevHead = head;
		prevLineLen = lineLen;
	}
}

void check_ready_fn() {
	bool alive = true;
	uint8_t prevHead = uart1_rx_buffer->head;
	uint8_t prevLineLen = 0;

	while(alive) {
		while(uart1Flag == 0);
		uint8_t head = uart1_rx_buffer->head;
		uint8_t lineLen = head - prevHead;
		uart1Flag--;

		if(lineLen + prevLineLen == 9) {	// cheap - check for match of message length "\r\nready\r\n"
			if(BUF_GET_AT_BIG(uart1_rx_buffer,head-7) == 'r' && BUF_GET_AT_BIG(uart1_rx_buffer,head-6) == 'e' && BUF_GET_AT_BIG(uart1_rx_buffer,head-5) == 'a' && BUF_GET_AT_BIG(uart1_rx_buffer,head-4) == 'd' && BUF_GET_AT_BIG(uart1_rx_buffer,head-3) == 'y') {
				alive = false;
			}
		}

		prevHead = head;
		prevLineLen = lineLen;
	}
}

void check_smartconn_fn() {
	bool alive = true;
	uint8_t prevHead = uart1_rx_buffer->head;

//	uint8_t connArr[12]; // debug
//	uint8_t connI = 0; // debug

	char match[] = "smartconfig connected wifi\r\n";
	uint8_t strLen = strlen(match);

	while(alive) {
		while(uart1Flag == 0);
		uint8_t head = uart1_rx_buffer->head;
		uint8_t lineLen = head - prevHead;
		uart1Flag--;

//		connArr[connI++] = lineLen; // debug

		if(lineLen == strLen) {
			alive = true;
			unsigned char *letter = &(uart1_rx_buffer->buffer[head - strLen - 1]);
			unsigned char *last = &(uart1_rx_buffer->buffer[head]);
			char *matchI = match;
			for(;letter < last; letter++, matchI++) {
				if(*letter != *matchI) {
					alive = false;
					break;
				}
			}
		}

		prevHead = head;
	}
}
