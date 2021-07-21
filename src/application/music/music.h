#ifndef MUSIC_H
#define MUSIC_H

#include <stdint.h>
#include "../../library/Fatfs/ff.h"

//music play control

typedef struct {
    uint8_t *i2sBuf1;
    uint8_t *i2sBuf2;
    volatile uint8_t wavWhichBuf; //which buf is use now
    FIL file; //music file pointer
    uint8_t status; //bit0: 0, pause play; 1, continue play
    //bit1: 0, stop play; 1, begin play
} AudioDevice;

extern AudioDevice audiodev;


void music_start(void);
void music_stop(void);
uint16_t music_get_tnum(uint8_t *path);
void music_index_show(uint16_t x, uint16_t y, uint16_t index, uint16_t total);
void music_vol_show(uint16_t x, uint16_t y, uint8_t vol);
void music_msg_show(uint16_t x, uint16_t y, uint32_t totsec, uint32_t cursec, uint32_t bitrate);
void music_init();
uint8_t music_play(uint8_t* fname);
uint32_t music_buffer_fill(uint8_t *buf, uint16_t size, uint8_t bits);
uint8_t music_play_song(uint8_t* fname);


#endif
