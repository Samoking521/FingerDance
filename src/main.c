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


// *****************************************************************************
// *****************************************************************************
// Section: Main Entry Point
// *****************************************************************************
// *****************************************************************************



#define MY_BUFFER_SIZE 1024
uint8_t myBuffer[MY_BUFFER_SIZE];

int main(void)
{
    /* Initialize all modules */
    SYS_Initialize(NULL);
    printf("\nInitialize finish!\n");

    OLED_Test();

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

