/*
 * spidrv.h
 *
 *  Created on: 28. sept 2018
 *      Author: JRE
 */

#ifndef HW_SPIDRV_H_
#define HW_SPIDRV_H_


#include "typedefs.h"

#define MAP_SPI_MSG_LENGTH    128u /* This must be the same as in master. */

//Public Boolean SpiCommandHandler_handleCommand(U8 * message);

typedef Boolean (*SpiCommandHandlerFunc)(U8 * message);

extern SpiCommandHandlerFunc spidrv_callback;

extern void spidrv_init(void);
extern void spidrv_cyclic10ms(void);
extern void spidrv_setResponse(U8 * data, U8 data_len);


#endif /* HW_SPIDRV_H_ */
