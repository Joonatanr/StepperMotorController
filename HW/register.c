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

Private void mainTimerInterrupt(void);


/***************************** Private variable declarations ****************************/



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
    priv_delay_counter = msec;
    while(priv_delay_counter > 0u);
}

/***************************** Private function definitions ******************************/

//#define CLOCK_FREQ 12u
#define CLOCK_FREQ 24u

Private void clocks_init(void)
{
    WDT_A_holdTimer();

#if (CLOCK_FREQ == 12u)
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
#elif (CLOCK_FREQ == 24u)
    /* Setting DCO to 24MHz (upping Vcore) */
    FlashCtl_setWaitState(FLASH_BANK0, 2);
    FlashCtl_setWaitState(FLASH_BANK1, 2);
    MAP_PCM_setCoreVoltageLevel(PCM_VCORE1);
    CS_setDCOCenteredFrequency(CS_DCO_FREQUENCY_24);

    /* Initializing the clock source as follows:
     *      MCLK =      DCO  =   24MHz
     *      HSMCLK =    DCO/2 =  12Mhz
     *      SMCLK =     DCO/2 =  12MHz
     */

    MAP_CS_initClockSignal(CS_MCLK,     CS_DCOCLK_SELECT,   CS_CLOCK_DIVIDER_1);
    MAP_CS_initClockSignal(CS_HSMCLK,   CS_DCOCLK_SELECT,   CS_CLOCK_DIVIDER_2);
    MAP_CS_initClockSignal(CS_SMCLK,    CS_DCOCLK_SELECT,   CS_CLOCK_DIVIDER_2);

#else
#error "Clock frequency not defined"
#endif
}


Private void timer_init(void)
{
    /* Set up the 32-bit timer */

    /* Main clock is at 24MHz                           */
    /* Set up the 32 bit timer for 10msec interrupts    */

    Timer32_initModule(TIMER32_BASE, TIMER32_PRESCALER_16, TIMER32_32BIT, TIMER32_PERIODIC_MODE);
    Timer32_registerInterrupt(TIMER32_0_INTERRUPT, mainTimerInterrupt);

    Timer32_setCount(TIMER32_0_BASE, 15000u);
    Timer32_startTimer(TIMER32_0_BASE, FALSE);

}


/* Called every 1 msec */
Private void mainTimerInterrupt(void)
{
    static U8 counter;

    Timer32_clearInterruptFlag(TIMER32_0_BASE);

    if (priv_delay_counter > 0u)
    {
        priv_delay_counter--;
    }

    if (++counter >= 10u)
    {
        timer_10msec_callback();
    }

}


Private void ports_init(void)
{
    //First lets set up LED ports as outputs.
    GPIO_setAsOutputPin(GPIO_PORT_P1, GPIO_PIN0);
    GPIO_setAsOutputPin(GPIO_PORT_P2, GPIO_PIN0 | GPIO_PIN1 | GPIO_PIN2);

    //Set test motor pin as output.
    //GPIO_setAsOutputPin(GPIO_PORT_P5, GPIO_PIN2);
}


/****************************************************************************************
 * INPUT OUTPUT PORTS
 ****************************************************************************************/

Public void ports_setPort(U32 port, U32 pin, Boolean value)
{
    if (value)
    {
        GPIO_setOutputHighOnPin(port, pin);
    }
    else
    {
        GPIO_setOutputLowOnPin(port, pin);
    }
}


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
