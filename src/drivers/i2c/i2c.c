#include "../../config/default/device.h"
#include "../../config/default/peripheral/i2c/master/plib_i2c1_master.h"
#include "../../config/default/peripheral/i2c/master/plib_i2c2_master.h"
#include "i2c.h"
#include "drivers/OLED/OLED.h"

/**
 * @brief  Write a byte in blocking mode
 * @param  txData
 * @retval return 1 if is successful
 */
int I2C1_WriteByte(uint16_t address, uint8_t txData)
{
    if(I2C1_Write(address, &txData, 1))
    {
        while (I2C1_IsBusy());
        return 1;
    }
    else
        return 0;
}

/**
 * @brief  Write n bytes in blocking mode
 * @param  txData
 * @retval return 1 if is successful
 */
int I2C1_WriteBytes(uint16_t address, uint8_t *txData, uint16_t len)
{
    if(I2C1_Write(address, txData, len))
    {
        while (I2C1_IsBusy());
        return 1;
    }
    else
        return 0;
}

/**
 * @brief  Read a byte in blocking mode
 * @param  rxData
 * @retval return 1 if is successful
 */
int I2C1_ReadByte(uint16_t address, uint8_t *rxData)
{
    if(I2C1_Read(address, rxData, 1))
    {
        while (I2C1_IsBusy());
        return 1;
    }
    else
        return 0;
}

/**
 * @brief  Write a byte in blocking mode
 * @param  txData
 * @retval return 1 if is successful
 */
int I2C2_WriteByte(uint16_t address, uint8_t txData)
{
    if(I2C2_Write(address, &txData, 1))
    {
        while (I2C2_IsBusy());
        return 1;
    }
    else
        return 0;
}

/**
 * @brief  Write n bytes in blocking mode
 * @param  txData
 * @retval return 1 if is successful
 */
int I2C2_WriteBytes(uint16_t address, uint8_t *txData, uint16_t len)
{
    if(I2C2_Write(address, txData, len))
    {
        while (I2C2_IsBusy());
        return 1;
    }
    else
        return 0;
}

/**
 * @brief  Read a byte in blocking mode
 * @param  rxData
 * @retval return 1 if is successful
 */
int I2C2_ReadByte(uint16_t address, uint8_t *rxData)
{
    if(I2C2_Read(address, rxData, 1))
    {
        while (I2C2_IsBusy());
        return 1;
    }
    else
        return 0;
}