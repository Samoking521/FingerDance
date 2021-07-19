#include "wav.h"
#include "music.h"
#include "../../library/Fatfs/ff.h"
#include <stdio.h>

#define WAV_DEBUG

#define WAV_I2S_TX_DMA_BUFSIZE 8192

BYTE WAV_ReadBuffer[512] = {0};

WavCtrl wavCtrl;              //WAV play control
extern AudioDevice audioDev;

/**
  * @brief  WAV decode init
  * @param  fname: file path and file name
  * @param  wavx: wav info struct pointer
  * @retval @arg 0: success
  *         @arg 1: open file failure
  *         @arg 2: no WAV file
  *         @arg 3: DATA block no find
  */
uint8_t wav_DecodeInit(uint8_t *fname, WavCtrl *wavx)
{
#ifdef WAV_DEBUG
  printf("----wav_DecodeInit----\n");
#endif
  uint8_t res = 0;
  FRESULT res_sd;
  UINT fnum;
  ChunkRIFF *riff;
  ChunkFMT *fmt;
  ChunkFACT *fact;
  ChunkDATA *data;

  res_sd = f_open(&audioDev.file, (TCHAR *)fname, FA_OPEN_EXISTING | FA_READ);
  if (res_sd == FR_OK)
  {
    f_read(&audioDev.file, WAV_ReadBuffer, 512, &fnum);
    riff = (ChunkRIFF *)WAV_ReadBuffer; //get RIFF block
    if (riff->Format == 0X45564157) //is WAV file?
    {
      fmt = (ChunkFMT *)(WAV_ReadBuffer + 12);                        //get FMT bolck
      fact = (ChunkFACT *)(WAV_ReadBuffer + 12 + 8 + fmt->ChunkSize); //get FACT block
      if (fact->ChunkID == 0X74636166 || fact->ChunkID == 0X5453494C)
        wavx->datastart = 12 + 8 + fmt->ChunkSize + 8 + fact->ChunkSize; //fact/LIST block
      else
        wavx->datastart = 12 + 8 + fmt->ChunkSize;
      data = (ChunkDATA *)(WAV_ReadBuffer + wavx->datastart); //get DATA block
      if (data->ChunkID == 0X61746164)                    //decode success
      {
        wavx->audioformat = fmt->AudioFormat;
        wavx->nchannels = fmt->NumOfChannels;
        wavx->samplerate = fmt->SampleRate;
        wavx->bitrate = fmt->ByteRate * 8;
        wavx->blockalign = fmt->BlockAlign;
        wavx->bps = fmt->BitsPerSample;
        wavx->datasize = data->ChunkSize;
        wavx->datastart = wavx->datastart + 8;
#ifdef WAV_DEBUG
        printf("wavx->audioformat:%d\n", wavx->audioformat);
        printf("wavx->nchannels:%d\n", wavx->nchannels);
        printf("wavx->samplerate:%d\n", wavx->samplerate);
        printf("wavx->bitrate:%d\n", wavx->bitrate);
        printf("wavx->blockalign:%d\n", wavx->blockalign);
        printf("wavx->bps:%d\n", wavx->bps);
        printf("wavx->datasize:%d\n", wavx->datasize);
        printf("wavx->datastart:%d\n", wavx->datastart);
#endif
      }
      else
        res = 3; //DATA block no find
    }
    else
      res = 2; //no WAV file
  }
  else
    res = 1; //open file failure

  f_close(&audioDev.file);
#ifdef WAV_DEBUG
  printf("res_sd: %d\n", res_sd);
  printf("res: %d\n", res);
#endif
  return res;
}
