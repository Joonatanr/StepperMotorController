/*
 * Scheduler.h
 *
 *  Created on: 16. märts 2018
 *      Author: JRE
 */

#ifndef LOGIC_SCHEDULER_H_
#define LOGIC_SCHEDULER_H_

#include "typedefs.h"

#define SCHEDULER_PERIOD 10u //Define this at 10 milliseconds.

typedef void (*Scheduler_TaskFunc)(void);

typedef enum
{
    TASK_LED_BLINK,
    TASK_CYCLIC_SPIDRV,
    NUMBER_OF_SCHEDULER_TASKS
} Scheduler_LogicTaskEnum;

typedef struct
{
    U16 period;
    Scheduler_TaskFunc init_fptr;
    Scheduler_TaskFunc start_fptr;
    Scheduler_TaskFunc stop_fptr;
    Scheduler_TaskFunc cyclic_fptr;
} Scheduler_LogicTask;

extern void Scheduler_initTasks(void);
extern void Scheduler_cyclic10ms(void);
extern void Scheduler_StartTasks(void);


#endif /* LOGIC_SCHEDULER_H_ */
