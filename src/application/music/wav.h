#ifndef WAV_H
#define WAV_H

#include <stdint.h>

//RIFF block
typedef struct
{
    uint32_t ChunkID;   //RIFF -- 0X46464952
    uint32_t ChunkSize; //file total size - 8
    uint32_t Format;    //WAVE -- 0X45564157
} ChunkRIFF;

//fmt block
typedef struct
{
    uint32_t ChunkID;       //fmt -- 0X20746D66
    uint32_t ChunkSize;     //20
    uint16_t AudioFormat;   //0X01, PCM; 0X11, IMA ADPCM
    uint16_t NumOfChannels; //1, mono; 2, stereo
    uint32_t SampleRate;    
    uint32_t ByteRate;      
    uint16_t BlockAlign;    
    uint16_t BitsPerSample; 
    //uint16_t ByteExtraData;
} ChunkFMT;

//fact block
typedef struct
{
    uint32_t ChunkID;      //fact -- 0X74636166
    uint32_t ChunkSize;    //4
    uint32_t NumOfSamples;
} ChunkFACT;

//LIST block
typedef struct
{
    uint32_t ChunkID;   //LIST -- 0X74636166
    uint32_t ChunkSize; //4
} ChunkLIST;

//data block
typedef struct
{
    uint32_t ChunkID;   //data -- 0X5453494C
    uint32_t ChunkSize;
} ChunkDATA;

//wav head
typedef struct
{
    ChunkRIFF riff; //riff
    ChunkFMT fmt;   //fmt
                    //ChunkFACT fact
                    //fact
    ChunkDATA data; //data
} WaveHeader;

//wav play control struct
typedef struct
{
    uint16_t audioformat; //0X01, PCM; 0X11, IMA ADPCM
    uint16_t nchannels;   //1, mono; 2, stereo
    uint16_t blockalign;  
    uint32_t datasize;    //WAV data size
    uint32_t totsec;      //song total time
    uint32_t cursec;      //song cur time
    uint32_t bitrate;     
    uint32_t samplerate;  
    uint16_t bps;         //suck as 16bit, 24bit, 32bit
    uint32_t datastart;   //offset in file
} WavCtrl;

extern WavCtrl wavCtrl;

uint8_t wav_DecodeInit(uint8_t *fname, WavCtrl *wavx);

#endif
