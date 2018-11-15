/*
 * frequency.h
 *
 *  Created on: 22. juuni 2018
 *      Author: JRE
 */

#ifndef HW_FREQUENCY_H_
#define HW_FREQUENCY_H_

#include "typedefs.h"

typedef enum
{
    FRQ_CH1,
    FRQ_CH2,
    FRQ_CH3,
    FRQ_CH4,
    FRQ_NUMBER_OF_CHANNELS
} frequency_Channel_t;

extern void frequency_init(void);

extern Boolean frequency_setStepsPerMinute(U32 frequency, frequency_Channel_t ch);
extern Boolean frequency_setInterval(U32 interval, frequency_Channel_t ch);
extern void frequency_setEnable(Boolean mode, frequency_Channel_t ch);

extern U16 frequency_getInterval(frequency_Channel_t ch);



#endif /* HW_FREQUENCY_H_ */
