/*
 * frequency.c
 *
 *  Created on: 22. juuni 2018
 *      Author: JRE
 */

/* The purpose of this module is to drive 4 separate PWM channels and generate frequency signals. */


#include "frequency.h"
#include "driverlib.h"

/* Timer_A PWM Configuration Parameter */
Timer_A_PWMConfig pwmConfig =
{
        TIMER_A_CLOCKSOURCE_SMCLK,          /* Clocked at 12MHz     */
        TIMER_A_CLOCKSOURCE_DIVIDER_4,      /* Currently at 3MHz    */
        32000,                              /* This is arbitrary    */
        TIMER_A_CAPTURECOMPARE_REGISTER_1,  /* Output is P2.4       */
        TIMER_A_OUTPUTMODE_RESET_SET,
        16000
};

/* Initially lets just test this with a single channel... */
Public void frequency_init(void)
{
    /* Configuring GPIO2.4 as peripheral output for PWM */
    MAP_GPIO_setAsPeripheralModuleFunctionOutputPin(GPIO_PORT_P2, GPIO_PIN4,
            GPIO_PRIMARY_MODULE_FUNCTION);

    frequency_setInterval(1600);

}


Public void frequency_setInterval(U16 interval)
{
    /* Since this is a frequency generator then the duty cycle will always be 50% s*/
    pwmConfig.timerPeriod = interval;
    pwmConfig.dutyCycle = interval >> 1u;

    MAP_Timer_A_generatePWM(TIMER_A0_BASE, &pwmConfig);
}



