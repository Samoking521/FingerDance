#include "music.h"
#include "../../config/default/peripheral/coretimer/plib_coretimer.h"
#include "../../library/Fatfs/ff.h"
#include "../../drivers/wm8978/wm8978.h"
#include "drivers/i2s/i2s.h"
#include "wav.h"
#include <stdio.h>

//#define MUSIC_DEBUG
#define I2S_TX_DMA_BUFSIZE 8192

FATFS fs; //define here temporarily

AudioDevice audioDev;
extern WavCtrl wavCtrl;
static uint8_t i2sBuf[2][I2S_TX_DMA_BUFSIZE];
volatile uint8_t wavWhichBuf = 0; //which buf is use now

static void music_pre_play();
static uint32_t music_buffer_fill(uint8_t *buf, uint16_t size, uint8_t bits);
static void music_tx_CallbackHandler(DMAC_TRANSFER_EVENT status, uintptr_t contextHandler);

void music_init()
{
    if (wm8978_Init())
    {
        wm8978_CfgAudioPath(DAC_ON, EAR_LEFT_ON | EAR_RIGHT_ON);
        wm8978_SetOUT1Volume(30);
        //wm8978_CfgAudioIF(I2S_STANDARD_PHILLIPS, 16);
    }

    I2S_Init();
}

uint8_t music_play(void)
{
#ifdef MUSIC_DEBUG
    printf("----music_play----\n");
#endif
    f_mount(&fs, "0:", 1);

    uint8_t res;
    FRESULT res_sd;

    audioDev.i2sBuf1 = i2sBuf[0];
    audioDev.i2sBuf2 = i2sBuf[1];

    uint8_t fname[] = "0:song-bg.wav";
    res = wav_DecodeInit(fname, &wavCtrl);

    if (res == 0)
    {
        music_pre_play();

        res_sd = f_open(&audioDev.file, (TCHAR *) fname, FA_OPEN_EXISTING | FA_READ);
        if (res_sd == FR_OK)
        {
            f_lseek(&audioDev.file, wavCtrl.datastart); //jump to data block
            wavWhichBuf = 0;

            music_buffer_fill(i2sBuf[0], I2S_TX_DMA_BUFSIZE, wavCtrl.bps);
            music_buffer_fill(i2sBuf[1], I2S_TX_DMA_BUFSIZE, wavCtrl.bps);

            music_start();

            while (1)
            {
                if (wavWhichBuf == 1)
                {
#ifdef MUSIC_DEBUG
                    printf("Buf2 start fill: %d\n", CORETIMER_CounterGet());
#endif
                    music_buffer_fill(i2sBuf[1], I2S_TX_DMA_BUFSIZE, wavCtrl.bps);
#ifdef MUSIC_DEBUG
                    printf("Buf2 end fill: %d\n", CORETIMER_CounterGet());
#endif
                    wavWhichBuf = 0;
                }
                else if (wavWhichBuf == 2)
                {
#ifdef MUSIC_DEBUG
                    printf("Buf1 start fill: %d\n", CORETIMER_CounterGet());
#endif
                    music_buffer_fill(i2sBuf[0], I2S_TX_DMA_BUFSIZE, wavCtrl.bps);
#ifdef MUSIC_DEBUG
                    printf("Buf1 end fill: %d\n", CORETIMER_CounterGet());
#endif
                    wavWhichBuf = 0;
                }
            }
        }
        else
            res = 0XFF;
    }
    else
        res = 0XFF;
    return res;
}

/**
 * @brief  start play music
 * @param  None
 * @retval None
 */
void music_start(void)
{
    I2S_Start();
}

/**
 * @brief  stop play music
 * @param  None
 * @retval None
 */
void music_stop(void)
{
    I2S_Stop();
}

/**
 * @brief  call before a piece of music play
 * @param  None
 * @retval None
 */
void music_pre_play()
{
    i2s_setup.i2sTransmitAddress = (uint8_t *) i2sBuf;
    i2s_setup.i2sTransmitSize = 2 * I2S_TX_DMA_BUFSIZE;
    i2s_setup.i2sStandard = I2S_STANDARD_PHILLIPS;

    if (wavCtrl.bps == 16)
    {
        i2s_setup.i2sDataFormat = I2S_DATAFORMAT_16B;
        i2s_setup.dmaDataLength = I2S_DMA_WIDTH_16_BIT;
        wm8978_CfgAudioIF(I2S_STANDARD_PHILLIPS, 16);
    }
    else if (wavCtrl.bps == 24)
    {
        i2s_setup.i2sDataFormat = I2S_DATAFORMAT_24B;
        i2s_setup.dmaDataLength = I2S_DMA_WIDTH_32_BIT;
        wm8978_CfgAudioIF(I2S_STANDARD_PHILLIPS, 24);
    }
    else if (wavCtrl.bps == 32)
    {
        i2s_setup.i2sDataFormat = I2S_DATAFORMAT_32B;
        i2s_setup.dmaDataLength = I2S_DMA_WIDTH_32_BIT;
        wm8978_CfgAudioIF(I2S_STANDARD_PHILLIPS, 32);
    }

    if (wavCtrl.samplerate == 48000)
        i2s_setup.i2sSampleRate = I2S_AUDIOFREQ_48K;
    else if (wavCtrl.samplerate == 44100)
        i2s_setup.i2sSampleRate = I2S_AUDIOFREQ_44K;

    if (wavCtrl.nchannels == 1)
        i2s_setup.i2sAudioMono = I2S_MONO;
    else
        i2s_setup.i2sAudioMono = I2S_STEREO;
    I2S_TransferSetup();

    I2S_TxCallbackRegister(music_tx_CallbackHandler);
}

/**
 * @brief  fill buf
 * @param  buf: data address
 * @param  size: data size
 * @param  bits: 16 or 24
 * @retval fill data number successfully
 */
uint32_t music_buffer_fill(uint8_t *buf, uint16_t size, uint8_t bits)
{
    uint16_t readlen = 0;
    UINT fnum;
    uint16_t i;
    uint8_t *p;
    if (bits == 24) //24bit
    {
        readlen = (size / 4) * 3;
        f_read(&audioDev.file, audioDev.tmpBuf, readlen, &fnum);
        p = audioDev.tmpBuf;
        for (i = 0; i < size;)
        {
            buf[i++] = p[1];
            buf[i] = p[2];
            i += 2;
            buf[i++] = p[0];
            p += 3;
        }
        fnum = (fnum * 4) / 3;
    }
    else
    {
        f_read(&audioDev.file, buf, size, &fnum);
        if (fnum < size)
        {
            for (i = fnum; i < size - fnum; i++)
                buf[i] = 0;
        }
    }
    return fnum;
}

void music_tx_CallbackHandler(DMAC_TRANSFER_EVENT status, uintptr_t contextHandler)
{
    if (DMAC_TRANSFER_EVENT_HALF_COMPLETE == status)
    {
#ifdef MUSIC_DEBUG
        printf("Buf1 end play, Buf2 start play: %d\n", CORETIMER_CounterGet());
#endif
        wavWhichBuf = 2;
    }
    else if (DMAC_TRANSFER_EVENT_COMPLETE == status)
    {
#ifdef MUSIC_DEBUG
        printf("Buf2 end play, Buf1 start play: %d\n", CORETIMER_CounterGet());
#endif
        wavWhichBuf = 1;
    }
}