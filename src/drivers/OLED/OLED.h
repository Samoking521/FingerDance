#ifndef OLED_H
#define OLED_H

#include <stdint.h>

void OLED_Init(void);
void OLED_SetPos(uint8_t x, uint8_t y);
void OLED_Fill(uint8_t fill_Data);
void OLED_FillArea(uint8_t x1, uint8_t x2, uint8_t y, uint8_t data);
void OLED_CLS(void);
void OLED_ON(void);
void OLED_OFF(void);
void OLED_Show(uint8_t x, uint8_t y, uint8_t ch[], uint8_t len);
void OLED_ShowStr(uint8_t x, uint8_t y, uint8_t ch[], uint8_t TextSize);
void OLED_ShowStrEx(uint8_t x, uint8_t y, uint8_t ch[]);
void OLED_ShowCN(uint8_t x, uint8_t y, uint8_t N);
void OLED_Test();
void OLED_DebugBuf();
void OLED_DebugPage(int page);

#endif
