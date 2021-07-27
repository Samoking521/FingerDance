#include "game_mode.h"
#include "../screen/screen.h"
#include "../sdfile/sdfile.h"
#include "../../config/default/peripheral/coretimer/plib_coretimer.h"
#include "../../config/default/peripheral/tmr/plib_tmr4.h"
#include "../../drivers/key/key.h"
#include "../../drivers/ws2812b/ws2812b.h"
//#include "../../drivers/ws2812b/ws2812b_data.h"
#include "../../main.h"
#include <stdio.h>
#include <string.h>

LEDProperty judgeLine[4] = {0};
uint8_t key_col_1 = 0; //SW4
uint8_t key_col_2 = 0; //SW3
uint8_t key_col_3 = 0; //SW2
uint8_t key_col_4 = 0; //SW1

uint32_t begin_tick = 0;
uint32_t beat_num = 0;
uint32_t tick_buffer[300];

void game_main()
{
    game_screen();

    uint32_t frequency;
    frequency = TMR4_FrequencyGet();
    printf("fre: %d\n", frequency);

    uint16_t beat_interval = 100;
    uint16_t tim_beat_period;
    tim_beat_period = beat_interval * frequency / 1000;
    printf("period: %d\n", tim_beat_period);
    TMR4_PeriodSet(tim_beat_period);
    TMR4_Start();

    begin_tick = CORETIMER_CounterGet();
    printf("begin_tick: %d\n", begin_tick);

    LEDProperty LEDLine[4] = {0};
    int cur_line = 0;

    while (1)
    {
        game_key();
        if (fall_flag == 1)
        {
            memset(LEDLine, 0, sizeof (LEDLine));
            if (cur_line == 0)
                LEDLine[0].color = BLUE;
            else
                LEDLine[0].color = BLACK;
            LEDLine[1].color = BLACK;
            LEDLine[2].color = BLACK;
            LEDLine[3].color = BLACK;
            if (cur_line == 9)
            {
                printf("led_tick: %d\n", CORETIMER_CounterGet());
                beat_num += 1;
            }
            WS2812B_FallALine(LEDLine);
            cur_line = (cur_line + 1) % 10;
            fall_flag = 0;
        }
    }
}

void game_screen()
{
    Screen_LoadGameMode();
}

void game_key()
{
    key_scan();
    if (keyCtrl.key4 == 1)
    {
        if (key_col_1 == 0)
        {
            key_col_1 = 1;

            uint32_t key_tick = CORETIMER_CounterGet() - begin_tick;
            
            key_tick = key_tick % 40000000;
            printf("key_tick: %d\n", key_tick);

            for (int i = 0; i < 4; i++)
                judgeLine[i].index = 0;
            if (39200000 <= key_tick || key_tick <= 800000)
            {
                judgeLine[0].index = 10;
                judgeLine[0].color = GREEN;
            }
            else
            {
                judgeLine[0].index = 10;
                judgeLine[0].color = RED;
            }
            WS2812B_SetJudgeLine(judgeLine);
        }
    }
    else
    {
        if (key_col_1 == 1)
        {
            key_col_1 = 0;

            for (int i = 0; i < 4; i++)
                judgeLine[i].index = 0;
            judgeLine[0].index = 10;
            judgeLine[0].color = JUDGE;
            WS2812B_SetJudgeLine(judgeLine);
        }
    }
    if (keyCtrl.key3 == 1)
    {
        if (key_col_2 == 0)
        {
            key_col_2 = 1;

            for (int i = 0; i < 4; i++)
                judgeLine[i].index = 0;
            judgeLine[1].index = 20;
            judgeLine[1].color = GREEN;
            WS2812B_SetJudgeLine(judgeLine);
        }
    }
    else
    {
        if (key_col_2 == 1)
        {
            key_col_2 = 0;

            for (int i = 0; i < 4; i++)
                judgeLine[i].index = 0;
            judgeLine[1].index = 20;
            judgeLine[1].color = JUDGE;
            WS2812B_SetJudgeLine(judgeLine);
        }
    }
    if (keyCtrl.key2 == 1)
    {
        if (key_col_3 == 0)
        {
            key_col_3 = 1;

            for (int i = 0; i < 4; i++)
                judgeLine[i].index = 0;
            judgeLine[2].index = 30;
            judgeLine[2].color = GREEN;
            WS2812B_SetJudgeLine(judgeLine);
        }
    }
    else
    {
        if (key_col_3 == 1)
        {
            key_col_3 = 0;

            for (int i = 0; i < 4; i++)
                judgeLine[i].index = 0;
            judgeLine[2].index = 30;
            judgeLine[2].color = JUDGE;
            WS2812B_SetJudgeLine(judgeLine);
        }
    }
    if (keyCtrl.key1 == 1)
    {
        if (key_col_4 == 0)
        {
            key_col_4 = 1;

            for (int i = 0; i < 4; i++)
                judgeLine[i].index = 0;
            judgeLine[3].index = 40;
            judgeLine[3].color = GREEN;
            WS2812B_SetJudgeLine(judgeLine);
        }
    }
    else
    {
        if (key_col_4 == 1)
        {
            key_col_4 = 0;

            for (int i = 0; i < 4; i++)
                judgeLine[i].index = 0;
            judgeLine[3].index = 40;
            judgeLine[3].color = JUDGE;
            WS2812B_SetJudgeLine(judgeLine);
        }
    }
}

void game_test()
{
}