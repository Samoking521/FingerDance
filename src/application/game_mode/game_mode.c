#include "game_mode.h"
#include "../music/music.h"
#include "../music/led.h"
#include "../screen/screen.h"
#include "../sdfile/sdfile.h"
#include "../pre_mode/pre_mode.h"
#include "../../config/default/peripheral/coretimer/plib_coretimer.h"
#include "../../config/default/peripheral/tmr/plib_tmr4.h"
#include "../../config/default/peripheral/tmr/plib_tmr5.h"
#include "../../config/default/peripheral/gpio/plib_gpio.h"
#include "../../drivers/key/key.h"
#include "../../drivers/ws2812b/ws2812b.h"
#include "../../utilities/led_queue/led_queue.h"
#include "../../utilities/ans_queue/ans_queue.h"
#include "../../main.h"
#include "peripheral/tmr/plib_tmr3.h"
#include "utilities/ans_queue/ans_queue.h"
#include <stdio.h>
#include <string.h>

#define GAME_MODE_DEBUG
//#define GAME_MODE_LEDBUF_DEBUG
#define GAME_MODE_LED_BUFFSIZE 1024
#define GAME_MODE_MUSIC_BUFFSIZE 8192

#define GAME_MODE_JUDGE_OFFSET 0
#define GAME_MODE_JUDGE_TIME 2000000

uint32_t begin_tick = 0;
uint32_t led_tick = 0;

GAME_MODE_KEYCTRL game_keyCtrl;
GAME_MODE_CTRL game_Ctrl;

static void _GAMESHOW_TIM3_CallbackHandler(uint32_t status, uintptr_t context);

void game_init()
{
    LED_Queue_Init();
    memset(&game_keyCtrl, 0, sizeof (game_keyCtrl));

    key_Init();
    key_CallbackRegister(game_key);

    TMR3_CallbackRegister(_GAMESHOW_TIM3_CallbackHandler, (uintptr_t) NULL);

    memset(&game_keyCtrl, 0, sizeof (game_keyCtrl));
    memset(&game_Ctrl, 0, sizeof (game_Ctrl));

    game_screen();
}

void game_main()
{
    game_init();

    LED_Play(musicInfo[cursorCtrl.fileIndex].filename);
    music_Play(musicInfo[cursorCtrl.fileIndex].filename);

    LEDProperty LEDLine[4] = {0};
    AQNode judgeLine;
    game_Ctrl.beat_num = 0;

    TMR3_Start();
    key_Start();
    LED_Start();
    music_Start();

    while (1)
    {
        if (game_Ctrl.game_end == 1)
        {
            game_Ctrl.game_end = 0;
            modeCtrl = pre_mode;
            music_Stop();
            LED_Stop();
            key_Stop();
            TMR3_Stop();
            break;
        }
        if (game_Ctrl.len_flag == 1)
        {
            uint16_t cursec = music_GetCurSec();
            uint8_t cur_min = cursec / 60;
            uint8_t cur_sec = cursec % 60;

            Screen_ShowGameMusicCurLen(cur_min, cur_sec);

            game_Ctrl.len_flag = 0;
        }
        if (LED_GetFallSign())
        {
            memset(LEDLine, 0, sizeof (LEDLine));
            uint32_t k = (game_Ctrl.beat_num % 512) * 4;
            if (game_Ctrl.beat_num < ledDev.beatNum)
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
            //memset(game_keyCtrl.greenHold, 0, sizeof (game_keyCtrl.greenHold));

            while (WS2812B_Lock() == true)
                ;
            WS2812B_FallALine(LEDLine);
            WS2812B_UnLock();

            ledDev.ledTick = CORETIMER_CounterGet();

            //printf("beat_num: %d tick: %u\n", game_Ctrl.beat_num, ledDev.ledTick);
            //LED_Queue_Debug();

            if (LED_Queue_IsFull())
            {
                LQNode* lqnode = LED_Queue_GetFront();
                judgeLine.tick = ledDev.ledTick;
                for (int i = 0; i < 4; i++)
                    judgeLine.data[i] = lqnode->data[i];
                ANS_Queue_Add(&judgeLine);
                //ANS_Queue_Debug();
            }

            if (game_Ctrl.beat_num % 512 == 255)
            {
                ledDev.ledWhichBufUse = 2;
                ledDev.ledWhichBufFill = 1;
                //printf("buf1 end play, buf2 start play: %u\n", CORETIMER_CounterGet());
            }
            else if (game_Ctrl.beat_num % 512 == 511)
            {
                ledDev.ledWhichBufUse = 1;
                ledDev.ledWhichBufFill = 2;
                //printf("buf2 end play, buf1 start play: %u\n", CORETIMER_CounterGet());
            }

            if (game_Ctrl.beat_num == ledDev.beatNum + 10)
                game_Ctrl.game_end = 1;


#ifdef GAME_MODE_DEBUG
            //printf("led tick: %u\n", CORETIMER_CounterGet());
#endif

            game_Ctrl.beat_num += 1;

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
            //printf("Buf2 start fill: %u\n", CORETIMER_CounterGet());
            music_buffer_fill(audioDev.i2sBuf1, MUSIC_BUFSIZE, wavCtrl.bps);
            //printf("Buf2 end fill: %u\n", CORETIMER_CounterGet());
            audioDev.wavWhichBuf = 0;
        }
        else if (audioDev.wavWhichBuf == 2)
        {
            //printf("Buf1 start fill: %u\n", CORETIMER_CounterGet());
            music_buffer_fill(audioDev.i2sBuf2, MUSIC_BUFSIZE, wavCtrl.bps);
            //printf("Buf1 end fill: %u\n", CORETIMER_CounterGet());
            audioDev.wavWhichBuf = 0;
        }
    }
}

void game_screen()
{
    Screen_LoadGameMode();
    Screen_ShowGameMusicName(musicInfo[cursorCtrl.fileIndex].filename);
    Screen_ShowGameMusicTotLen(musicInfo[cursorCtrl.fileIndex].len_min, musicInfo[cursorCtrl.fileIndex].len_sec);
    Screen_ShowGameMusicCurLen(0, 0);
    Screen_ShowGameMusicScore(0);
    Screen_ShowGameMusicCombo(0);
}

void game_key()
{
    uint32_t key_tick = CORETIMER_CounterGet();

    if (!ANS_Queue_IsFull())
        return;
    AQNode* prenode = ANS_Queue_GetPre();
    AQNode* curnode = ANS_Queue_GetCur();

    memset(game_keyCtrl.judgeLine, 0, sizeof (game_keyCtrl.judgeLine));

    if (keyCtrl.key4 == 1)
    {
#ifdef GAME_MODE_DEBUG
        if (game_keyCtrl.key_col_1 == 0)
        {
            printf("key_tick: %u\n", key_tick / 20000);
            printf("cur_tick: %u pre_tick: %u\n", curnode->tick / 20000, prenode->tick / 20000);
            printf("cur_led: %d pre_led: %d\n", curnode->data[0], prenode->data[0]);
        }

#endif
        /*
        if (game_keyCtrl.judgeHold[0] == 1)
        {
            game_keyCtrl.judgeLine[0].index = 10;
            game_keyCtrl.judgeLine[0].color = JUDGE;
        }
        else if (game_keyCtrl.greenHold[0] > 0)
        {
            game_keyCtrl.greenHold[0] -= 1;

            game_keyCtrl.judgeLine[0].index = 10;
            game_keyCtrl.judgeLine[0].color = GREEN;
        }
        else if (lqnode->data[0] == 2)
        {
            game_keyCtrl.judgeLine[0].index = 10;
            game_keyCtrl.judgeLine[0].color = GREEN;
        }
        else if (lqnode->data[0] == 1)
        {
            if (low_tick <= key_tick && key_tick <= high_tick && game_keyCtrl.key_col_1 == 0)
            {
                game_keyCtrl.greenHold[0] = 12;

                game_keyCtrl.judgeLine[0].index = 10;
                game_keyCtrl.judgeLine[0].color = GREEN;
            }
            else
            {
                game_keyCtrl.judgeLine[0].index = 10;
                game_keyCtrl.judgeLine[0].color = JUDGE;
            }
        }
        else
        {
            game_keyCtrl.judgeHold[0] = 1;

            game_keyCtrl.judgeLine[0].index = 10;
            game_keyCtrl.judgeLine[0].color = JUDGE;
        }*/

        if (game_keyCtrl.greenHold[0] > 0)
        {
            game_keyCtrl.greenHold[0] -= 1;

            game_keyCtrl.judgeLine[0].index = 10;
            game_keyCtrl.judgeLine[0].color = GREEN;
        }
        else if (curnode->data[0] == 1 || prenode->data[0] == 1)
        {
            //if (low_tick <= key_tick && key_tick <= high_tick && game_keyCtrl.key_col_1 == 0)
            //{
            game_keyCtrl.greenHold[0] = 10;

            game_keyCtrl.judgeLine[0].index = 10;
            game_keyCtrl.judgeLine[0].color = GREEN;
            //}
            //else
            //{
            //    game_keyCtrl.judgeLine[0].index = 10;
            //    game_keyCtrl.judgeLine[0].color = JUDGE;
            //}
        }
        else if (curnode->data[0] == 2 || prenode->data[0] == 2)
        {
            game_keyCtrl.greenHold[0] = 10;

            game_keyCtrl.judgeLine[0].index = 10;
            game_keyCtrl.judgeLine[0].color = GREEN;
        }
        else
        {
            game_keyCtrl.judgeLine[0].index = 10;
            game_keyCtrl.judgeLine[0].color = JUDGE;
        }

        game_keyCtrl.key_col_1 = 1;

    }
    else
    {
        if (game_keyCtrl.key_col_1 == 1)
        {
            game_keyCtrl.key_col_1 = 0;

        }
        /*
        game_keyCtrl.judgeHold[0] = 0;

        if (game_keyCtrl.redHold[0] > 0)
        {
            game_keyCtrl.redHold[0] -= 1;

            game_keyCtrl.judgeLine[0].index = 10;
            game_keyCtrl.judgeLine[0].color = RED;
        }
        else if (game_keyCtrl.greenHold[0] > 0)
        {
            game_keyCtrl.greenHold[0] -= 1;

            game_keyCtrl.judgeLine[0].index = 10;
            game_keyCtrl.judgeLine[0].color = GREEN;
        }
        else if (lqnode->data[0] > 0)
        {
            game_keyCtrl.judgeLine[0].index = 10;
            game_keyCtrl.judgeLine[0].color = RED;

            game_keyCtrl.redHold[0] = 8; // red conitnue 80ms
        }
        else
        {
            game_keyCtrl.judgeLine[0].index = 10;
            game_keyCtrl.judgeLine[0].color = JUDGE;
        }*/

        if (game_keyCtrl.greenHold[0] > 0)
        {
            game_keyCtrl.greenHold[0] -= 1;

            game_keyCtrl.judgeLine[0].index = 10;
            game_keyCtrl.judgeLine[0].color = GREEN;
        }
        else
        {
            game_keyCtrl.judgeLine[0].index = 10;
            game_keyCtrl.judgeLine[0].color = JUDGE;
        }
    }

    if (keyCtrl.key3 == 1)
    {
        //printf("key3: 1\n");
        /*
        if (game_keyCtrl.greenHold[1] > 0)
        {
            game_keyCtrl.greenHold[1] -= 1;

            game_keyCtrl.judgeLine[1].index = 20;
            game_keyCtrl.judgeLine[1].color = GREEN;
        }
        else if (lqnode->data[1] == 1)
        {
            if (low_tick <= key_tick && key_tick <= high_tick && game_keyCtrl.key_col_2 == 0)
            {
                game_keyCtrl.greenHold[1] = 10;

                game_keyCtrl.judgeLine[1].index = 20;
                game_keyCtrl.judgeLine[1].color = GREEN;
            }
            else
            {
                game_keyCtrl.judgeLine[1].index = 20;
                game_keyCtrl.judgeLine[1].color = JUDGE;
            }
        }
        else if (lqnode->data[1] == 2)
        {
            game_keyCtrl.greenHold[1] = 10;

            game_keyCtrl.judgeLine[1].index = 20;
            game_keyCtrl.judgeLine[1].color = GREEN;
        }
        else
        {
            game_keyCtrl.judgeLine[1].index = 20;
            game_keyCtrl.judgeLine[1].color = JUDGE;
        }*/

        if (game_keyCtrl.greenHold[1] > 0)
        {
            game_keyCtrl.greenHold[1] -= 1;

            game_keyCtrl.judgeLine[1].index = 20;
            game_keyCtrl.judgeLine[1].color = GREEN;
        }
        else if (curnode->data[1] == 1 || prenode->data[1] == 1)
        {
            //if (low_tick <= key_tick && key_tick <= high_tick && game_keyCtrl.key_col_1 == 0)
            //{
            game_keyCtrl.greenHold[1] = 10;

            game_keyCtrl.judgeLine[1].index = 20;
            game_keyCtrl.judgeLine[1].color = GREEN;
            //}
            //else
            //{
            //    game_keyCtrl.judgeLine[0].index = 10;
            //    game_keyCtrl.judgeLine[0].color = JUDGE;
            //}
        }
        else if (curnode->data[1] == 2 || prenode->data[1] == 2)
        {
            game_keyCtrl.greenHold[1] = 10;

            game_keyCtrl.judgeLine[1].index = 20;
            game_keyCtrl.judgeLine[1].color = GREEN;
        }
        else
        {
            game_keyCtrl.judgeLine[1].index = 20;
            game_keyCtrl.judgeLine[1].color = JUDGE;
        }

        game_keyCtrl.key_col_2 = 1;
    }
    else
    {
        if (game_keyCtrl.key_col_2 == 1)
        {
            game_keyCtrl.key_col_2 = 0;
        }

        if (game_keyCtrl.greenHold[1] > 0)
        {
            game_keyCtrl.greenHold[1] -= 1;

            game_keyCtrl.judgeLine[1].index = 20;
            game_keyCtrl.judgeLine[1].color = GREEN;
        }
        else
        {
            game_keyCtrl.judgeLine[1].index = 20;
            game_keyCtrl.judgeLine[1].color = JUDGE;
        }
    }

    if (keyCtrl.key2 == 1)
    {
        if (game_keyCtrl.greenHold[2] > 0)
        {
            game_keyCtrl.greenHold[2] -= 1;

            game_keyCtrl.judgeLine[2].index = 30;
            game_keyCtrl.judgeLine[2].color = GREEN;
        }
        /*
        else if (lqnode->data[2] == 1)
        {
            if (low_tick <= key_tick && key_tick <= high_tick && game_keyCtrl.key_col_3 == 0)
            {
                game_keyCtrl.greenHold[2] = 10;

                game_keyCtrl.judgeLine[2].index = 30;
                game_keyCtrl.judgeLine[2].color = GREEN;
            }
            else
            {
                game_keyCtrl.judgeLine[2].index = 30;
                game_keyCtrl.judgeLine[2].color = JUDGE;
            }
        }
        else if (lqnode->data[2] == 2)
        {
            game_keyCtrl.greenHold[2] = 10;

            game_keyCtrl.judgeLine[2].index = 30;
            game_keyCtrl.judgeLine[2].color = GREEN;
        }
        else
        {
            game_keyCtrl.judgeLine[2].index = 30;
            game_keyCtrl.judgeLine[2].color = JUDGE;
        }*/

        if (game_keyCtrl.greenHold[2] > 0)
        {
            game_keyCtrl.greenHold[2] -= 1;

            game_keyCtrl.judgeLine[2].index = 30;
            game_keyCtrl.judgeLine[2].color = GREEN;
        }
        else if (curnode->data[2] > 0 || prenode->data[2] > 0)
        {
            //if (low_tick <= key_tick && key_tick <= high_tick && game_keyCtrl.key_col_1 == 0)
            //{
            game_keyCtrl.greenHold[2] = 10;

            game_keyCtrl.judgeLine[2].index = 30;
            game_keyCtrl.judgeLine[2].color = GREEN;
            //}
            //else
            //{
            //    game_keyCtrl.judgeLine[0].index = 10;
            //    game_keyCtrl.judgeLine[0].color = JUDGE;
            //}
        }
        else
        {
            game_keyCtrl.judgeLine[2].index = 30;
            game_keyCtrl.judgeLine[2].color = JUDGE;
        }

        game_keyCtrl.key_col_3 = 1;
    }
    else
    {
        if (game_keyCtrl.key_col_3 == 1)
        {
            game_keyCtrl.key_col_3 = 0;

            game_keyCtrl.judgeLine[2].index = 30;
            game_keyCtrl.judgeLine[2].color = JUDGE;
        }

        if (game_keyCtrl.greenHold[2] > 0)
        {
            game_keyCtrl.greenHold[2] -= 1;

            game_keyCtrl.judgeLine[2].index = 30;
            game_keyCtrl.judgeLine[2].color = GREEN;
        }
        else
        {
            game_keyCtrl.judgeLine[2].index = 30;
            game_keyCtrl.judgeLine[2].color = JUDGE;
        }
    }

    if (keyCtrl.key1 == 1)
    {
        if (game_keyCtrl.greenHold[3] > 0)
        {
            game_keyCtrl.greenHold[3] -= 1;

            game_keyCtrl.judgeLine[3].index = 40;
            game_keyCtrl.judgeLine[3].color = GREEN;
        }
        /*
        else if (lqnode->data[3] == 1)
        {
            if (low_tick <= key_tick && key_tick <= high_tick && game_keyCtrl.key_col_4 == 0)
            {
                game_keyCtrl.greenHold[3] = 10;

                game_keyCtrl.judgeLine[3].index = 40;
                game_keyCtrl.judgeLine[3].color = GREEN;
            }
            else
            {
                game_keyCtrl.judgeLine[3].index = 40;
                game_keyCtrl.judgeLine[3].color = JUDGE;
            }
        }
        else if (lqnode->data[3] == 2)
        {
            game_keyCtrl.greenHold[3] = 10;

            game_keyCtrl.judgeLine[3].index = 40;
            game_keyCtrl.judgeLine[3].color = GREEN;
        }
        else
        {
            game_keyCtrl.judgeLine[3].index = 40;
            game_keyCtrl.judgeLine[3].color = JUDGE;
        }*/

        if (game_keyCtrl.greenHold[3] > 0)
        {
            game_keyCtrl.greenHold[3] -= 1;

            game_keyCtrl.judgeLine[3].index = 40;
            game_keyCtrl.judgeLine[3].color = GREEN;
        }
        else if (curnode->data[3] > 0 || prenode->data[3] > 0)
        {
            //if (low_tick <= key_tick && key_tick <= high_tick && game_keyCtrl.key_col_1 == 0)
            //{
            game_keyCtrl.greenHold[3] = 10;

            game_keyCtrl.judgeLine[3].index = 40;
            game_keyCtrl.judgeLine[3].color = GREEN;
            //}
            //else
            //{
            //    game_keyCtrl.judgeLine[0].index = 10;
            //    game_keyCtrl.judgeLine[0].color = JUDGE;
            //}
        }
        else
        {
            game_keyCtrl.judgeLine[2].index = 40;
            game_keyCtrl.judgeLine[2].color = JUDGE;
        }

        game_keyCtrl.key_col_4 = 1;
    }
    else
    {
        if (game_keyCtrl.key_col_4 == 1)
        {
            game_keyCtrl.key_col_4 = 0;

            game_keyCtrl.judgeLine[3].index = 40;
            game_keyCtrl.judgeLine[3].color = JUDGE;
        }

        if (game_keyCtrl.greenHold[3] > 0)
        {
            game_keyCtrl.greenHold[3] -= 1;

            game_keyCtrl.judgeLine[3].index = 40;
            game_keyCtrl.judgeLine[3].color = GREEN;
        }
        else
        {
            game_keyCtrl.judgeLine[3].index = 40;
            game_keyCtrl.judgeLine[3].color = JUDGE;
        }
    }
    if (WS2812B_Lock())
        return;
    WS2812B_SetJudgeLine(game_keyCtrl.judgeLine);
    WS2812B_UnLock();
}

void game_test()
{
}

void _GAMESHOW_TIM3_CallbackHandler(uint32_t status, uintptr_t context)
{
    game_Ctrl.show_tick += 1;
    if (game_Ctrl.show_tick % 10 == 0 && game_Ctrl.len_flag == 0)
        game_Ctrl.len_flag = 1;
}