/*
 * register.h
 *
 *  Created on: Jun 3, 2018
 *      Author: Joonatan
 */

#ifndef HW_REGISTER_H_
#define HW_REGISTER_H_

#include "typedefs.h"

//#define CLOCK_FREQ 12u
#define CLOCK_FREQ 24u

/* Externally defined timer callback functions. */
typedef void (*TimerHandler)(void);

extern TimerHandler timer_10msec_callback;

extern void register_init(void);
extern void register_enable_low_powermode(void);

extern void delay_msec(U16 msec);

extern void set_led_one(Boolean state);
extern void set_led_two_red(Boolean state);
extern void set_led_two_green(Boolean state);
extern void set_led_two_blue(Boolean state);


#endif /* HW_REGISTER_H_ */
