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
    /*
    uint8_t buf[10] = {0, 1, 2, 3, 4, 5, 6, 7, 8, 9};
    void *srcAddr = (uint8_t *) buf;
    void *destAddr = (uint8_t*) &SPI2BUF;
    size_t size = 10;

    DMAC_ChannelTransfer(DMAC_CHANNEL_0,
            srcAddr,
            size,
            destAddr,
            1,
            1);*/

    while (true)
    {
        /* Maintain state machines of all polled MPLAB Harmony modules. */
        GPIO_PinSet(LED1_PIN);
        CORETIMER_DelayMs(500);
        GPIO_PinClear(LED1_PIN);
        CORETIMER_DelayMs(500);
    }

    /* Execution should not come here during normal operation */

    return ( EXIT_FAILURE);
}


/*******************************************************************************
 End of File
 */

