/*
 * xbee.h
 *
 *  Created on: Apr 19, 2021
 *      Author: grantweiss
 */

#ifndef XBEE_H_
#define XBEE_H_


#include <stdbool.h>
#include "uart.h"
#include "String.h"

/********************** DEFINES **********************/
/* XBee Control Bytes */
#define XBEE_CTRL_START 0x7e

/* XBee AT Transparent Mode Commands */
#define XBEE_AT_ENTERCMD "+++"
#define XBEE_AT_EXITCMD "ATCN"

#define XBEE_AT_RESTORE "ATRE"
#define XBEE_AT_WRITE "ATWR"

#define XBEE_AT_APIMODE "ATAP"
#define XBEE_AT_NETID "ATID"
#define XBEE_AT_PREAMBLEID "ATHP"
#define XBEE_AT_POWLEVEL "ATPL"

/* XBee Frames */
#define XBEE_FRAME_TX_REQ 0x10
#define XBEE_FRAME_TX_STAT 0x8B
#define XBEE_FRAME_AT_CMD 0x08
#define XBEE_FRAME_AT_CMD_RESP 0x88
#define XBEE_FRAME_RX_PACKET 0x90

/* Helpers */
//#define ENDIAN_SWAP16(VAL) (((VAL) >> 8) | ((VAL) << 8))
#define ENDIAN_SWAP16(VAL) ((((VAL) & 0xFF00) >> 8) | (((VAL) & 0xFF) << 8))
//#define ENDIAN_SWAP32(VAL) ((((VAL)>>24) & 0xFF) | (((VAL)>>8) & 0xFF00) | (((VAL)<<8) & 0xFF0000) | (((VAL)<<24) & 0xFF000000))
#define ENDIAN_SWAP32(VAL) (((((VAL) & 0xFF000000)>>24)) | ((((VAL) & 0xFF0000)>>8)) | ((((VAL) & 0xFF00)<<8)) | ((((VAL) & 0xFF)<<24)))

/********************** STRUCTS **********************/
//typedef bool (*respFn)(uint8_t *, uint16_t);

typedef struct __attribute__((__packed__)) {
	uint8_t startByte;
	uint16_t length;
	uint8_t frameType;
	uint8_t frameId;
} start_of_frame_t;

typedef struct {
	uint8_t *prePayload;
	uint16_t prePayloadLen;

	uint8_t *payload;
	uint16_t payloadLen;

	uint8_t respFrameType;
	bool expectResponse;
//	respFn respCheck;
} message_t;

typedef struct __attribute__((__packed__))  {
	uint8_t startDelim;
	uint16_t length;
	uint8_t frameType;
	uint8_t frameID;
	uint16_t reserved;
	uint8_t txRetry;
	uint8_t deliveryStatus;
	uint8_t discoveryStatus;
	uint8_t checksum;
} tx_status_frame_t;

typedef struct __attribute__((__packed__))  {
	uint8_t frameType;
	uint8_t frameID;
	uint32_t addrH;
	uint32_t addrL;
	uint16_t reserved;
	uint8_t broadcast_radius;
	uint8_t tx_opts;
} tx_req_frame_t;

typedef struct __attribute__((__packed__))  {
	uint8_t startDelim;
	uint16_t length;
	uint8_t frameType;
	uint32_t addrH;
	uint32_t addrL;
	uint16_t reserved;
	uint8_t rx_opts;
	uint8_t payload_start;
} rx_frame_t;

typedef enum {
	unused,
	discover,
	update,
	send_value
} xbee_msg_cmd_t;


/********************** FUNCTIONS **********************/
void xbee_setup();
void xbee_send_message();
void xbee_rx_complete(uint16_t);

/********************** Variables **********************/
message_t *xbee_msg;
uint8_t xbee_frameID;


#endif /* XBEE_H_ */
