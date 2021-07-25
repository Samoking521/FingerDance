#ifndef MC_H
#define MC_H

#include <stdint.h>

typedef struct
{
    uint16_t ChunkID;
} MC_RIFF;

typedef struct
{
    uint32_t ChunkID;
    uint16_t Data;
} MC_BPM;

typedef struct
{
    uint32_t ChunkID;
    uint32_t ChunkSize;
} MC_DATA;

typedef struct
{
    MC_RIFF riff;
    MC_BPM bpm;
    MC_DATA data;
} McHeader;

typedef struct
{
    uint8_t bpm;
    uint32_t ledDataLen;
} MCCtrl;

extern MCCtrl mcCtrl;

uint8_t mc_DecodeInit(uint8_t *fname, MCCtrl *mcx);

#endif
