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
#include "drivers/key/key.h"
#include "application/music/music.h"
#include "application/screen/screen.h"
#include "application/sdfile/sdfile.h"
#include "application/pre_mode/pre_mode.h"
#include "application/play_mode/play_mode.h"
#include "application/game_mode/game_mode.h"
#include "drivers/microSD/microSD.h"
#include "main.h"

MODE modeCtrl = init_mode;

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

    main_init();


    while (true)
    {
        switch (modeCtrl)
        {
            case init_mode:
                break;
            case start_mode:
                break;
            case pre_mode:
                pre_main();
                break;
            case play_mode:
                play_main();
                break;
            case game_mode:
                game_main();
                break;
        }
    }

    /* Execution should not come here during normal operation */

    return ( EXIT_FAILURE);
}

void main_init()
{
    //drivers init
    sdFile_Init();
    Screen_Init();
    WS2812B_Init();
    //mode init
    modeCtrl = pre_mode;
}


/*******************************************************************************
 End of File
 */

