#include "play_mode.h"
#include "../music/music.h"
#include "../music/wav.h"
#include "../../config/default/peripheral/coretimer/plib_coretimer.h"
#include "../../config/default/peripheral/tmr/plib_tmr4.h"
#include "../../drivers/ws2812b/ws2812b.h"
#include "../../drivers/ws2812b/ws2812b_data.h"
#include "../../config/default/device.h"
#include <stdio.h>
#include <string.h>

//#define PLAY_DEBUG
#define MUSIC_BUFSIZE 8192

FATFS fs; //define here temporarily
extern AudioDevice audioDev;
extern WavCtrl wavCtrl;

void play_main()
{
    f_mount(&fs, "0:", 1);

    uint8_t song[] = "0:song-bg.wav";

    WS2812B_Init();
    LEDProperty LEDLine[4] = {0};
    int cur_line = 0;
    int start_adjust = 12;

    music_init();
    //music_play(song);
    
    TMR4_Start();
    
    while (1)
    {
        if (audioDev.wavWhichBuf == 1)
        {
#ifdef PLAY_DEBUG
            printf("Buf2 start fill: %d\n", CORETIMER_CounterGet());
#endif
            music_buffer_fill(audioDev.i2sBuf1, MUSIC_BUFSIZE, wavCtrl.bps);
#ifdef PLAY_DEBUG
            printf("Buf2 end fill: %d\n", CORETIMER_CounterGet());
#endif
            audioDev.wavWhichBuf = 0;
        }
        else if (audioDev.wavWhichBuf == 2)
        {
#ifdef PLAY_DEBUG
            printf("Buf1 start fill: %d\n", CORETIMER_CounterGet());
#endif
            music_buffer_fill(audioDev.i2sBuf2, MUSIC_BUFSIZE, wavCtrl.bps);
#ifdef PLAY_DEBUG
            printf("Buf1 end fill: %d\n", CORETIMER_CounterGet());
#endif
            audioDev.wavWhichBuf = 0;
        }
        else if (fall_flag == 1)
        {
            if(start_adjust > 0)
                start_adjust --;
            else if(start_adjust == 0)
            {
                music_play(song);
                start_adjust = -1;
            }
            if (cur_line < 712)
            {
                memset(LEDLine, 0, sizeof (LEDLine));
                for (int j = 0; j < 4; j++)
                {
                    if (led_data[cur_line][j] == 1)
                        LEDLine[j].color = NOTE;
                    else if (led_data[cur_line][j] == 2)
                        LEDLine[j].color = HOLD;
                }
                WS2812B_FallALine(LEDLine);
                cur_line += 1;
            }
            fall_flag = 0;
        }
    }
}