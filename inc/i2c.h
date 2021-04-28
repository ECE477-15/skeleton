/*
 * i2c.h
 *
 *  Created on: Apr 28, 2021
 *      Author: grantweiss
 */

#ifndef I2C_H_
#define I2C_H_

void i2c_hat_setup(void);
void I2C_Mem_Tx(uint16_t device_addr, uint16_t reg_addr, uint16_t reg_addr_size, uint8_t *data, uint16_t data_size);
void I2C_Mem_Rx(uint16_t device_addr, uint16_t reg_addr, uint16_t reg_addr_size, uint8_t *data, uint16_t data_size);

#endif /* I2C_H_ */
