/*
 * stepper.h
 *
 *  Created on: 8. juuni 2018
 *      Author: JRE
 */

#ifndef HW_STEPPER_H_
#define HW_STEPPER_H_

#include "typedefs.h"

typedef enum
{
    STEPPER1,
    NUMBER_OF_STEPPERS
} Stepper_Id;

extern void stepper_init(void);
extern Boolean stepper_setSpeed(U32 rpm, Stepper_Id id);
extern void stepper_setTimerValue(U32 value, Stepper_Id id);


#endif /* HW_STEPPER_H_ */
