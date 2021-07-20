#include "i2c_gpio.h"
#include "../../config/default/peripheral/coretimer/plib_coretimer.h"

/**
  * @brief  CPU generate I2C start signal
  * @param  None
  * @retval None
  */
void i2c2_Start(void)
{
  // when SCL in high level, SDA from hign to low
  I2C2_SDA_SET();
  I2C2_SCL_SET();
  CORETIMER_DelayUs(I2C2_DELAY);
  I2C2_SDA_RESET();
  CORETIMER_DelayUs(I2C2_DELAY);
  I2C2_SCL_RESET();
  CORETIMER_DelayUs(I2C2_DELAY);
}

/**
  * @brief  CPU generate I2C stop signal
  * @param  None
  * @retval None
  */
void i2c2_Stop(void)
{
  // when SCL in high level, SDA from low to high
  I2C2_SDA_RESET();
  I2C2_SCL_SET();
  CORETIMER_DelayUs(I2C2_DELAY);
  I2C2_SDA_SET();
}

/**
  * @brief  CPU send 8 bit data to I2C bus device
  * @param  byte: data to be send
  * @retval None
  */
void i2c2_SendByte(uint8_t byte)
{
  uint8_t i;
  // send bit7 firstly
  for (i = 0; i < 8; i++)
  {
    if (byte & 0x80)
    {
      I2C2_SDA_SET();
    }
    else
    {
      I2C2_SDA_RESET();
    }
    CORETIMER_DelayUs(I2C2_DELAY);
    I2C2_SCL_SET();
    CORETIMER_DelayUs(I2C2_DELAY);
    I2C2_SCL_RESET();
    if (i == 7)
    {
      I2C2_SDA_SET(); // release bus
    }
    byte <<= 1; // left shift a bit
    CORETIMER_DelayUs(I2C2_DELAY);
  }
}

/**
  * @brief  CPU read 8 bit data to I2C bus device
  * @param  None
  * @retval read data
  */
uint8_t i2c2_ReadByte(void)
{
  uint8_t i;
  uint8_t value;

  // read bit7 firstly
  value = 0;
  for (i = 0; i < 8; i++)
  {
    value <<= 1;
    I2C2_SCL_SET();
    CORETIMER_DelayUs(I2C2_DELAY);
    if (I2C2_SDA_READ())
    {
      value++;
    }
    I2C2_SCL_RESET();
    CORETIMER_DelayUs(I2C2_DELAY);
  }
  return value;
}

/**
  * @brief  CPU generate a clock, and read ack send from device
  * @param  None
  * @retval 0: ack, 1: nack
  */
uint8_t i2c2_WaitAck(void)
{
  uint8_t re;

  I2C2_SDA_SET(); // CPU release SDA bus
  CORETIMER_DelayUs(I2C2_DELAY);
  I2C2_SCL_SET(); // CPU drive SCL = 1, then device return ACK
  CORETIMER_DelayUs(I2C2_DELAY);
  if (I2C2_SDA_READ()) // CPU read SDA state
  {
    re = 1;
  }
  else
  {
    re = 0;
  }
  I2C2_SCL_RESET();
  CORETIMER_DelayUs(I2C2_DELAY);
  return re;
}

/**
  * @brief  CPU generate a ack
  * @param  None
  * @retval None
  */
void i2c2_Ack(void)
{
  I2C2_SDA_RESET(); // CPU drive SDA = 0
  CORETIMER_DelayUs(I2C2_DELAY);
  I2C2_SCL_SET(); // CPU generate a clock
  CORETIMER_DelayUs(I2C2_DELAY);
  I2C2_SCL_RESET();
  CORETIMER_DelayUs(I2C2_DELAY);
  I2C2_SDA_SET(); // CPU release SDA bus
}

/**
  * @brief  CPU generate a nack
  * @param  None
  * @retval None
  */
void i2c2_NAck(void)
{
  I2C2_SDA_SET(); // CPU drive SDA = 1
  CORETIMER_DelayUs(I2C2_DELAY);
  I2C2_SCL_SET(); // CPU generate a clock 
  CORETIMER_DelayUs(I2C2_DELAY);
  I2C2_SCL_RESET();
  CORETIMER_DelayUs(I2C2_DELAY);
}

/**
  * @brief  check I2C bus device, CPU send device address, and read device response to check device if exist
  * @param  address: device I2C bus address
  * @retval 0: successful, 1: no detect
  */
uint8_t i2c2_CheckDevice(uint8_t address)
{
  uint8_t ucAck;
  // generate a stop signal, reset all device on I2C bus to standby mode
  i2c2_Stop();
  i2c2_Start(); // send start up signal
  // send device address + write/read bit(0 = w 1 = r) bit7 first
  i2c2_SendByte(address << 1 | I2C_WR);
  ucAck = i2c2_WaitAck(); // check device's ACK
  i2c2_Stop();            // send stop signal
  return ucAck;
}