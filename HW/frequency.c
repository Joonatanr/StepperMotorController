/*
 * frequency.c
 *
 *  Created on: 22. juuni 2018
 *      Author: JRE
 */

/* The purpose of this module is to drive 4 separate PWM channels and generate frequency signals. */


#include "frequency.h"
#include "driverlib.h"
#include "math.h"


#define STEPPER_TIMER_FREQUENCY 3000000u /* 3 MHz */
#define STEPPER_ACCELERATION 2000 /* 2000 steps per second per second */


typedef struct
{
    U32               timer;
    U8                ccr;
    U32               port;
    U32               pin;
} ChannelConfig_t;

typedef struct
{
    U16               target_interval;
    U16               current_interval;
    float             calculated_interval;
    float             acceleration_constant; //m
} FrequencyState_t;

Private const ChannelConfig_t   priv_freq_conf[FRQ_NUMBER_OF_CHANNELS] =
{
     { TIMER_A0_BASE, TIMER_A_CAPTURECOMPARE_REGISTER_1, GPIO_PORT_P2, GPIO_PIN4 },
     { TIMER_A2_BASE, TIMER_A_CAPTURECOMPARE_REGISTER_2, GPIO_PORT_P5, GPIO_PIN7 },
     { TIMER_A3_BASE, TIMER_A_CAPTURECOMPARE_REGISTER_2, GPIO_PORT_P8, GPIO_PIN2 },
     { TIMER_A1_BASE, TIMER_A_CAPTURECOMPARE_REGISTER_4, GPIO_PORT_P7, GPIO_PIN4 }
};


Private Timer_A_PWMConfig       priv_freq_pwm_cfg[FRQ_NUMBER_OF_CHANNELS];
Private FrequencyState_t        priv_freq_state[FRQ_NUMBER_OF_CHANNELS];

/* TODO : This is currently experimental. */
Private const float priv_calc_r = (float)STEPPER_ACCELERATION / ((float)STEPPER_TIMER_FREQUENCY * (float)STEPPER_TIMER_FREQUENCY);


/* Initially lets just test this with a single channel... */
Public void frequency_init(void)
{
    U8 ix;

    /* Initialize PWM structs. */
    for (ix = 0u; ix < FRQ_NUMBER_OF_CHANNELS; ix++)
    {
        priv_freq_pwm_cfg[ix].clockSource = TIMER_A_CLOCKSOURCE_SMCLK;
        priv_freq_pwm_cfg[ix].clockSourceDivider = TIMER_A_CLOCKSOURCE_DIVIDER_4;
        priv_freq_pwm_cfg[ix].timerPeriod = 32000u; /* TODO : This is arbitrarily chosen, might not be necessary */
        priv_freq_pwm_cfg[ix].compareRegister = priv_freq_conf[ix].ccr;
        priv_freq_pwm_cfg[ix].compareOutputMode = TIMER_A_OUTPUTMODE_RESET_SET;
        priv_freq_pwm_cfg[ix].dutyCycle = 16000u; /* TODO : This is arbitrarily chosen, might not be necessary */

        priv_freq_state[ix].current_interval = 0u;
        priv_freq_state[ix].target_interval = 0u;
        priv_freq_state[ix].acceleration_constant = 0;
    }


    /* Initialize output pins */
    for (ix = 0u; ix < FRQ_NUMBER_OF_CHANNELS; ix++)
    {
        MAP_GPIO_setAsPeripheralModuleFunctionOutputPin(priv_freq_conf[ix].port, priv_freq_conf[ix].pin,
                GPIO_PRIMARY_MODULE_FUNCTION);
    }

    /* Enable timer interrupt */
    Timer_A_enableCaptureCompareInterrupt(TIMER_A0_BASE, TIMER_A_CAPTURECOMPARE_REGISTER_1);
    MAP_Interrupt_enableInterrupt(INT_TA0_N);
}


/* We use steps per minute so we do not lose accuracy */
Public Boolean frequency_setStepsPerMinute(U32 steps_per_minute, frequency_Channel_t ch)
{
    if (ch < FRQ_NUMBER_OF_CHANNELS)
    {
        //Calculate target interval.
        U32 target_interval;
        float first_interval;
        float acceleration;
        float current_speed;
        float acceleration_constant;

        target_interval = STEPPER_TIMER_FREQUENCY * 60u;
        target_interval = target_interval / steps_per_minute;

        if (target_interval > 0xffffu)
        {
            return FALSE;
        }

        Interrupt_disableMaster(); //Begin critical section.

        if ((priv_freq_state[ch].current_interval > target_interval) || (priv_freq_state[ch].current_interval == 0u))
        {
            /* We are accelerating */
            acceleration = 1;
        }
        else if(priv_freq_state[ch].current_interval < target_interval)
        {
            /* We are decelerating. */
            acceleration = -1;
        }
        else
        {
            /* Looks like we are not changing anything... */
            return TRUE;
        }

        /* TODO : Also add some value range checks... */

        /* Idea is that all more complex calculations should be done here. */
        /* TODO : Analyze situation where stepper actually is accelerating previously when these calculations are done...
         * Probably need critical section here (stepper will hopefully still keep turning :) ) */

        /* First we need to get the current actual speed of the motor. */
        if (priv_freq_state[ch].current_interval == 0u)
        {
            current_speed = 0u;
        }
        else
        {
            current_speed = STEPPER_TIMER_FREQUENCY / priv_freq_state[ch].current_interval; /* TODO : Make sure we also calculate this when reaching target interval. */
        }

        /* TODO : This is experimental - also we do not take microstepping into account yet. */
        //1. Calculate first interval.
        first_interval = (current_speed * current_speed) + 2 * (acceleration * STEPPER_ACCELERATION);
        first_interval = sqrt(first_interval);
        first_interval = STEPPER_TIMER_FREQUENCY / first_interval;

        acceleration_constant = acceleration * (-1) * priv_calc_r;

        if (first_interval > 0xffff)
        {
            if (acceleration > 0)
            {
                //We are accelerating. We immediately should give "minimum speed"
                while(first_interval > 0xffff)
                {
                    first_interval = first_interval * (1 + (acceleration_constant * first_interval * first_interval));
                }
            }
            else
            {
                //Curious case
                return FALSE;
            }
        }

        //2. Set target interval and other variables.
        priv_freq_state[ch].target_interval = target_interval;
        priv_freq_state[ch].acceleration_constant = acceleration_constant;
        priv_freq_state[ch].calculated_interval = first_interval;
        Interrupt_enableMaster(); //End critical section.


        //3. Begin with first interval.
        /* TODO : What if this fails??? */
        frequency_setInterval((U32)first_interval, ch);

        return TRUE;
    }
    else
    {
        return FALSE;
    }
}


Public void frequency_setEnable(Boolean mode, frequency_Channel_t ch)
{
    Timer_A_PWMConfig * conf_ptr;
    conf_ptr = &priv_freq_pwm_cfg[ch];

    if (mode == TRUE)
    {
        MAP_Timer_A_generatePWM(priv_freq_conf[ch].timer, conf_ptr);
    }
    else
    {
        MAP_Timer_A_stopTimer(priv_freq_conf[ch].timer);
        priv_freq_state[ch].current_interval = 0u;
        priv_freq_state[ch].target_interval = 0u;
    }
}


/* TODO : Make this into a private function... */
/* Note that since we use a 16-bit timer for frequency generation then we
 * cannot use intervals larger than 0xffffu.
 *
 * In practice this means that maximum interval is around 20ms or 45Hz.
 *
 * Maybe if it turns out that we do need to use frequecies lower than 5Hz, then
 * we could use the 32 bit timer and drive the output pin directly?
 *
 * */
Public Boolean frequency_setInterval(U32 interval, frequency_Channel_t ch)
{
    /* Since this is a frequency generator then the duty cycle will always be 50% */
    Timer_A_PWMConfig * conf_ptr;
    U32 duty_cycle;
    Boolean res = FALSE;

    if (interval < 0xffffu)
    {
        duty_cycle = interval / 2;
        conf_ptr = &priv_freq_pwm_cfg[ch];

        conf_ptr->timerPeriod = interval;
        conf_ptr->dutyCycle = (U16)duty_cycle;

        priv_freq_state[ch].current_interval = interval;
        MAP_Timer_A_generatePWM(priv_freq_conf[ch].timer, conf_ptr);

        res = TRUE;
    }

    return res;
}


Public U16 frequency_getInterval(frequency_Channel_t ch)
{
    if (ch < FRQ_NUMBER_OF_CHANNELS)
    {
        return priv_freq_pwm_cfg[ch].dutyCycle;
    }
    else
    {
        return 0u;
    }
}


//******************************************************************************
//
//This is the TIMERA interrupt vector service routine.
//
//******************************************************************************
void TA0_N_IRQHandler(void)
{
    float new_interval;
    float old_interval = priv_freq_state[0].calculated_interval;

    MAP_Timer_A_clearInterruptFlag(TIMER_A0_BASE);
    Timer_A_clearCaptureCompareInterrupt(TIMER_A0_BASE, TIMER_A_CAPTURECOMPARE_REGISTER_1);

    if (priv_freq_state[0].current_interval == priv_freq_state[0].target_interval)
    {
        return;
    }

    new_interval = old_interval * (1 + (priv_freq_state[0].acceleration_constant * old_interval * old_interval));

    /* Check if we have reached the target... */
    if (new_interval > old_interval)
    {
        //Decelerating...
        if (new_interval > priv_freq_state[0].target_interval)
        {
            new_interval = priv_freq_state[0].target_interval;
            priv_freq_state[0].acceleration_constant = 0;
        }
    }
    else if (new_interval < old_interval)
    {
        //Accelerating...
        if (new_interval < priv_freq_state[0].target_interval)
        {
            new_interval = priv_freq_state[0].target_interval;
            priv_freq_state[0].acceleration_constant = 0;
        }
    }

    priv_freq_state[0].calculated_interval = new_interval; //We also have to keep track of the calculated float value.
    priv_freq_state[0].current_interval = (U32)new_interval;

    TA0CCR0 = priv_freq_state[0].current_interval;
    TA0CCR1 = priv_freq_state[0].current_interval / 2;
}

/* TODO : Handle other interrupts !!! */


