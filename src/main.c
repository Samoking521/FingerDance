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
#include "definitions.h"                // SYS function prototypes


// *****************************************************************************
// *****************************************************************************
// Section: Main Entry Point
// *****************************************************************************
// *****************************************************************************
#define TX_NUM_BYTES     1
#define RX_NUM_BYTES     1

uint8_t txBuffer[TX_NUM_BYTES];
uint8_t rxBuffer[RX_NUM_BYTES];

#define MY_BUFFER_SIZE 1024
uint8_t myBuffer[MY_BUFFER_SIZE];

int main(void)
{
    /* Initialize all modules */
    SYS_Initialize(NULL);
    printf("\nInitialize finish!\n");

    SD_Init();
    /*
    uint8_t tx[6] = {0x40, 0x00, 0x00, 0x00, 0x00, 0x95};
    uint8_t rx[8];

    SD_CS_Clear();
    SPI1_WriteRead(&tx, 6, &rx, 8);
    SD_CS_Set();

    printf("tx: ");
    for (int i = 0; i < 6; i++)
        printf("%02x ", tx[i]);
    printf("\n");

    printf("rx: ");
    for (int i = 0; i < 8; i++)
        printf("%02x ", rx[i]);
    printf("\n");
    */
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

