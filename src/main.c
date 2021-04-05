#include "main.h"

// separate file for each peripheral, separate file for each firmware/driver for sensor

int main(void) {
	get_initial_state();
	xbee_init();

	while(1) {
		if(hat_flag != 0 && hat_flag_poll != 0) {	// attend to hat
			hat_flag_poll();
		}
		if(xbee_uart_flag) {		// xbee uart action needed
			xbee_uart_handler();
		}
	}
}

void get_initial_state() {
	if(1) { // hat is connected
//		declared_config = read_from_eeprom(); // what was the last declared hat (hat config sent to home assistant)

		uint32_t hat_adc = get_hat_adc();
		global_state.connectedHat = get_hat_from_adc(hat_adc);
		global_state.connectedHatConfig = &(hat_list[global_state.connectedHat]);

		if(1) {	// current hat == declared hat
			// do nothing
		} else {
			declare_hat(global_state.connectedHat);
		}

		setup_hat();
	} else {
		declare_hat(not_connected);
		// setup interrupt for hat connection
		// go to sleep
	}
}

void declare_hat(hat_t connHat) {
	// send current hat setup to home assistant
	// set declared_config in eeprom
}

void xbee_init(void) {
	// TODO
}

void xbee_uart_handler(void) {
	// TODO
}

uint32_t get_hat_adc(void) {
	// TODO

	return 0;
}

hat_t get_hat_from_adc(uint32_t adcReading) {
	// TODO

	return 0;
}

void setup_hat() {
	reset_hat_gpio();
	global_state.connectedHatConfig->gpio_setup();

	hat_flag_poll = 0; 	// set to some function
	hat_flag = 0; 		// reset flag
}

void reset_hat_gpio() {
	// TODO
}
