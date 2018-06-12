/*
 * comm.c
 *
 *  Created on: 28. mai 2017
 *      Author: Joonatan
 */

/* This file handles communication over the UART interface.
 * Unfortunately driverlib already has a source file named uart.c */

#include <uart.h>
#include <driverlib.h>
#include "typedefs.h"
#include "uartmgr.h"
#include "misc.h"
#include "stepper.h"
#include <stdlib.h>


/********************************************* Private definitions ************************************/

typedef Boolean (*CommandHandlerFunc)(char * data, U8 len);

typedef struct
{
    char prefix;
    CommandHandlerFunc handler_fptr;
} CommandHandler_T;


/************************************* Private function prototypes ************************************/

Private Boolean HandleCommandTimerSet(char * data, U8 len);
Private Boolean HandleCommandRPMSet(char * data, U8 len);


/************************************* Private data declarations **************************************/

Private U8 priv_receive_buffer[UART_BUF_LEN];
Private char priv_uart_buffer[UART_BUF_LEN];
Private U8 priv_receive_cnt;
Private U8 priv_receive_flag = 0;
Private char priv_char_buf[16];

Private const CommandHandler_T priv_command_handlers[] =
{
     { .prefix = 'T', .handler_fptr = HandleCommandTimerSet },
     { .prefix = 'R', .handler_fptr = HandleCommandRPMSet   }
};


/* Set to 9600 baudrate */
/* TODO : Review this.  */
Private const eUSCI_UART_Config uartConfig =
{
        EUSCI_A_UART_CLOCKSOURCE_SMCLK,          // SMCLK Clock Source
        78,                                      // BRDIV = 78
        2,                                       // UCxBRF = 2
        0,                                       // UCxBRS = 0
        EUSCI_A_UART_NO_PARITY,                  // No Parity
        EUSCI_A_UART_LSB_FIRST,                  // LSB First
        EUSCI_A_UART_ONE_STOP_BIT,               // One stop bit
        EUSCI_A_UART_MODE,                       // UART mode
        EUSCI_A_UART_OVERSAMPLING_BAUDRATE_GENERATION  // Oversampling
};

Public void uartmgr_send_char(char c)
{
    UART_transmitData(EUSCI_A0_BASE, c);

    //So should we also wait until data has been sent out???
    //We should not send any chars out if UART is busy.
}

Public void uartmgr_send_str(const char * str)
{
    const char * ps = str;
    while(*ps)
    {
        uartmgr_send_char(*ps);
        ps++;
    }
}


Public void uartmgr_send_number(long nr)
{
    U32 len = long2string(nr, priv_char_buf);
    priv_char_buf[len] = 0;
    uartmgr_send_str(priv_char_buf);
}

Public void uartmgr_send_rn(void)
{
    uartmgr_send_char('\r');
    uartmgr_send_char('\n');
}

/* EUSCI A0 UART ISR */
/* Handles incoming data over the UART.*/
Public void EUSCIA0_IRQHandler(void)
{
    uint32_t status = UART_getEnabledInterruptStatus(EUSCI_A0_BASE);
    U8 c;

    UART_clearInterruptFlag(EUSCI_A0_BASE, status);

    if (status & EUSCI_A_UART_RECEIVE_INTERRUPT_FLAG)
    {
        //MAP_UART_transmitData(EUSCI_A0_BASE, MAP_UART_receiveData(EUSCI_A0_BASE));
        c = UART_receiveData(EUSCI_A0_BASE);

        if ((priv_receive_cnt < UART_BUF_LEN) && (priv_receive_flag == 0u))
        {
            priv_receive_buffer[priv_receive_cnt] = c;
            priv_receive_cnt++;

            if (c == '\n')
            {
                //We got new data that needs to be processed.
                priv_receive_flag = 1u;
            }
        }
        else
        {
            //Silently discard data to prevent buffer overflow.
        }
    }
}


Public U8 uartmgr_receiveData(char * dest)
{
    //Return length of received data. If none received then return 0.
    U8 res = 0u;

    if (priv_receive_flag)
    {
        res = priv_receive_cnt;
        (void)memcpy(dest, priv_receive_buffer, priv_receive_cnt);
        priv_receive_cnt = 0u;
        priv_receive_flag = 0u;
    }

    return res;
}


Public void uartmgr_init(void)
{
    /* Selecting P1.2 and P1.3 in UART mode */
    MAP_GPIO_setAsPeripheralModuleFunctionInputPin(GPIO_PORT_P1,
            GPIO_PIN1 | GPIO_PIN2 | GPIO_PIN3, GPIO_PRIMARY_MODULE_FUNCTION);

    /* Configuring UART Module */
    MAP_UART_initModule(EUSCI_A0_BASE, &uartConfig);

    /* Enable UART module */
    MAP_UART_enableModule(EUSCI_A0_BASE);

    /* Enabling interrupts */
    MAP_UART_enableInterrupt(EUSCI_A0_BASE, EUSCI_A_UART_RECEIVE_INTERRUPT);
    MAP_Interrupt_enableInterrupt(INT_EUSCIA0);
}


Public void uartmgr_cyclic(void)
{
    U8 msg_len;
    U8 ix;
    Boolean res = FALSE;

    msg_len = uartmgr_receiveData(priv_uart_buffer);

    if (msg_len > 0u)
    {
        for (ix = 0u; ix < NUMBER_OF_ITEMS(priv_command_handlers); ix++)
        {
            if (priv_uart_buffer[0] == priv_command_handlers[ix].prefix)
            {
               res = priv_command_handlers[ix].handler_fptr(priv_uart_buffer + 1, msg_len - 1u);
               break;
            }
        }

        if (res)
        {
            /* Send positive response */
            uartmgr_send_str("OK");
            uartmgr_send_rn();
        }
        else
        {
           /* Send error response   */
            uartmgr_send_str("ERR");
            uartmgr_send_rn();
        }
    }
}


/********************************* Command handlers ********************************/

Private Boolean HandleCommandTimerSet(char * data, U8 len)
{
    Boolean res = FALSE;
    long parsedValue;

    if (len > 0u)
    {
        parsedValue = atol(data);

        if (parsedValue > 0)
        {
            stepper_setTimerValue(parsedValue);
            res = TRUE;
        }
    }

    return res;
}


Private Boolean HandleCommandRPMSet(char * data, U8 len)
{
    Boolean res = FALSE;
    int parsedValue;

    if (len > 0u)
    {
        parsedValue = atoi(data);
        if (parsedValue >= 0)
        {
            res = stepper_setSpeed((U32)parsedValue);
        }
    }

    return res;
}



