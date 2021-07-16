#ifndef I2C_H
#define I2C_H

void I2C1_WriteByte(uint16_t address, uint8_t txData);
void I2C1_WriteBytes(uint16_t address, uint8_t *txData, uint16_t len);
void I2C1_ReadByte(uint16_t address, uint8_t *rxData);


#endif