#ifndef LED_H
#define LED_H

#include <stdint.h>
#include <stdbool.h>
#include "../../library/Fatfs/ff.h"

//led play control

typedef struct {
    uint8_t* ledBuf;
    uint8_t* ledBuf1;
    uint8_t* ledBuf2;
    volatile uint8_t ledWhichBufUse; //which buf is used now
    volatile uint8_t ledWhichBufFill; //which buf need fill now
    FIL file; //music file pointer
    uint8_t status; //bit0: 0, pause play; 1, continue play
    //bit1: 0, stop play; 1, begin play
    uint32_t beginTick;
    uint32_t ledTick;
    uint32_t beatNum;
    uint32_t beatInterval;
} LedDevice;

extern LedDevice ledDev;
//extern int fall_flag;

void LED_Init();
void LED_Start(void);
void LED_Stop(void);
uint8_t LED_Play(uint8_t* fname);
uint32_t LED_BufferFill(uint8_t *buf, uint16_t size);
bool LED_GetFallSign();
void LED_ClearFallSign();

#endif
