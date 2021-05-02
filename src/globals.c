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
	[not_connected] = {
			.friendly_name = "No Hat Detected!",
			.hat_resistance = 0,
			.gpio_setup = blank_setup,
			.type = none,
			.handler = 0,
			.hat_initial_setup = blank_setup,
			.dev_type = type_none,
			.dev_class = class_none,
			.bin_off_delay = 0,
			.state_topic = false,
			.cmd_topic = false,
	},
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
	[magnet_switch] = {
			.friendly_name = "Magnet Sensor",
			.hat_resistance = 56000,
			.gpio_setup = hat_interrupt_PB11,
			.type = external_interrupt,
			.handler = send_homeassistant_boolean_PB11,
			.hat_initial_setup = blank_setup,
			.dev_type = binary_sensor,
			.dev_class = motion,	// todo
			.bin_off_delay = 0,
			.state_topic = true,
			.cmd_topic = false,
			.interruptCfg = (INT_CFG_FALL | INT_CFG_RISE | INT_CFG_ACTIVELOW),
	},
	[push_button] = {
			.friendly_name = "Push Button",
			.hat_resistance = 2,
			.gpio_setup = hat_interrupt_PB11,
			.type = external_interrupt,
			.handler = send_homeassistant_boolean_PB11,
			.hat_initial_setup = blank_setup,
			.dev_type = binary_sensor, // todo
			.dev_class = motion, // todo
			.bin_off_delay = 0,
			.state_topic = true,
			.cmd_topic = false,
	},
	[PIR_motion] = {
		.friendly_name = "PIR Motion",
		.hat_resistance = 47000,
		.gpio_setup = hat_interrupt_PB11,
		.type = external_interrupt,
		.handler = send_homeassistant_boolean_PB11,
		.hat_initial_setup = blank_setup,
		.dev_type = binary_sensor,
		.dev_class = motion,
		.bin_off_delay = 30,
		.state_topic = true,
		.cmd_topic = false,
		.interruptCfg = INT_CFG_RISE,
	},
	[force_resistor] = {
		.friendly_name = "Force Sensor",
		.hat_resistance = 4,
		.gpio_setup = hat_interrupt_PB11,
		.type = comparator, //todo
		.handler = send_homeassistant_boolean_PB11,
		.hat_initial_setup = blank_setup,
		// TODO: below
		.dev_type = type_none,
		.dev_class = class_none,
		.bin_off_delay = 0,
		.state_topic = false,
		.cmd_topic = false,
	},
	[wifi_gateway] = {
		.friendly_name = "WiFi Gateway",
		.hat_resistance = 10000,
		.gpio_setup = hat_uart_115200,
		.type = uart,
		.handler = 0,
		.hat_initial_setup = wifi_setup,
		.dev_type = type_none,
		.dev_class = class_none,
		.bin_off_delay = 0,
		.state_topic = false,
		.cmd_topic = false,
	},
};

// flags & handlers
volatile uint8_t hat_flag = 0;
volatile uint8_t hat_conn_flag = 0;

