#ifndef KEY_H
#define KEY_H

#include <stdint.h>

typedef struct
{
    uint8_t key1;
    uint8_t key2;
    uint8_t key3;
    uint8_t key4;
}KEY_CTRL;

extern KEY_CTRL keyCtrl;

void key_Init();
void key_scan();
void key_Test();

#endif
