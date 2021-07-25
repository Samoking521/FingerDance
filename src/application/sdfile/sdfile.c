#include "sdFile.h"
#include "../../library/Fatfs/ff.h"
#include "../music/mc.h"
#include "../music/wav.h"
#include <stdio.h>
#include <string.h>

#define SDFILE_DEBUG
#define MAX_FILENUM 10
FATFS sdFile_fs;

void sdFile_Init()
{
    FRESULT res_sd;
    res_sd = f_mount(&sdFile_fs, "0:", 1);
    if (res_sd != FR_OK)
    {
#ifdef SDFILE_DEBUG
        printf("SD card mount file system failure:(%d)\n", res_sd);
#endif
        while (1)
            ;
    }
    else
    {
#ifdef SDFILE_DEBUG
        printf("SD card mount file system success\n");
#endif
    }
}

void sdFile_GetFileList(MusicInfo *musicInfo, int *len)
{
    char path[] = "0:";
    FRESULT res;
    FILINFO fno;
    DIR dir;
    char *fn; /* This function assumes non-Unicode configuration */
#if _USE_LFN
    static char lfn[_MAX_LFN + 1]; /* Buffer to store the LFN */
    fno.lfname = lfn;
    fno.lfsize = sizeof lfn;
#endif

    res = f_opendir(&dir, path); /* Open the directory */
    if (res == FR_OK)
    {
        *len = 0;
        for (;;)
        {
            res = f_readdir(&dir, &fno); /* Read a directory item */
            if (res != FR_OK || fno.fname[0] == 0)
                break; /* Break on error or end of dir */
            if (fno.fname[0] == '.')
                continue; /* Ignore dot entry */
#if _USE_LFN
            fn = *fno.lfname ? fno.lfname : fno.fname;
#else
            fn = fno.fname;
#endif
            if (fno.fattrib & AM_DIR)
            {
            }
            else
            { /* It is a file. */
                //printf("%s/%s\n", path, fn);
                int fnLen = strlen(fn) + 1;
                for (int i = 0; i + 3 < fnLen; i++)
                {
                    if (fn[i] == '.' && fn[i + 1] == 'w' && fn[i + 2] == 'a' && fn[i + 3] == 'v')
                    {
                        fn[i] = '\0';
                        memcpy(musicInfo[*len].filename, fn, strlen(fn) + 1);
                        *len += 1;
#ifdef SDFILE_DEBUG
                        printf("len:%d\n", *len);
#endif
                        break;
                    }
                }
            }
            if (*len == MAX_FILENUM)
                break;
        }
        f_closedir(&dir);
    }
}

void sdFile_GetWAVFiles(MusicInfo *musicInfo, int len)
{
    uint8_t wav_filename[25];
    for (int i = 0; i < len; i++)
    {
        memcpy(wav_filename, musicInfo[i].filename, strlen((char *) musicInfo[i].filename) + 1);
        int j = 0;
        while (wav_filename[j] != '\0')
            j++;
        wav_filename[j] = '.';
        wav_filename[j + 1] = 'w';
        wav_filename[j + 2] = 'a';
        wav_filename[j + 3] = 'v';
        wav_filename[j + 4] = '\0';
        
        printf("%s\n", wav_filename);

        if (wav_DecodeInit(wav_filename, &wavCtrl) == 0)
        {
            uint16_t wav_len;
            wav_len = wavCtrl.datasize / wavCtrl.samplerate / 4;
            musicInfo[i].len_min = wav_len / 60;
            musicInfo[i].len_sec = wav_len % 60;
        }
        else
        {
            musicInfo[i].len_min = 0;
            musicInfo[i].len_sec = 0;
        }
    }
}

void sdFile_GetMCFiles(MusicInfo *musicInfo, int len)
{
    uint8_t mc_filename[25];
    for (int i = 0; i < len; i++)
    {
        memcpy(mc_filename, musicInfo[i].filename, strlen((char *) musicInfo[i].filename) + 1);
        int j = 0;
        while (mc_filename[j] != '\0')
            j++;
        mc_filename[j] = '.';
        mc_filename[j + 1] = 'm';
        mc_filename[j + 2] = 'c';
        mc_filename[j + 3] = '\0';

        if (mc_DecodeInit(mc_filename, &mcCtrl) == 0)
        {
            musicInfo[i].bpm = mcCtrl.bpm;
        }
        else
        {
            musicInfo[i].bpm = 0;
        }
    }
}