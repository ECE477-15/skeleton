#include "main.h"
#include <assert.h>

void gpio_test() {
	RCC->IOPENR |= RCC_IOPENR_GPIOBEN;	// GPIOB Clock enable

	GPIOB->MODER &= ~(GPIO_MODER_MODE4);
	GPIOB->MODER |= GPIO_MODER_MODE4_0;
	GPIOB->OTYPER &= ~(GPIO_OTYPER_OT_4);

	GPIOB->BSRR = GPIO_BSRR_BS_4;
}

void sanity_tests() {	// assertions
	assert( HAT_LIST_LEN == sizeof(hat_list) / sizeof(hat_list[0]) );
}
