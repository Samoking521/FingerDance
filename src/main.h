#ifndef MAIN_H
#define MAIN_H

#include <stdint.h>
#include <stdbool.h>

typedef enum {
    init_mode = 0,
    start_mode = 1,
    pre_mode = 2,
    play_mode = 3,
    game_mode = 4
} MODE;

extern MODE modeCtrl;

void main_init();

#endif
