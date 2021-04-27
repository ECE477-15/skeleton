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
	[led_driver].hat_resistance = 2,
	[led_driver].gpio_setup = blank_setup,	// i2c gpio init
	[led_driver].type = i2c,
	[led_driver].handler = 0,
	[led_driver].hat_initial_setup = blank_setup,

	[magnet_switch].friendly_name = "Magnet Sensor",
	[magnet_switch].hat_resistance = 56000,
	[magnet_switch].gpio_setup = hat_interrupt_PB11,
	[magnet_switch].type = external_interrupt,
	[magnet_switch].handler = send_homeassistant_boolean_PB11,
	[magnet_switch].hat_initial_setup = blank_setup,

	[push_button].friendly_name = "Push Button",
	[push_button].hat_resistance = 2,
	[push_button].gpio_setup = hat_interrupt_PB11,
	[push_button].type = external_interrupt,
	[push_button].handler = send_homeassistant_boolean_PB11,
	[push_button].hat_initial_setup = blank_setup,

	[PIR_motion].friendly_name = "PIR Motion",
	[PIR_motion].hat_resistance = 47000,
	[PIR_motion].gpio_setup = hat_interrupt_PB11,
	[PIR_motion].type = external_interrupt,
	[PIR_motion].handler = send_homeassistant_boolean_PB11,
	[PIR_motion].hat_initial_setup = blank_setup,

	[force_resistor].friendly_name = "Force Sensor",
	[force_resistor].hat_resistance = 4,
	[force_resistor].gpio_setup = hat_interrupt_PB11,
	[force_resistor].type = comparator,
	[force_resistor].handler = send_homeassistant_boolean_PB11,
	[force_resistor].hat_initial_setup = blank_setup,

	[wifi_gateway].friendly_name = "WiFi Gateway",
	[wifi_gateway].hat_resistance = 10000,
	[wifi_gateway].gpio_setup = hat_uart_115200,
	[wifi_gateway].type = uart,
	[wifi_gateway].handler = 0,
	[wifi_gateway].hat_initial_setup = wifi_setup,
};

// flags & handlers
volatile uint8_t hat_flag = 0;
volatile uint8_t hat_conn_flag = 0;

