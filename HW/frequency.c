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


#define STEPPER_TIMER_BASE_FREQUENCY 12000000u /* 12 MHz --- base frequency at 32 microsteps.       */
#define STEPPER_BASE_ACCELERATION 4000         /* 4000 steps per second per second at 32 microsteps */


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

    float             acceleration;     //In steps per second per second.
    float             timer_frequency;  //In Hz

    float             calculated_interval;
    float             acceleration_constant; //m
} FrequencyState_t;

/*************************** Private function forward declarations ********************************/
Private void handleInterrupt(frequency_Channel_t ch);

/************************** Private variable declarations *********************/
Private const ChannelConfig_t   priv_freq_conf[FRQ_NUMBER_OF_CHANNELS] =
{
     { TIMER_A0_BASE, TIMER_A_CAPTURECOMPARE_REGISTER_1, GPIO_PORT_P2, GPIO_PIN4 },
     { TIMER_A2_BASE, TIMER_A_CAPTURECOMPARE_REGISTER_2, GPIO_PORT_P5, GPIO_PIN7 },
     { TIMER_A3_BASE, TIMER_A_CAPTURECOMPARE_REGISTER_2, GPIO_PORT_P8, GPIO_PIN2 },
     { TIMER_A1_BASE, TIMER_A_CAPTURECOMPARE_REGISTER_4, GPIO_PORT_P7, GPIO_PIN4 }
};


Private Timer_A_PWMConfig       priv_freq_pwm_cfg[FRQ_NUMBER_OF_CHANNELS];
Private FrequencyState_t        priv_freq_state[FRQ_NUMBER_OF_CHANNELS];


/* Initially lets just test this with a single channel... */
Public void frequency_init(void)
{
    U8 ix;

    /* Initialize PWM structs. */
    for (ix = 0u; ix < FRQ_NUMBER_OF_CHANNELS; ix++)
    {
        priv_freq_pwm_cfg[ix].clockSource = TIMER_A_CLOCKSOURCE_SMCLK;
        priv_freq_pwm_cfg[ix].clockSourceDivider = TIMER_A_CLOCKSOURCE_DIVIDER_1;
        priv_freq_pwm_cfg[ix].timerPeriod = 32000u; /* TODO : This is arbitrarily chosen, might not be necessary */
        priv_freq_pwm_cfg[ix].compareRegister = priv_freq_conf[ix].ccr;
        priv_freq_pwm_cfg[ix].compareOutputMode = TIMER_A_OUTPUTMODE_RESET_SET;
        priv_freq_pwm_cfg[ix].dutyCycle = 16000u; /* TODO : This is arbitrarily chosen, might not be necessary */

        priv_freq_state[ix].current_interval = 0u;
        priv_freq_state[ix].target_interval = 0u;
        priv_freq_state[ix].acceleration_constant = 0;
        priv_freq_state[ix].acceleration = STEPPER_BASE_ACCELERATION;
        priv_freq_state[ix].timer_frequency = STEPPER_TIMER_BASE_FREQUENCY;
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


Public Boolean frequency_setMicroSteppingMode(U8 mode, frequency_Channel_t ch)
{
    Boolean res = TRUE;
    Timer_A_PWMConfig * conf_ptr;
    float acceleration;
    float timer_frequency;

    if(ch < FRQ_NUMBER_OF_CHANNELS)
    {
        return FALSE;
    }

    conf_ptr = &priv_freq_pwm_cfg[ch];

    /* TODO : Take into account the use case, where we area currently accelerating. */

    switch(mode)
    {
    case 1u:
        conf_ptr->clockSourceDivider = TIMER_A_CLOCKSOURCE_DIVIDER_32;
        acceleration = STEPPER_BASE_ACCELERATION / 32u;
        timer_frequency = STEPPER_TIMER_BASE_FREQUENCY / 32u;
        break;
    case 2u:
        conf_ptr->clockSourceDivider = TIMER_A_CLOCKSOURCE_DIVIDER_16;
        acceleration = STEPPER_BASE_ACCELERATION / 16u;
        timer_frequency = STEPPER_TIMER_BASE_FREQUENCY / 16u;
        break;
    case 4u:
        conf_ptr->clockSourceDivider = TIMER_A_CLOCKSOURCE_DIVIDER_8;
        acceleration = STEPPER_BASE_ACCELERATION / 8u;
        timer_frequency = STEPPER_TIMER_BASE_FREQUENCY / 8u;
        break;
    case 8u:
        conf_ptr->clockSourceDivider = TIMER_A_CLOCKSOURCE_DIVIDER_4;
        acceleration = STEPPER_BASE_ACCELERATION / 4u;
        timer_frequency = STEPPER_TIMER_BASE_FREQUENCY / 4u;
        break;
    case 16u:
        conf_ptr->clockSourceDivider = TIMER_A_CLOCKSOURCE_DIVIDER_2;
        acceleration = STEPPER_BASE_ACCELERATION / 2u;
        timer_frequency = STEPPER_TIMER_BASE_FREQUENCY / 2u;
        break;
    case 32u:
        conf_ptr->clockSourceDivider = TIMER_A_CLOCKSOURCE_DIVIDER_1;
        acceleration = STEPPER_BASE_ACCELERATION / 4u;
        timer_frequency = STEPPER_TIMER_BASE_FREQUENCY / 4u;
        break;
    default:
        res = FALSE;
    }

    if (res == TRUE)
    {
        priv_freq_state[ch].acceleration = acceleration;
        priv_freq_state[ch].timer_frequency = timer_frequency;

        if (priv_freq_state[ch].current_interval != 0u)
        {
            /* Update immediately. */
            MAP_Timer_A_generatePWM(priv_freq_conf[ch].timer, conf_ptr);
        }
    }
    return res;
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
        float r_value;

        /* I think we don't need to take microstepping into account here, if steps_per_minute are always given as 32 microstep values.
         * Target intervals area same anyway, so we can optimize here. */
        target_interval = STEPPER_TIMER_BASE_FREQUENCY * 60u;
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
            current_speed = priv_freq_state[ch].timer_frequency / priv_freq_state[ch].current_interval; /* TODO : Make sure we also calculate this when reaching target interval. */
        }

        /* TODO : This is experimental - also we do not take microstepping into account yet. */
        //1. Calculate first interval.
        first_interval = (current_speed * current_speed) + 2 * (acceleration * priv_freq_state[ch].acceleration);
        first_interval = sqrt(first_interval);
        first_interval = STEPPER_TIMER_BASE_FREQUENCY / first_interval;

        r_value = priv_freq_state[ch].acceleration / (priv_freq_state[ch].timer_frequency * priv_freq_state[ch].timer_frequency);
        acceleration_constant = acceleration * (-1) * r_value;

        if (first_interval > 0xffff)
        {
            if (acceleration > 0)
            {
                /* TODO : This solution is a placeholder, should replace with a pretty much immediate minimum speed. */
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


Private void handleInterrupt(frequency_Channel_t ch)
{
    if (ch < FRQ_NUMBER_OF_CHANNELS)
    {
        float new_interval;
        float old_interval = priv_freq_state[ch].calculated_interval;

        if (priv_freq_state[ch].current_interval == priv_freq_state[ch].target_interval)
        {
            return;
        }

        /* TODO : Add debug GPIO here. */
        /* Currently this is only line with actual float calculations, might want to check how many cycles this actually takes... */
        new_interval = old_interval * (1 + (priv_freq_state[0].acceleration_constant * old_interval * old_interval));
        /* TODO : Add debug GPIO here. */

        /* Check if we have reached the target... */
        if (new_interval > old_interval)
        {
            //Decelerating...
            if (new_interval > priv_freq_state[ch].target_interval)
            {
                new_interval = priv_freq_state[ch].target_interval;
                priv_freq_state[ch].acceleration_constant = 0;
            }
        }
        else if (new_interval < old_interval)
        {
            //Accelerating...
            if (new_interval < priv_freq_state[ch].target_interval)
            {
                new_interval = priv_freq_state[ch].target_interval;
                priv_freq_state[ch].acceleration_constant = 0;
            }
        }

        priv_freq_state[ch].calculated_interval = new_interval; //We also have to keep track of the calculated float value.
        priv_freq_state[ch].current_interval = (U32)new_interval;

        /*
        TA0CCR0 = priv_freq_state[ch].current_interval;
        TA0CCR1 = priv_freq_state[ch].current_interval / 2;
        */

        /* This part got pretty much copied from driverlib. We need to have fast access here... */
        TIMER_A_CMSIS(priv_freq_conf[ch].timer)->CCR[0] = priv_freq_state[ch].current_interval;
        uint8_t idx = (priv_freq_conf[ch].ccr >> 1) - 1;
        TIMER_A_CMSIS(priv_freq_conf[ch].timer)->CCR[idx] = priv_freq_state[ch].current_interval / 2;
    }
}

//******************************************************************************
//
//This is the TIMERA interrupt vector service routine.
//
//******************************************************************************
void TA0_N_IRQHandler(void)
{
    MAP_Timer_A_clearInterruptFlag(TIMER_A0_BASE);
    Timer_A_clearCaptureCompareInterrupt(TIMER_A0_BASE, TIMER_A_CAPTURECOMPARE_REGISTER_1);

    handleInterrupt(FRQ_CH1);
}

/* TODO : Handle other interrupts !!! */


