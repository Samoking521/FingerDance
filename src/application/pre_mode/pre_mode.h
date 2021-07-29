#ifndef PRE_MODE_H
#define PRE_MODE_H

#include <stdint.h>

typedef struct {
    uint8_t filename[20];
    uint8_t bpm;
    uint8_t len_min;
    uint8_t len_sec;
} MusicInfo;

typedef struct {
    uint8_t musicFileNum;
    uint8_t screenFileNum;
    uint8_t curIndex;
    uint8_t fileIndex;
    uint8_t firstlineIndex;
} CursorCtrl;

typedef struct {
    uint8_t curIndex;
} ModeCursorCtrl;

extern MusicInfo musicInfo[10];
extern CursorCtrl cursorCtrl;

void pre_main();
void pre_screen();
void pre_dialog();
void pre_test();

#endif
