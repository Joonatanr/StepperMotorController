/*
 * stepper.c
 *
 *  Created on: 8. juuni 2018
 *      Author: JRE
 */


#include "stepper.h"
#include "driverlib.h"
#include "register.h"

/************************** Private function forward declarations *****************************/
Private void TA1_0_IRQHandler(void);


/***************************Private variable declarations *************************************/

/* TODO : Rename this. */
//Lo priority timer for motor This is currently used for motor tests.
Private const Timer_A_UpModeConfig lo_prio_timer_config =
{
     .captureCompareInterruptEnable_CCR0_CCIE = TIMER_A_CCIE_CCR0_INTERRUPT_ENABLE, /* We enable capture compare, since this is a periodic timer. */
     .clockSource = TIMER_A_CLOCKSOURCE_SMCLK,
     .clockSourceDivider = TIMER_A_CLOCKSOURCE_DIVIDER_8, //Currently divided by 8.
     .timerClear = TIMER_A_DO_CLEAR,
     .timerInterruptEnable_TAIE = TIMER_A_TAIE_INTERRUPT_DISABLE, //Disable general interrupt.
     //.timerPeriod = 56040u,     //For 1 RPM
     //.timerPeriod = 1868u,    //For 30RPM
     //.timerPeriod = 934u,     //For 60  RPM
     .timerPeriod = 233u,     //For 240 RPM
     //.timerPeriod = 60u //For 240 RPM on 1/32
};


/************************** Public function definitions **************************************/

Public void stepper_init(void)
{
    /* Set up timer 1 */
    Timer_A_configureUpMode(TIMER_A1_BASE, &lo_prio_timer_config);
    Timer_A_registerInterrupt(TIMER_A1_BASE, TIMER_A_CCR0_INTERRUPT, TA1_0_IRQHandler);
    Timer_A_startCounter(TIMER_A1_BASE, TIMER_A_UP_MODE);

    Interrupt_setPriority(INT_TA1_0, 255u);
    Interrupt_enableInterrupt(INT_TA1_0);
}


/* TODO : This is temporary, in the future should set speed in other units. */
Public void stepper_setTimerValue(U32 value)
{
    __disable_interrupts();
    if(value > 0u)
    {
        TA1CCR0 = value;
    }
    __enable_interrupt();
}



/********************* Private function definitions ******************************************/

//Fired every 20 mseconds, this is motor prio interrupt handler.
Private void TA1_0_IRQHandler(void)
{
    //static U8 motor_state = 0u;
    static U8 led_count = 0u;
    Timer_A_clearCaptureCompareInterrupt(TIMER_A1_BASE, TIMER_A_CAPTURECOMPARE_REGISTER_0);

    led_count++;
    if(led_count >= 3u)
    {
        led_count = 0u;
    }

    switch(led_count)
    {
    case 0u:
        set_led_two_blue(1u);
        set_led_two_red(0u);
        set_led_two_green(0u);
        break;
    case 1u:
        set_led_two_blue(0u);
        set_led_two_red(1u);
        set_led_two_green(0u);
        break;
    case 2u:
        set_led_two_blue(0u);
        set_led_two_red(0u);
        set_led_two_green(1u);
        break;
    default:
        break;
    }

    set_test_motor_port(1u);
    __delay_cycles(25);
    set_test_motor_port(0u);
}
