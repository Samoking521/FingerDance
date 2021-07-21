#include "ws2812b.h"
#include "ws2812b_data.h"
#include "../../config/default/device.h"
#include "../../config/default/peripheral/dmac/plib_dmac.h"
#include "../../config/default/peripheral/coretimer/plib_coretimer.h"
#include "../../config/default/peripheral/tmr/plib_tmr2.h"
#include "../../config/default/peripheral/tmr/plib_tmr4.h"
#include "../../config/default/peripheral/ocmp/plib_ocmp1.h"
#include "peripheral/ocmp/plib_ocmp1.h"
#include <stdio.h>

//#define WS2812B_DEBUG
#define WS2812B_DMA_CHANNEL DMAC_CHANNEL_1
#define WS2812B_DMA_RECV_ADDRESS (uint8_t *) & OC1RS
#define WS2812B_DMA_RECV_SIZE 2

#define WS2812B_ZERO 8
#define WS2812B_ONE 16

#define LED_NUM 40



static uint16_t sendData[24 * LED_NUM + 40];
static uint32_t colorBuf[41];
int flag = 0;

int fall_flag = 0;

static void _WS2812B_DMA_CallbackHandler(DMAC_TRANSFER_EVENT status, uintptr_t contextHandler);
static void _WS2812B_TIM4_CallbackHandler(uint32_t status, uintptr_t context);

void WS2812B_Init(void)
{
    int i = 0;
    for (; i < 24 * LED_NUM; i++)
        sendData[i] = WS2812B_ZERO;
    for (; i < 24 * LED_NUM + 40; i++)
        sendData[i] = 0;
    memset(colorBuf, 0, sizeof (colorBuf));

    DMAC_ChannelCallbackRegister(WS2812B_DMA_CHANNEL, _WS2812B_DMA_CallbackHandler, (uintptr_t) NULL);
    TMR4_CallbackRegister(_WS2812B_TIM4_CallbackHandler, (uintptr_t) NULL);

    TMR2_Start();
    OCMP1_Enable();

    WS2812B_SetLED(10, JUDGE);
    WS2812B_SetLED(20, JUDGE);
    WS2812B_SetLED(30, JUDGE);
    WS2812B_SetLED(40, JUDGE);
    WS2812B_SendData();
}

void WS2812B_SetLED(uint8_t index, uint32_t color)
{
#ifdef WS2812B_DEBUG
    printf("LED %02d data: ", index);
#endif
    uint16_t i, j, k;
    i = 24 * (index - 1);
    j = 24 * index;
    k = 23;
    for (; i < j; i++, k--)
    {
        if (color & (1 << k))
            sendData[i] = WS2812B_ONE;
        else
            sendData[i] = WS2812B_ZERO;
#ifdef WS2812B_DEBUG
        printf("%02d ", sendData[i]);
#endif
    }
    //updata LED buffer
    colorBuf[index] = color;
#ifdef WS2812B_DEBUG
    printf("\n");
#endif
}

void WS2812B_SendData()
{
    DMAC_ChannelTransfer(WS2812B_DMA_CHANNEL,
            (const void*) sendData,
            2000,
            WS2812B_DMA_RECV_ADDRESS,
            WS2812B_DMA_RECV_SIZE,
            2);
    while (!flag)
    {
        CORETIMER_DelayUs(50);
    }
    flag = 0;
}

void WS2812B_RefreshData()
{
    int i = 0;
    for (; i < 24 * LED_NUM; i++)
        sendData[i] = WS2812B_ZERO;
}

void WS2812B_FallALine(LEDProperty *ledLine)
{
    //Existing data fall
    int line = 8;
    for (; line >= 1; line--)
    {
        /*
        WS2812B_DataCopy(line + 1, line);
        WS2812B_DataCopy(10 + line + 1, 10 + line);
        WS2812B_DataCopy(20 + line + 1, 20 + line);
        WS2812B_DataCopy(30 + line + 1, 30 + line);*/
        WS2812B_SetLED(line + 1, colorBuf[line]);
        WS2812B_SetLED(10 + line + 1, colorBuf[10 + line]);
        WS2812B_SetLED(20 + line + 1, colorBuf[20 + line]);
        WS2812B_SetLED(30 + line + 1, colorBuf[30 + line]);
    }
    //Set first line data
    WS2812B_SetLED(1, ledLine[0].color);
    WS2812B_SetLED(11, ledLine[1].color);
    WS2812B_SetLED(21, ledLine[2].color);
    WS2812B_SetLED(31, ledLine[3].color);
    WS2812B_SendData();
}

void WS2812B_DataCopy(uint8_t dst, uint8_t src)
{
    uint16_t Dst = 24 * (dst - 1);
    uint16_t Src = 24 * (src - 1);
    memcpy(sendData + Dst, sendData + Src, 24 * sizeof (uint16_t));
}

void WS2812B_Test()
{
    /*//Test LED coloue
    WS2812B_SetLED(1, BLACK);
    WS2812B_SetLED(2, GREEN);
    WS2812B_SetLED(3, SILVER);
    WS2812B_SetLED(4, LIME);
    WS2812B_SetLED(5, GRAY);
    WS2812B_SetLED(6, ALIVE);
    WS2812B_SetLED(7, WHITE);
    WS2812B_SetLED(8, YELLOW);
    WS2812B_SetLED(9, MAROON);
    WS2812B_SetLED(10, NAVY);
    WS2812B_SetLED(11, RED);
    WS2812B_SetLED(12, BLUE);
    WS2812B_SetLED(13, PURPLE);
    WS2812B_SetLED(14, TEAL);
    WS2812B_SetLED(15, FUCHSIA);
    WS2812B_SetLED(16, AQUA);
    WS2812B_SendData();*/
    /*//Test LED flow
    int i = 1;
    while (1)
    {
        WS2812B_RefreshData();
        WS2812B_SetLED(i, NOTE);
        WS2812B_SendData();
        CORETIMER_DelayMs(200);
        i = i % 9 + 1;
    }*/

    WS2812B_SetLED(10, JUDGE);
    WS2812B_SetLED(20, JUDGE);
    WS2812B_SetLED(30, JUDGE);
    WS2812B_SetLED(40, JUDGE);
    WS2812B_SendData();

    LEDProperty LEDLine[4] = {0};
    for (int i = 0; i < 712; i++)
    {
        memset(LEDLine, 0, sizeof (LEDLine));
        for (int j = 0; j < 4; j++)
        {
            if (led_data[i][j] == 1)
                LEDLine[j].color = NOTE;
            else if (led_data[i][j] == 2)
                LEDLine[j].color = HOLD;
        }
        WS2812B_FallALine(LEDLine);
        CORETIMER_DelayMs(115);
    }
}

void _WS2812B_DMA_CallbackHandler(DMAC_TRANSFER_EVENT status, uintptr_t contextHandler)
{
    if (DMAC_TRANSFER_EVENT_COMPLETE == status)
    {
        flag = 1;

    }
}

void _WS2812B_TIM4_CallbackHandler(uint32_t status, uintptr_t context)
{
    fall_flag = 1;
}