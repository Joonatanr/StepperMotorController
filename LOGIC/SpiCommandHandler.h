/*
 * SpiCommandHandler.h
 *
 *  Created on: 30. okt 2018
 *      Author: JRE
 */

#ifndef LOGIC_SPICOMMANDHANDLER_H_
#define LOGIC_SPICOMMANDHANDLER_H_

#include "typedefs.h"

typedef enum
{
    CMD_NO_COMMAND,
    CMD_REPORT_STATUS,
    CMD_SET_MOTOR_SPEED,
    NUMBER_OF_SPI_COMMANDS
} Spi_CommandId;

typedef enum
{
    SPI_RESPONSE_ACK,
    SPI_RESPONSE_NACK,
    NUMBER_OF_SPI_RESPONSE_CODES
} Spi_ResponseCode;

extern Boolean SpiCommandHandler_handleCommand(U8 * data);


#endif /* LOGIC_SPICOMMANDHANDLER_H_ */
