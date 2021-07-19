#ifndef I2S_H
#define I2S_H

#include "../default/peripheral/dmac/plib_dmac.h"

#define I2S_CALLBACK DMAC_CHANNEL_CALLBACK

typedef enum {
    I2S_STANDARD_PHILLIPS = 0,
    I2S_STANDARD_MSB = 1,
    I2S_STANDARD_LSB = 2,
    I2S_STANDARD_PCM = 3
} I2S_STANDARD;

typedef enum {
    I2S_DATAFORMAT_16B = 0,
    I2S_DATAFORMAT_16B_EXTENDED = 1,
    I2S_DATAFORMAT_32B = 2,
    I2S_DATAFORMAT_24B = 3,
} I2S_DATAFORMAT;

typedef enum {
    I2S_AUDIOFREQ_DEFAULT = 2,
    I2S_AUDIOFREQ_8K = 8000,
    I2S_AUDIOFREQ_11K = 11025,
    I2S_AUDIOFREQ_16K = 16000,
    I2S_AUDIOFREQ_22K = 22050,
    I2S_AUDIOFREQ_32K = 32000,
    I2S_AUDIOFREQ_44K = 44100,
    I2S_AUDIOFREQ_48K = 48000,
    I2S_AUDIOFREQ_96K = 96000,
    I2S_AUDIOFREQ_192K = 192000
} I2S_AUDIOFREQ;

typedef enum {
    I2S_STEREO = 0,
    I2S_MONO = 1,
} I2S_AUDMONO;

typedef enum {
    I2S_DMA_WIDTH_8_BIT = 0,
    I2S_DMA_WIDTH_16_BIT = 1,
    I2S_DMA_WIDTH_32_BIT = 2
} I2S_DMA_WIDTH;

typedef struct {
    void *i2sTransmitAddress; // I2S transmit register address used for DMA operation.
    size_t i2sTransmitSize;
    void *i2sReceiveAddress; // I2S transmit register address used for DMA operation.
    size_t i2sReceiveSize;
    I2S_STANDARD i2sStandard;
    I2S_DATAFORMAT i2sDataFormat;
    I2S_AUDIOFREQ i2sSampleRate;
    I2S_AUDMONO i2sAudioMono;
    I2S_DMA_WIDTH dmaDataLength;  // DMA data length from I2S or SSC PLIB
} I2S_TRANSFER_SETUP;

extern I2S_TRANSFER_SETUP i2s_setup;

void I2S_Init();
void I2S_TransferSetup();
void I2S_TxCallbackRegister(I2S_CALLBACK eventHandler);
void I2S_Start();
void I2S_Stop();
int I2S_LRCLK_Sync(uint32_t sample_rate);

#endif
