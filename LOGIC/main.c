#include "msp.h"
#include "typedefs.h"
#include "register.h"
#include "uartmgr.h"


/**
 * main.c
 */

Private void timer_10msec(void);
Private void timer_1sec(void);

Public TimerHandler timer_10msec_callback = timer_10msec;

void main(void)
{
	WDT_A->CTL = WDT_A_CTL_PW | WDT_A_CTL_HOLD;		// stop watchdog timer

	register_init();

	while(1)
	{
	    /* Currently just continuously poll the USART module */
	    uartmgr_cyclic();
	}

	register_enable_low_powermode();
}


/******************************* Private function definitions *********************************/

Private void timer_10msec(void)
{
    static U8 second_cnt = 0u;

    second_cnt++;

    if (second_cnt >= 100u)
    {
        second_cnt = 0u;
        timer_1sec();
    }
}

Private void timer_1sec(void)
{
    static U8 led_state;

    led_state = !led_state;
    set_led_one(led_state);
}
