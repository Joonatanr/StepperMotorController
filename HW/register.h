/*
 * register.h
 *
 *  Created on: Jun 3, 2018
 *      Author: Joonatan
 */

#ifndef HW_REGISTER_H_
#define HW_REGISTER_H_

#include "typedefs.h"

/* Externally defined timer callback functions. */
typedef void (*TimerHandler)(void);

extern TimerHandler timer_10msec_callback;

extern void register_init(void);
extern void register_enable_low_powermode(void);

extern void delay_msec(U16 msec);

extern void set_led_one(U8 state);
extern void set_led_two_red(U8 state);
extern void set_led_two_green(U8 state);
extern void set_led_two_blue(U8 state);

extern void set_test_motor_port(U8 state);
extern void ports_setPort(U32 port, U32 pin, Boolean value);

#endif /* HW_REGISTER_H_ */
