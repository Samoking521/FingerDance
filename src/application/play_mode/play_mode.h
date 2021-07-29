#ifndef PLAY_MODE_H
#define PLAY_MODE_H

#include "../../drivers/ws2812b/ws2812b.h"
#include <stdint.h>

typedef struct {
    LEDProperty judgeLine[4];
    uint8_t key_col_1; //SW4
    uint8_t key_col_2; //SW3
    uint8_t key_col_3; //SW2
    uint8_t key_col_4; //SW1

    uint8_t redHold[4];
} PLAY_MODE_KEYCTRL;

typedef struct {
    uint8_t game_end;
    uint8_t len_flag;
    uint32_t beat_num;
    uint32_t show_tick;
} PLAY_MODE_CTRL;

void play_Init();
void play_main();
void play_screen();
void play_key();
void play_test();

#endif
