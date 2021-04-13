#ifndef MAIN_H_
#define MAIN_H_

#include <stm32l0xx.h>

#define MCU_HAT_REF_RES (10000) /* MCU Hat reference resistor */
#define GET_HAT_CONFIG(HAT) (&(hat_list[HAT]))

/****** Type Defs & Structs ********/
typedef void (*fn_ptr)(void);

typedef enum {
	not_connected,
	temp_hum,
	led_driver,
	magnet_switch,
	wifi_gateway,

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

typedef enum {
	UNINIT,
	CONFIGURING_HAT,
	WAITING_FOR_HAT,
	SETTING_UP_HAT
} mcu_status_t;

typedef enum {
	hat_connect,
	hat_disconnect
} hat_detect_trigger_t;

typedef struct {
	char * friendly_name;
	uint16_t hat_resistance;
	fn_ptr gpio_setup;
	hat_conn_t type;
	fn_ptr handler;
} hat_config_t;

typedef struct {
	mcu_status_t mcu_status;
	state_t state;
	hat_t connectedHat;
	hat_detect_trigger_t hatDetectTrig;
} global_state_t;

/******* function declarations ********/
void setup(void);
uint16_t hat_is_connected();
void get_initial_state(void);
void xbee_init(void);
void battery_baby_init(void);
void xbee_uart_handler(void);
void setup_hat(void);
uint32_t get_hat_adc(void);
hat_t get_hat_from_adc(float adcReading);
void declare_hat();
void reset_hat_gpio(void);

/******* Global Variables ********/
global_state_t global_state;
hat_config_t hat_list[HAT_LIST_LEN];
uint8_t hat_flag;
uint8_t xbee_uart_flag;
uint8_t hat_conn_flag;

#endif /* MAIN_H_ */



