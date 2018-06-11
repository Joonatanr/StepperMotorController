/*
 * register.c
 *
 *  Created on: Jun 3, 2018
 *      Author: Joonatan
 */


#include "register.h"
#include "uartmgr.h"
#include <driverlib.h>

/***************************** Private function forward declarations ********************/

Private void timer_init(void);
Private void ports_init(void);
Private void clocks_init(void);

/* Interrupt handlers */
Private void TA0_0_IRQHandler(void);


/***************************** Private variable declarations ****************************/

//Hi priority timer runs at 10msec interval.
Private const Timer_A_UpModeConfig hi_prio_timer_config =
{
     .captureCompareInterruptEnable_CCR0_CCIE = TIMER_A_CCIE_CCR0_INTERRUPT_ENABLE, /* We enable capture compare, since this is a periodic timer. */
     .clockSource = TIMER_A_CLOCKSOURCE_SMCLK,
     .clockSourceDivider = TIMER_A_CLOCKSOURCE_DIVIDER_16, //Currently divided by 16.
     .timerClear = TIMER_A_DO_CLEAR,
     .timerInterruptEnable_TAIE = TIMER_A_TAIE_INTERRUPT_DISABLE, //Disable general interrupt.
     .timerPeriod = 7500u
};


Private volatile U16 priv_delay_counter = 0u;


/***************************** Public function definitions ******************************/

Public void register_init(void)
{
    /* Initialize the system clocks as the first thing. */
    clocks_init();

    /* Initialize I/O port directions. */
    ports_init();

    /* Initialize main timer */
    timer_init();
}


Public void register_enable_low_powermode(void)
{
    //Go to low power mode with interrupts.
    while(1)
    {
        PCM_gotoLPM0();
    }
}


#pragma FUNCTION_OPTIONS(delay_msec, "--opt_level=off")
Public void delay_msec(U16 msec)
{
    priv_delay_counter = msec / 10;
    while(priv_delay_counter > 0u);
}

/***************************** Private function definitions ******************************/

Private void clocks_init(void)
{
    WDT_A_holdTimer();

    //Lets configure the DCO to 12MHz
    CS_setDCOCenteredFrequency(CS_DCO_FREQUENCY_12);

    /* Initializing the clock source as follows:
     *      MCLK = MODOSC/2 = 12MHz
     *      HSMCLK = DCO/2 = 6Mhz
     *      SMCLK = DCO =  12MHz
     */
    MAP_CS_initClockSignal(CS_MCLK,     CS_MODOSC_SELECT,   CS_CLOCK_DIVIDER_2);
    MAP_CS_initClockSignal(CS_HSMCLK,   CS_DCOCLK_SELECT,   CS_CLOCK_DIVIDER_2);
    MAP_CS_initClockSignal(CS_SMCLK,    CS_DCOCLK_SELECT,   CS_CLOCK_DIVIDER_1);
}


Private void timer_init(void)
{
    //Set up timer 0

    //Set up timer for high priority interrupts.
    Timer_A_configureUpMode(TIMER_A0_BASE, &hi_prio_timer_config);
    Timer_A_registerInterrupt(TIMER_A0_BASE, TIMER_A_CCR0_INTERRUPT, TA0_0_IRQHandler);
    Timer_A_startCounter(TIMER_A0_BASE, TIMER_A_UP_MODE);

    //Enable this interrupt in NVIC.
    //Interrupt_setPriority(INT_TA0_0, 254u);
    Interrupt_setPriority(INT_TA0_0, 4u); /* TODO : 4U has been chosen quite randomly... */
    Interrupt_enableInterrupt(INT_TA0_0);
}


Private void ports_init(void)
{
    //First lets set up LED ports as outputs.
    GPIO_setAsOutputPin(GPIO_PORT_P1, GPIO_PIN0);
    GPIO_setAsOutputPin(GPIO_PORT_P2, GPIO_PIN0 | GPIO_PIN1 | GPIO_PIN2);

    //Set test motor pin as output.
    GPIO_setAsOutputPin(GPIO_PORT_P5, GPIO_PIN2);
}


/* This should be fired every 10 msec */
//Hi priority interrupt handler.
Private void TA0_0_IRQHandler(void)
{
    Timer_A_clearCaptureCompareInterrupt(TIMER_A0_BASE, TIMER_A_CAPTURECOMPARE_REGISTER_0);
    if (priv_delay_counter > 0u)
    {
        priv_delay_counter--;
    }

    timer_10msec_callback();
}



/****************************************************************************************
 * INPUT OUTPUT PORTS
 ****************************************************************************************/

Public void set_led_one(U8 state)
{
    if (state == 1u)
    {
        GPIO_setOutputHighOnPin(GPIO_PORT_P1, GPIO_PIN0);
    }
    else
    {
        GPIO_setOutputLowOnPin(GPIO_PORT_P1, GPIO_PIN0);
    }
}

Public void set_led_two_red(U8 state)
{
    if (state == 1u)
    {
        GPIO_setOutputHighOnPin(GPIO_PORT_P2, GPIO_PIN0);
    }
    else
    {
        GPIO_setOutputLowOnPin(GPIO_PORT_P2, GPIO_PIN0);
    }
}

Public void set_led_two_green(U8 state)
{
    if (state == 1u)
    {
        GPIO_setOutputHighOnPin(GPIO_PORT_P2, GPIO_PIN1);
    }
    else
    {
        GPIO_setOutputLowOnPin(GPIO_PORT_P2, GPIO_PIN1);
    }
}

Public void set_led_two_blue(U8 state)
{
    if (state == 1u)
    {
        GPIO_setOutputHighOnPin(GPIO_PORT_P2, GPIO_PIN2);
    }
    else
    {
        GPIO_setOutputLowOnPin(GPIO_PORT_P2, GPIO_PIN2);
    }
}


Public void set_test_motor_port(U8 state)
{
    if (state == 1u)
    {
        GPIO_setOutputHighOnPin(GPIO_PORT_P5, GPIO_PIN2);
    }
    else
    {
        GPIO_setOutputLowOnPin(GPIO_PORT_P5, GPIO_PIN2);
    }
}
