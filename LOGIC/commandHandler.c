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
typedef Boolean (*SubFunctionHandler)(Stepper_Id id, int arg);

typedef struct
{
    char prefix;
    CommandHandlerFunc handler_fptr;
} CommandHandler_T;


typedef struct
{
    char prefix;
    SubFunctionHandler handler_fptr;
} SubFunc_T;


/************************************* Private function prototypes ************************************/

Private Boolean handleCommand(char * cmd, U16 msg_len);

/* Command handlers */
Private Boolean HandleCommandToStepper(char * data, U8 len);
Private Boolean HandleQueryState(char * data, U8 len);

/* Subfunction handlers */
Private Boolean HandleCommandTimerSet(Stepper_Id id, int arg);
Private Boolean HandleCommandRPMSet(Stepper_Id id, int arg);




/************************************ Private variable definitions ***********************************/

Public UartCommandHandler CmdHandlerFunc = handleCommand;


Private const CommandHandler_T priv_command_handlers[] =
{
     { .prefix = 'S', .handler_fptr = HandleCommandToStepper    },
     { .prefix = 'Q', .handler_fptr = HandleQueryState          }
};


Private const SubFunc_T priv_subfunctions[] =
{
     { .prefix = 'T', .handler_fptr = HandleCommandTimerSet     },
     { .prefix = 'R', .handler_fptr = HandleCommandRPMSet       },
};



/********************************** Public function definitions  **************************************/


Public void commandHandler_init(void)
{

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

/* Format is S1R200 */
Private Boolean HandleCommandToStepper(char * data, U8 len)
{
    Boolean res = FALSE;
    int stepper_id;
    char subcmd;
    char * ps = data;
    U8 ix;

    if (len > 0u)
    {
        stepper_id = atoi(ps);
        if ((stepper_id >= 0) && (stepper_id < NUMBER_OF_STEPPERS))
        {
            ps++;

            if(!(*ps))
            {
                return FALSE;
            }

            /* Extract sub command */
            subcmd = *ps;
            ps++;

            if(!(*ps))
            {
                return FALSE;
            }

            for (ix = 0u; ix < NUMBER_OF_ITEMS(priv_subfunctions); ix++)
            {
                if (priv_subfunctions[ix].prefix == subcmd)
                {
                    int arg = atoi(ps);
                    res = priv_subfunctions[ix].handler_fptr((Stepper_Id)stepper_id, arg);
                    break;
                }
            }
        }
    }

    return res;
}


Private Boolean HandleQueryState(char * data, U8 len)
{
    /* TODO : Create handler for reporting current state of stepper motors */
    return TRUE;
}


/********************************* Subfunction handlers ********************************/
Private Boolean HandleCommandTimerSet(Stepper_Id id, int arg)
{
    Boolean res = FALSE;

    if ((id < NUMBER_OF_STEPPERS) && (arg >= 0))
    {
        stepper_setTimerValue(arg, id);
        res = TRUE;
    }
    return res;
}


Private Boolean HandleCommandRPMSet(Stepper_Id id, int arg)
{
    Boolean res = FALSE;

    if ((id < NUMBER_OF_STEPPERS) && (arg >= 0))
    {
        stepper_setSpeed(arg, id);
        res = TRUE;
    }
    return res;
}





