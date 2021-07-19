#include "../../config/default/device.h"
#include "oled.h"
#include "codetab.h"
#include "../i2c/i2c.h"
#include "peripheral/coretimer/plib_coretimer.h"
#include <stdio.h>

//#define OLED_DEBUG
#define OLED_ADDRESS 0x3C

static uint8_t cmdBuf[4] = {0};
static uint8_t dataBuf[2] = {0};
static uint8_t pageBuf[129] = {0};

static void SendCmd(uint8_t cmd);
static void SendCmds(uint16_t len);
//static void SendData(uint8_t data);
static void SendPage(uint8_t len);

/**
 * @brief  write cmd to OLED
 * @param  cmd
 * @retval None
 */
void SendCmd(uint8_t cmd)
{
    //I2C1_WriteByte(OLED_ADDRESS, cmd);
    cmdBuf[1] = cmd;
    I2C1_WriteBytes(OLED_ADDRESS, cmdBuf, 2);
}

/**
 * @brief  write n cmds to OLED
 * @param  cmd
 *         len <= 3
 * @retval None
 */
void SendCmds(uint16_t len)
{
    //I2C1_WriteByte(OLED_ADDRESS, cmd);
    I2C1_WriteBytes(OLED_ADDRESS, cmdBuf, len + 1);
}

/**
 * @brief  write data to OLED
 * @param  data
 * @retval None
 */
/*
void SendData(uint8_t data)
{
    //I2C1_WriteByte(OLED_ADDRESS, data);
    dataBuf[1] = data;
    I2C1_WriteBytes(OLED_ADDRESS, dataBuf, 2);
}*/

/**
 * @brief  write a page to OLED
 * @param  data
 * @retval None
 */
void SendPage(uint8_t len)
{
    //I2C1_WriteByte(OLED_ADDRESS, data);
    I2C1_WriteBytes(OLED_ADDRESS, pageBuf, len + 1);
}

/**
 * @brief  OLED_Init
 * @param  None
 * @retval None
 */
void OLED_Init(void)
{
#ifdef OLED_DEBUG
    printf("----OLED Init----\n");
#endif
    //Buf init
    cmdBuf[0] = 0x00;
    dataBuf[0] = 0x40;
    pageBuf[0] = 0x40;

    CORETIMER_DelayMs(1000); // delay 1s
    //close display
    SendCmd(0xAE); //display off
    //Set Memory Addressing Mode
    SendCmd(0x20);
    SendCmd(0x10); //00,Horizontal Addressing Mode;01,Vertical Addressing Mode;10,Page Addressing Mode (RESET);11,Invalid
    //Set Page Start Address for Page Addressing Mode,0-7
    SendCmd(0xB0);
    //Set COM Output Scan Direction
    SendCmd(0xC8);

    SendCmd(0x02); //--set low column address
    SendCmd(0x10); //--set high column address
    SendCmd(0x40); //--set start line address
    //--set contrast control register
    SendCmd(0x81);
    SendCmd(0xFF); //brightness 0x00~0xff

    SendCmd(0xA1); //--set segment re-map 0 to 127
    SendCmd(0xA6); //--set normal display
    //--set multiplex ratio(1 to 64)
    SendCmd(0xA8);
    SendCmd(0x3F); //64

    SendCmd(0xA4); //0xa4,Output follows RAM content;0xa5,Output ignores RAM content

    SendCmd(0xD3); //-set display offset
    SendCmd(0x00); //-not offset

    SendCmd(0xD5); //--set display clock divide ratio/oscillator frequency
    SendCmd(0xF0); //--set divide ratio

    SendCmd(0xD9); //--set pre-charge period
    SendCmd(0x22); //

    SendCmd(0xDA); //--set com pins hardware configuration
    SendCmd(0x12);

    SendCmd(0xDB); //--set vcomh
    SendCmd(0x20); //0x20,0.77xVcc
    //open DC-DC
    SendCmd(0x8D); //--set DC-DC enable
    SendCmd(0x14); //
    //open display
    SendCmd(0xAF); //--turn on oled panel
}

/**
 * @brief  set point position
 * @param  x: 0-127
 *	@param	y: 0-7
 * @retval None
 */
void OLED_SetPos(uint8_t x, uint8_t y)
{
    cmdBuf[1] = 0xB0 + y; //set start point
    x += 2;
    cmdBuf[2] = x & 0x0F;
    cmdBuf[3] = ((x & 0xF0) >> 4) | 0x10;
    SendCmds(3);
}

/**
 * @brief  fill whole screen
 * @param  fill_Data
 * @retval None
 */
void OLED_Fill(uint8_t fill_Data)
{
    uint8_t m, n;
    for (m = 0; m < 8; m++)
    {
        cmdBuf[1] = 0xb0 + m; //page0-page1
        cmdBuf[2] = 0x02; //low column start address
        cmdBuf[3] = 0x10; //high column start address
        SendCmds(3);
        for (n = 0; n < 128; n++)
            pageBuf[n + 1] = fill_Data;
        SendPage(128);
    }
}

/**
 * @brief  fill specfic area
 * @param  x1, x2
 *	@param	y: 0-7
 * @retval None
 */
void OLED_FillArea(uint8_t x1, uint8_t x2, uint8_t y, uint8_t data)
{
    OLED_SetPos(x1, y);
    uint8_t x = x2 - x1;
    for (int i = 0; i < x; i++)
        pageBuf[i + 1] = data;
    SendPage(x);
}

/**
 * @brief  clear whole screen
 * @param  None
 * @retval None
 */
void OLED_CLS(void)
{
    OLED_Fill(0x00);
}

/**
 * @brief  wake OLED up
 * @param  None
 * @retval None
 */
void OLED_ON(void)
{
    cmdBuf[1] = 0x8D;
    cmdBuf[2] = 0x14;
    cmdBuf[3] = 0xAF;
    SendCmds(3);
}

/**
 * @brief  make OLED go into sleep mode
 * @param  ?
 * @retval ?
 */
void OLED_OFF(void)
{
    cmdBuf[1] = 0x8D;
    cmdBuf[2] = 0x10;
    cmdBuf[3] = 0xAE;
    SendCmds(3);
}

/**
 * @brief  show ASCII character
 * @param  x, y: (x:0~127, y:0~7)
 *          ch[]: show string 
 *          TextSize: (1:6*8 ; 2:8*16)
 * @retval None
 */
void OLED_ShowStr(uint8_t x, uint8_t y, uint8_t ch[], uint8_t TextSize)
{
    uint8_t c = 0, i = 0, j = 0;
    switch (TextSize)
    {
        case 1:
        {
            while (ch[j] != '\0')
            {
                c = ch[j] - 32;
                if (x > 127)
                {
                    x = 0;
                    y++;
                }
                OLED_SetPos(x, y);
                for (i = 0; i < 6; i++)
                    pageBuf[i + 1] = F6x8[c][i];
                SendPage(6);
                x += 6;
                j++;
            }
        }
            break;
        case 2:
        {
            while (ch[j] != '\0')
            {
                c = ch[j] - 32;
                if (x > 120)
                {
                    x = 0;
                    y++;
                }
                OLED_SetPos(x, y);
                for (i = 0; i < 8; i++)
                    //SendData(F8X16[c * 16 + i]);
                    pageBuf[i + 1] = F8X16[c * 16 + i];
                SendPage(8);
                OLED_SetPos(x, y + 1);
                for (i = 0; i < 8; i++)
                    //SendData(F8X16[c * 16 + i + 8]);
                    pageBuf[i + 1] = F8X16[c * 16 + i + 8];
                SendPage(8);
                x += 8;
                j++;
            }
        }
            break;
    }
}

/**
 * @brief  show Chinese character
 * @param  x,y: (x:0~127, y:0~7)
 *          N: reference in codetab.h
 * @retval ?
 */
void OLED_ShowCN(uint8_t x, uint8_t y, uint8_t N)
{
    uint8_t wm = 0;
    unsigned int adder = 32 * N;
    OLED_SetPos(x, y);
    for (wm = 0; wm < 16; wm++)
    {
        //SendData(F16x16[adder]);
        pageBuf[wm + 1] = F16x16[adder];
        adder += 1;
    }
    SendPage(16);
    OLED_SetPos(x, y + 1);
    for (wm = 0; wm < 16; wm++)
    {
        //SendData(F16x16[adder]);
        pageBuf[wm + 1] = F16x16[adder];
        adder += 1;
    }
    SendPage(16);
}

void OLED_Test()
{
    OLED_Init();
    OLED_Fill(0x00);
    CORETIMER_DelayMs(10);
    
    OLED_FillArea(0, 128, 0, 0xFF);
    
    uint8_t num[] = "0";
    for (int i = 0;;)
    {
        OLED_FillArea(0, 128, 2, 0x00);
        OLED_FillArea(0, 128, 3, 0x00);
        num[0] = '0' + i;
        OLED_ShowStr(50, 2, num, 2);
        CORETIMER_DelayMs(500);
        i = (i + 1) % 10;
    }
}