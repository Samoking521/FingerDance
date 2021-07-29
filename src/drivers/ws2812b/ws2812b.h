#ifndef WS2812B_H
#define WS2812B_H

#include <stdint.h>
#include <stdbool.h>

// led colour, GRB format
#define BLACK 0x000000
#define GREEN 0x800000
#define SILVER 0xC0C0C0
#define LIME 0xFF0000
#define GRAY 0x808080
#define ALIVE 0x808000
#define WHITE 0xFFFFFF
#define YELLOW 0xFFFF00
#define MAROON 0x008000
#define NAVY 0x000080
#define RED 0x00FF00
#define BLUE 0x0000FF
#define PURPLE 0x008080
#define TEAL 0x800080
#define FUCHSIA 0x00FFFF
#define AQUA 0xFF00FF

//#define NOTE 0x431560
#define NOTE 0x03050F
//#define HOLD 0x5CCD5C
#define HOLD  0x030A0A
//#define JUDGE 0x234A5A
#define JUDGE 0x0C0D0C

typedef struct
{
    uint8_t index;
    uint32_t color;
} LEDProperty;

void WS2812B_Init(void);
void WS2812B_SetLED(uint8_t index, uint32_t color);
void WS2812B_SendData();
void WS2812B_RefreshData();
void WS2812B_FallALine(LEDProperty *ledLine);
void WS2812B_DataCopy(uint8_t dst, uint8_t src);
void WS2812B_SetJudgeLine(LEDProperty *ledLine);
bool WS2812B_Lock();
void WS2812B_UnLock();
void WS2812B_Test();

#endif
