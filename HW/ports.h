/*
 * ports.h
 *
 *  Created on: 17. aug 2018
 *      Author: JRE
 */

#ifndef HW_PORTS_H_
#define HW_PORTS_H_

#include "typedefs.h"
#include <driverlib.h>

typedef struct
{
    U32 port;
    U32 pin;
} IOPort_t;


extern void ports_init(void);
extern void ports_setPort(U32 port, U32 pin, Boolean value);



#endif /* HW_PORTS_H_ */
