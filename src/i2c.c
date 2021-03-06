/*
 * i2c.c
 *
 *  Created on: Apr 28, 2021
 *      Author: grantweiss
 */
#include "main.h"

void i2c_hat_setup() {
	SET_BIT(RCC->IOPENR, RCC_IOPENR_GPIOBEN); // GPIOB Clock Enable
	SET_BIT(GPIOB->OSPEEDR, (0x3 << GPIO_OSPEEDER_OSPEED10_Pos) | (0x3 << GPIO_OSPEEDER_OSPEED11_Pos));	// very high speed
	SET_BIT(GPIOB->OTYPER, GPIO_OTYPER_OT_10 | GPIO_OTYPER_OT_11);	// Open drain

	CLEAR_BIT(GPIOB->PUPDR,(GPIO_PUPDR_PUPD10_Msk | GPIO_PUPDR_PUPD11_Msk)); // Disable pulls
//	MODIFY_REG(GPIOB->PUPDR,(GPIO_PUPDR_PUPD10_Msk | GPIO_PUPDR_PUPD11_Msk), (GPIO_PUPDR_PUPD10_0 | GPIO_PUPDR_PUPD11_0)); // Set pullup

	MODIFY_REG(GPIOB->AFR[1], (GPIO_AFRH_AFSEL10_Msk | GPIO_AFRH_AFSEL11_Msk), (0x6 << GPIO_AFRH_AFSEL10_Pos) | (0x6 << GPIO_AFRH_AFSEL11_Pos)); // Set PB10, PB11 Alternate Function Reg to 0x6 (I2C2)
	MODIFY_REG(GPIOB->MODER, (GPIO_MODER_MODE10 | GPIO_MODER_MODE11), (GPIO_MODER_MODE10_1 | GPIO_MODER_MODE11_1));	// Set PB10, PB11 mode to alternate 0b10

	SET_BIT(RCC->APB1ENR, RCC_APB1ENR_I2C2EN);	// I2C2 clock enable
	CLEAR_BIT(I2C2->CR1, I2C_CR1_PE);			// Disable I2C2
	I2C2->TIMINGR = 0x00707CBB & 0xF0FFFFFFU; 	// 0xF0FFFFFFU = TIMING_CLEAR_MASK
	CLEAR_BIT(I2C2->OAR1, I2C_OAR1_OA1EN_Msk);	// Disable Own Address
	I2C2->OAR1 = (I2C_OAR1_OA1EN | 0x0); 		// set own address and re-enable
	SET_BIT(I2C2->CR2, (I2C_CR2_AUTOEND | I2C_CR2_NACK));
	CLEAR_BIT(I2C2->CR2, I2C_OAR2_OA2EN_Msk);	// Disable own address 2
	SET_BIT(I2C2->CR1, I2C_CR1_PE);				// Enable I2C2
}

void I2C_Mem_Tx(uint16_t device_addr, uint16_t reg_addr, uint16_t reg_addr_size, uint8_t *data, uint16_t data_size) {
	while(I2C2->ISR & I2C_ISR_BUSY);

	device_addr <<= 1;
	uint16_t size = reg_addr_size + data_size;

//	I2C2->CR2 &= ~(I2C_CR2_SADD | I2C_CR2_NBYTES | I2C_CR2_RELOAD | I2C_CR2_AUTOEND | I2C_CR2_RD_WRN | I2C_CR2_START | I2C_CR2_STOP);
	CLEAR_BIT(I2C2->CR2, I2C_CR2_SADD | I2C_CR2_NBYTES | I2C_CR2_RELOAD | I2C_CR2_AUTOEND | I2C_CR2_RD_WRN | I2C_CR2_START | I2C_CR2_STOP);
	SET_BIT(I2C2->CR2, (device_addr & I2C_CR2_SADD) | (size << I2C_CR2_NBYTES_Pos) | I2C_CR2_AUTOEND | I2C_CR2_START);
//	I2C2->CR2 |= (device_addr & I2C_CR2_SADD) | (size << I2C_CR2_NBYTES_Pos) | I2C_CR2_AUTOEND | I2C_CR2_START;

	if(reg_addr_size == 2) {	// send reg_addr MSB
		while(!(I2C2->ISR & I2C_ISR_TXIS));
		I2C2->TXDR = ((reg_addr >> 8) & 0xFF);
	}

	while(!(I2C2->ISR & I2C_ISR_TXIS));	// send reg_addr LSB
	I2C2->TXDR = (reg_addr & 0xFF);

	uint8_t * data_pointer = data;
	for(uint16_t tx_remaining = data_size; tx_remaining > 0; tx_remaining--) {
		while(!(I2C2->ISR & I2C_ISR_TXIS));
		I2C2->TXDR = *data_pointer;

		data_pointer++;
	}

	while(!(I2C2->ISR & I2C_ISR_STOPF));
//	I2C2->ICR |= I2C_ICR_STOPCF;
	SET_BIT(I2C2->ICR, I2C_ICR_STOPCF);
}

void I2C_Mem_Rx(uint16_t device_addr, uint16_t reg_addr, uint16_t reg_addr_size, uint8_t *data, uint16_t data_size) {
	while(I2C2->ISR & I2C_ISR_BUSY);

	device_addr <<= 1;

//	I2C2->CR2 &= ~(I2C_CR2_SADD | I2C_CR2_NBYTES | I2C_CR2_RELOAD | I2C_CR2_AUTOEND | I2C_CR2_RD_WRN | I2C_CR2_START | I2C_CR2_STOP);
	CLEAR_BIT(I2C2->CR2, I2C_CR2_SADD | I2C_CR2_NBYTES | I2C_CR2_RELOAD | I2C_CR2_AUTOEND | I2C_CR2_RD_WRN | I2C_CR2_START | I2C_CR2_STOP);
	SET_BIT(I2C2->CR2, (device_addr & I2C_CR2_SADD) | (reg_addr_size << I2C_CR2_NBYTES_Pos) | I2C_CR2_START);
//	I2C2->CR2 |= (device_addr & I2C_CR2_SADD) | (reg_addr_size << I2C_CR2_NBYTES_Pos) | I2C_CR2_START;

	if(reg_addr_size == 2) {	// send reg_addr MSB
		while(!(I2C2->ISR & I2C_ISR_TXIS));
		I2C2->TXDR = ((reg_addr >> 8) & 0xFF);
	}

	while(!(I2C2->ISR & I2C_ISR_TXIS));	// send reg_addr LSB
	I2C2->TXDR = (reg_addr & 0xFF);

	while(!(I2C2->ISR & I2C_ISR_TC));	// wait for tx complete

//	I2C2->CR2 &= ~(I2C_CR2_SADD | I2C_CR2_NBYTES | I2C_CR2_RELOAD | I2C_CR2_AUTOEND | I2C_CR2_RD_WRN | I2C_CR2_START | I2C_CR2_STOP);
//	I2C2->CR2 |= (device_addr & I2C_CR2_SADD) | (data_size << I2C_CR2_NBYTES_Pos) | I2C_CR2_AUTOEND | I2C_CR2_START | I2C_CR2_RD_WRN;
	CLEAR_BIT(I2C2->CR2, I2C_CR2_SADD | I2C_CR2_NBYTES | I2C_CR2_RELOAD | I2C_CR2_AUTOEND | I2C_CR2_RD_WRN | I2C_CR2_START | I2C_CR2_STOP);
	SET_BIT(I2C2->CR2, (device_addr & I2C_CR2_SADD) | (data_size << I2C_CR2_NBYTES_Pos) | I2C_CR2_AUTOEND | I2C_CR2_START | I2C_CR2_RD_WRN);

	uint8_t * data_pointer = data;
	for(uint16_t tx_remaining = data_size; tx_remaining > 0; tx_remaining--) {
		while(!(I2C2->ISR & I2C_ISR_RXNE));	// wait for read data
		*data_pointer = I2C2->RXDR;

		data_pointer++;
	}

	while(!(I2C2->ISR & I2C_ISR_STOPF));
//	I2C2->ICR |= I2C_ICR_STOPCF;
	SET_BIT(I2C2->ICR, I2C_ICR_STOPCF);
}
