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


/* TODO : This is preliminary, should add more response codes, checksum error, format error etc... */
typedef enum
{
    SPI_RESPONSE_ACK,               //0x00
    SPI_RESPONSE_NACK,              //0x01 - General Reject.
    SPI_RESPONSE_UNKNOWN_CMD,       //0x02 - Command out of range.
    SPI_RESPONSE_UNKNOWN_FORMAT,    //0x03 - Incorrect command format.
    SPI_RESPONSE_CHECKSUM_ERROR,    //0x04 - Checksum error.
    SPI_RESPONSE_OOR,               //0x05 - Out of range.
    NUMBER_OF_SPI_RESPONSE_CODES
} Spi_ResponseCode;

extern Boolean SpiCommandHandler_handleCommand(U8 * data);


#endif /* LOGIC_SPICOMMANDHANDLER_H_ */
