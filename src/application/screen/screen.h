#ifndef SCREEN_H
#define SCREEN_H

#include <stdint.h>
#include "../pre_mode/pre_mode.h"

void Screen_Init();

void Screen_LoadPreMode();
void Screen_ShowFileList(MusicInfo *musicInfo, int fileIndexStart, int fileIndexStop, int fileNum);
void Screen_ShowCursor(uint8_t last, uint8_t cur);
void Screen_ShowMusicText();
void Screen_ShowMusicInfo(uint8_t bpm, uint8_t min, uint8_t sec);
void Screen_LineDown(MusicInfo *musicInfo, int fileNum, int fileIndex);
void Screen_LineUp(MusicInfo *musicInfo, int fileNum, int fileIndex);

void Screen_LoadPlayMode();
void Screen_LoadGameMode();

void Screen_ShowStr(int x, int y, uint8_t str[]);
void Screen_ClearArea(int x, int y, int len);
void Screen_Test();

#endif
