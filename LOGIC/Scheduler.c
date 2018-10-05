/*
 * Scheduler.c
 *
 *  Created on: 16. märts 2018
 *      Author: JRE
 */

#include "Scheduler.h"
#include "register.h"
#include <driverlib.h>
#include "spidrv.h"


/*************  Private function prototypes.  **************/

Private void led_cyclic1000ms(void);

/* This array contains the tasks that run all the time. */
/* Small incremental changes :) - So lets enable the modules part first and then look at this part. */
Private const Scheduler_LogicTask priv_tasks[NUMBER_OF_SCHEDULER_TASKS] =
{
     {.init_fptr = NULL,        .start_fptr = NULL, .cyclic_fptr = led_cyclic1000ms,  .stop_fptr = NULL, .period = 1000u }, //TASK_LED_BLINK - For showing scheduler works OK.
     {.init_fptr = spidrv_init, .start_fptr = NULL, .cyclic_fptr = spidrv_cyclic10ms, .stop_fptr = NULL, .period = 10u   }, //TASK_CYCLIC_SPIDRV
};


/*************  Private variable declarations.  **************/
Private U16 priv_task_timer = 0u;
Private Boolean priv_isInitComplete = FALSE;


/* Should be called once at startup. */
void Scheduler_initTasks(void)
{
    U8 ix;

    for (ix = 0u; ix < NUMBER_OF_ITEMS(priv_tasks); ix++)
    {
        if (priv_tasks[ix].init_fptr != NULL)
        {
            priv_tasks[ix].init_fptr();
        }
    }

    priv_isInitComplete = TRUE;
}

/* Should be called once at startup.. */
void Scheduler_StartTasks(void)
{
    U8 ix;

    for (ix = 0u; ix < NUMBER_OF_SCHEDULER_TASKS; ix++)
    {
        if (priv_tasks[ix].start_fptr != NULL)
        {
            priv_tasks[ix].start_fptr();
        }
    }
}

/* Called every 10 ms, by the main.c lo prio interrupt. */
void Scheduler_cyclic10ms(void)
{
    U8 ix;
    if (priv_isInitComplete == FALSE)
    {
        return;
    }

    priv_task_timer += SCHEDULER_PERIOD;

    /* Prevent overflow. */
    if (priv_task_timer > 50000u)
    {
        priv_task_timer = SCHEDULER_PERIOD;
    }


    /* Deal with other constantly firing modules. */
    for (ix = 0u; ix < NUMBER_OF_ITEMS(priv_tasks); ix++)
    {
        /* No point to check this, if there is no corresponding function... */
        if (priv_tasks[ix].cyclic_fptr != NULL)
        {
            if ((priv_task_timer % priv_tasks[ix].period) == 0u)
            {
                priv_tasks[ix].cyclic_fptr();
            }
        }
    }
}


/**********  Private function definitions *************/
Private void led_cyclic1000ms(void)
{
    static U8 led_state;

    led_state = !led_state;
    set_led_one((Boolean)led_state);
}

