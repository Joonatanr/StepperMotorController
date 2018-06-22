/*
 * commandHandler.c
 *
 *  Created on: 22. juuni 2018
 *      Author: JRE
 */


#include "commandHandler.h"
#include "uartmgr.h"
#include "stepper.h"
#include <stdlib.h>


/********************************************* Private definitions ************************************/

typedef Boolean (*CommandHandlerFunc)(char * data, U8 len);

typedef struct
{
    char prefix;
    CommandHandlerFunc handler_fptr;
} CommandHandler_T;

/************************************* Private function prototypes ************************************/

Private Boolean HandleCommandTimerSet(char * data, U8 len);
Private Boolean HandleCommandRPMSet(char * data, U8 len);
Private Boolean handleCommand(char * cmd, U16 msg_len);


/************************************ Private variable definitions ***********************************/

Private const CommandHandler_T priv_command_handlers[] =
{
     { .prefix = 'T', .handler_fptr = HandleCommandTimerSet },
     { .prefix = 'R', .handler_fptr = HandleCommandRPMSet   }
};



/********************************** Public function definitions  **************************************/


Public void commandHandler_init(void)
{
    uartmgr_setCallbackPtr(handleCommand);
}



/*********************************  Private function definitions  *******************/

Private Boolean handleCommand(char * cmd, U16 msg_len)
{
    Boolean res = FALSE;
    U8 ix;

    for (ix = 0u; ix < NUMBER_OF_ITEMS(priv_command_handlers); ix++)
    {
        if (cmd[0] == priv_command_handlers[ix].prefix)
        {
           res = priv_command_handlers[ix].handler_fptr(cmd + 1, msg_len - 1u);
           break;
        }
    }

    return res;
}


/********************************* Command handlers ********************************/
Private Boolean HandleCommandTimerSet(char * data, U8 len)
{
    Boolean res = FALSE;
    long parsedValue;

    if (len > 0u)
    {
        parsedValue = atol(data);

        if (parsedValue > 0)
        {
            stepper_setTimerValue(parsedValue);
            res = TRUE;
        }
    }

    return res;
}


Private Boolean HandleCommandRPMSet(char * data, U8 len)
{
    Boolean res = FALSE;
    int parsedValue;

    if (len > 0u)
    {
        parsedValue = atoi(data);
        if (parsedValue >= 0)
        {
            res = stepper_setSpeed((U32)parsedValue);
        }
    }

    return res;
}


