#ifndef MUSIC_H
#define MUSIC_H

#include <stdint.h>
#include "../../library/Fatfs/ff.h"
#include "wav.h"

#define MUSIC_BUFSIZE 8192

//music play control

typedef struct {
    uint8_t *i2sBuf1;
    uint8_t *i2sBuf2;
    volatile uint8_t wavWhichBuf; //which buf is use now
    FIL file; //music file pointer
    uint8_t status; //bit0: 0, pause play; 1, continue play
                    //bit1: 0, stop play; 1, begin play
    uint32_t txSize;
    uint32_t curSec;
    uint32_t totSec;
} AudioDevice;

extern AudioDevice audioDev;


void music_Start(void);
void music_Stop(void);
void music_Init();
uint8_t music_Play(uint8_t* fname);
uint32_t music_buffer_fill(uint8_t *buf, uint16_t size, uint8_t bits);
uint16_t music_GetCurSec();

#endif
