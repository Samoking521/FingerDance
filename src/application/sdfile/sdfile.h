#ifndef SDFILE_H
#define SDFILE_H

#include "../pre_mode/pre_mode.h"

void sdFile_Init();
void sdFile_GetFileList(MusicInfo *musicInfo, int *len);
void sdFile_GetWAVFiles(MusicInfo *musicInfo, int len);
void sdFile_GetMCFiles(MusicInfo *musicInfo, int len);

#endif
