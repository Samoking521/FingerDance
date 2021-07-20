/*******************************************************************************
  Main Source File

  Company:
    Microchip Technology Inc.

  File Name:
    main.c

  Summary:
    This file contains the "main" function for a project.

  Description:
    This file contains the "main" function for a project.  The
    "main" function calls the "SYS_Initialize" function to initialize the state
    machines of all modules in the system
 *******************************************************************************/

// *****************************************************************************
// *****************************************************************************
// Section: Included Files
// *****************************************************************************
// *****************************************************************************

#include <stddef.h>                     // Defines NULL
#include <stdbool.h>                    // Defines true
#include <stdlib.h>                     // Defines EXIT_FAILURE
#include "definitions.h"
#include "library/Fatfs/ff.h"
#include "drivers/OLED/OLED.h"
#include "drivers/ws2812b/ws2812b.h"
#include "drivers/i2c/i2c_gpio.h"
#include "application/music/music.h"



// *****************************************************************************
// *****************************************************************************
// Section: Main Entry Point
// *****************************************************************************
// *****************************************************************************

int main(void)
{
    /* Initialize all modules */
    SYS_Initialize(NULL);
    printf("Initialize finish!\n");

    //OLED_Test();
    music_init();
    music_play();
    //WS2812B_Init();
    //WS2812B_Test();
    //i2c2_CheckDevice(0x1A);

    while (true)
    {
        /* Maintain state machines of all polled MPLAB Harmony modules. */
        GPIO_PinSet(LED1_PIN);
        //GPIO_PinSet(I2C2_SCL_PIN);
        //GPIO_PinClear(I2C2_SDA_PIN);
        CORETIMER_DelayMs(500);
        GPIO_PinClear(LED1_PIN);
        //GPIO_PinClear(I2C2_SCL_PIN);
        //GPIO_PinSet(I2C2_SDA_PIN);
        CORETIMER_DelayMs(500);
    }

    /* Execution should not come here during normal operation */

    return ( EXIT_FAILURE);
}


/*******************************************************************************
 End of File
 */

