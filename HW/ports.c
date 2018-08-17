/*
 * ports.c
 *
 *  Created on: 17. aug 2018
 *      Author: JRE
 */

#include "ports.h"

/* TODO : Fully develop this module -> All IO ports should be defined there. */


Public void ports_init(void)
{
    GPIO_setAsOutputPin(GPIO_PORT_P1, GPIO_PIN0);
    GPIO_setAsOutputPin(GPIO_PORT_P2, GPIO_PIN0 | GPIO_PIN1 | GPIO_PIN2);
}


Public void ports_setPort(U32 port, U32 pin, Boolean value)
{
    if (value)
    {
        GPIO_setOutputHighOnPin(port, pin);
    }
    else
    {
        GPIO_setOutputLowOnPin(port, pin);
    }
}


