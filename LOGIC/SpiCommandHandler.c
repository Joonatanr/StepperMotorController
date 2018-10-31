/*
 * SpiCommandHandler.c
 *
 *  Created on: 30. okt 2018
 *      Author: JRE
 */

#include "SpiCommandHandler.h"
#include "spidrv.h"

#define SPI_COMMAND_LENGTH MAP_SPI_MSG_LENGTH

#define INITIAL_CRC_16     0xFFFFu

#define CMD_HEADER_LEN 7u
#define CMD_CHECKSUM_LEN 2u

#define CMD_METADATA_LEN (CMD_HEADER_LEN + CMD_CHECKSUM_LEN)
#define CMD_DATA_LEN SPI_COMMAND_LENGTH - CMD_METADATA_LEN

#define CMD_PACKET_BEGIN 0xFFFEu

/* Connection to SPI driver. */
Public SpiCommandHandlerFunc spidrv_callback = SpiCommandHandler_handleCommand;


/**************************************** Private function forward declarations  **********************************************/

Private U16 calculate_crc16(U16 crc, const U8* data, U32 len);
Private Boolean handleCommand(U8 cmd_id, U8 sub, U8 * args, U8 args_len);

/**************************************** Private variable declarations **********************************************/

/** 16 bits CRC CCITT table (x^16+x^12+x^5+1), precalculated for fast table look-ups. */
Private const U16 priv_crcTable_16[256] = {
   /* CCITT */
   /* x^16+x^12+x^5+1 */
   0x0000u, 0x1021u, 0x2042u, 0x3063u, 0x4084u, 0x50a5u, 0x60c6u, 0x70e7u,
   0x8108u, 0x9129u, 0xa14au, 0xb16bu, 0xc18cu, 0xd1adu, 0xe1ceu, 0xf1efu,
   0x1231u, 0x0210u, 0x3273u, 0x2252u, 0x52b5u, 0x4294u, 0x72f7u, 0x62d6u,
   0x9339u, 0x8318u, 0xb37bu, 0xa35au, 0xd3bdu, 0xc39cu, 0xf3ffu, 0xe3deu,
   0x2462u, 0x3443u, 0x0420u, 0x1401u, 0x64e6u, 0x74c7u, 0x44a4u, 0x5485u,
   0xa56au, 0xb54bu, 0x8528u, 0x9509u, 0xe5eeu, 0xf5cfu, 0xc5acu, 0xd58du,
   0x3653u, 0x2672u, 0x1611u, 0x0630u, 0x76d7u, 0x66f6u, 0x5695u, 0x46b4u,
   0xb75bu, 0xa77au, 0x9719u, 0x8738u, 0xf7dfu, 0xe7feu, 0xd79du, 0xc7bcu,
   0x48c4u, 0x58e5u, 0x6886u, 0x78a7u, 0x0840u, 0x1861u, 0x2802u, 0x3823u,
   0xc9ccu, 0xd9edu, 0xe98eu, 0xf9afu, 0x8948u, 0x9969u, 0xa90au, 0xb92bu,
   0x5af5u, 0x4ad4u, 0x7ab7u, 0x6a96u, 0x1a71u, 0x0a50u, 0x3a33u, 0x2a12u,
   0xdbfdu, 0xcbdcu, 0xfbbfu, 0xeb9eu, 0x9b79u, 0x8b58u, 0xbb3bu, 0xab1au,
   0x6ca6u, 0x7c87u, 0x4ce4u, 0x5cc5u, 0x2c22u, 0x3c03u, 0x0c60u, 0x1c41u,
   0xedaeu, 0xfd8fu, 0xcdecu, 0xddcdu, 0xad2au, 0xbd0bu, 0x8d68u, 0x9d49u,
   0x7e97u, 0x6eb6u, 0x5ed5u, 0x4ef4u, 0x3e13u, 0x2e32u, 0x1e51u, 0x0e70u,
   0xff9fu, 0xefbeu, 0xdfddu, 0xcffcu, 0xbf1bu, 0xaf3au, 0x9f59u, 0x8f78u,
   0x9188u, 0x81a9u, 0xb1cau, 0xa1ebu, 0xd10cu, 0xc12du, 0xf14eu, 0xe16fu,
   0x1080u, 0x00a1u, 0x30c2u, 0x20e3u, 0x5004u, 0x4025u, 0x7046u, 0x6067u,
   0x83b9u, 0x9398u, 0xa3fbu, 0xb3dau, 0xc33du, 0xd31cu, 0xe37fu, 0xf35eu,
   0x02b1u, 0x1290u, 0x22f3u, 0x32d2u, 0x4235u, 0x5214u, 0x6277u, 0x7256u,
   0xb5eau, 0xa5cbu, 0x95a8u, 0x8589u, 0xf56eu, 0xe54fu, 0xd52cu, 0xc50du,
   0x34e2u, 0x24c3u, 0x14a0u, 0x0481u, 0x7466u, 0x6447u, 0x5424u, 0x4405u,
   0xa7dbu, 0xb7fau, 0x8799u, 0x97b8u, 0xe75fu, 0xf77eu, 0xc71du, 0xd73cu,
   0x26d3u, 0x36f2u, 0x0691u, 0x16b0u, 0x6657u, 0x7676u, 0x4615u, 0x5634u,
   0xd94cu, 0xc96du, 0xf90eu, 0xe92fu, 0x99c8u, 0x89e9u, 0xb98au, 0xa9abu,
   0x5844u, 0x4865u, 0x7806u, 0x6827u, 0x18c0u, 0x08e1u, 0x3882u, 0x28a3u,
   0xcb7du, 0xdb5cu, 0xeb3fu, 0xfb1eu, 0x8bf9u, 0x9bd8u, 0xabbbu, 0xbb9au,
   0x4a75u, 0x5a54u, 0x6a37u, 0x7a16u, 0x0af1u, 0x1ad0u, 0x2ab3u, 0x3a92u,
   0xfd2eu, 0xed0fu, 0xdd6cu, 0xcd4du, 0xbdaau, 0xad8bu, 0x9de8u, 0x8dc9u,
   0x7c26u, 0x6c07u, 0x5c64u, 0x4c45u, 0x3ca2u, 0x2c83u, 0x1ce0u, 0x0cc1u,
   0xef1fu, 0xff3eu, 0xcf5du, 0xdf7cu, 0xaf9bu, 0xbfbau, 0x8fd9u, 0x9ff8u,
   0x6e17u, 0x7e36u, 0x4e55u, 0x5e74u, 0x2e93u, 0x3eb2u, 0x0ed1u, 0x1ef0u
};


Private U8 priv_response_buffer[SPI_COMMAND_LENGTH];



/************************************** Public function definitions ********************************************/


Public Boolean SpiCommandHandler_handleCommand(U8 * message)
{
    U8 ix = 0u;
    U8 * data_ptr = NULL;
    U8 * packet_begin_ptr = NULL;

    U16 packet_len;
    Boolean res;

    U16 calculated_checksum;
    U16 sent_checksum;

    U8 cmd_id;
    U8 sub_id;
    //U8 resp_code;

    /* Begin by looking for the packet beginning. */
    while (ix < (SPI_COMMAND_LENGTH - 2u))
    {
        if ((message[ix] == (U8)CMD_PACKET_BEGIN >> 8u) && (message[ix + 1u] == (U8)CMD_PACKET_BEGIN & 0xffu))
        {
            packet_begin_ptr = &message[ix];
            break;
        }

        ix++;
    }

    if (packet_begin_ptr != NULL)
    {
        data_ptr = packet_begin_ptr + 2u;

        packet_len = data_ptr[0] << 8u;
        packet_len |= data_ptr[1];

        data_ptr += 2;

        if ((packet_len < CMD_METADATA_LEN) || (packet_len > SPI_COMMAND_LENGTH))
        {
            /* Got malformed response, packet length is out of bounds.  */
            res = FALSE;
        }
        else
        {
            /* Lets verify the checksum. */
            calculated_checksum = calculate_crc16(INITIAL_CRC_16, packet_begin_ptr, packet_len - CMD_CHECKSUM_LEN);
            sent_checksum = (U16)((U16)packet_begin_ptr[packet_len - 2u] << 8u);
            sent_checksum |= packet_begin_ptr[packet_len - 1u];

            if (calculated_checksum == sent_checksum)
            {
                cmd_id = *data_ptr;
                data_ptr++;
                sub_id = *data_ptr;
                data_ptr++;
                //resp_code = *data_ptr;
                data_ptr++;

                /* Command is in correct format... */
                res = TRUE;

                /* Pass response on to command handler... */
                handleCommand(cmd_id, sub_id, data_ptr, packet_len - CMD_METADATA_LEN);
            }
            else
            {
                res = FALSE;
            }
        }
    }
    else
    {
        /* Could not find packet header, response is malformed. */
        res = FALSE;
    }

    return res;
}


/******************************************* Private function definitions *******************************************/

Private Boolean handleCommand(U8 cmd_id, U8 sub, U8 * args, U8 args_len)
{
    U16 checksum;
    U16 packet_length = 13u;

    /* Lets prepare a simple test response. */
    priv_response_buffer[0] = 0xffu;
    priv_response_buffer[1] = 0xfeu;
    priv_response_buffer[2] = 0x00u;
    priv_response_buffer[3] = (U8)packet_length;

    priv_response_buffer[4] = cmd_id;
    priv_response_buffer[5] = sub;
    priv_response_buffer[6] = (U8)SPI_RESPONSE_ACK;

    /* Lets write 4 data bytes, just for testing. */
    priv_response_buffer[7]     = 0xDEu;
    priv_response_buffer[8]     = 0xADu;
    priv_response_buffer[9]     = 0xBEu;
    priv_response_buffer[10]    = 0xEFu;

    checksum = calculate_crc16(INITIAL_CRC_16, priv_response_buffer, packet_length - CMD_CHECKSUM_LEN);
    priv_response_buffer[11] = (U8)(checksum >> 8u);
    priv_response_buffer[12] = (U8)(checksum & 0xffu);

    spidrv_setResponse(priv_response_buffer, packet_length);
    return TRUE;

}


Private U16 calculate_crc16
(
   U16 crc,        /**< [in] Initial checksum value. */
   const U8* data, /**< [in] Pointer to data for calculate checksum for. */
   U32 len         /**< [in] Number of bytes to calculate checksum over. */
)
{
   while (len)
   {
      crc = priv_crcTable_16[((U16)(crc >> 8) ^ (*data)) & 0xffu] ^ (U16)(crc << 8);
      data++;
      len--;
   }

   return crc;
} /* util_crc16() */

