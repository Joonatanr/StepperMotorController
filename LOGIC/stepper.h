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
    STEPPER0,
    STEPPER1,
    STEPPER2,
    STEPPER3,
    NUMBER_OF_STEPPERS
} Stepper_Id;

extern void stepper_init(void);
extern Boolean stepper_setSpeed(U32 rpm, Stepper_Id id);
extern Boolean stepper_setTimerValue(U32 value, Stepper_Id id);
extern U16 stepper_getSpeed(Stepper_Id id);
extern Boolean stepper_setMicrosteppingMode(Stepper_Id id, U8 mode);

#endif /* HW_STEPPER_H_ */
