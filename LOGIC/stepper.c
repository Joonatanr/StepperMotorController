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

Private void StepperStopCompleteCallback(frequency_Channel_t ch);
Private void stopStepper(Stepper_Id id);
Private Boolean setStepperMicrostep(Stepper_Id id, U8 mode);

/***************************Private variable declarations *************************************/

Public Frequency_StoppedCallback frequency_stopped_cb = StepperStopCompleteCallback;

#define DEFAULT_MICROSTEPPING_MODE 32u
#define NUMBER_OF_FULL_STEPS_PER_ROUND 200u
#define DEFAULT_STEPS_PER_ROUND DEFAULT_MICROSTEPPING_MODE * NUMBER_OF_FULL_STEPS_PER_ROUND

typedef struct
{
    IOPort_t    reset_pin;
    IOPort_t    sleep_pin;

    IOPort_t    m0_pin;
    IOPort_t    m1_pin;
    IOPort_t    m2_pin;

    frequency_Channel_t frq_ch;
    const U16           max_speed;          //  In RPM
    const U16           min_speed;          //  In RPM
} StepperConf_t;


typedef struct
{
    const StepperConf_t *   conf;

    U8                      microstepping_mode;
    U16                     target_speed;
} StepperState_T;


Private const StepperConf_t priv_stepper_conf[NUMBER_OF_STEPPERS] =
{
     { /* Stepper 0 */
       .reset_pin = { GPIO_PORT_P5, GPIO_PIN6 },
       .sleep_pin = { GPIO_PORT_P2, GPIO_PIN6 },

       .m0_pin =    { GPIO_PORT_P3, GPIO_PIN0 },
       .m1_pin =    { GPIO_PORT_P2, GPIO_PIN5 },
       .m2_pin =    { GPIO_PORT_P1, GPIO_PIN6 },

       .frq_ch = FRQ_CH1,
       .max_speed = 1000u,
       .min_speed = 5u
     },

     { /* Stepper 1 */
       .reset_pin = { GPIO_PORT_P6, GPIO_PIN6 },
       .sleep_pin = { GPIO_PORT_P6, GPIO_PIN7 },

       .m0_pin    = { GPIO_PORT_P1, GPIO_PIN7 },
       .m1_pin    = { GPIO_PORT_P5, GPIO_PIN0 },
       .m2_pin    = { GPIO_PORT_P5, GPIO_PIN2 },

       /* TODO : Check that this works and remove. */
/*
       .m0_pin =    { GPIO_PORT_P3, GPIO_PIN7 },
       .m1_pin =    { GPIO_PORT_P3, GPIO_PIN5 },
       .m2_pin =    { GPIO_PORT_P3, GPIO_PIN6 },
*/

       .frq_ch = FRQ_CH2,
       .max_speed = 500u,
       .min_speed = 5u
     },

     { /* Stepper 2 */
       .reset_pin = { GPIO_PORT_P2, GPIO_PIN3 },
       .sleep_pin = { GPIO_PORT_P5, GPIO_PIN1 },

       .m0_pin =    { GPIO_PORT_P4, GPIO_PIN5 },
       .m1_pin =    { GPIO_PORT_P5, GPIO_PIN4 },
       .m2_pin =    { GPIO_PORT_P5, GPIO_PIN5 },

       .frq_ch = FRQ_CH3,
       .max_speed = 300u,
       .min_speed = 5u
     },

     { /* Stepper 3 */
       .reset_pin = { GPIO_PORT_P10, GPIO_PIN2 },
       .sleep_pin = { GPIO_PORT_P9,  GPIO_PIN2 },

       .m0_pin =    { 0u, 0u },
       .m1_pin =    { 0u, 0u },
       .m2_pin =    { 0u, 0u },

       .frq_ch = FRQ_CH4,
       .max_speed = 300u,
       .min_speed = 5u
     }
};

Private StepperState_T priv_stepper_state[NUMBER_OF_STEPPERS];



/************************** Public function definitions **************************************/

Public void stepper_init(void)
{
    U8 stepper;
    const StepperConf_t * conf_ptr;

    /* 1. Initialize HW pins */
    for (stepper = 0u; stepper < NUMBER_OF_STEPPERS; stepper++)
    {
        conf_ptr = &priv_stepper_conf[stepper];

        if(conf_ptr->reset_pin.port == 0)
        {
            /* Ports not yet connected for this stepper. */
            continue;
        }

        GPIO_setAsOutputPin(conf_ptr->reset_pin.port, conf_ptr->reset_pin.pin);
        GPIO_setAsOutputPin(conf_ptr->sleep_pin.port, conf_ptr->sleep_pin.pin);

        /* Set sleep and reset pins to inital values */
        GPIO_setOutputHighOnPin(conf_ptr->reset_pin.port, conf_ptr->reset_pin.pin);
        GPIO_setOutputLowOnPin(conf_ptr->sleep_pin.port, conf_ptr->sleep_pin.pin);

        //Initialize microstepping control pins.
        if(conf_ptr->m0_pin.port != 0u)
        {
            GPIO_setAsOutputPin(conf_ptr->m0_pin.port, conf_ptr->m0_pin.pin);
            GPIO_setAsOutputPin(conf_ptr->m1_pin.port, conf_ptr->m1_pin.pin);
            GPIO_setAsOutputPin(conf_ptr->m2_pin.port, conf_ptr->m2_pin.pin);
        }

    }

    /* 2. Initialize stepper states */
    for (stepper = 0u; stepper < NUMBER_OF_STEPPERS; stepper++)
    {
        /* TODO : Should be possible to define default microstepping mode for all steppers separately. */
        priv_stepper_state[stepper].conf = &priv_stepper_conf[stepper];

        if(!setStepperMicrostep((Stepper_Id)stepper, (U8)DEFAULT_MICROSTEPPING_MODE))
        {
            /* Ports are driven manually - assume default microstepping mode for calculations. */
            priv_stepper_state[stepper].microstepping_mode = DEFAULT_MICROSTEPPING_MODE;
        }

        priv_stepper_state[stepper].target_speed = 0u;
    }
}


Public Boolean stepper_setSpeed(U32 rpm, Stepper_Id id)
{
    U32 microsteps_per_minute;
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
        res = TRUE;
    }
    else if ((rpm <= conf_ptr->max_speed) && (rpm >= conf_ptr->min_speed))
    {
        state_ptr->target_speed = rpm;
        microsteps_per_minute = rpm * DEFAULT_STEPS_PER_ROUND;

        //Wake up if sleeping.
        if (priv_stepper_conf[id].sleep_pin.port != 0)
        {
            GPIO_setOutputHighOnPin(priv_stepper_conf[id].sleep_pin.port, priv_stepper_conf[id].sleep_pin.pin);
        }

        res = frequency_setStepsPerMinute(microsteps_per_minute, priv_stepper_conf[id].frq_ch);
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


/* Sets timer interval directly. */
/* This is only for debugging. */
Public Boolean stepper_setTimerValue(U32 value, Stepper_Id id)
{
    if (priv_stepper_conf[id].sleep_pin.port != 0)
    {
        GPIO_setOutputHighOnPin(priv_stepper_conf[id].sleep_pin.port, priv_stepper_conf[id].sleep_pin.pin);
    }

    return frequency_setInterval(value, priv_stepper_conf[id].frq_ch);
}


Public Boolean stepper_setMicrosteppingMode(Stepper_Id id, U8 mode)
{
    return setStepperMicrostep(id, mode);
}


Public Boolean stepper_getState(Stepper_Id id, Stepper_Query_t * res)
{
    if ((id < NUMBER_OF_STEPPERS) && (res != NULL))
    {
        res->interval = frequency_getInterval((frequency_Channel_t)id);
        res->rpm = priv_stepper_state[id].target_speed;
        res->microstepping_mode = priv_stepper_state[id].microstepping_mode;
        return TRUE;
    }
    else
    {
        return FALSE;
    }
}


/********************* Private function definitions ******************************************/

/* This is called to stop a stepper motor. */
Private void stopStepper(Stepper_Id id)
{
    frequency_setStepsPerMinute(0u, (frequency_Channel_t)id);
    priv_stepper_state[id].target_speed = 0u;
}


/* This is called when a stepper motor has reached zero speed through the frequency module...
 * Implementation is not ideal, but will do for now. */
Private void StepperStopCompleteCallback(frequency_Channel_t ch)
{
    /* TODO : Should also drive enable, sleep pins etc. */
    if (priv_stepper_conf[ch].sleep_pin.port != 0)
    {
        GPIO_setOutputLowOnPin(priv_stepper_conf[ch].sleep_pin.port, priv_stepper_conf[ch].sleep_pin.pin);
    }
}


Private Boolean setStepperMicrostep(Stepper_Id id, U8 mode)
{
    Boolean res = FALSE;

    if (id < NUMBER_OF_STEPPERS)
    {
        //Check if we have microstepping control enabled for this stepper motor.
        if (priv_stepper_conf[id].m0_pin.port != 0)
        {
            Boolean m0_mode = FALSE;
            Boolean m1_mode = FALSE;
            Boolean m2_mode = FALSE;

            switch(mode)
            {
            case 1u:
                /* Full step */
                m0_mode = FALSE;
                m1_mode = FALSE;
                m2_mode = FALSE;
                break;
            case 2u:
                /* Half step */
                m0_mode = TRUE;
                m1_mode = FALSE;
                m2_mode = FALSE;
                break;
            case 4u:
                /* 1/4 step */
                m0_mode = FALSE;
                m1_mode = TRUE;
                m2_mode = FALSE;
                break;
            case 8u:
                /* 1/8 step */
                m0_mode = TRUE;
                m1_mode = TRUE;
                m2_mode = FALSE;
                break;
            case 16u:
                m0_mode = FALSE;
                m1_mode = FALSE;
                m2_mode = TRUE;
                break;
            case 32u:
                m0_mode = TRUE;
                m1_mode = FALSE;
                m2_mode = TRUE;
                break;
            default:
                //Unsupported number of microsteps.
                return FALSE;
            }

            /* Drive the actual physical ports to the desired configuration. */
            ports_setPort(priv_stepper_conf[id].m0_pin.port, priv_stepper_conf[id].m0_pin.pin, m0_mode);
            ports_setPort(priv_stepper_conf[id].m1_pin.port, priv_stepper_conf[id].m1_pin.pin, m1_mode);
            ports_setPort(priv_stepper_conf[id].m2_pin.port, priv_stepper_conf[id].m2_pin.pin, m2_mode);

            priv_stepper_state[id].microstepping_mode = mode;
            frequency_setMicroSteppingMode(mode, (frequency_Channel_t)id);

            res = TRUE;
        }
    }

    return res;
}


