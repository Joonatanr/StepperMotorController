#include "msp.h"
#include "typedefs.h"
#include "register.h"
#include "uartmgr.h"
#include "stepper.h"
#include "commandHandler.h"
#include "frequency.h"
#include "spidrv.h"
#include "Scheduler.h"
#include "led.h"

/**
 * main.c
 */

Private void timer_10msec(void);

Public TimerHandler timer_10msec_callback = timer_10msec;

void main(void)
{
	WDT_A->CTL = WDT_A_CTL_PW | WDT_A_CTL_HOLD;		// stop watchdog timer

	/* Low level HW init. */
	register_init();

	/* TODO : Move these init functions also under the scheduler in the future. */

	/* Initialize USART hardware. */
    uartmgr_init();

    /* Initialize frequency generator */
    frequency_init();

    stepper_init();

    /* Initialize command handler. */
    commandHandler_init();

    set_led_two_blue(TRUE);
    set_led_two_green(FALSE);
    set_led_two_red(TRUE);

    delay_msec(3000u);

    set_led_two_blue(FALSE);
    set_led_two_green(FALSE);
    set_led_two_red(FALSE);


    /* Initialize main scheduler.*/
    Scheduler_initTasks();

    /* Start tasks */
    Scheduler_StartTasks();

	while(1)
	{
	    /* Currently just continuously poll the USART module */
	    /* This basically constitutes the background layer, taking up any free CPU time. */
	    uartmgr_cyclic();
	}

	register_enable_low_powermode();
}


/******************************* Private function definitions *********************************/

Private void timer_10msec(void)
{
    /* Main scheduler is called here, might have to call this more frequently... */
    Scheduler_cyclic10ms();
}


