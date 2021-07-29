#include "led.h"
#include "mc.h"
#include "../../drivers/ws2812b/ws2812b.h"
#include "../../config/default/peripheral/tmr/plib_tmr4.h"
#include "../../config/default/peripheral/coretimer/plib_coretimer.h"
#include <stdio.h>
#include <string.h>

#define LED_DEBUG
#define WS2812B_DATA_BUFSIZE 256

LedDevice ledDev;
static uint8_t ledBuf[2][WS2812B_DATA_BUFSIZE][4];
static int fall_flag = 0;

static void _LED_TIM4_CallbackHandler(uint32_t status, uintptr_t context);

void LED_Init()
{
    WS2812B_Init();

    TMR4_CallbackRegister(_LED_TIM4_CallbackHandler, (uintptr_t) NULL);
}

uint8_t LED_Play(uint8_t* fname)
{
#ifdef LED_DEBUG
    printf("----led_play----\n");
#endif
    uint8_t res;
    FRESULT res_sd;

    fall_flag = 0;

    ledDev.ledBuf = (uint8_t*) ledBuf;
    ledDev.ledBuf1 = (uint8_t*) ledBuf[0];
    ledDev.ledBuf2 = (uint8_t*) ledBuf[1];

    uint8_t mc_filename[25];
    memcpy(mc_filename, fname, strlen((char *) fname) + 1);
    int j = 0;
    while (mc_filename[j] != '\0')
        j++;
    mc_filename[j] = '.';
    mc_filename[j + 1] = 'm';
    mc_filename[j + 2] = 'c';
    mc_filename[j + 3] = '\0';

    res = mc_DecodeInit(mc_filename, &mcCtrl);

    if (res == 0)
    {
        res_sd = f_open(&ledDev.file, (TCHAR *) mc_filename, FA_OPEN_EXISTING | FA_READ);
        if (res_sd == FR_OK)
        {
            f_lseek(&ledDev.file, mcCtrl.datastart); //jump to data block
            ledDev.ledWhichBufUse = 1;
            ledDev.ledWhichBufFill = 0;
            ledDev.beatNum = mcCtrl.ledDataLen >> 2;
            ledDev.beatInterval = 15 / mcCtrl.bpm * 20000000;

            LED_BufferFill((uint8_t*) ledBuf[0], WS2812B_DATA_BUFSIZE * 4);
            LED_BufferFill((uint8_t*) ledBuf[1], WS2812B_DATA_BUFSIZE * 4);

            uint32_t frequency;
            frequency = TMR4_FrequencyGet();
#ifdef LED_DEBUG
            printf("fre: %d\n", frequency);
#endif

            uint16_t tim_beat_period;
            tim_beat_period = 15 * frequency / mcCtrl.bpm;
#ifdef LED_DEBUG
            printf("period: %d\n", tim_beat_period);
#endif
            TMR4_PeriodSet(tim_beat_period);
        }
        else
            res = 0XFF;
    }
    else
        res = 0XFF;
    return res;
}

void LED_Start(void)
{
    TMR4_Start();

    ledDev.beginTick = CORETIMER_CounterGet();
    ledDev.ledTick = ledDev.beginTick;
#ifdef LED_DEBUG
    printf("begin_tick: %u\n", ledDev.beginTick);
#endif
}

void LED_Stop(void)
{
    TMR4_Stop();
    f_close(&ledDev.file);
}

uint32_t LED_BufferFill(uint8_t *buf, uint16_t size)
{
    UINT fnum;
    uint16_t i;

    f_read(&ledDev.file, buf, size, &fnum);
    if (fnum < size)
    {
        for (i = fnum; i < size - fnum; i++)
            buf[i] = 0;
    }
    /*
    for (int i = 0; i < fnum; i++)
    {
        printf("%x", buf[i]);
        if(i % 4 == 3)
            printf("\n");
        else
            printf(" ");
    }*/

    return fnum;
}

bool LED_GetFallSign()
{
    return fall_flag;
}

void LED_ClearFallSign()
{
    fall_flag = 0;
}

void _LED_TIM4_CallbackHandler(uint32_t status, uintptr_t context)
{
    fall_flag = 1;
}