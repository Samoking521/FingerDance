/*******************************************************************************
  GPIO PLIB

  Company:
    Microchip Technology Inc.

  File Name:
    plib_gpio.h

  Summary:
    GPIO PLIB Header File

  Description:
    This library provides an interface to control and interact with Parallel
    Input/Output controller (GPIO) module.

*******************************************************************************/

/*******************************************************************************
* Copyright (C) 2019 Microchip Technology Inc. and its subsidiaries.
*
* Subject to your compliance with these terms, you may use Microchip software
* and any derivatives exclusively with Microchip products. It is your
* responsibility to comply with third party license terms applicable to your
* use of third party software (including open source software) that may
* accompany Microchip software.
*
* THIS SOFTWARE IS SUPPLIED BY MICROCHIP "AS IS". NO WARRANTIES, WHETHER
* EXPRESS, IMPLIED OR STATUTORY, APPLY TO THIS SOFTWARE, INCLUDING ANY IMPLIED
* WARRANTIES OF NON-INFRINGEMENT, MERCHANTABILITY, AND FITNESS FOR A
* PARTICULAR PURPOSE.
*
* IN NO EVENT WILL MICROCHIP BE LIABLE FOR ANY INDIRECT, SPECIAL, PUNITIVE,
* INCIDENTAL OR CONSEQUENTIAL LOSS, DAMAGE, COST OR EXPENSE OF ANY KIND
* WHATSOEVER RELATED TO THE SOFTWARE, HOWEVER CAUSED, EVEN IF MICROCHIP HAS
* BEEN ADVISED OF THE POSSIBILITY OR THE DAMAGES ARE FORESEEABLE. TO THE
* FULLEST EXTENT ALLOWED BY LAW, MICROCHIP'S TOTAL LIABILITY ON ALL CLAIMS IN
* ANY WAY RELATED TO THIS SOFTWARE WILL NOT EXCEED THE AMOUNT OF FEES, IF ANY,
* THAT YOU HAVE PAID DIRECTLY TO MICROCHIP FOR THIS SOFTWARE.
*******************************************************************************/

#ifndef PLIB_GPIO_H
#define PLIB_GPIO_H

#include <device.h>
#include <stdint.h>
#include <stdbool.h>
#include <stddef.h>

// DOM-IGNORE-BEGIN
#ifdef __cplusplus  // Provide C++ Compatibility

    extern "C" {

#endif
// DOM-IGNORE-END

// *****************************************************************************
// *****************************************************************************
// Section: Data types and constants
// *****************************************************************************
// *****************************************************************************


/*** Macros for K4DATA pin ***/
#define K4DATA_Set()               (LATCSET = (1<<9))
#define K4DATA_Clear()             (LATCCLR = (1<<9))
#define K4DATA_Toggle()            (LATCINV= (1<<9))
#define K4DATA_OutputEnable()      (TRISCCLR = (1<<9))
#define K4DATA_InputEnable()       (TRISCSET = (1<<9))
#define K4DATA_Get()               ((PORTC >> 9) & 0x1)
#define K4DATA_PIN                  GPIO_PIN_RC9

/*** Macros for I2C2_SDA pin ***/
#define I2C2_SDA_Set()               (LATBSET = (1<<2))
#define I2C2_SDA_Clear()             (LATBCLR = (1<<2))
#define I2C2_SDA_Toggle()            (LATBINV= (1<<2))
#define I2C2_SDA_OutputEnable()      (TRISBCLR = (1<<2))
#define I2C2_SDA_InputEnable()       (TRISBSET = (1<<2))
#define I2C2_SDA_Get()               ((PORTB >> 2) & 0x1)
#define I2C2_SDA_PIN                  GPIO_PIN_RB2

/*** Macros for I2C2_SCL pin ***/
#define I2C2_SCL_Set()               (LATBSET = (1<<3))
#define I2C2_SCL_Clear()             (LATBCLR = (1<<3))
#define I2C2_SCL_Toggle()            (LATBINV= (1<<3))
#define I2C2_SCL_OutputEnable()      (TRISBCLR = (1<<3))
#define I2C2_SCL_InputEnable()       (TRISBSET = (1<<3))
#define I2C2_SCL_Get()               ((PORTB >> 3) & 0x1)
#define I2C2_SCL_PIN                  GPIO_PIN_RB3

/*** Macros for LED1 pin ***/
#define LED1_Set()               (LATCSET = (1<<0))
#define LED1_Clear()             (LATCCLR = (1<<0))
#define LED1_Toggle()            (LATCINV= (1<<0))
#define LED1_OutputEnable()      (TRISCCLR = (1<<0))
#define LED1_InputEnable()       (TRISCSET = (1<<0))
#define LED1_Get()               ((PORTC >> 0) & 0x1)
#define LED1_PIN                  GPIO_PIN_RC0

/*** Macros for LED2 pin ***/
#define LED2_Set()               (LATCSET = (1<<1))
#define LED2_Clear()             (LATCCLR = (1<<1))
#define LED2_Toggle()            (LATCINV= (1<<1))
#define LED2_OutputEnable()      (TRISCCLR = (1<<1))
#define LED2_InputEnable()       (TRISCSET = (1<<1))
#define LED2_Get()               ((PORTC >> 1) & 0x1)
#define LED2_PIN                  GPIO_PIN_RC1

/*** Macros for KEY1 pin ***/
#define KEY1_Set()               (LATCSET = (1<<2))
#define KEY1_Clear()             (LATCCLR = (1<<2))
#define KEY1_Toggle()            (LATCINV= (1<<2))
#define KEY1_OutputEnable()      (TRISCCLR = (1<<2))
#define KEY1_InputEnable()       (TRISCSET = (1<<2))
#define KEY1_Get()               ((PORTC >> 2) & 0x1)
#define KEY1_PIN                  GPIO_PIN_RC2

/*** Macros for SD_CS pin ***/
#define SD_CS_Set()               (LATBSET = (1<<4))
#define SD_CS_Clear()             (LATBCLR = (1<<4))
#define SD_CS_Toggle()            (LATBINV= (1<<4))
#define SD_CS_OutputEnable()      (TRISBCLR = (1<<4))
#define SD_CS_InputEnable()       (TRISBSET = (1<<4))
#define SD_CS_Get()               ((PORTB >> 4) & 0x1)
#define SD_CS_PIN                  GPIO_PIN_RB4

/*** Macros for KEY2 pin ***/
#define KEY2_Set()               (LATCSET = (1<<3))
#define KEY2_Clear()             (LATCCLR = (1<<3))
#define KEY2_Toggle()            (LATCINV= (1<<3))
#define KEY2_OutputEnable()      (TRISCCLR = (1<<3))
#define KEY2_InputEnable()       (TRISCSET = (1<<3))
#define KEY2_Get()               ((PORTC >> 3) & 0x1)
#define KEY2_PIN                  GPIO_PIN_RC3

/*** Macros for KEY3 pin ***/
#define KEY3_Set()               (LATCSET = (1<<4))
#define KEY3_Clear()             (LATCCLR = (1<<4))
#define KEY3_Toggle()            (LATCINV= (1<<4))
#define KEY3_OutputEnable()      (TRISCCLR = (1<<4))
#define KEY3_InputEnable()       (TRISCSET = (1<<4))
#define KEY3_Get()               ((PORTC >> 4) & 0x1)
#define KEY3_PIN                  GPIO_PIN_RC4

/*** Macros for KEY4 pin ***/
#define KEY4_Set()               (LATCSET = (1<<5))
#define KEY4_Clear()             (LATCCLR = (1<<5))
#define KEY4_Toggle()            (LATCINV= (1<<5))
#define KEY4_OutputEnable()      (TRISCCLR = (1<<5))
#define KEY4_InputEnable()       (TRISCSET = (1<<5))
#define KEY4_Get()               ((PORTC >> 5) & 0x1)
#define KEY4_PIN                  GPIO_PIN_RC5

/*** Macros for DIN pin ***/
#define DIN_Get()               ((PORTB >> 7) & 0x1)
#define DIN_PIN                  GPIO_PIN_RB7


// *****************************************************************************
/* GPIO Port

  Summary:
    Identifies the available GPIO Ports.

  Description:
    This enumeration identifies the available GPIO Ports.

  Remarks:
    The caller should not rely on the specific numbers assigned to any of
    these values as they may change from one processor to the next.

    Not all ports are available on all devices.  Refer to the specific
    device data sheet to determine which ports are supported.
*/

typedef enum
{
    GPIO_PORT_A = 0,
    GPIO_PORT_B = 1,
    GPIO_PORT_C = 2,
} GPIO_PORT;

// *****************************************************************************
/* GPIO Port Pins

  Summary:
    Identifies the available GPIO port pins.

  Description:
    This enumeration identifies the available GPIO port pins.

  Remarks:
    The caller should not rely on the specific numbers assigned to any of
    these values as they may change from one processor to the next.

    Not all pins are available on all devices.  Refer to the specific
    device data sheet to determine which pins are supported.
*/

typedef enum
{
    GPIO_PIN_RA0 = 0,
    GPIO_PIN_RA1 = 1,
    GPIO_PIN_RA2 = 2,
    GPIO_PIN_RA3 = 3,
    GPIO_PIN_RA4 = 4,
    GPIO_PIN_RA7 = 7,
    GPIO_PIN_RA8 = 8,
    GPIO_PIN_RA9 = 9,
    GPIO_PIN_RA10 = 10,
    GPIO_PIN_RB0 = 16,
    GPIO_PIN_RB1 = 17,
    GPIO_PIN_RB2 = 18,
    GPIO_PIN_RB3 = 19,
    GPIO_PIN_RB4 = 20,
    GPIO_PIN_RB5 = 21,
    GPIO_PIN_RB7 = 23,
    GPIO_PIN_RB8 = 24,
    GPIO_PIN_RB9 = 25,
    GPIO_PIN_RB10 = 26,
    GPIO_PIN_RB11 = 27,
    GPIO_PIN_RB13 = 29,
    GPIO_PIN_RB14 = 30,
    GPIO_PIN_RB15 = 31,
    GPIO_PIN_RC0 = 32,
    GPIO_PIN_RC1 = 33,
    GPIO_PIN_RC2 = 34,
    GPIO_PIN_RC3 = 35,
    GPIO_PIN_RC4 = 36,
    GPIO_PIN_RC5 = 37,
    GPIO_PIN_RC6 = 38,
    GPIO_PIN_RC7 = 39,
    GPIO_PIN_RC8 = 40,
    GPIO_PIN_RC9 = 41,

    /* This element should not be used in any of the GPIO APIs.
       It will be used by other modules or application to denote that none of the GPIO Pin is used */
    GPIO_PIN_NONE = -1

} GPIO_PIN;


void GPIO_Initialize(void);

// *****************************************************************************
// *****************************************************************************
// Section: GPIO Functions which operates on multiple pins of a port
// *****************************************************************************
// *****************************************************************************

uint32_t GPIO_PortRead(GPIO_PORT port);

void GPIO_PortWrite(GPIO_PORT port, uint32_t mask, uint32_t value);

uint32_t GPIO_PortLatchRead ( GPIO_PORT port );

void GPIO_PortSet(GPIO_PORT port, uint32_t mask);

void GPIO_PortClear(GPIO_PORT port, uint32_t mask);

void GPIO_PortToggle(GPIO_PORT port, uint32_t mask);

void GPIO_PortInputEnable(GPIO_PORT port, uint32_t mask);

void GPIO_PortOutputEnable(GPIO_PORT port, uint32_t mask);

// *****************************************************************************
// *****************************************************************************
// Section: GPIO Functions which operates on one pin at a time
// *****************************************************************************
// *****************************************************************************

static inline void GPIO_PinWrite(GPIO_PIN pin, bool value)
{
    GPIO_PortWrite((GPIO_PORT)(pin>>4), (uint32_t)(0x1) << (pin & 0xF), (uint32_t)(value) << (pin & 0xF));
}

static inline bool GPIO_PinRead(GPIO_PIN pin)
{
    return (bool)(((GPIO_PortRead((GPIO_PORT)(pin>>4))) >> (pin & 0xF)) & 0x1);
}

static inline bool GPIO_PinLatchRead(GPIO_PIN pin)
{
    return (bool)((GPIO_PortLatchRead((GPIO_PORT)(pin>>4)) >> (pin & 0xF)) & 0x1);
}

static inline void GPIO_PinToggle(GPIO_PIN pin)
{
    GPIO_PortToggle((GPIO_PORT)(pin>>4), 0x1 << (pin & 0xF));
}

static inline void GPIO_PinSet(GPIO_PIN pin)
{
    GPIO_PortSet((GPIO_PORT)(pin>>4), 0x1 << (pin & 0xF));
}

static inline void GPIO_PinClear(GPIO_PIN pin)
{
    GPIO_PortClear((GPIO_PORT)(pin>>4), 0x1 << (pin & 0xF));
}

static inline void GPIO_PinInputEnable(GPIO_PIN pin)
{
    GPIO_PortInputEnable((GPIO_PORT)(pin>>4), 0x1 << (pin & 0xF));
}

static inline void GPIO_PinOutputEnable(GPIO_PIN pin)
{
    GPIO_PortOutputEnable((GPIO_PORT)(pin>>4), 0x1 << (pin & 0xF));
}


// DOM-IGNORE-BEGIN
#ifdef __cplusplus  // Provide C++ Compatibility

    }

#endif
// DOM-IGNORE-END
#endif // PLIB_GPIO_H
