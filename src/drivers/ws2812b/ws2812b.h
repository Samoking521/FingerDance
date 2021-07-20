#ifndef WS2812B_H
#define WS2812B_H

#include <stdint.h>

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
void WS2812B_Test();

#endif
