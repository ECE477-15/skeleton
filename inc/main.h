#ifndef MAIN_H_
#define MAIN_H_

#include <stm32l0xx.h>

/****** Type Defs & Structs ********/
typedef void (*fn_ptr)(void);

typedef enum {
	not_connected,
	temp_hum,
	led_driver,
	bool_switch,

	HAT_LIST_LEN
} hat_t;

typedef enum {
	none,
	external_interrupt,
	i2c,
	uart,
	comparator
} hat_conn_t;

typedef enum {
	NO_HAT,
	POLLING,
	SLEEPING,
	SETUP,
	AWAKE,
	ALWAYS_ON
} state_t;

typedef struct {
	char * friendly_name;
	uint16_t hat_resistance;
	fn_ptr gpio_setup;
	hat_conn_t type;
} hat_config_t;

typedef struct {
	state_t state;
	hat_t connectedHat;
	hat_config_t *connectedHatConfig;
} global_state_t;

/******* function declarations ********/
void setup(void);
uint16_t hat_is_connected();
void get_initial_state(void);
void xbee_init(void);
void xbee_uart_handler(void);
void setup_hat(void);
uint32_t get_hat_adc(void);
hat_t get_hat_from_adc(uint32_t adcReading);
void declare_hat(hat_t);
void reset_hat_gpio(void);

/******* Global Variables ********/
global_state_t global_state;
hat_config_t hat_list[HAT_LIST_LEN];
fn_ptr hat_flag_poll;
uint8_t hat_flag;
uint8_t xbee_uart_flag;

#endif /* MAIN_H_ */



