#include "mc.h"
#include "../../library/Fatfs/ff.h"
#include <stdio.h>

//#define MC_DEBUG

MCCtrl mcCtrl;
uint8_t mc_ReadBuffer[32];

uint8_t mc_DecodeInit(uint8_t *fname, MCCtrl *mcx)
{
    uint8_t res = 0;
    FRESULT res_sd;
    UINT fnum;
    FIL mc_file;

    MC_RIFF *riff;
    MC_BPM *bpm;
    MC_DATA *data;

    res_sd = f_open(&mc_file, (TCHAR *) fname, FA_OPEN_EXISTING | FA_READ);
    if (res_sd == FR_OK)
    {
        f_read(&mc_file, mc_ReadBuffer, 32, &fnum);
        riff = (MC_RIFF *) mc_ReadBuffer;
        if (riff->ChunkID == 0x636d) //is MC file?
        {
            bpm = (MC_BPM *) (mc_ReadBuffer + 2);
            if (bpm->ChunkID == 0x206d7062)
                mcx->bpm = bpm->Data;
            data = (MC_DATA *) (mc_ReadBuffer + 8);
            if (data->ChunkID == 0x61746164)
                mcx->ledDataLen = data->ChunkSize;
            mcx->datastart = 16;
#ifdef MC_DEBUG
            printf("mcx->bpm:%d\n", mcx->bpm);
            printf("mcx->ledDataLen:%d\n", mcx->ledDataLen);
#endif
        }
        else
            res = 2; //DATA block no find
    }
    else
        res = 1; //no MC file

    f_close(&mc_file);
#ifdef MC_DEBUG
    if (res)
        printf("mc:res %d\n", res);
#endif
    return res;
}