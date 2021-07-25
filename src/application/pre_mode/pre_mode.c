#include "pre_mode.h"
#include "../screen/screen.h"
#include "../sdfile/sdfile.h"
#include "../../config/default/peripheral/coretimer/plib_coretimer.h"
#include <stdio.h>

MusicInfo musicInfo[10];
int musicFileNum = 0;
CursorCtrl cursorCtrl;

void pre_main()
{
    sdFile_Init();
    sdFile_GetFileList(musicInfo, &musicFileNum);
    cursorCtrl.musicFileNum = musicFileNum;
    cursorCtrl.screenFileNum = musicFileNum > 6 ? 6 : musicFileNum;
    cursorCtrl.curIndex = 0;
    cursorCtrl.fileIndex = 0;
    sdFile_GetWAVFiles(musicInfo, musicFileNum);
    sdFile_GetMCFiles(musicInfo, musicFileNum);
    for (int i = 0; i < musicFileNum; i++)
        printf("%s:%d %d:%02d\n", musicInfo[i].filename, musicInfo[i].bpm, musicInfo[i].len_min, musicInfo[i].len_sec);

    Screen_Init();

    pre_screen();
    
    int i = cursorCtrl.curIndex;
    int j = cursorCtrl.fileIndex;
    int down = 1;
    while (1)
    {
        if (down)
        {
            if (i == 5)
            {
                Screen_ShowCursor(5, 5);
                Screen_LineDown(musicInfo, cursorCtrl.musicFileNum, j);
                j = (j + 1) % cursorCtrl.musicFileNum;
            }
            else
            {
                Screen_ShowCursor(i % cursorCtrl.screenFileNum, (i + 1) % cursorCtrl.screenFileNum);
                i = (i + 1) % cursorCtrl.screenFileNum;
                j = (j + 1) % cursorCtrl.musicFileNum;
            }
        }
        else
        {
            if (i == 0)
            {
                Screen_ShowCursor(0, 0);
                Screen_LineUp(musicInfo, cursorCtrl.musicFileNum, j);
                j = (j - 1 + cursorCtrl.musicFileNum) % cursorCtrl.musicFileNum;
            }
            else
            {
                Screen_ShowCursor(i % cursorCtrl.screenFileNum, (i - 1 + cursorCtrl.screenFileNum) % cursorCtrl.screenFileNum);
                i = (i - 1 + cursorCtrl.screenFileNum) % cursorCtrl.screenFileNum;
                j = (j - 1 + cursorCtrl.musicFileNum) % cursorCtrl.musicFileNum;
            }
        }

        Screen_ShowMusicInfo(musicInfo[j].bpm, musicInfo[j].len_min, musicInfo[j].len_sec);
        cursorCtrl.curIndex = i;
        cursorCtrl.fileIndex = j;
        CORETIMER_DelayMs(800);
    }
}

void pre_screen()
{
    Screen_LoadPreMode();
    Screen_ShowFileList(musicInfo, 0, cursorCtrl.screenFileNum - 1, musicFileNum);
    Screen_ShowCursor(0, 0);
    Screen_ShowMusicInfo(musicInfo[0].bpm, musicInfo[0].len_min, musicInfo[0].len_sec);
    //Screen_ShowMusicInfo(123, 1, 30);
}