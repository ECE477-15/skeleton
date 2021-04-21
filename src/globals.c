#include "main.h"
#include "hats.h"

global_state_t global_state = {
	.state = NO_HAT
};

hat_config_t hat_list[HAT_LIST_LEN] = {
	[not_connected].friendly_name = "No Hat Detected!",
	[not_connected].hat_resistance = 0,
	[not_connected].gpio_setup = blank_setup,
	[not_connected].type = none,
	[not_connected].handler = 0,
	[not_connected].hat_initial_setup = blank_setup,

	[temp_hum].friendly_name = "Temperature & Humidity Sensor",
	[temp_hum].hat_resistance = 1,
	[temp_hum].gpio_setup = blank_setup,		// i2c gpio init
	[temp_hum].type = i2c,
	[temp_hum].handler = 0,
	[temp_hum].hat_initial_setup = blank_setup,

	[led_driver].friendly_name = "LED Driver",
	[led_driver].hat_resistance = 22000,
	[led_driver].gpio_setup = blank_setup,	// i2c gpio init
	[led_driver].type = i2c,
	[led_driver].handler = 0,
	[led_driver].hat_initial_setup = blank_setup,

	[magnet_switch].friendly_name = "Magnet Sensor",
	[magnet_switch].hat_resistance = 33000,
	[magnet_switch].gpio_setup = hat_interrupt_PB11,
	[magnet_switch].type = external_interrupt,
	[magnet_switch].handler = send_homeassistant_boolean_PB11,
	[magnet_switch].hat_initial_setup = blank_setup,

	[wifi_gateway].friendly_name = "WiFi Gateway",
	[wifi_gateway].hat_resistance = 56000,
	[wifi_gateway].gpio_setup = hat_gpio_setup_uart,
	[wifi_gateway].type = uart,
	[wifi_gateway].handler = 0,
	[wifi_gateway].hat_initial_setup = wifi_setup,
};

// flags & handlers
volatile uint8_t hat_flag = 0;
volatile uint8_t hat_conn_flag = 0;

