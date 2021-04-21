/*
 * ringBuf.c
 *
 *  Created on: Apr 19, 2021
 *      Author: grantweiss
 */


#include "ringBuf.h"
#include <string.h>

uint16_t buf_writeStr(const char *str, Buffer *buffer) {
	int strLen = strlen(str);

	if(strLen > BUF_AVAIL(buffer)) {
		return 0;
	}

	if(buffer->head + strLen < BUFFER_SIZE) { // single memcpy, not circular
		memcpy(&(buffer->buffer[buffer->head]), str, strLen);
		buffer->head += strLen;
	} else { // two memcpy, circular
		int firstSize = BUFFER_SIZE - buffer->head;
		memcpy(&(buffer->buffer[buffer->head]), str, firstSize);

		int secondSize = strLen - firstSize;
		memcpy(&(buffer->buffer[0]), &(str[firstSize]), secondSize);

		buffer->head = secondSize;
	}

	return 1;
}

uint16_t buf_writeStr_var(const char *str, Buffer *buffer) {	// Uses length stored in buffer, not defined (VARiable size)
	int strLen = strlen(str);

	if(strLen > BUF_AVAIL_VAR(buffer)) {
		return 0;
	}

	if(buffer->head + strLen < (buffer->len)) { // single memcpy, not circular
		memcpy(&(buffer->buffer[buffer->head]), str, strLen);
		buffer->head += strLen;
	} else { // two memcpy, circular
		int firstSize = (buffer->len) - buffer->head;
		memcpy(&(buffer->buffer[buffer->head]), str, firstSize);

		int secondSize = strLen - firstSize;
		memcpy(&(buffer->buffer[0]), &(str[firstSize]), secondSize);

		buffer->head = secondSize;
	}

	return 1;
}

uint16_t buf_writeChars(Buffer *buffer, const char *str, size_t strLen) {
	if(strLen > BUF_AVAIL(buffer)) {
		return 0;
	}

	if(buffer->head + strLen < BUFFER_SIZE) { // single memcpy, not circular
		memcpy(&(buffer->buffer[buffer->head]), str, strLen);
		buffer->head += strLen;
	} else { // two memcpy, circular
		int firstSize = BUFFER_SIZE - buffer->head;
		memcpy(&(buffer->buffer[buffer->head]), str, firstSize);

		int secondSize = strLen - firstSize;
		memcpy(&(buffer->buffer[0]), &(str[firstSize]), secondSize);

		buffer->head = secondSize;
	}

	return 1;
}

//uint16_t buf_ok(Buffer *buffer) {
//	const char * str = "OK\r";
//	const uint16_t strLen = 3;
//
//	int index = buffer->head - 1;
//	if(index < 0) {
//		index += buffer->len;
//	}
//
//	for(int i = strLen-1; i >= 0; i--) {
//		if(buffer->buffer[index] != str[i]) {
//			return 0;
//		}
//		index--;
//		if(index < 0) {
//			index += buffer->len;
//		}
//	}
//	return 1;
//}

uint16_t buf_ok(Buffer *buffer) {
	if(BUF_GET_AT(buffer, buffer->head-3) == 'O' && BUF_GET_AT(buffer, buffer->head-2) == 'K' && BUF_GET_AT(buffer, buffer->head-1) == '\r') {
		return 1;
	}
	return 0;
}

uint16_t buf_crcr(Buffer *buffer) {
	if(BUF_GET_AT(buffer, buffer->head-1) == '\r' && BUF_GET_AT(buffer, buffer->head-2) == '\r') {
		return 1;
	}
	return 0;
}

void buf_pop(Buffer *buffer, uint16_t len) {
	buffer->head -= len;
	if(buffer->head < 0) {
		buffer->head += buffer->len;
	}
}

void buf_pop_tail(Buffer *buffer, uint16_t len) {
	buffer->tail = (buffer->tail + len) % BUFFER_SIZE;
}

uint16_t buf_writeByte(Buffer *buffer, unsigned char c) {
	int i = (unsigned int)(buffer->head + 1) % BUFFER_SIZE;

	if(i != buffer->tail) {
		buffer->buffer[buffer->head] = c;
		buffer->head = i;
		return 1;
	} else {	// buffer overflow
		return 0;
	}
}

