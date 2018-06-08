/*
 * comm.h
 *
 *  Created on: 28. mai 2017
 *      Author: Joonatan
 */

#ifndef HW_COMM_H_
#define HW_COMM_H_

#include "typedefs.h"

#define UART_BUF_LEN 32

extern void uartmgr_send_char(char c);
extern void uartmgr_send_str(const char * str);
extern U8 uartmgr_receiveData(U8 * dest);
extern void uartmgr_send_number(long nr);
extern void uartmgr_send_rn(void);
extern void uartmgr_init(void);
extern void uartmgr_cyclic(void);

#endif /* HW_COMM_H_ */
