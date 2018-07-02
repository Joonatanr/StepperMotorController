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

Private void stopStepper(Stepper_Id id);


/***************************Private variable declarations *************************************/


#define DEFAULT_MICROSTEPPING_MODE 32u
#define NUMBER_OF_FULL_STEPS_PER_ROUND 200u
#define STEPPER_TIMER_FREQUENCY 3000000u /* 3 MHz */

typedef struct
{
    U32 port;
    U32 pin;
} Stepper_IOPort_t;

typedef struct
{
    Stepper_IOPort_t    reset_pin;
    Stepper_IOPort_t    sleep_pin;

    frequency_Channel_t frq_ch;
    const U8            microstepping_mode; //  For now this cannot be changed during runtime.
    const U16           max_speed;          //  In RPM
} StepperConf_t;


typedef struct
{
    const StepperConf_t *   conf;

    U16                     microsteps_per_round;
    U16                     target_speed;
} StepperState_T;


Private const StepperConf_t priv_stepper_conf[NUMBER_OF_STEPPERS] =
{
     { /* Stepper 0 */
       .reset_pin = {GPIO_PORT_P5, GPIO_PIN6},
       .sleep_pin = {GPIO_PORT_P2, GPIO_PIN6},
       .frq_ch = FRQ_CH1,
       .microstepping_mode = DEFAULT_MICROSTEPPING_MODE,
       .max_speed = 1000u
     },

     { /* Stepper 1 */
       .reset_pin = {0, 0}, /* These are not connected yet. */
       .sleep_pin = {0, 0},
       .frq_ch = FRQ_CH2,
       .microstepping_mode = DEFAULT_MICROSTEPPING_MODE,
       .max_speed = 1000u
     },

     { /* Stepper 2 */
       .reset_pin = {0, 0},
       .sleep_pin = {0, 0},
       .frq_ch = FRQ_CH3,
       .microstepping_mode = DEFAULT_MICROSTEPPING_MODE,
       .max_speed = 1000u
     },

     { /* Stepper 3 */
       .reset_pin = {0, 0},
       .sleep_pin = {0, 0},
       .frq_ch = FRQ_CH4,
       .microstepping_mode = DEFAULT_MICROSTEPPING_MODE,
       .max_speed = 1000u
     }
};

Private StepperState_T priv_stepper_state[NUMBER_OF_STEPPERS];



/************************** Public function definitions **************************************/

Public void stepper_init(void)
{
    U8 stepper;

    /* 1. Initialize HW pins */
    for (stepper = 0u; stepper < NUMBER_OF_STEPPERS; stepper++)
    {
        if(priv_stepper_conf[stepper].reset_pin.port == 0)
        {
            /* Ports not yet connected for this stepper. */
            continue;
        }

        GPIO_setAsOutputPin(priv_stepper_conf[stepper].reset_pin.port, priv_stepper_conf[stepper].reset_pin.pin);
        GPIO_setAsOutputPin(priv_stepper_conf[stepper].sleep_pin.port, priv_stepper_conf[stepper].sleep_pin.pin);

        /* Set sleep and reset pins high initally */
        GPIO_setOutputHighOnPin(priv_stepper_conf[stepper].reset_pin.port, priv_stepper_conf[stepper].reset_pin.pin);
        GPIO_setOutputHighOnPin(priv_stepper_conf[stepper].sleep_pin.port, priv_stepper_conf[stepper].sleep_pin.pin);
    }

    /* 2. Initialize stepper states */
    for (stepper = 0u; stepper < NUMBER_OF_STEPPERS; stepper++)
    {
        priv_stepper_state[stepper].conf = &priv_stepper_conf[stepper];
        priv_stepper_state[stepper].microsteps_per_round = priv_stepper_conf[stepper].microstepping_mode * NUMBER_OF_FULL_STEPS_PER_ROUND;
        priv_stepper_state[stepper].target_speed = 0u;
    }
}


Public Boolean stepper_setSpeed(U32 rpm, Stepper_Id id)
{
    U32 microsteps_per_minute;
    U32 value;
    Boolean res = FALSE;

    StepperState_T * state_ptr = &priv_stepper_state[id];
    const StepperConf_t * conf_ptr = state_ptr->conf;

    if (conf_ptr == NULL)
    {
        //Something has gone seriously wrong.
        return FALSE;
    }

    if (rpm == 0)
    {
        stopStepper(id);
    }
    else if (rpm <= conf_ptr->max_speed)
    {
        state_ptr->target_speed = rpm;

        microsteps_per_minute = rpm * state_ptr->microsteps_per_round;
        value = STEPPER_TIMER_FREQUENCY * 60u;
        value = value / microsteps_per_minute;

        /* TODO : Should set a minimum value for this, otherwise the timer will freeze the system with too low values... */
        if (value > 20u) /* Current value is quite arbitrarily chosen. */
        {
            stepper_setTimerValue(value, id);
        }

        res = TRUE;
    }

    return res;
}


Public U16 stepper_getSpeed(Stepper_Id id)
{
    if (id < NUMBER_OF_STEPPERS)
    {
        return priv_stepper_state[id].target_speed;
    }
    else
    {
        return 0;
    }
}


/* TODO : This is temporary, in the future should set speed in other units. */
Public void stepper_setTimerValue(U32 value, Stepper_Id id)
{
    frequency_setInterval(value, priv_stepper_conf[id].frq_ch);
}


/********************* Private function definitions ******************************************/

Private void stopStepper(Stepper_Id id)
{
    /* TODO : Should also drive enable, sleep pins etc. */
    frequency_setEnable(FALSE, priv_stepper_conf[id].frq_ch);
    priv_stepper_state[id].target_speed = 0u;
}



