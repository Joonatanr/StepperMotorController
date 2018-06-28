/*
 * stepper.c
 *
 *  Created on: 8. juuni 2018
 *      Author: JRE
 */


#include "stepper.h"
#include "driverlib.h"
#include "register.h"
#include "frequency.h"

/************************** Private function forward declarations *****************************/




/***************************Private variable declarations *************************************/


#define NUMBER_OF_MICROSTEPS 8u
#define NUMBER_OF_FULL_STEPS_PER_ROUND 200u

typedef struct
{
    const U8    microstepping_mode; //For now this cannot be changed during runtime.
    U16         microsteps_per_round;

    const U16   max_speed;
    U16         target_speed;
} StepperState_T;


StepperState_T myStepper =
{
     .microstepping_mode = NUMBER_OF_MICROSTEPS,
     .microsteps_per_round = NUMBER_OF_MICROSTEPS * NUMBER_OF_FULL_STEPS_PER_ROUND,
     .max_speed = 1000u,
     .target_speed = 0u
};

#define STEPPER_TIMER_FREQUENCY 3000000u /* 3 MHz */


/************************** Public function definitions **************************************/

Public void stepper_init(void)
{
    //Initialize reset and sleep pins.
    GPIO_setAsOutputPin(GPIO_PORT_P5, GPIO_PIN6);
    GPIO_setAsOutputPin(GPIO_PORT_P2, GPIO_PIN6);

    //Initially we set these high.
    GPIO_setOutputHighOnPin(GPIO_PORT_P5, GPIO_PIN6);
    GPIO_setOutputHighOnPin(GPIO_PORT_P2, GPIO_PIN6);
}


Public Boolean stepper_setSpeed(U32 rpm)
{
    U32 microsteps_per_minute;
    U32 value;
    Boolean res = FALSE;

    if (rpm <= myStepper.max_speed)
    {
        myStepper.target_speed = rpm;

        microsteps_per_minute = rpm * myStepper.microsteps_per_round;
        value = STEPPER_TIMER_FREQUENCY * 60u;
        value = value / microsteps_per_minute;

        /* TODO : Should set a minimum value for this, otherwise the timer will freeze the system with too low values... */
        if (value > 20u) /* Current value is quite arbitrarily chosen. */
        {
            stepper_setTimerValue(value);
        }

        res = TRUE;
    }

    return res;
}


/* TODO : This is temporary, in the future should set speed in other units. */
Public void stepper_setTimerValue(U32 value)
{
    frequency_setInterval(value, FRQ_CH1);
}



/********************* Private function definitions ******************************************/



