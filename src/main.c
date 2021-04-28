#include "uart.h"
#include "main.h"
#include "adc.h"
#include "eeprom.h"
#include "hats.h"
#include "sleep.h"
#include "xbee.h"
#include "ringBuf.h"
#include "delay.h"

// separate file for each peripheral, separate file for each firmware/driver for sensor
void hat_init();
void hat_deinit();

int main(void) {
	delay_init();
	setup();

	delay_ms(1000);

	uint16_t xbee_status = 0;
	uint32_t xbee_length = 0;

	uart2_update_match(XBEE_CTRL_START);
	uart2_receive();

	get_initial_state();

	while(1) {
		/****** Hat Connected or Disconnected ******/
		if(hat_conn_flag != 0) {
			if(global_state.hatDetectTrig == hat_connect) {
				// A hat was connected
				hat_init();
			} else {
				// A hat was disconnected
				hat_deinit();
			}
			hat_conn_flag = 0x0;
		}

		/****** Hat needs attention ******/
		if(hat_flag != 0) {
			fn_ptr handler = GET_HAT_CONFIG(global_state.connectedHat)->handler;
			if(handler != 0) {
				handler();
			}
			hat_flag = 0;
		}

		/****** XBee receiving start ******/
		if(uart2Flag != 0) {
			xbee_status = 1; // xbee frame started
			uart2Flag = 0;
		}

		/****** XBee receiving state machine ******/
		if(xbee_status > 0) {
			if(xbee_status == 1 && BUF_USED(uart2_rx_buffer) >= 4) {
//				if(BUF_GET_AT(uart2_rx_buffer, (uart2_rx_buffer->tail + 3)) != XBEE_FRAME_RX_PACKET) {
//					error(__LINE__);
//				}
				xbee_length = (BUF_GET_AT(uart2_rx_buffer, (uart2_rx_buffer->tail + 1)) << 8) | BUF_GET_AT(uart2_rx_buffer, (uart2_rx_buffer->tail + 2));
				xbee_status = 2;	// xbee frame length available
			} else if(xbee_status == 2 && BUF_USED(uart2_rx_buffer) == (xbee_length + 4)) {
				xbee_status = 3;
			} else if(xbee_status == 3) {
				// packet complete
				if(BUF_GET_AT(uart2_rx_buffer, (uart2_rx_buffer->tail + 3)) == XBEE_FRAME_RX_PACKET) {
					xbee_rx_complete(xbee_length + 4);
				} else {
					// Some other type of packet was received, should be concerned?
					buf_clear(uart2_rx_buffer);
				}
				xbee_status = 0;
			}
		}
	}
}

void setup() {
	// Clocks - enable, speed, etc
	SET_BIT(RCC->APB2ENR, RCC_APB2ENR_SYSCFGEN);

	// Disable everything - don't know what state it's in
	hat_conn_flag = 0x0;
	hat_flag = 0x0;

	// xbee inits
	uart2_init();
	xbee_setup();

	// battery babysitter inits
	battery_baby_init();
}

uint16_t hat_is_connected() {
	// VIHmin = 0.39VDD+0.59 (all pins except BOOT0, PC15, PH0/1 VIHmin = 0.45VDD+0.38 for BOOT0, PC15, PH0/1
	// 			= 1.877V @ 3.3Vdd
	// VILmax = 0.3VDD

	// Enable GPIOA clock
	SET_BIT(RCC->IOPENR, RCC_IOPENR_GPIOAEN);

	// Set PA8 to analog (disable the external pull-down of voltage divider)
	SET_BIT(GPIOA->MODER, GPIO_MODER_MODE8);

	// Set PA4 to input
	CLEAR_BIT(GPIOA->MODER, GPIO_MODER_MODE4);

	// wait, in case of any capacitance/general stabalization
	delay_ms(5);

	uint16_t retVal = (READ_BIT(GPIOA->IDR, GPIO_IDR_ID4) != RESET);
	return retVal;
}

void get_initial_state() {
	if(hat_is_connected()) { // hat is connected
		hat_init();
	} else { // no hat connected
		hat_deinit();
	}
}

// A hat is determined to be connected, start to do some setup
void hat_init() {
	// Get last hat config reported to homeassistant
	uint16_t lastHat;
	EEPROM_READ(eeprom_config->declaredHat, lastHat);

	// Get hat currently connected
	uint32_t hat_adc = get_hat_adc();
	global_state.connectedHat = get_hat_from_adc(hat_adc);

	// update homeassistant if this is a different hat
	// TODO fixme, re-enable if statement
//	if(global_state.connectedHat != lastHat) {
		declare_hat();
//	}

	// setup GPIO aspect of hat
	setup_hat();

	// setup interrupt for hat disconnection
	global_state.hatDetectTrig = hat_disconnect;
	hat_detect_interrupt();
}

void hat_deinit() {
	// no hat connected
	global_state.connectedHat = not_connected;

	// tell homeassistant so (this also updates eeprom)
//	declare_hat();	// ACTUALLY, DONT DO THIS: if same hat gets disconnected then reconnected, want nothing to change

	// nothing connected, reset the GPIO
	reset_hat_gpio();

	// setup interrupt for hat connection
	global_state.hatDetectTrig = hat_connect;
	hat_detect_interrupt();

	// go to sleep (stop); can only wake-up on EXTI event
//	enter_stop_cond(hat_is_connected);
	enter_stop();
}

void declare_hat() {
	uint16_t connHat = (uint16_t)global_state.connectedHat;

	// TODO MAYBE: if its a "not_connected" hat, don't update

	// send current hat setup to home assistant
	if(connHat != wifi_gateway) {
		tx_req_frame_t txReq = {
				.addrH = ENDIAN_SWAP32(0x0),
				.addrL = ENDIAN_SWAP32(0xFFFF),
		};
		uint8_t payload[3] = {(char)discover, 0x0, 0x0};
		payload[1] = connHat & 0xFF;
		payload[2] = connHat >> 8;
		xbee_msg->payload = payload;
		xbee_msg->payloadLen = 3;
		xbee_send_message(&txReq);
	}

	// set declared_config in eeprom
	EEPROM_WRITE(eeprom_config->declaredHat, connHat);
}

void battery_baby_init(void) {
	// TODO
}

void xbee_uart_handler(void) {
	// TODO
}

uint32_t get_hat_adc(void) {
	// Enable GPIOA clock
	SET_BIT(RCC->IOPENR, RCC_IOPENR_GPIOAEN);

	// Set PA4 to analog
	SET_BIT(GPIOA->MODER, GPIO_MODER_MODE4);

	// Setup PA8 to output (external pulldown)
	MODIFY_REG(GPIOA->MODER, GPIO_MODER_MODE8, GPIO_MODER_MODE8_0);				// output
	SET_BIT(GPIOA->OTYPER, GPIO_OTYPER_OT_8);									// open-drain
	MODIFY_REG(GPIOA->OSPEEDR, GPIO_OSPEEDER_OSPEED8, GPIO_OSPEEDER_OSPEED8_1);	// high speed

	// Set output to low (enable external pulldown)
	WRITE_REG(GPIOA->BSRR, GPIO_BSRR_BR_8); // Do the DRAIN

	// setup adc clocks, etc
	adc_setup();
	adc_calibrate();

	adc_enable();
	// todo, combine into a scan mode reading
	uint32_t adc_read = adc_oneshot(ADC_CHSELR_CHSEL4);
	uint32_t VREFINT_DATA = adc_get_vref();
	adc_disable();

	// get reference voltage
	float vdd_a = 3.0f * (float)(VREFINT_CAL) / (float)VREFINT_DATA;

	// get voltage at the adc pin (calibrated)
	float v_pin = vdd_a * (float)adc_read / (float)0xFFF; // using 0xFFF as max value of 12bit reading

	// calculate resistance based on MCU_HAT_REF_RES
	float hat_res = (((float)MCU_HAT_REF_RES)*(vdd_a-v_pin)) / v_pin;

	// Set PA8 to analog to prevent current drain
	SET_BIT(GPIOA->MODER, GPIO_MODER_MODE8);
	return hat_res;
}

hat_t get_hat_from_adc(float hat_resistor_value) {
	float upLim, lwLim;
    for(int i = 0; i < HAT_LIST_LEN; i++) {
        upLim = 1.05 * hat_list[i].hat_resistance;
        lwLim = 0.95 * hat_list[i].hat_resistance;
        if(hat_resistor_value >= lwLim && hat_resistor_value <= upLim) {
            return (hat_t)i;
        }
    }
	return not_connected;
}

void setup_hat() {
	reset_hat_gpio();
	GET_HAT_CONFIG(global_state.connectedHat)->gpio_setup();
	GET_HAT_CONFIG(global_state.connectedHat)->hat_initial_setup();

	hat_flag = 0; 			// reset flag
}

void reset_hat_gpio() {
	SET_BIT(GPIOB->MODER, GPIO_MODER_MODE1 | GPIO_MODER_MODE10 | GPIO_MODER_MODE11);	// set to analog (reset value)
	SET_BIT(GPIOA->MODER, GPIO_MODER_MODE6);											// set to analog (reset value)
}

void HardFault_Handler(void) {
	error(__LINE__);
}

void error(uint32_t source) {
//	delay_init();

	SET_BIT(RCC->IOPENR, RCC_IOPENR_GPIOBEN);	// GPIOB Clock enable

	MODIFY_REG(GPIOB->MODER, GPIO_MODER_MODE4, GPIO_MODER_MODE4_0);
	CLEAR_BIT(GPIOB->OTYPER, GPIO_OTYPER_OT_4);

	while(1) {
		WRITE_REG(GPIOB->BSRR, GPIO_BSRR_BS_4);
//		delay_ms(5000);
		for(uint32_t i = 0; i < 800000; ++i);
		WRITE_REG(GPIOB->BSRR, GPIO_BSRR_BR_4);
//		delay_ms(500);
		for(uint32_t i = 0; i < 80000; ++i);

		uint32_t sInt = source;
		while(sInt != 0x0) {
			if((sInt & 0x1)) {
				WRITE_REG(GPIOB->BSRR, GPIO_BSRR_BS_4);
//				delay_ms(1000);
				for(uint32_t i = 0; i < 160000; ++i);
				WRITE_REG(GPIOB->BSRR, GPIO_BSRR_BR_4);
//				delay_ms(200);
				for(uint32_t i = 0; i < 32000; ++i);
			} else {
				WRITE_REG(GPIOB->BSRR, GPIO_BSRR_BS_4);
//				delay_ms(200);
				for(uint32_t i = 0; i < 32000; ++i);
				WRITE_REG(GPIOB->BSRR, GPIO_BSRR_BR_4);
//				delay_ms(500);
				for(uint32_t i = 0; i < 80000; ++i);
			}
			sInt >>= 1;
		}
	}
}

uint16_t get_digits(uint32_t digits) {
	uint16_t count = 0;
	while (digits != 0) {
		digits /= 10;     // n = n/10
		++count;
	}
	return count;
}
