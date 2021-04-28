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
#include "delay.h"
#include "i2c.h"
#include "rtc.h"

void hcd2010_get(float *temp, float *hum);

// Setup hat for PB11 generating interrupts on LO and HI (falling and rising)
void hat_interrupt_PB11(void) {
	SET_BIT(RCC->IOPENR, RCC_IOPENR_GPIOBEN);	// Enable GPIOB Clock

	CLEAR_BIT(GPIOB->MODER, GPIO_MODER_MODE11); // input mode
	CLEAR_BIT(GPIOB->PUPDR, GPIO_PUPDR_PUPD11); // no pull resistors

	SET_BIT(RCC->IOPSMENR, RCC_IOPSMENR_GPIOBSMEN); // enable GPIOB Clock in sleep

	// set EXTI 11 to trigger from Port B
	MODIFY_REG(SYSCFG->EXTICR[2], SYSCFG_EXTICR3_EXTI11, SYSCFG_EXTICR3_EXTI11_PB);

	SET_BIT(EXTI->RTSR, EXTI_RTSR_RT11);	// rising edge
//	SET_BIT(EXTI->FTSR, EXTI_FTSR_FT11); 	// falling edge
	WRITE_REG(EXTI->PR, EXTI_PR_PIF11);		// Clear pending bit (if any)
//	SET_BIT(EXTI->EMR, EXTI_EMR_EM11);		// Enable Event
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

	// set EXTI 4 to trigger from Port A
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
	uint8_t payload[3] = {(char)send_value, (global_state.connectedHat & 0xFF), (value & 0xFF)};
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

void RTC_IRQHandler()
{
    RTC -> ISR &= ~RTC_ISR_WUTF;
    hat_flag = 1;
}

void hdc2010_setup() {
    uint8_t value = 0;
    I2C_Mem_Tx(HDC2010_Addr, HDC2010_CONFIG, 1, &value, 1);


//
//     rtc_init();
//     rtc_setTimeout(15); // this function puts it to sleep, rtc interrupt sets hatFlag
}

void lp5523_setup() {
	  uint8_t data[1] = {0xFF};
	  I2C_Mem_Tx(0x32, 0x3D, 1, data, 1); // reset chip

	  data[0] = 0x40;
	  I2C_Mem_Tx(0x32, 0x00, 1, data, 1); // chip enable

	  data[0] = 0x53;
	  I2C_Mem_Tx(0x32, 0x36, 1, data, 1); // chip clock enable

	  // todo, remove me
	  data[0] = 0xFF;
	  I2C_Mem_Tx(0x32, 0x16, 1, data, 1); // turn on LED
}

void hdc2010_send() {
	union {
	    float myFloats[2];
	    uint8_t myInts[8];
	  } myData;
	hcd2010_get(&(myData.myFloats[0]), &(myData.myFloats[1]));

	tx_req_frame_t txReq = {
			.addrH = ENDIAN_SWAP32(0x0),
			.addrL = ENDIAN_SWAP32(0xFFFF),
	};
	uint8_t payload[10] = {
			(char)send_value,
			(global_state.connectedHat & 0xFF),
			myData.myInts[0], myData.myInts[1], myData.myInts[2], myData.myInts[3], myData.myInts[4], myData.myInts[5], myData.myInts[6], myData.myInts[7]
	};
	xbee_msg->payload = payload;
	xbee_msg->payloadLen = 10;
	xbee_send_message(&txReq);
}

void hcd2010_get(float *temp, float *hum) {
	uint8_t command[1] =  {0x01};
	uint8_t data[2];

	I2C_Mem_Tx(HDC2010_Addr, HDC2010_MEASUREMENT_CONFIG, 1, command, 1); // tell it to take a reading
	delay_ms(100);	// allow it to finish reading

	I2C_Mem_Rx(HDC2010_Addr, HDC2010_TEMP_LOW, 1, data, 2);	// get the temp reading
	uint16_t tempRaw = (data[1]<<8) | data[0];
	*temp = (((float)(tempRaw) * 165) / 65536) - 40;

	I2C_Mem_Rx(HDC2010_Addr, HDC2010_HUMID_LOW, 1, data, 2);	// get hum reading
	uint16_t humRaw = (data[1]<<8) | data[0];
	*hum = ((float)(humRaw) / 65536) * 100;
}

float hcd2010_getTemp() {
	uint8_t command[1] =  {0x01};
	uint8_t data[2];

	I2C_Mem_Tx(HDC2010_Addr, HDC2010_MEASUREMENT_CONFIG, 1, command, 1); // tell it to take a reading
	delay_ms(100);	// allow it's reading to finish

	I2C_Mem_Rx(HDC2010_Addr, HDC2010_TEMP_LOW, 1, data, 2);	// get the temp reading

	uint16_t tempRaw = (data[1]<<8) | data[0];
	float temperature = (((float)(tempRaw) * 165) / 65536) - 40;
	return temperature;
}

float hdc2010_getHum() {
	uint8_t command[1] =  {0x01};
	uint8_t data[2];

	I2C_Mem_Tx(HDC2010_Addr, HDC2010_MEASUREMENT_CONFIG, 1, command, 1); // tell it to take a reading
	delay_ms(100);	// allow it's reading to finish

	I2C_Mem_Rx(HDC2010_Addr, HDC2010_HUMID_LOW, 1, data, 2);

	uint16_t humRaw = (data[1]<<8) | data[0];
	float humidity = ((float)(humRaw) / 65536) * 100;
	return humidity;
}

void blank_setup() {

}

void hat_uart_115200() {
	lpuart_init(115200);
}

void hat_uart_9600() {
	lpuart_init(9600);
}

