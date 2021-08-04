#include "play_mode.h"
#include "../music/music.h"
#include "../music/wav.h"
#include "../music/led.h"
#include "../screen/screen.h"
#include "../pre_mode/pre_mode.h"
#include "../../config/default/peripheral/coretimer/plib_coretimer.h"
#include "../../config/default/peripheral/tmr/plib_tmr3.h"
#include "../../drivers/ws2812b/ws2812b.h"
#include "../../drivers/key/key.h"
#include "../../utilities/led_queue/led_queue.h"
#include "../../main.h"
#include <stdio.h>
#include <string.h>

//#define PLAY_DEBUG

PLAY_MODE_KEYCTRL play_keyCtrl;
PLAY_MODE_CTRL play_Ctrl;

static void _PLAYSHOW_TIM3_CallbackHandler(uint32_t status, uintptr_t context);

void play_Init()
{
    LED_Queue_Init();

    key_Init();
    key_CallbackRegister(play_key);

    TMR3_CallbackRegister(_PLAYSHOW_TIM3_CallbackHandler, (uintptr_t) NULL);

    memset(&play_keyCtrl, 0, sizeof (play_keyCtrl));
    memset(&play_Ctrl, 0, sizeof (play_Ctrl));

    play_screen();
}

void play_main()
{
    play_Init();

    LED_Play(musicInfo[cursorCtrl.fileIndex].filename);
    music_Play(musicInfo[cursorCtrl.fileIndex].filename);

    LEDProperty LEDLine[4] = {0};
    int start_adjust = 12;

    TMR3_Start();
    key_Start();
    LED_Start();

    while (1)
    {
        if (play_Ctrl.game_end == 1)
        {
            play_Ctrl.game_end = 0;
            modeCtrl = pre_mode;
            music_Stop();
            LED_Stop();
            key_Stop();
            TMR3_Stop();
            printf("play end.\n");
            break;
        }
        if (play_Ctrl.len_flag == 1)
        {
            uint16_t cursec = music_GetCurSec();
            uint8_t cur_min = cursec / 60;
            uint8_t cur_sec = cursec % 60;

            Screen_ShowPlayMusicCurLen(cur_min, cur_sec);

            play_Ctrl.len_flag = 0;
        }
        if (LED_GetFallSign())
        {
            if (start_adjust > 0)
                start_adjust--;
            else if (start_adjust == 0)
            {
                music_Start();
                start_adjust = -1;
            }

            memset(LEDLine, 0, sizeof (LEDLine));
            uint32_t k = (play_Ctrl.beat_num % 512) * 4;
            if (play_Ctrl.beat_num < ledDev.beatNum)
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

            while (WS2812B_Lock() == true)
                ;
            WS2812B_FallALine(LEDLine);
            //WS2812B_UnLock();

            if (play_Ctrl.beat_num == ledDev.beatNum + 10)
                play_Ctrl.game_end = 1;

            play_Ctrl.beat_num += 1;

            LED_ClearFallSign();
        }
        if (audioDev.wavWhichBuf == 1)
        {
#ifdef PLAY_DEBUG
            printf("Buf2 start fill: %d\n", CORETIMER_CounterGet());
#endif
            music_buffer_fill(audioDev.i2sBuf1, MUSIC_BUFSIZE, wavCtrl.bps);
#ifdef PLAY_DEBUG
            printf("Buf2 end fill: %d\n", CORETIMER_CounterGet());
#endif
            audioDev.wavWhichBuf = 0;
        }
        else if (audioDev.wavWhichBuf == 2)
        {
#ifdef PLAY_DEBUG
            printf("Buf1 start fill: %d\n", CORETIMER_CounterGet());
#endif
            music_buffer_fill(audioDev.i2sBuf2, MUSIC_BUFSIZE, wavCtrl.bps);
#ifdef PLAY_DEBUG
            printf("Buf1 end fill: %d\n", CORETIMER_CounterGet());
#endif
            audioDev.wavWhichBuf = 0;
        }
    }
}

void play_screen()
{
    Screen_LoadPlayMode();
    Screen_ShowPlayMusicName(musicInfo[cursorCtrl.fileIndex].filename);
    Screen_ShowPlayMusicTotLen(musicInfo[cursorCtrl.fileIndex].len_min, musicInfo[cursorCtrl.fileIndex].len_sec);
    Screen_ShowPlayMusicCurLen(0, 0);
}

void play_key()
{
    if (!LED_Queue_IsFull())
        return;
    LQNode* lqnode = LED_Queue_GetFront();

    memset(play_keyCtrl.judgeLine, 0, sizeof (play_keyCtrl.judgeLine));

    if (keyCtrl.key4 == 1)
    {
        play_keyCtrl.key_col_1 = 1;
    }
    else
    {
        if (play_keyCtrl.key_col_1 == 1)
        {
            play_keyCtrl.key_col_1 = 0;
        }

        if (play_keyCtrl.redHold[0] > 0)
        {
            play_keyCtrl.redHold[0] -= 1;

            play_keyCtrl.judgeLine[0].index = 10;
            play_keyCtrl.judgeLine[0].color = GREEN;
        }
        else if (lqnode->data[0] > 0)
        {
            play_keyCtrl.judgeLine[0].index = 10;
            play_keyCtrl.judgeLine[0].color = GREEN;

            play_keyCtrl.redHold[0] = 8; // red conitnue 80ms
        }
        else
        {
            play_keyCtrl.judgeLine[0].index = 10;
            play_keyCtrl.judgeLine[0].color = JUDGE;
        }
    }

    if (keyCtrl.key3 == 1)
    {
        play_keyCtrl.key_col_2 = 1;
    }
    else
    {
        if (play_keyCtrl.key_col_2 == 1)
        {
            play_keyCtrl.key_col_2 = 0;
        }

        if (play_keyCtrl.redHold[1] > 0)
        {
            play_keyCtrl.redHold[1] -= 1;

            play_keyCtrl.judgeLine[1].index = 10;
            play_keyCtrl.judgeLine[1].color = GREEN;
        }
        else if (lqnode->data[1] > 0)
        {
            play_keyCtrl.judgeLine[1].index = 10;
            play_keyCtrl.judgeLine[1].color = GREEN;

            play_keyCtrl.redHold[1] = 8; // red conitnue 80ms
        }
        else
        {
            play_keyCtrl.judgeLine[1].index = 10;
            play_keyCtrl.judgeLine[1].color = JUDGE;
        }
    }

    if (keyCtrl.key2 == 1)
    {
        play_keyCtrl.key_col_3 = 1;
    }
    else
    {
        if (play_keyCtrl.key_col_3 == 1)
        {
            play_keyCtrl.key_col_3 = 0;
        }

        if (play_keyCtrl.redHold[2] > 0)
        {
            play_keyCtrl.redHold[2] -= 1;

            play_keyCtrl.judgeLine[2].index = 10;
            play_keyCtrl.judgeLine[2].color = GREEN;
        }
        else if (lqnode->data[2] > 0)
        {
            play_keyCtrl.judgeLine[2].index = 10;
            play_keyCtrl.judgeLine[2].color = GREEN;

            play_keyCtrl.redHold[2] = 8; // red conitnue 80ms
        }
        else
        {
            play_keyCtrl.judgeLine[2].index = 10;
            play_keyCtrl.judgeLine[2].color = JUDGE;
        }
    }

    if (keyCtrl.key1 == 1)
    {
        play_keyCtrl.key_col_4 = 1;
    }
    else
    {
        if (play_keyCtrl.key_col_4 == 1)
        {
            play_keyCtrl.key_col_4 = 0;
        }

        if (play_keyCtrl.redHold[3] > 0)
        {
            play_keyCtrl.redHold[3] -= 1;

            play_keyCtrl.judgeLine[3].index = 10;
            play_keyCtrl.judgeLine[3].color = GREEN;
        }
        else if (lqnode->data[3] > 0)
        {
            play_keyCtrl.judgeLine[3].index = 10;
            play_keyCtrl.judgeLine[3].color = GREEN;

            play_keyCtrl.redHold[3] = 8; // red conitnue 80ms
        }
        else
        {
            play_keyCtrl.judgeLine[3].index = 10;
            play_keyCtrl.judgeLine[3].color = JUDGE;
        }
    }
    if (WS2812B_Lock())
        return;
    WS2812B_SetJudgeLine(play_keyCtrl.judgeLine);
    //WS2812B_UnLock();
}

void play_test()
{

}

void _PLAYSHOW_TIM3_CallbackHandler(uint32_t status, uintptr_t context)
{
    play_Ctrl.show_tick += 1;
    if (play_Ctrl.show_tick % 10 == 0 && play_Ctrl.len_flag == 0)
        play_Ctrl.len_flag = 1;
}