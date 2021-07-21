#ifndef WS2812B_H
#define WS2812B_H

#include <stdint.h>

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

#define NOTE 0x431560
#define HOLD 0x5CCD5C
#define JUDGE 0x234A5A

typedef struct
{
    uint8_t index;
    uint32_t color;
} LEDProperty;

extern int fall_flag;

void WS2812B_Init(void);
void WS2812B_SetLED(uint8_t index, uint32_t color);
void WS2812B_SendData();
void WS2812B_RefreshData();
void WS2812B_FallALine(LEDProperty *ledLine);
void WS2812B_DataCopy(uint8_t dst, uint8_t src);
void WS2812B_Test();

#endif
