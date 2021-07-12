#include "../../config/default/peripheral/spi/spi_master/plib_spi1_master.h"
#include "spi.h"

/**
  * @brief  Read and write a byte
  * @param  txData
  * @retval 
  */
uint8_t SPI1_ReadWriteByte(uint8_t txData)
{
    uint8_t rxData;
    SPI1_WriteRead(&txData, 1, &rxData, 1);
    return rxData;
}

/**
  * @brief  Read n bytes
  * @param  txData
  *         size     
  * @retval 
  */
void SPI1_ReadBytes(uint8_t *rxData, uint16_t size)
{
    SPI1_Read(rxData, size);
}

/**
  * @brief  Read and write n bytes
  * @param  txData
  *         size     
  * @retval 
  */
void SPI1_ReadWriteBytes(uint8_t *txData, uint8_t *rxData, uint16_t size)
{
    SPI1_WriteRead(txData, size, rxData, size);
}
