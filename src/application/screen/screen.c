#include "screen.h"
#include "../../drivers/OLED/OLED.h"
#include <stdio.h>

extern const unsigned char F6x8[][6];

/**
 * @brief  show whole game background text
 * @param  
 * @retval None
 */
void Screen_Init()
{
    OLED_Init();

    uint8_t ch[128];
    for (int i = 0; i < 128; i++)
        ch[i] = 0x01;
    OLED_Show(0, 0, ch, 128);

    for (int i = 0; i < 128; i++)
        ch[i] = 0x80;
    OLED_Show(0, 56, ch, 128);
}

/**
 * @brief  show pre mode background text
 * @param  
 * @retval None
 */
void Screen_LoadPreMode()
{
    OLED_Fill(0x00);
    uint8_t ch[128];
    for (int i = 0; i < 128; i++)
        ch[i] = 0x01;
    OLED_Show(0, 0, ch, 128);

    for (int i = 0; i < 128; i++)
        ch[i] = 0x80;
    OLED_Show(0, 56, ch, 128);

    Screen_ShowMusicText();
}

/**
 * @brief  show music file
 * @param  
 * @retval None
 */
void Screen_ShowFileList(MusicInfo *musicInfo, int fileIndexStart, int fileIndexStop, int fileNum)
{
    uint8_t startline[] = {3, 13, 23, 33, 43, 53};
    int i = fileIndexStart;
    int j = 0;
    while (1)
    {
        Screen_ShowStr(4, startline[j], musicInfo[i].filename);
        i = (i + 1) % fileNum;
        j += 1;
        if (i == fileIndexStop)
        {
            Screen_ShowStr(4, startline[j], musicInfo[i].filename);
            break;
        }
    }
}

/**
 * @brief  show absolute cursor
 * @param  last: 0-5
 * @param  cur: 0-5
 * @retval None
 */
void Screen_ShowCursor(uint8_t last, uint8_t cur)
{
    uint8_t startline[] = {3, 13, 23, 33, 43, 53};
    uint8_t lastch[] = {0x00, 0x00};
    OLED_Show(1, startline[last], lastch, 2);
    uint8_t curch[] = {0x3C, 0x18};
    OLED_Show(1, startline[cur], curch, 2);
}

/**
 * @brief  show music text
 * @param  bpm
 * @param  min:sec
 * @retval None
 */
void Screen_ShowMusicText()
{
    uint8_t startline[] = {10, 22, 34, 46};
    uint8_t * str[] = {
        (uint8_t *) "bpm: ",
        (uint8_t *) "len: ",
    };
    Screen_ShowStr(80, startline[0], str[0]);
    Screen_ShowStr(80, startline[2], str[1]);
}

/**
 * @brief  show music bpm and length
 * @param  bpm
 * @param  min:sec
 * @retval None
 */
void Screen_ShowMusicInfo(uint8_t bpm, uint8_t min, uint8_t sec)
{
    uint8_t startline[] = {10, 22, 34, 46};
    //bpm
    uint8_t strBpm[4] = {0x00, 0x00, 0x00, 0x00};
    uint8_t h, m, n;
    h = bpm / 100;
    m = (bpm - h * 100) / 10;
    n = bpm % 10;
    Screen_ShowStr(86, startline[1], strBpm);
    if (bpm == 0)
    {
        strBpm[0] = '-';
        strBpm[1] = '-';
        strBpm[2] = '-';
        strBpm[3] = '\0';
        Screen_ShowStr(86, startline[1], strBpm);
    }
    else if (h == 0)
    {
        strBpm[0] = '0' + m;
        strBpm[1] = '0' + n;
        strBpm[2] = '\0';
        Screen_ShowStr(92, startline[1], strBpm);
    }
    else
    {
        strBpm[0] = '0' + h;
        strBpm[1] = '0' + m;
        strBpm[2] = '0' + n;
        strBpm[3] = '\0';
        Screen_ShowStr(86, startline[1], strBpm);
    }
    //len
    uint8_t strLen[5] = {0x00, 0x00, 0x00, 0x00, 0x00};
    m = sec / 10;
    n = sec % 10;
    Screen_ShowStr(86, startline[3], strLen);
    strLen[0] = '0' + min;
    strLen[1] = ':';
    strLen[2] = '0' + m;
    strLen[3] = '0' + n;
    strLen[4] = '\0';
    Screen_ShowStr(86, startline[3], strLen);
}

/**
 * @brief  cursor down a line
 * @param  
 * @retval None
 */
void Screen_LineDown(MusicInfo *musicInfo, int fileNum, int fileIndex)
{
    uint8_t startline[] = {3, 13, 23, 33, 43, 53};
    uint8_t ch[72] = {0};
    for (int i = 0; i < 72; i++)
        ch[i] = 0x00;
    for (int i = 0; i < 6; i++)
        OLED_Show(4, startline[i], ch, 72);
    Screen_ShowFileList(musicInfo, (fileIndex - 4 + fileNum) % fileNum, (fileIndex + 1) % fileNum, fileNum);
}

/**
 * @brief  cursor up a line
 * @param  
 * @retval None
 */
void Screen_LineUp(MusicInfo *musicInfo, int fileNum, int fileIndex)
{
    uint8_t startline[] = {3, 13, 23, 33, 43, 53};
    uint8_t ch[72] = {0};
    for (int i = 0; i < 72; i++)
        ch[i] = 0x00;
    for (int i = 0; i < 6; i++)
        OLED_Show(4, startline[i], ch, 72);
    Screen_ShowFileList(musicInfo, (fileIndex - 1 + fileNum) % fileNum, (fileIndex + 4) % fileNum, fileNum);
}

void Screen_LoadPreDialog()
{
    //clear dialog area
    for (int i = 2; i <= 5; i++)
        OLED_FillArea(30, 98, i, 0x00);
    //draw dialog border
    uint8_t ch[128];
    for (int i = 0; i < 66; i++)
        ch[i] = 0x02;
    ch[0] = 0xFE;
    ch[65] = 0xFE;
    OLED_Show(31, 16, ch, 66);
    for (int i = 0; i < 66; i++)
        ch[i] = 0x00;
    ch[0] = 0xFF;
    ch[65] = 0xFF;
    OLED_Show(31, 24, ch, 66);
    OLED_Show(31, 32, ch, 66);
    for (int i = 0; i < 66; i++)
        ch[i] = 0x40;
    ch[0] = 0x7F;
    ch[65] = 0x7F;
    OLED_Show(31, 38, ch, 66);
    //draw dialog test
    uint8_t startline[] = {22, 32};
    uint8_t * str[] = {
        (uint8_t *) "play mode",
        (uint8_t *) "game mode",
    };
    Screen_ShowStr(38, startline[0], str[0]);
    Screen_ShowStr(38, startline[1], str[1]);
}

void Screen_ShowDlgCursor(uint8_t last, uint8_t cur)
{
    uint8_t startline[] = {22, 32};
    uint8_t lastch[] = {0x00, 0x00};
    OLED_Show(34, startline[last], lastch, 2);
    uint8_t curch[] = {0x3C, 0x18};
    OLED_Show(34, startline[cur], curch, 2);
}

void Screen_LoadPlayMode()
{
    OLED_Fill(0x00);
    uint8_t ch[128];
    for (int i = 0; i < 128; i++)
        ch[i] = 0x01;
    OLED_Show(0, 0, ch, 128);

    for (int i = 0; i < 128; i++)
        ch[i] = 0x80;
    OLED_Show(0, 56, ch, 128);

    Screen_ShowPlayText();
}

void Screen_ShowPlayText()
{
    uint8_t startline[] = {22, 34};
    uint8_t * str[] = {
        (uint8_t *) "name:",
        (uint8_t *) "len:     /",
    };
    Screen_ShowStr(12, startline[0], str[0]);
    Screen_ShowStr(18, startline[1], str[1]);
}

void Screen_ShowPlayMusicName(uint8_t *fname)
{
    Screen_ShowStr(46, 22, fname);
}

void Screen_ShowPlayMusicTotLen(uint8_t tol_min, uint8_t tol_sec)
{
    uint8_t strLen[5] = {0};
    uint8_t m, n;
    m = tol_sec / 10;
    n = tol_sec % 10;
    Screen_ShowStr(80, 34, strLen);
    strLen[0] = '0' + tol_min;
    strLen[1] = ':';
    strLen[2] = '0' + m;
    strLen[3] = '0' + n;
    strLen[4] = '\0';
    Screen_ShowStr(80, 34, strLen);
}

void Screen_ShowPlayMusicCurLen(uint8_t cur_min, uint8_t cur_sec)
{
    uint8_t strLen[5] = {0};
    uint8_t m, n;
    m = cur_sec / 10;
    n = cur_sec % 10;
    Screen_ShowStr(46, 34, strLen);
    strLen[0] = '0' + cur_min;
    strLen[1] = ':';
    strLen[2] = '0' + m;
    strLen[3] = '0' + n;
    strLen[4] = '\0';
    Screen_ShowStr(46, 34, strLen);
}

/**
 * @brief  show game mode background text
 * @param  
 * @retval None
 */
void Screen_LoadGameMode()
{
    OLED_Fill(0x00);
    uint8_t ch[128];
    for (int i = 0; i < 128; i++)
        ch[i] = 0x01;
    OLED_Show(0, 0, ch, 128);

    for (int i = 0; i < 128; i++)
        ch[i] = 0x80;
    OLED_Show(0, 56, ch, 128);

    Screen_ShowGameText();
}

void Screen_ShowGameText()
{
    uint8_t startline[] = {10, 22, 34, 46};
    uint8_t * str[] = {
        (uint8_t *) "name:",
        (uint8_t *) "len:     /",
        (uint8_t *) "score:",
        (uint8_t *) "combo: x"
    };
    Screen_ShowStr(12, startline[0], str[0]);
    Screen_ShowStr(18, startline[1], str[1]);
    Screen_ShowStr(6, startline[2], str[2]);
    Screen_ShowStr(6, startline[3], str[3]);
}

void Screen_ShowGameMusicName(uint8_t *fname)
{
    Screen_ShowStr(46, 10, fname);
}

void Screen_ShowGameMusicTotLen(uint8_t tol_min, uint8_t tol_sec)
{
    uint8_t strLen[5] = {0};
    uint8_t m, n;
    m = tol_sec / 10;
    n = tol_sec % 10;
    Screen_ShowStr(80, 22, strLen);
    strLen[0] = '0' + tol_min;
    strLen[1] = ':';
    strLen[2] = '0' + m;
    strLen[3] = '0' + n;
    strLen[4] = '\0';
    Screen_ShowStr(80, 22, strLen);
}

void Screen_ShowGameMusicCurLen(uint8_t cur_min, uint8_t cur_sec)
{
    uint8_t strLen[5] = {0};
    uint8_t m, n;
    m = cur_sec / 10;
    n = cur_sec % 10;
    Screen_ShowStr(46, 22, strLen);
    strLen[0] = '0' + cur_min;
    strLen[1] = ':';
    strLen[2] = '0' + m;
    strLen[3] = '0' + n;
    strLen[4] = '\0';
    Screen_ShowStr(46, 22, strLen);
}

void Screen_ShowGameMusicScore(uint32_t score)
{
    uint8_t m, n, p, q;
    m = score / 1000;
    score = score % 1000;
    n = score / 100;
    score = score % 100;
    p = score / 10;
    q = score % 10;
    uint8_t strLen[5] = {0};
    Screen_ShowStr(58, 34, strLen);
    if (m == 0)
        strLen[0] = ' ';
    else
        strLen[0] = '0' + m;
    if (n == 0)
        strLen[1] = ' ';
    else
        strLen[1] = '0' + n;
    if (p == 0)
        strLen[2] = ' ';
    else
        strLen[2] = '0' + p;
    strLen[3] = '0' + q;
    strLen[4] = '\0';
    Screen_ShowStr(58, 34, strLen);
}

void Screen_ShowGameMusicCombo(uint32_t combo)
{
    uint8_t m, n, p, q;
    m = combo / 1000;
    combo = combo % 1000;
    n = combo / 100;
    combo = combo % 100;
    p = combo / 10;
    q = combo % 10;
    uint8_t strLen[5] = {0};
    Screen_ShowStr(58, 46, strLen);
    if (m == 0)
        strLen[0] = ' ';
    else
        strLen[0] = '0' + m;
    if (n == 0)
        strLen[1] = ' ';
    else
        strLen[1] = '0' + n;
    if (q == 0)
        strLen[2] = ' ';
    else
        strLen[2] = '0' + q;
    strLen[3] = '0' + p;
    strLen[4] = '\0';
    Screen_ShowStr(58, 46, strLen);
}

void Screen_ShowStr(int x, int y, uint8_t str[])
{
    int i = 0;
    while (str[i] != '\0')
    {
        int c = str[i] - 32;
        OLED_Show(x + 6 * i, y, (uint8_t *) F6x8[c], 6);
        i++;
    }
}

void Screen_ClearArea(int x, int y, int len)
{
    uint8_t ch[128] = {0};
    OLED_Show(x, y, ch, len);
}

void Screen_Test()
{
    /*
    uint8_t *filestr[] = {
        (uint8_t *)"apple",
        (uint8_t *)"boy",
        (uint8_t *)"cat",
        (uint8_t *)"dog",
        (uint8_t *)"early",
        (uint8_t *)"flower"};
    uint8_t startline[] = {3, 13, 23, 33, 43, 53};
    for (int i = 0; i < 6; i++)
        OLED_ShowStrEx(4, startline[i], filestr[i]);

    uint8_t title[] = "FingerDance";
    //OLED_ShowStrEx(4, 10, title, 1);

    uint8_t *keyTips[] = {
        (uint8_t *)"bpm:",
        (uint8_t *)"len:"};
    uint8_t keyline[] = {30, 40};
    for (int i = 0; i < 2; i++)
        OLED_ShowStrEx(82, keyline[i], keyTips[i]);

    OLED_FillArea(79, 80, 1, 0xF0);
    for (int i = 2; i < 6; i++)
        OLED_FillArea(79, 80, i, 0xFF);
    OLED_FillArea(79, 80, 6, 0x0F);*/
}