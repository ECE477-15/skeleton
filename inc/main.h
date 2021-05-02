#ifndef MAIN_H_
#define MAIN_H_

#include <stm32l0xx.h>
#include "stdbool.h"

#define MCU_HAT_REF_RES (22000) /* MCU Hat reference resistor */
#define GET_HAT_CONFIG(HAT) (&(hat_list[HAT]))

#define GET_DEV_TYPE(DEV) (device_types[DEV.dev_type])
#define GET_DEV_CLASS(DEV) (device_classes[DEV.dev_class])

#define ATTR_OPTIMIZE(n)          __attribute__ ((optimize(XSTRING_(O##n))))
#define __ALWAYS_INLINE 		  __attribute__ ((always_inline))

#define INT_CFG_FALL 0x1
#define INT_CFG_RISE 0x2
#define INT_CFG_ACTIVELOW 0x4

/****** Type Defs & Structs ********/
typedef void (*fn_ptr)(void);

typedef enum {
	not_connected,
	
	/* I2C */
	temp_hum,
	led_driver,
	
	/* Interrupt */
	magnet_switch,
	push_button,
	PIR_motion,
	force_resistor, /* Might be comparator (special interrupt) */
	
	/* Other */
	IR_blaster,
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
	class_none,
	motion,
	class_temp,
	class_hum,
	class_tempHum,

	DEVICE_CLASS_SIZE
} device_class_t;

typedef enum {
	type_none,
	sensor,
	binary_sensor,

	DEVICE_TYPE_SIZE
} device_type_t;

typedef enum {
	hat_connect,
	hat_disconnect
} hat_detect_trigger_t;

typedef struct {
	char * friendly_name;
	device_class_t dev_class;
	device_type_t dev_type;
	uint32_t hat_resistance;
	fn_ptr gpio_setup;
	fn_ptr hat_initial_setup;
	hat_conn_t type;
	fn_ptr handler;
	uint16_t bin_off_delay;
	bool state_topic;
	bool cmd_topic;
	uint8_t interruptCfg;
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
void setup_hat(void);
uint32_t get_hat_adc(void);
hat_t get_hat_from_adc(float adcReading);
void declare_hat();
void reset_hat_gpio(void);
void error(uint32_t);
uint16_t get_digits(uint32_t);

/******* Global Variables ********/
global_state_t global_state;
hat_config_t hat_list[HAT_LIST_LEN];
volatile uint8_t hat_flag;
volatile uint8_t xbee_uart_flag;
volatile uint8_t hat_conn_flag;

const char* const device_classes[DEVICE_CLASS_SIZE];
const char* const device_types[DEVICE_TYPE_SIZE];

#endif /* MAIN_H_ */



