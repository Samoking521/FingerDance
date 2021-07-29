#ifndef GAME_MODE_H
#define GAME_MODE_H

#include <stdint.h>

typedef struct
{
    uint8_t redHold[4];
    uint8_t judgeHold[4];
    uint8_t greenHold[4];
}GAME_MODE_KEYCTRL;

void game_init();
void game_main();
void game_screen();
void game_key();
void game_test();

#endif
