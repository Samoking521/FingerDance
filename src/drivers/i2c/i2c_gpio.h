#ifndef I2C_GPIO_H
#define I2C_GPIO_H

#include "../../config/default/peripheral/gpio/plib_gpio.h"

#define I2C_WR 0
#define I2C_RD 1

#define I2C2_SCL_SET() I2C2_SCL_Set()     // SCL = 1
#define I2C2_SCL_RESET() I2C2_SCL_Clear() // SCL = 0

#define I2C2_SDA_SET() I2C2_SDA_Set()     // SDA = 1
#define I2C2_SDA_RESET() I2C2_SDA_Clear() // SDA = 0

#define I2C2_SDA_READ() I2C2_SDA_Get()

#define I2C2_DELAY 1

void i2c2_Start(void);
void i2c2_Stop(void);
void i2c2_SendByte(uint8_t byte);
uint8_t i2c2_ReadByte(void);
uint8_t i2c2_WaitAck(void);
void i2c2_Ack(void);
void i2c2_NAck(void);
uint8_t i2c2_CheckDevice(uint8_t address);

#endif
