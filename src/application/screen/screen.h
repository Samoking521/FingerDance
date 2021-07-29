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
void Screen_LoadPreDialog();
void Screen_ShowDlgCursor(uint8_t last, uint8_t cur);

void Screen_LoadPlayMode();
void Screen_ShowPlayText();
void Screen_ShowPlayMusicName(uint8_t *fname);
void Screen_ShowPlayMusicTotLen(uint8_t tol_min, uint8_t tol_sec);
void Screen_ShowPlayMusicCurLen(uint8_t cur_min, uint8_t cur_sec);

void Screen_LoadGameMode();
void Screen_ShowGameText();
void Screen_ShowGameMusicName(uint8_t *fname);
void Screen_ShowGameMusicTotLen(uint8_t tol_min, uint8_t tol_sec);
void Screen_ShowGameMusicCurLen(uint8_t cur_min, uint8_t cur_sec);
void Screen_ShowGameMusicScore(uint32_t score);
void Screen_ShowGameMusicCombo(uint32_t combo);

void Screen_ShowStr(int x, int y, uint8_t str[]);
void Screen_ClearArea(int x, int y, int len);
void Screen_Test();

#endif
