#ifndef SPI_H
#define SPI_H

uint8_t SPI1_ReadWriteByte(uint8_t txData);
void SPI1_ReadBytes(uint8_t *rxData, uint16_t size);
void SPI1_ReadWriteBytes(uint8_t *txData, uint8_t *rxData, uint16_t size);

#endif