#ifndef I2C_H
#define I2C_H

int I2C1_WriteByte(uint16_t address, uint8_t txData);
int I2C1_WriteBytes(uint16_t address, uint8_t *txData, uint16_t len);
int I2C1_ReadByte(uint16_t address, uint8_t *rxData);

int I2C2_WriteByte(uint16_t address, uint8_t txData);
int I2C2_WriteBytes(uint16_t address, uint8_t *txData, uint16_t len);
int I2C2_ReadByte(uint16_t address, uint8_t *rxData);


#endif