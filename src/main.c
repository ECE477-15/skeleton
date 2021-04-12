#include "main.h"
#include "adc.h"

// separate file for each peripheral, separate file for each firmware/driver for sensor

int main(void) {
	setup();
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

void setup() {
}

uint16_t hat_is_connected() {
	// Enable GPIOA clock
	RCC->IOPENR |= RCC_IOPENR_GPIOAEN;

	// Set PA5 to input
	GPIOA->MODER &= ~(GPIO_MODER_MODE5);

	return ((GPIOA->IDR & GPIO_IDR_ID5) != 0U);
}

void get_initial_state() {
	if(hat_is_connected()) { // hat is connected
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
	// Enable GPIOA clock
	RCC->IOPENR |= RCC_IOPENR_GPIOAEN;
	// Set PA5 to analog
	GPIOA->MODER |= (GPIO_MODER_MODE5);

	// setup adc clocks, etc
	adc_setup();

	uint32_t adc_read = adc_oneshot(ADC_CHSELR_CHSEL5);

	uint32_t VREFINT_DATA = adc_get_vref();

	float vdd_a = 3.0f * (float)(VREFINT_CAL) / (float)VREFINT_DATA;
	float v_pin = vdd_a * (float)adc_read / (float)0xFFF; // using 0xFFF as max value of 12bit reading

	float hat_res = ((10000)*(vdd_a-v_pin)) / v_pin;

	return hat_res;
}

hat_t get_hat_from_adc(float hat_resistor_value) {
	// TODO
    for (int i = 0, i <= HAT_LIST_LEN, i++)
    {
        upLim = 1.05 * hat_list[i].hat_resistance;
        lwLim = 0.95 * hat_list[i].hat_resistance;
        if ((resVal >= lwLim) || (resVal <= upLim))
        {
            return((hat_t)i)
        }
    }
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
