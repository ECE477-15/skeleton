#include "main.h"
#include "hats.h"

global_state_t global_state = {
	.state = NO_HAT
};

hat_config_t hat_list[HAT_LIST_LEN] = {
	[not_connected].friendly_name = "No Hat Detected!",
	[not_connected].hat_resistance = 0,
	[not_connected].gpio_setup = 0,
	[not_connected].type = none,
	[not_connected].handler = 0,

	[temp_hum].friendly_name = "Temperature & Humidity Sensor",
	[temp_hum].hat_resistance = 1,
	[temp_hum].gpio_setup = 0,		// i2c gpio init
	[temp_hum].type = i2c,
	[temp_hum].handler = 0,

	[led_driver].friendly_name = "LED Driver",
	[led_driver].hat_resistance = 2,
	[led_driver].gpio_setup = 0,	// i2c gpio init
	[led_driver].type = i2c,
	[led_driver].handler = 0,

	[magnet_switch].friendly_name = "Magnet Sensor",
	[magnet_switch].hat_resistance = 15000,
	[magnet_switch].gpio_setup = hat_interrupt_PB11,
	[magnet_switch].type = external_interrupt,
	[magnet_switch].handler = send_homeassistant_boolean_PB11,

	[wifi_gateway].friendly_name = "WiFi Gateway",
	[wifi_gateway].hat_resistance = 10000,
	[wifi_gateway].gpio_setup = 0,	// external interrupt gpio init
	[wifi_gateway].type = external_interrupt,
	[wifi_gateway].handler = 0,
};

// flags & handlers
uint8_t hat_flag = 0;
uint8_t xbee_uart_flag = 0;
uint8_t hat_conn_flag = 0;

