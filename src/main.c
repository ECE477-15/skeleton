#include "main.h"
#include "adc.h"
#include "eeprom.h"
#include "hats.h"
#include "sleep.h"

// separate file for each peripheral, separate file for each firmware/driver for sensor
void hat_init();
void hat_deinit();

int main(void) {
	setup();
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

		/****** XBee needs attention ******/
		if(xbee_uart_flag) {
			xbee_uart_handler();
		}
	}
}

void setup() {
	// Clocks - enable, speed, etc
	RCC->APB2ENR |= RCC_APB2ENR_SYSCFGEN;

	// Disable everything - don't know what state it's in
	hat_conn_flag = 0x0;
	hat_flag = 0x0;

	// xbee inits
	xbee_init();

	// battery babysitter inits
	battery_baby_init();
}

uint16_t hat_is_connected() {
	// Enable GPIOA clock
	RCC->IOPENR |= RCC_IOPENR_GPIOAEN;

	// Set PA5 to input
	GPIOA->MODER &= ~(GPIO_MODER_MODE5);

	// Set PA7 to analog (disable the external pull-down of voltage divider)
	GPIOA->MODER |= GPIO_MODER_MODE7;

	// wait, in case of any capacitance?
	for(uint16_t i = 0; i < 500; ++i);

	return ((GPIOA->IDR & GPIO_IDR_ID5) != 0U);
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
	if(global_state.connectedHat != lastHat) {
		declare_hat();
	}

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
	enter_stop();
}

void declare_hat() {
	// send current hat setup to home assistant
	// TODO

	// set declared_config in eeprom
	uint16_t connHat = (uint16_t)global_state.connectedHat;
	EEPROM_WRITE(eeprom_config->declaredHat, connHat);
}

void xbee_init(void) {
	// TODO
}

void battery_baby_init(void) {
	// TODO
}

void xbee_uart_handler(void) {
	// TODO
}

uint32_t get_hat_adc(void) {
	// Enable GPIOA clock
	RCC->IOPENR |= RCC_IOPENR_GPIOAEN;
	// Set PA5 to analog
	GPIOA->MODER |= (GPIO_MODER_MODE5);

	// Setup PA7 to output (external pulldown)
	GPIOA->MODER &= ~(GPIO_MODER_MODE7);
	GPIOA->MODER |= (GPIO_MODER_MODE7_0);
	GPIOA->OTYPER &= ~(GPIO_OTYPER_OT_7);
	GPIOA->OSPEEDR &= ~(GPIO_OSPEEDER_OSPEED7);
	GPIOA->OSPEEDR |= GPIO_OSPEEDER_OSPEED7_1;

	// Set output to low (enable external pulldown)
	GPIOA->BSRR = GPIO_BSRR_BR_7;

//	for (int i = 0; i < 100; i++) {}
	// setup adc clocks, etc
	adc_setup();
	adc_calibrate();

	adc_enable();
	uint32_t adc_read = adc_oneshot(ADC_CHSELR_CHSEL5);
	uint32_t VREFINT_DATA = adc_get_vref();
	adc_disable();

	// get reference voltage
	float vdd_a = 3.0f * (float)(VREFINT_CAL) / (float)VREFINT_DATA;

	// get voltage at the adc pin (calibrated)
	float v_pin = vdd_a * (float)adc_read / (float)0xFFF; // using 0xFFF as max value of 12bit reading

	// calculate resistance based on MCU_HAT_REF_RES
	float hat_res = ((MCU_HAT_REF_RES)*(vdd_a-v_pin)) / v_pin;

	// Set PA7 to analog to prevent current drain
	GPIOA->MODER |= GPIO_MODER_MODE7;
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

	hat_flag = 0; 			// reset flag
}

void reset_hat_gpio() {
	SET_BIT(GPIOB->MODER, GPIO_MODER_MODE1 | GPIO_MODER_MODE10 | GPIO_MODER_MODE11);	// set to analog (reset value)
	SET_BIT(GPIOA->MODER, GPIO_MODER_MODE6);											// set to analog (reset value)
}
