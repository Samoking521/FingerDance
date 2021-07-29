#include "game_mode.h"
#include "../music/music.h"
#include "../music/led.h"
#include "../screen/screen.h"
#include "../sdfile/sdfile.h"
#include "../pre_mode/pre_mode.h"
#include "../../config/default/peripheral/coretimer/plib_coretimer.h"
#include "../../config/default/peripheral/tmr/plib_tmr4.h"
#include "../../config/default/peripheral/tmr/plib_tmr5.h"
#include "../../drivers/key/key.h"
#include "../../drivers/ws2812b/ws2812b.h"
#include "../../utilities/led_queue/led_queue.h"
#include "../../main.h"
#include <stdio.h>
#include <string.h>

#define GAME_MODE_DEBUG
//#define GAME_MODE_LEDBUF_DEBUG
#define GAME_MODE_LED_BUFFSIZE 1024
#define GAME_MODE_MUSIC_BUFFSIZE 8192

#define GAME_MODE_JUDGE_OFFSET 200000
#define GAME_MODE_JUDGE_TIME 1600000

LEDProperty judgeLine[4] = {0};
uint8_t key_col_1 = 0; //SW4
uint8_t key_col_2 = 0; //SW3
uint8_t key_col_3 = 0; //SW2
uint8_t key_col_4 = 0; //SW1

uint8_t game_end = 0;

uint32_t begin_tick = 0;
uint32_t led_tick = 0;
uint32_t beat_num = 0;

GAME_MODE_KEYCTRL game_keyCtrl;

static void _KEY_TIM5_CallbackHandler(uint32_t status, uintptr_t context);

void game_init()
{
    LED_Queue_Init();
    memset(&game_keyCtrl, 0, sizeof (game_keyCtrl));

    game_screen();
}

void game_main()
{
    game_init();

    TMR5_CallbackRegister(_KEY_TIM5_CallbackHandler, (uintptr_t) NULL);
    TMR5_Start();

    LED_Play(musicInfo[cursorCtrl.fileIndex].filename);
    music_Play(musicInfo[cursorCtrl.fileIndex].filename);

    LEDProperty LEDLine[4] = {0};
    beat_num = 0;

    LED_Start();
    music_Start();

    while (1)
    {
        if (game_end == 1)
        {
            game_end = 0;
            modeCtrl = pre_mode;
            LED_Stop();
            TMR5_Stop();
            printf("game end. beat_num is: %d!\n", beat_num);
            break;
        }
        if (LED_GetFallSign())
        {
            memset(LEDLine, 0, sizeof (LEDLine));
            uint32_t k = (beat_num % 512) * 4;
            if (beat_num < ledDev.beatNum)
            {
                LED_Queue_Add(ledDev.ledBuf + k);
                for (int j = 0; j < 4; j++)
                {
                    if (ledDev.ledBuf[k + j] == 1)
                        LEDLine[j].color = NOTE;
                    else if (ledDev.ledBuf[k + j] == 2)
                        LEDLine[j].color = HOLD;
                }
            }
            else
            {
                LED_Queue_Add(ledDev.ledBuf + k);
            }
            memset(game_keyCtrl.greenHold, 0, sizeof (game_keyCtrl.greenHold));

            while (WS2812B_Lock() == true)
                ;
            WS2812B_FallALine(LEDLine);
            WS2812B_UnLock();

            ledDev.ledTick = CORETIMER_CounterGet();

            if (beat_num % 512 == 255)
            {
                ledDev.ledWhichBufUse = 2;
                ledDev.ledWhichBufFill = 1;
                //printf("buf1 end play, buf2 start play: %u\n", CORETIMER_CounterGet());
            }
            else if (beat_num % 512 == 511)
            {
                ledDev.ledWhichBufUse = 1;
                ledDev.ledWhichBufFill = 2;
                //printf("buf2 end play, buf1 start play: %u\n", CORETIMER_CounterGet());
            }

            if (beat_num == ledDev.beatNum + 10)
                game_end = 1;


#ifdef GAME_MODE_DEBUG
            //printf("led tick: %u\n", CORETIMER_CounterGet());
#endif

            beat_num += 1;

            LED_ClearFallSign();
        }

        if (ledDev.ledWhichBufFill == 1)
        {
            //printf("buf1 start fill: %u\n", CORETIMER_CounterGet());
            LED_BufferFill(ledDev.ledBuf1, GAME_MODE_LED_BUFFSIZE);
            ledDev.ledWhichBufFill = 0;
            //printf("buf1 end fill: %u\n", CORETIMER_CounterGet());
        }
        else if (ledDev.ledWhichBufFill == 2)
        {
            //printf("buf2 start fill: %u\n", CORETIMER_CounterGet());
            LED_BufferFill(ledDev.ledBuf2, GAME_MODE_LED_BUFFSIZE);
            ledDev.ledWhichBufFill = 0;
            //printf("buf2 end fill: %u\n", CORETIMER_CounterGet());
        }
        if (audioDev.wavWhichBuf == 1)
        {
            printf("Buf2 start fill: %u\n", CORETIMER_CounterGet());
            music_buffer_fill(audioDev.i2sBuf1, MUSIC_BUFSIZE, wavCtrl.bps);
            printf("Buf2 end fill: %u\n", CORETIMER_CounterGet());
            audioDev.wavWhichBuf = 0;
        }
        else if (audioDev.wavWhichBuf == 2)
        {
            printf("Buf1 start fill: %u\n", CORETIMER_CounterGet());
            music_buffer_fill(audioDev.i2sBuf2, MUSIC_BUFSIZE, wavCtrl.bps);
            printf("Buf1 end fill: %u\n", CORETIMER_CounterGet());
            audioDev.wavWhichBuf = 0;
        }
    }
}

void game_screen()
{
    Screen_LoadGameMode();
    Screen_ShowMusicName(musicInfo[cursorCtrl.fileIndex].filename);
    Screen_ShowMusicTotLen(musicInfo[cursorCtrl.fileIndex].len_min, musicInfo[cursorCtrl.fileIndex].len_sec);
    Screen_ShowMusicCurLen(0, 0);
    Screen_ShowMusicScore(0);
    Screen_ShowMusicCombo(0);
}

void game_key()
{
    key_scan_ex();

    uint32_t key_tick = CORETIMER_CounterGet();
    uint32_t low_tick = ledDev.ledTick + GAME_MODE_JUDGE_OFFSET;
    uint32_t high_tick = low_tick + GAME_MODE_JUDGE_OFFSET + GAME_MODE_JUDGE_TIME;

    if (!LED_Queue_IsFull())
        return;
    LQNode* lqnode = LED_Queue_GetFront();

    memset(judgeLine, 0, sizeof (judgeLine));

    if (keyCtrl.key4 == 1)
    {
#ifdef GAME_MODE_DEBUG
        if (key_col_1 == 0)
            printf("low_tick: %u key_tick: %u high_tick:%u\n", low_tick / 20000, key_tick / 20000, high_tick / 20000);
#endif

        if (game_keyCtrl.judgeHold[0] == 1)
        {
            judgeLine[0].index = 10;
            judgeLine[0].color = JUDGE;
        }
        else if (game_keyCtrl.greenHold[0] > 0)
        {
            game_keyCtrl.greenHold[0] -= 1;

            judgeLine[0].index = 10;
            judgeLine[0].color = GREEN;
        }
        else if (lqnode->data[0] == 2)
        {
            judgeLine[0].index = 10;
            judgeLine[0].color = GREEN;
        }
        else if (lqnode->data[0] == 1)
        {
            if (low_tick <= key_tick && key_tick <= high_tick && key_col_1 == 0)
            {
                game_keyCtrl.greenHold[0] = 12;

                judgeLine[0].index = 10;
                judgeLine[0].color = GREEN;
            }
            else
            {
                judgeLine[0].index = 10;
                judgeLine[0].color = JUDGE;
            }
        }
        else
        {
            game_keyCtrl.judgeHold[0] = 1;

            judgeLine[0].index = 10;
            judgeLine[0].color = JUDGE;
        }

        key_col_1 = 1;

    }
    else
    {
        if (key_col_1 == 1)
        {
            key_col_1 = 0;
        }

        game_keyCtrl.judgeHold[0] = 0;

        if (game_keyCtrl.redHold[0] > 0)
        {
            game_keyCtrl.redHold[0] -= 1;

            judgeLine[0].index = 10;
            judgeLine[0].color = RED;
        }
        else if (game_keyCtrl.greenHold[0] > 0)
        {
            game_keyCtrl.greenHold[0] -= 1;

            judgeLine[0].index = 10;
            judgeLine[0].color = GREEN;
        }
        else if (lqnode->data[0] > 0)
        {
            judgeLine[0].index = 10;
            judgeLine[0].color = RED;

            game_keyCtrl.redHold[0] = 8; // red conitnue 80ms
        }
        else
        {
            judgeLine[0].index = 10;
            judgeLine[0].color = JUDGE;
        }
    }

    if (keyCtrl.key3 == 1)
    {
        //printf("key3: 1\n");
        if (key_col_2 == 0)
        {
            key_col_2 = 1;

            if (low_tick <= key_tick && key_tick <= high_tick && lqnode->data[1] > 0)
            {
                judgeLine[1].index = 20;
                judgeLine[1].color = GREEN;
            }
            else
            {
                judgeLine[1].index = 20;
                judgeLine[1].color = RED;
            }
        }
    }
    else
    {
        if (key_col_2 == 1)
        {
            key_col_2 = 0;

            judgeLine[1].index = 20;
            judgeLine[1].color = JUDGE;
        }
    }
    if (keyCtrl.key2 == 1)
    {
        if (key_col_3 == 0)
        {
            key_col_3 = 1;

            if (low_tick <= key_tick && key_tick <= high_tick && lqnode->data[2] > 0)
            {
                judgeLine[2].index = 30;
                judgeLine[2].color = GREEN;
            }
            else
            {
                judgeLine[2].index = 30;
                judgeLine[2].color = RED;
            }
        }
    }
    else
    {
        if (key_col_3 == 1)
        {
            key_col_3 = 0;

            judgeLine[2].index = 30;
            judgeLine[2].color = JUDGE;
        }
    }
    if (keyCtrl.key1 == 1)
    {
        if (key_col_4 == 0)
        {
            key_col_4 = 1;

            if (low_tick <= key_tick && key_tick <= high_tick && lqnode->data[3] > 0)
            {
                judgeLine[3].index = 40;
                judgeLine[3].color = GREEN;
            }
            else
            {
                judgeLine[3].index = 40;
                judgeLine[3].color = RED;
            }
        }
    }
    else
    {
        if (key_col_4 == 1)
        {
            key_col_4 = 0;

            judgeLine[3].index = 40;
            judgeLine[3].color = JUDGE;
        }
    }
    if (WS2812B_Lock())
        return;
    WS2812B_SetJudgeLine(judgeLine);
    WS2812B_UnLock();
}

void game_test()
{
}

void _KEY_TIM5_CallbackHandler(uint32_t status, uintptr_t context)
{
    game_key();
}