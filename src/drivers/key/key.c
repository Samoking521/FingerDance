#include "key.h"
#include "../../config/default/peripheral/gpio/plib_gpio.h"
#include "../../config/default/peripheral/coretimer/plib_coretimer.h"
#include "../../config/default/peripheral/tmr/plib_tmr5.h"
#include <stdio.h>

#define KEY_SCAN_DELAY 10
#define KEY_SCAN_DELAY_US 1000

KEY_CTRL keyCtrl;

static void _KEY_TIM5_CallbackHandler(uint32_t status, uintptr_t context);

void key_Init()
{
    TMR5_CallbackRegister(_KEY_TIM5_CallbackHandler, (uintptr_t) NULL);
    
    keyCtrl.key1 = 0;
    keyCtrl.key2 = 0;
    keyCtrl.key3 = 0;
    keyCtrl.key4 = 0;
    keyCtrl.callback_fn = NULL;
}

void key_Start()
{
    TMR5_Start();
}

void key_Stop()
{
    TMR5_Stop();
}

void key_CallbackRegister(TMR_CALLBACK callback_fn)
{
    keyCtrl.callback_fn = callback_fn;
}

void key_scan()
{
    if (GPIO_PinRead(GPIO_PIN_RC2) == 0)
    {
        CORETIMER_DelayMs(KEY_SCAN_DELAY);
        if (GPIO_PinRead(GPIO_PIN_RC2) == 0)
        {
            keyCtrl.key1 = 1;
        }
    }
    else if (GPIO_PinRead(GPIO_PIN_RC2) == 1)
    {
        CORETIMER_DelayMs(KEY_SCAN_DELAY);
        if (GPIO_PinRead(GPIO_PIN_RC2) == 1)
        {
            keyCtrl.key1 = 0;
        }
    }
    if (GPIO_PinRead(GPIO_PIN_RC3) == 0)
    {
        CORETIMER_DelayMs(KEY_SCAN_DELAY);
        if (GPIO_PinRead(GPIO_PIN_RC3) == 0)
        {
            keyCtrl.key2 = 1;
        }
    }
    else if (GPIO_PinRead(GPIO_PIN_RC3) == 1)
    {
        CORETIMER_DelayMs(KEY_SCAN_DELAY);
        if (GPIO_PinRead(GPIO_PIN_RC3) == 1)
        {
            keyCtrl.key2 = 0;
        }
    }
    if (GPIO_PinRead(GPIO_PIN_RC4) == 0)
    {
        CORETIMER_DelayMs(KEY_SCAN_DELAY);
        if (GPIO_PinRead(GPIO_PIN_RC4) == 0)
        {
            keyCtrl.key3 = 1;
        }
    }
    else if (GPIO_PinRead(GPIO_PIN_RC4) == 1)
    {
        CORETIMER_DelayMs(KEY_SCAN_DELAY);
        if (GPIO_PinRead(GPIO_PIN_RC4) == 1)
        {
            keyCtrl.key3 = 0;
        }
    }
    if (GPIO_PinRead(GPIO_PIN_RC5) == 0)
    {
        CORETIMER_DelayMs(KEY_SCAN_DELAY);
        if (GPIO_PinRead(GPIO_PIN_RC5) == 0)
        {
            keyCtrl.key4 = 1;
        }
    }
    else if (GPIO_PinRead(GPIO_PIN_RC5) == 1)
    {
        CORETIMER_DelayMs(KEY_SCAN_DELAY);
        if (GPIO_PinRead(GPIO_PIN_RC5) == 1)
        {
            keyCtrl.key4 = 0;
        }
    }
}

void key_scan_ex()
{
    if (GPIO_PinRead(GPIO_PIN_RC2) == 0)
    {
        keyCtrl.key1 = 1;
    }
    else if (GPIO_PinRead(GPIO_PIN_RC2) == 1)
    {
        keyCtrl.key1 = 0;
    }
    if (GPIO_PinRead(GPIO_PIN_RC3) == 0)
    {
        keyCtrl.key2 = 1;
    }
    else if (GPIO_PinRead(GPIO_PIN_RC3) == 1)
    {
        keyCtrl.key2 = 0;
    }
    if (GPIO_PinRead(GPIO_PIN_RC4) == 0)
    {
        keyCtrl.key3 = 1;
    }
    else if (GPIO_PinRead(GPIO_PIN_RC4) == 1)
    {
        keyCtrl.key3 = 0;
    }
    if (GPIO_PinRead(GPIO_PIN_RC5) == 0)
    {
        keyCtrl.key4 = 1;
    }
    else if (GPIO_PinRead(GPIO_PIN_RC5) == 1)
    {
        keyCtrl.key4 = 0;
    }
}

void key_Test()
{
    uint8_t key1_down = 0;
    uint8_t key2_down = 0;
    uint8_t key3_down = 0;
    uint8_t key4_down = 0;

    while (1)
    {
        if (GPIO_PinRead(GPIO_PIN_RC2) == 0 && key1_down == 0)
        {
            CORETIMER_DelayMs(10);
            if (GPIO_PinRead(GPIO_PIN_RC2) == 0)
            {
                key1_down = 1;
                printf("SW1 is down!\n");
            }
        }
        else if (GPIO_PinRead(GPIO_PIN_RC2) == 1 && key1_down == 1)
        {
            CORETIMER_DelayMs(10);
            if (GPIO_PinRead(GPIO_PIN_RC2) == 1)
            {
                key1_down = 0;
                printf("SW1 is up!\n");
            }
        }
        if (GPIO_PinRead(GPIO_PIN_RC3) == 0 && key2_down == 0)
        {
            CORETIMER_DelayMs(10);
            if (GPIO_PinRead(GPIO_PIN_RC3) == 0)
            {
                key2_down = 1;
                printf("SW2 is down!\n");
            }

        }
        else if (GPIO_PinRead(GPIO_PIN_RC3) == 1 && key2_down == 1)
        {
            CORETIMER_DelayMs(10);
            if (GPIO_PinRead(GPIO_PIN_RC3) == 1)
            {
                key2_down = 0;
                printf("SW2 is up!\n");
            }
        }
        if (GPIO_PinRead(GPIO_PIN_RC4) == 0 && key3_down == 0)
        {
            CORETIMER_DelayMs(10);
            if (GPIO_PinRead(GPIO_PIN_RC4) == 0)
            {
                key3_down = 1;
                printf("SW3 is down!\n");
            }
        }
        else if (GPIO_PinRead(GPIO_PIN_RC4) == 1 && key3_down == 1)
        {
            CORETIMER_DelayMs(10);
            if (GPIO_PinRead(GPIO_PIN_RC4) == 1)
            {
                key3_down = 0;
                printf("SW3 is up!\n");
            }
        }
        if (GPIO_PinRead(GPIO_PIN_RC5) == 0 && key4_down == 0)
        {
            CORETIMER_DelayMs(10);
            if (GPIO_PinRead(GPIO_PIN_RC5) == 0)
            {
                key4_down = 1;
                printf("SW4 is down!\n");
            }
        }
        else if (GPIO_PinRead(GPIO_PIN_RC5) == 1 && key4_down == 1)
        {
            CORETIMER_DelayMs(10);
            if (GPIO_PinRead(GPIO_PIN_RC5) == 1)
            {
                key4_down = 0;
                printf("SW4 is up!\n");
            }
        }
    }
}

void _KEY_TIM5_CallbackHandler(uint32_t status, uintptr_t context)
{
    key_scan_ex();
    if(keyCtrl.callback_fn != NULL)
        keyCtrl.callback_fn();
}