#include "../../config/default/device.h"
#include "i2s.h"
#include "../default/audio/peripheral/i2s/plib_i2s2.h"
#include <stdio.h>

//#define I2S_DEBUG
#define I2S_TX_DMA_CHANNEL DMAC_CHANNEL_0

I2S_TRANSFER_SETUP i2s_setup;

static void _I2S_TX_DMA_CallbackHandler(DMAC_TRANSFER_EVENT status, uintptr_t contextHandler);
void _I2S_CLK_Set();

/**
 * @brief  I2S init
 * @param  None
 * @retval None
 */
void I2S_Init()
{
    // Register a callback with DMA.
    DMAC_ChannelCallbackRegister(I2S_TX_DMA_CHANNEL,
            _I2S_TX_DMA_CallbackHandler,
            (uintptr_t) NULL);
    i2s_setup.i2sReceiveAddress = (uint8_t *) & SPI2BUF;
    i2s_setup.i2sReceiveSize = 2;

    REFOCONbits.OE = 1;
}

/**
 * @brief  Dynamic setup of I2S Peripheral.
 * @param  None
 * @retval None
 */
void I2S_TransferSetup()
{
    SPI2CONCLR = _SPI2CON_ON_MASK;

    if (i2s_setup.i2sStandard == I2S_STANDARD_PHILLIPS)
    {
        SPI2CON2bits.AUDMOD = 0;
        SPI2CONbits.FRMPOL = 0;
        SPI2CONbits.CKP = 1;
    }
    else if (i2s_setup.i2sStandard == I2S_STANDARD_MSB)
    {
        SPI2CON2bits.AUDMOD = 1;
        SPI2CONbits.FRMPOL = 1;
        SPI2CONbits.CKP = 0;
    }
    else if (i2s_setup.i2sStandard == I2S_STANDARD_LSB)
    {
        SPI2CON2bits.AUDMOD = 2;
        SPI2CONbits.FRMPOL = 1;
        SPI2CONbits.CKP = 0;
    }
    else
    {
        SPI2CON2bits.AUDMOD = 3;
    }

    if (i2s_setup.i2sDataFormat == I2S_DATAFORMAT_16B)
    {
        SPI2CONbits.MODE32 = 0;
        SPI2CONbits.MODE16 = 0;
    }
    else if (i2s_setup.i2sDataFormat == I2S_DATAFORMAT_16B_EXTENDED)
    {
        SPI2CONbits.MODE32 = 0;
        SPI2CONbits.MODE16 = 1;
    }
    else if (i2s_setup.i2sDataFormat == I2S_DATAFORMAT_32B)
    {
        SPI2CONbits.MODE32 = 1;
        SPI2CONbits.MODE16 = 0;
    }
    else
    {
        SPI2CONbits.MODE32 = 1;
        SPI2CONbits.MODE16 = 1;
    }

    //I2S2_BaudRateSet(40000000, i2s_setup.i2sSampleRate);
    //SPI2BRG = 3;
    //I2S2_RefClockSet(40000000, i2s_setup.i2sSampleRate * 32, 256);
    _I2S_CLK_Set();


    if (i2s_setup.i2sAudioMono == I2S_STEREO)
        SPI2CON2bits.AUDMONO = 0;
    else
        SPI2CON2bits.AUDMONO = 1;

    SPI2CONSET = _SPI2CON_ON_MASK;
}

/**
 * @brief  Dynamic setup of I2S tx callback function.
 * @param  None
 * @retval None
 */
void I2S_TxCallbackRegister(I2S_CALLBACK eventHandler)
{
    DMAC_ChannelCallbackRegister(I2S_TX_DMA_CHANNEL,
            eventHandler,
            (uintptr_t) NULL);
}

/**
 * @brief  start i2s transmit with DMA
 * @param  None
 * @retval None
 */
void I2S_Start()
{
    DMAC_ChannelTransfer(I2S_TX_DMA_CHANNEL,
            i2s_setup.i2sTransmitAddress,
            i2s_setup.i2sTransmitSize,
            i2s_setup.i2sReceiveAddress,
            i2s_setup.i2sReceiveSize,
            2);
#ifdef I2S_DEBUG
    printf("src address: %x\n", DCH0SSA);
    printf("dest address: %x\n", DCH0DSA);
    printf("cell size: %x\n", DCH0CSIZ);
#endif
}

/**
 * @brief  stop i2s transmit
 * @param  None
 * @retval None
 */
void I2S_Stop()
{
    uint8_t bufferSizeDmaWords = i2s_setup.i2sTransmitSize;
    if (i2s_setup.dmaDataLength == I2S_DMA_WIDTH_16_BIT)
        bufferSizeDmaWords /= 2;
    else if (i2s_setup.dmaDataLength == I2S_DMA_WIDTH_32_BIT)
        bufferSizeDmaWords /= 4;
    DMAC_ChannelTransfer(I2S_TX_DMA_CHANNEL,
            (const void *) i2s_setup.i2sTransmitAddress,
            (size_t) i2s_setup.i2sTransmitSize,
            (const void *) i2s_setup.i2sReceiveAddress,
            (size_t) i2s_setup.i2sReceiveSize,
            bufferSizeDmaWords);
}

/**
 * @brief  i2s LRCLK sync
 * @param  None
 * @retval None
 */
int I2S_LRCLK_Sync(uint32_t sample_rate)
{
    return sample_rate;
}

/**
 * @brief  DMA Complete Event callback registered as XDMAC service routine.
 * @param  None
 * @retval None
 */
void _I2S_TX_DMA_CallbackHandler(DMAC_TRANSFER_EVENT status, uintptr_t contextHandler)
{
    if (DMAC_TRANSFER_EVENT_COMPLETE == status)
    {

    }
    else if (DMAC_TRANSFER_EVENT_HALF_COMPLETE == status)
    {

    }
    else //DMAC_TRANSFER_EVENT_ERROR or DMAC_TRANSFER_EVENT_NONE
    {

    }
}

void _I2S_CLK_Set()
{
    /*
    REFOCONbits.OE = 0;
    REFOCONbits.ON = 0;
    
    // unlock system for clock configuration
    SYSKEY = 0x00000000;
    SYSKEY = 0xAA996655;
    SYSKEY = 0x556699AA;

    // Set up Reference Clock
    if (i2s_setup.i2sSampleRate == I2S_AUDIOFREQ_48K)
        // ROTRIM  = 321
        REFOTRIM = 0xa0800000;
    else if (i2s_setup.i2sSampleRate == I2S_AUDIOFREQ_44K)
        // ROTRIM  = 395
        REFOTRIM = 0xc5800000;

    // Lock system since done with clock configuration
    SYSKEY = 0x33333333;
    
    REFOCONbits.ON = 1;
    REFOCONbits.DIVSWEN = 1;
    REFOCONbits.OE = 1;
    printf("%08x %08x\n", REFOCON, REFOTRIM);*/
    if (i2s_setup.i2sSampleRate == I2S_AUDIOFREQ_48K)
        I2S2_RefClockSet(40000000, 48000, 256);
    else if (i2s_setup.i2sSampleRate == I2S_AUDIOFREQ_44K)
        I2S2_RefClockSet(40000000, 44100, 256);

    if (i2s_setup.i2sDataFormat == I2S_DATAFORMAT_16B)
        SPI2BRG = 3;
    else if (i2s_setup.i2sDataFormat == I2S_DATAFORMAT_32B)
        SPI2BRG = 1;
}