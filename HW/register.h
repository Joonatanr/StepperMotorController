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

typedef struct
{
    U32 port;
    U32 pin;
} IOPort_t;

extern TimerHandler timer_10msec_callback;

extern void register_init(void);
extern void register_enable_low_powermode(void);
extern void delay_msec(U16 msec);

extern void ports_init(void);
extern void ports_setPort(U32 port, U32 pin, Boolean value);


#endif /* HW_REGISTER_H_ */
