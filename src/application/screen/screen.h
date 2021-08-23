#ifndef SCREEN_H
#define SCREEN_H

#include <stdint.h>
#include "../pre_mode/pre_mode.h"

void Screen_Init();

void Screen_LoadPreMode();                                                                          //显示pre mode的UI
void Screen_ShowFileList(MusicInfo *musicInfo, int fileIndexStart, int fileIndexStop, int fileNum); //显示文件列表
void Screen_ShowCursor(uint8_t last, uint8_t cur);                                                  //显示光标
void Screen_ShowMusicText();                                                                        //显示音乐信息提示文本
void Screen_ShowMusicInfo(uint8_t bpm, uint8_t min, uint8_t sec);                                   //显示音乐bpm与时长
void Screen_LineDown(MusicInfo *musicInfo, int fileNum, int fileIndex);                             //文件列表向下滚动一行
void Screen_LineUp(MusicInfo *musicInfo, int fileNum, int fileIndex);                               //文件列表向上滚动一行
void Screen_LoadPreDialog();                                                                        //显示游戏模式选择的对话框
void Screen_ShowDlgCursor(uint8_t last, uint8_t cur);                                               //显示对话框光标

void Screen_LoadPlayMode();                                        //显示play mode的UI
void Screen_ShowPlayText();                                        //显示play mode的提示文本
void Screen_ShowPlayMusicName(uint8_t *fname);                     //显示音乐名称
void Screen_ShowPlayMusicTotLen(uint8_t tol_min, uint8_t tol_sec); //显示音乐总时长
void Screen_ShowPlayMusicCurLen(uint8_t cur_min, uint8_t cur_sec); //显示音乐当前进度

void Screen_LoadGameMode();                                        //显示game mode的UI
void Screen_ShowGameText();                                        //显示game mode的提示文本
void Screen_ShowGameMusicName(uint8_t *fname);                     //显示音乐名称
void Screen_ShowGameMusicTotLen(uint8_t tol_min, uint8_t tol_sec); //显示音乐总时长
void Screen_ShowGameMusicCurLen(uint8_t cur_min, uint8_t cur_sec); //显示音乐当前进度
void Screen_ShowGameMusicScore(uint32_t score);                    //显示游戏分数
void Screen_ShowGameMusicCombo(uint32_t combo);                    //显示按键连续击中次数

void Screen_ShowStr(int x, int y, uint8_t str[]);
void Screen_ClearArea(int x, int y, int len);
void Screen_Test();

#endif
