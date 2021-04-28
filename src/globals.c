#include "main.h"
#include "hats.h"
#include "i2c.h"

const char* const device_classes[DEVICE_CLASS_SIZE] = {
	[class_none] = "",
	[motion] = "motion",
	[class_temp] = "temperature",
	[class_hum] = "humidity",
	[class_tempHum] = "",
};
const char* const device_types[DEVICE_TYPE_SIZE] = {
	[sensor] = "sensor",
	[binary_sensor] = "binary_sensor",
};

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

	[temp_hum] = {
			.friendly_name = "Temperature & Humidity Sensor",
			.hat_resistance = 220000,
			.gpio_setup = i2c_hat_setup,
			.type = i2c,
			.handler = hdc2010_send,
			.hat_initial_setup = hdc2010_setup,
			.dev_type = sensor,
			.dev_class = class_tempHum,
			.bin_off_delay = 0,
			.state_topic = true,
			.cmd_topic = false,
	},

	[led_driver] = {
			.friendly_name = "LED Driver",
			.hat_resistance = 6800,
			.gpio_setup = i2c_hat_setup,
			.type = i2c,
			.handler = 0,
			.hat_initial_setup = lp5523_setup,
			.dev_type = binary_sensor,	// todo
			.dev_class = motion,	// todo
			.bin_off_delay = 0,
			.state_topic = false,
			.cmd_topic = true,
	},

	[magnet_switch].friendly_name = "Magnet Sensor",
	[magnet_switch].hat_resistance = 56000,
	[magnet_switch].gpio_setup = hat_interrupt_PB11,
	[magnet_switch].type = external_interrupt,
	[magnet_switch].handler = send_homeassistant_boolean_PB11,
	[magnet_switch].hat_initial_setup = blank_setup,

	[push_button] = {
			.friendly_name = "Push Button",
			.hat_resistance = 2,
			.gpio_setup = hat_interrupt_PB11,
			.type = external_interrupt,
			.handler = send_homeassistant_boolean_PB11,
			.hat_initial_setup = blank_setup,
			.dev_type = binary_sensor,
			.dev_class = motion,
			.bin_off_delay = 0,
			.state_topic = false,
			.cmd_topic = true,
	},

	[PIR_motion].friendly_name = "PIR Motion",
	[PIR_motion].dev_type = binary_sensor,
	[PIR_motion].dev_class = motion,
	[PIR_motion].hat_resistance = 47000,
	[PIR_motion].gpio_setup = hat_interrupt_PB11,
	[PIR_motion].type = external_interrupt,
	[PIR_motion].handler = send_homeassistant_boolean_PB11,
	[PIR_motion].hat_initial_setup = blank_setup,
	[PIR_motion].bin_off_delay = 30,
	[PIR_motion].state_topic = true,
	[PIR_motion].cmd_topic = false,

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

