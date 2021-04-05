#include "main.h"

global_state_t global_state = {
	.state = NO_HAT
};

hat_config_t hat_list[HAT_LIST_LEN] = {
	[not_connected].friendly_name = "No Hat Detected!",
	[not_connected].hat_resistance = 0,
	[not_connected].gpio_setup = 0,
	[not_connected].type = none,

	[temp_hum].friendly_name = "Temperature & Humidity Sensor",
	[temp_hum].hat_resistance = 1,
	[temp_hum].gpio_setup = 0,		// i2c gpio init
	[temp_hum].type = i2c,

	[led_driver].friendly_name = "LED Driver",
	[led_driver].hat_resistance = 2,
	[led_driver].gpio_setup = 0,	// i2c gpio init
	[led_driver].type = i2c,

	[bool_switch].friendly_name = "Boolean Sensor/Switch",
	[bool_switch].hat_resistance = 3,
	[bool_switch].gpio_setup = 0,	// external interrupt gpio init
	[bool_switch].type = external_interrupt,
};

// flags & handlers
uint8_t hat_flag = 0;
uint8_t xbee_uart_flag = 0;

