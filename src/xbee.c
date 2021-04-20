/*
 * xbee.c
 *
 *  Created on: Apr 13, 2021
 *      Author: grantweiss
 */


#include "xbee.h"
#include "main.h"
#include "delay.h"
#include "mqtt.h"

/********************** FUNCTION DECLARATIONS **********************/
void xbee_send_at(char * str, bool check);
bool xbee_tx_stat_check(uint8_t * msg, uint16_t msgLen);
void xbee_send();

/********************** VARIABLES **********************/
uint8_t xbee_frameID = 0x1;
message_t _xbee_msg;
message_t *xbee_msg = &_xbee_msg;

/********************** FUNCTION DEFINITIONS **********************/
void xbee_setup() {
	// Enter into AT Command Mode
	delay_ms(1100);
	xbee_send_at(XBEE_AT_ENTERCMD, true);
	delay_ms(1100);

	xbee_send_at(XBEE_AT_RESTORE	"\r", 		true);	// Reset settings, expect OK
	xbee_send_at(XBEE_AT_APIMODE	" 1\r", 	true);	// Set to API Mode 1, expect OK
	xbee_send_at(XBEE_AT_NETID		" 0x7FFF\r",true);	// Set Network ID, expect OK
	xbee_send_at(XBEE_AT_PREAMBLEID	" 0x0\r", 	true);	// Set Preamble ID, expect OK
	xbee_send_at(XBEE_AT_POWLEVEL	" 0x0\r", 	true);	// Set Power Level, expect OK

	xbee_send_at(XBEE_AT_WRITE	"\r", true);	// Write changes, expect OK
	xbee_send_at(XBEE_AT_EXITCMD"\r", true);	// Exit command mode, expect OK
}

void xbee_send_at(char * str, bool check) {
	// Clear the buffer
	buf_clear(uart2_rx_buffer);

	// Make sure receiving is enabled
	uart2_receive();

	// Set flag when received '\r'
	uart2_update_match('\r');

	// Send the command
	uart2_transmit(str);

	// wait for carriage return
	while(uart2Flag == 0);
	uart2Flag = 0;

	// check for good response
	if(check) {
		if(!buf_ok(uart2_rx_buffer)) {
			error(__LINE__);	// error
		}
	}

	// Clear the buffer
	buf_clear(uart2_rx_buffer);
}

void xbee_send() {
	// Clear buffer
	buf_clear(uart2_rx_buffer);
	buf_clear(uart2_tx_buffer);

	// Set uartFlag to trigger on START_BYTE
	uart2_update_match(XBEE_CTRL_START);

	uint32_t frameLen = xbee_msg->prePayloadLen + xbee_msg->payloadLen;

	// Send packet to XBee over UART
	buf_writeByte(uart2_tx_buffer, XBEE_CTRL_START);
	buf_writeByte(uart2_tx_buffer, frameLen >> 8);
	buf_writeByte(uart2_tx_buffer, frameLen & 0xFF);

	uint8_t checksumSend = 0;

	for(uint32_t i = 0; i < xbee_msg->prePayloadLen; i++) {
		buf_writeByte(uart2_tx_buffer, xbee_msg->prePayload[i]);
		checksumSend += xbee_msg->prePayload[i];
	}

	for(uint32_t i = 0; i < xbee_msg->payloadLen; i++) {
		buf_writeByte(uart2_tx_buffer, xbee_msg->payload[i]);
		checksumSend += xbee_msg->payload[i];
	}

	buf_writeByte(uart2_tx_buffer, (0xFF - checksumSend));

	// Enable the Transmit Data Register Empty interrupt
	SET_BIT(USART2->CR1, USART_CR1_TXEIE);

	// wait for start of frame (START_BYTE)
	while(uart2Flag == 0);
	uart2Flag = 0;

	// Format it as a response frame
	start_of_frame_t * response = (start_of_frame_t *)(uart2_rx_buffer->buffer);

	// wait for frame type and frameId information to come in
	while(BUF_USED(uart2_rx_buffer) < 5);

	// check to make sure it's the correct response frame and matches frameID sent earlier
	if(response->frameType != xbee_msg->respFrameType || response->frameId != xbee_frameID) {
		error(__LINE__);
	}

	// Wait for the rest of the packet to come in
	uint32_t targetLen = ENDIAN_SWAP16(response->length) + 4;
	while(BUF_USED(uart2_rx_buffer) < targetLen);

	// Ensure valid checksum
	uint8_t checksum = 0;
	uint8_t * checker = ((uint8_t *)response) + 3;
	uint8_t * checksumAddr = checker + ENDIAN_SWAP16(response->length);
	for(;checker < checksumAddr; checker++) {
		checksum += *(checker);
	}
	if((0xFF - checksum) != (*checksumAddr)) {
		error(__LINE__);
	}

	// Ensure response frame has expected values
	bool isGood = false;
	switch(response->frameType) {
		case XBEE_FRAME_TX_STAT:
			isGood = xbee_tx_stat_check((uint8_t *)response, response->length);
			break;
		default:
			break;
	}
	if(isGood == false) {
		error(__LINE__);
	}

	// Done with it now
	buf_clear(uart2_rx_buffer);

	// Increment frameID for next time
	xbee_frameID++;
	if(xbee_frameID == 0x0) {
		xbee_frameID = 0x1;
	}
}

bool xbee_tx_stat_check(uint8_t * msg, uint16_t msgLen) {
	tx_status_frame_t *msgFrame = (tx_status_frame_t *)msg;
	if(msgFrame->deliveryStatus == 0x0) {
		return true;
	}
	return false;
}

void xbee_send_message(tx_req_frame_t *txReq) {
	txReq->frameType = XBEE_FRAME_TX_REQ;
	txReq->frameID = xbee_frameID;
	txReq->broadcast_radius = 0x0;
	txReq->tx_opts = 0x0;
	txReq->reserved = ENDIAN_SWAP16(0xFFFE);

	xbee_msg->prePayload = (uint8_t *)txReq;
	xbee_msg->prePayloadLen = sizeof(tx_req_frame_t);
	xbee_msg->respFrameType = XBEE_FRAME_TX_STAT;

	xbee_send();
}

void xbee_rx_complete(uint16_t len) {
	rx_frame_t *msg = (rx_frame_t *)&(uart2_rx_buffer->buffer[uart2_rx_buffer->tail]);

	uint64_t addr = (uint64_t)ENDIAN_SWAP32(msg->addrH);
	addr <<= 32;
	addr |= ENDIAN_SWAP32(msg->addrL);

	uint8_t checksum = 0x0;
	uint8_t * checker = (uint8_t *)&(msg->frameType);
	uint8_t * checksumAddr = checker + ENDIAN_SWAP16(msg->length);
	for(;checker < checksumAddr; checker++) {
		checksum += *(checker);
	}
	if((0xFF - checksum) != (*checksumAddr)) {
		error(__LINE__);
	}

	if((msg->rx_opts & 0x1) != 0x1) {
		error(__LINE__);	// packet not ack'ed
	}

	char *payload = (char *)&(msg->payload_start);

	switch((xbee_msg_cmd_t)payload[0]) {
		case discover:
				mqtt_discover(addr, payload[1]);
			break;
		case send_value:
				mqtt_value(addr, payload[1], payload[2]);
			break;
		default:
			error(__LINE__);
	}

	buf_pop_tail(uart2_rx_buffer, len);
}
