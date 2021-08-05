#ifndef GAME_MODE_H
#define GAME_MODE_H

#include "../../drivers/ws2812b/ws2812b.h"
#include <stdint.h>

typedef struct
{
    LEDProperty judgeLine[4];
    uint8_t key_col_1; //SW4
    uint8_t key_col_2; //SW3
    uint8_t key_col_3; //SW2
    uint8_t key_col_4; //SW1
    
    uint8_t redHold[4];
    uint8_t judgeHold[4];
    uint8_t greenHold[4];
    
    uint32_t holdBeat[4];
}GAME_MODE_KEYCTRL;

typedef struct {
    uint8_t game_end;
    uint8_t len_flag;
    uint8_t score_flag;
    uint32_t beat_num;
    uint32_t score;
    uint32_t combo;
    uint32_t show_tick;
} GAME_MODE_CTRL;

void game_init();
void game_main();
void game_screen();
void game_key();
void game_test();

#endif
