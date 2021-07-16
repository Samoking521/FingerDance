#include "../../config/default/device.h"
#include "../../config/default/peripheral/i2c/master/plib_i2c1_master.h"
#include "i2c.h"
#include "drivers/OLED/OLED.h"

/**
 * @brief  Write a byte in blocking mode
 * @param  txData
 * @retval 
 */
void I2C1_WriteByte(uint16_t address, uint8_t txData)
{
    I2C1_Write(address, &txData, 1);
    while (I2C1_IsBusy());
}

/**
 * @brief  Write n bytes in blocking mode
 * @param  txData
 * @retval 
 */
void I2C1_WriteBytes(uint16_t address, uint8_t *txData, uint16_t len)
{
    I2C1_Write(address, txData, len);
    while (I2C1_IsBusy());
}

/**
 * @brief  Read a byte in blocking mode
 * @param  rxData
 * @retval 
 */
void I2C1_ReadByte(uint16_t address, uint8_t *rxData)
{
    I2C1_Read(address, rxData, 1);
    while (I2C1_IsBusy());
}