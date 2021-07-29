#include "pre_mode.h"
#include "../screen/screen.h"
#include "../sdfile/sdfile.h"
#include "../../config/default/peripheral/coretimer/plib_coretimer.h"
#include "../../config/default/peripheral/gpio/plib_gpio.h"
#include "../../drivers/key/key.h"
#include "../../main.h"
#include <stdio.h>

MusicInfo musicInfo[10];
int musicFileNum = 0;
CursorCtrl cursorCtrl;
ModeCursorCtrl modeCursorCtrl;

uint8_t key_cursor_up = 0; //SW4
uint8_t key_cursor_down = 0; //SW3
uint8_t key_music_start = 0; //SW2
uint8_t key_game_start = 0; //SW1

uint8_t mode_select = 0;

void pre_main()
{
    sdFile_GetFileList(musicInfo, &musicFileNum);
    cursorCtrl.musicFileNum = musicFileNum;
    cursorCtrl.screenFileNum = musicFileNum > 6 ? 6 : musicFileNum;
    cursorCtrl.curIndex = 0;
    cursorCtrl.fileIndex = 0;
    cursorCtrl.firstlineIndex = 0;
    sdFile_GetWAVFiles(musicInfo, musicFileNum);
    sdFile_GetMCFiles(musicInfo, musicFileNum);
    /*
    for (int i = 0; i < musicFileNum; i++)
        printf("%s:%d %d:%02d\n", musicInfo[i].filename, musicInfo[i].bpm, musicInfo[i].len_min, musicInfo[i].len_sec);*/
    mode_select = 0;

    pre_screen();

    uint8_t i, j;
    while (1)
    {
        key_scan();
        if (keyCtrl.key4 == 1)
        {
            if (key_cursor_up == 0)
            {
                key_cursor_up = 1;

                if (mode_select == 0)
                {
                    i = cursorCtrl.curIndex;
                    j = cursorCtrl.fileIndex;
                    if (i == 0)
                    {
                        Screen_ShowCursor(0, 0);
                        Screen_LineUp(musicInfo, cursorCtrl.musicFileNum, j);
                        cursorCtrl.firstlineIndex = (cursorCtrl.fileIndex - 1 + cursorCtrl.musicFileNum) % cursorCtrl.musicFileNum;
                        j = (j - 1 + cursorCtrl.musicFileNum) % cursorCtrl.musicFileNum;
                    }
                    else
                    {
                        Screen_ShowCursor(i % cursorCtrl.screenFileNum, (i - 1 + cursorCtrl.screenFileNum) % cursorCtrl.screenFileNum);
                        i = (i - 1 + cursorCtrl.screenFileNum) % cursorCtrl.screenFileNum;
                        j = (j - 1 + cursorCtrl.musicFileNum) % cursorCtrl.musicFileNum;
                    }
                    Screen_ShowMusicInfo(musicInfo[j].bpm, musicInfo[j].len_min, musicInfo[j].len_sec);
                    cursorCtrl.curIndex = i;
                    cursorCtrl.fileIndex = j;
                }
                else
                {
                    uint8_t tmp = modeCursorCtrl.curIndex;
                    modeCursorCtrl.curIndex = !modeCursorCtrl.curIndex;
                    Screen_ShowDlgCursor(tmp, modeCursorCtrl.curIndex);
                }
            }
        }
        else
        {
            key_cursor_up = 0;
        }
        if (keyCtrl.key3 == 1)
        {
            if (key_cursor_down == 0)
            {
                key_cursor_down = 1;

                if (mode_select == 0)
                {
                    i = cursorCtrl.curIndex;
                    j = cursorCtrl.fileIndex;
                    if (i == 5)
                    {
                        Screen_ShowCursor(5, 5);
                        Screen_LineDown(musicInfo, cursorCtrl.musicFileNum, j);
                        cursorCtrl.firstlineIndex = (cursorCtrl.fileIndex - 4 + cursorCtrl.musicFileNum) % cursorCtrl.musicFileNum;
                        j = (j + 1) % cursorCtrl.musicFileNum;
                    }
                    else
                    {
                        Screen_ShowCursor(i % cursorCtrl.screenFileNum, (i + 1) % cursorCtrl.screenFileNum);
                        i = (i + 1) % cursorCtrl.screenFileNum;
                        j = (j + 1) % cursorCtrl.musicFileNum;
                    }
                    Screen_ShowMusicInfo(musicInfo[j].bpm, musicInfo[j].len_min, musicInfo[j].len_sec);
                    cursorCtrl.curIndex = i;
                    cursorCtrl.fileIndex = j;
                }
                else
                {
                    uint8_t tmp = modeCursorCtrl.curIndex;
                    modeCursorCtrl.curIndex = !modeCursorCtrl.curIndex;
                    Screen_ShowDlgCursor(tmp, modeCursorCtrl.curIndex);
                }
            }
        }
        else
        {
            key_cursor_down = 0;
        }
        if (keyCtrl.key2 == 1)
        {
            if (key_music_start == 0)
            {
                key_music_start = 1;

                if (mode_select == 1)
                {
                    mode_select = 0;

                    pre_screen();
                }
            }
        }
        else
        {
            key_music_start = 0;
        }
        if (keyCtrl.key1 == 1)
        {
            if (key_game_start == 0)
            {
                key_game_start = 1;

                if (mode_select == 0)
                {
                    mode_select = 1;

                    pre_dialog();
                }
                else
                {
                    if (modeCursorCtrl.curIndex == 0)
                        modeCtrl = play_mode;
                    else
                        modeCtrl = game_mode;
                    break;
                }
            }
        }
        else
        {
            key_game_start = 0;
        }
    }
}

void pre_screen()
{
    Screen_LoadPreMode();
    Screen_ShowFileList(musicInfo, cursorCtrl.firstlineIndex, (cursorCtrl.firstlineIndex + cursorCtrl.screenFileNum - 1) % cursorCtrl.musicFileNum, musicFileNum);
    Screen_ShowCursor(cursorCtrl.curIndex, cursorCtrl.curIndex);
    Screen_ShowMusicInfo(musicInfo[cursorCtrl.fileIndex].bpm, musicInfo[cursorCtrl.fileIndex].len_min, musicInfo[cursorCtrl.fileIndex].len_sec);
}

void pre_dialog()
{
    modeCursorCtrl.curIndex = 0;
    Screen_LoadPreDialog();
    Screen_ShowDlgCursor(modeCursorCtrl.curIndex, modeCursorCtrl.curIndex);
}

void pre_test()
{
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