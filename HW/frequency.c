/*
 * frequency.c
 *
 *  Created on: 22. juuni 2018
 *      Author: JRE
 */

/* The purpose of this module is to drive 4 separate PWM channels and generate frequency signals. */


#include "frequency.h"
#include "driverlib.h"


typedef struct
{
    U32               timer;
    U8                ccr;
    U32               port;
    U32               pin;
} ChannelConfig_t;

Private const ChannelConfig_t   priv_freq_conf[FRQ_NUMBER_OF_CHANNELS] =
{
     { TIMER_A0_BASE, TIMER_A_CAPTURECOMPARE_REGISTER_1, GPIO_PORT_P2, GPIO_PIN4 },
     { TIMER_A2_BASE, TIMER_A_CAPTURECOMPARE_REGISTER_2, GPIO_PORT_P5, GPIO_PIN7 },
     { TIMER_A3_BASE, TIMER_A_CAPTURECOMPARE_REGISTER_2, GPIO_PORT_P8, GPIO_PIN2 },
     { TIMER_A1_BASE, TIMER_A_CAPTURECOMPARE_REGISTER_3, GPIO_PORT_P7, GPIO_PIN5 }
};


Private Timer_A_PWMConfig       priv_freq_pwm_cfg[FRQ_NUMBER_OF_CHANNELS];


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
    }


    /* Initialize output pins */
    for (ix = 0u; ix < FRQ_NUMBER_OF_CHANNELS; ix++)
    {
        MAP_GPIO_setAsPeripheralModuleFunctionOutputPin(priv_freq_conf[ix].port, priv_freq_conf[ix].pin,
                GPIO_PRIMARY_MODULE_FUNCTION);
    }

    //frequency_setInterval(1600);
}


Public void frequency_setInterval(U16 interval, frequency_Channel_t ch)
{
    /* Since this is a frequency generator then the duty cycle will always be 50% s*/
    Timer_A_PWMConfig * conf_ptr;

    conf_ptr = &priv_freq_pwm_cfg[ch];
    conf_ptr->timerPeriod = interval;
    conf_ptr->dutyCycle = interval >> 1u;

    MAP_Timer_A_generatePWM(priv_freq_conf[ch].timer, conf_ptr);
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
    }
}



