#include "msp.h"
#include "typedefs.h"
#include "register.h"


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
#if 0
	while(1)
	{
	    delay_msec(10);
	    set_test_motor_port(1u);
	    __delay_cycles(100);
	    set_test_motor_port(0u);
	}
#endif
	register_enable_low_powermode();
}


/******************************* Private function definitions *********************************/

Private void timer_10msec(void)
{
    static U8 second_cnt = 0u;
    //static U8 motor_state;

    //motor_state = !motor_state;
    //set_test_motor_port(motor_state);

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
