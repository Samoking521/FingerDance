#ifndef KEY_H
#define KEY_H

#include <stdint.h>

typedef void (*KEY_CALLBACK)();

typedef struct
{
    uint8_t key1;
    uint8_t key2;
    uint8_t key3;
    uint8_t key4;
    
    KEY_CALLBACK callback_fn;
}KEY_CTRL;

extern KEY_CTRL keyCtrl;

void key_Init();
void key_Start();
void key_Stop();
void key_CallbackRegister(KEY_CALLBACK callback_fn);
void key_scan();
void key_scan_ex();
void key_Test();

#endif
