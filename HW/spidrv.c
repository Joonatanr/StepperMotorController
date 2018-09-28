/*
 * spidrv.c
 *
 *  Created on: 28. sept 2018
 *      Author: JRE
 */

#include "spidrv.h"
#include "driverlib.h"

/* This file should contain the low-level SPI slave mode driver. */
/* SPI Configuration Parameter */

/* Ports :
 *
 * 3.6 - SIMO
 * 3.7 - SOMI
 * 3.5 - CLK
 *
 * */


const eUSCI_SPI_SlaveConfig spiSlaveConfig =
{ EUSCI_B_SPI_MSB_FIRST,
        EUSCI_B_SPI_PHASE_DATA_CAPTURED_ONFIRST_CHANGED_ON_NEXT,
        EUSCI_B_SPI_CLOCKPOLARITY_INACTIVITY_HIGH,
        EUSCI_B_SPI_3PIN
        };

/* DMA Control Table */
#if defined(__TI_COMPILER_VERSION__)
#pragma DATA_ALIGN(MSP_EXP432P401RLP_DMAControlTable, 1024)
#elif defined(__IAR_SYSTEMS_ICC__)
#pragma data_alignment=1024
#elif defined(__GNUC__)
__attribute__ ((aligned (1024)))
#elif defined(__CC_ARM)
__align(1024)
#endif
static DMA_ControlTable MSP_EXP432P401RLP_DMAControlTable[32];

#define MAP_SPI_MSG_LENGTH    32u /* This must probably be the same as in master. */

uint8_t sltxData[MAP_SPI_MSG_LENGTH] = "Hello, this is slave SPI";
uint8_t slrxData[MAP_SPI_MSG_LENGTH] =
{ 0 };


Public void spidrv_init(void)
{
    /* Configure SLAVE CLK, MOSI and SPMI (EUSCI_B2) */
    MAP_GPIO_setAsPeripheralModuleFunctionInputPin(GPIO_PORT_P3,
            GPIO_PIN5 | GPIO_PIN6 | GPIO_PIN7,
            GPIO_PRIMARY_MODULE_FUNCTION);

    /* TODO : Check this, why are all pins configured as input pins??? */

    /* Configuring SPI module */
    MAP_SPI_initSlave(EUSCI_B2_BASE, &spiSlaveConfig);

    /* Enable the SPI module */
    MAP_SPI_enableModule(EUSCI_B2_BASE);

    /* Configuring DMA module */
    MAP_DMA_enableModule();
    MAP_DMA_setControlBase(MSP_EXP432P401RLP_DMAControlTable);

    MAP_DMA_assignChannel(DMA_CH4_EUSCIB2TX0);
    MAP_DMA_assignChannel(DMA_CH5_EUSCIB2RX0);

    /* General setup is complete from here on. */

    /* TODO : Consider initiating the transmission with CS pin? */

    /* Setup the transfer. Currently only 1 for testing. */

    /* Slave Settings */
    /* Setup the TX transfer characteristics and buffers */
    MAP_DMA_setChannelControl(DMA_CH4_EUSCIB2TX0 | UDMA_PRI_SELECT,
    UDMA_SIZE_8 | UDMA_SRC_INC_8 | UDMA_DST_INC_NONE | UDMA_ARB_1);
    MAP_DMA_setChannelTransfer(DMA_CH4_EUSCIB2TX0 | UDMA_PRI_SELECT,
    UDMA_MODE_BASIC, sltxData,
            (void *) MAP_SPI_getTransmitBufferAddressForDMA(EUSCI_B2_BASE),
            MAP_SPI_MSG_LENGTH);

    /* Setup the RX transfer characteristics & buffers */
    MAP_DMA_setChannelControl(DMA_CH5_EUSCIB2RX0 | UDMA_PRI_SELECT,
    UDMA_SIZE_8 | UDMA_SRC_INC_NONE | UDMA_DST_INC_8 | UDMA_ARB_1);
    MAP_DMA_setChannelTransfer(DMA_CH5_EUSCIB2RX0 | UDMA_PRI_SELECT,
    UDMA_MODE_BASIC,
            (void *) MAP_SPI_getReceiveBufferAddressForDMA(EUSCI_B2_BASE),
            slrxData,
            MAP_SPI_MSG_LENGTH);


    /* Enable DMA interrupt */
    MAP_DMA_assignInterrupt(INT_DMA_INT1, 5); /* Channel 5 is used for Slave Rx */
    MAP_DMA_clearInterruptFlag(DMA_CH5_EUSCIB2RX0 & 0x0F);

    /* Assigning/Enabling Interrupts */
    MAP_Interrupt_enableInterrupt(INT_DMA_INT1);
    MAP_DMA_enableInterrupt(INT_DMA_INT1);
    MAP_DMA_enableChannel(5);
    MAP_DMA_enableChannel(4);

}


void DMA_INT1_IRQHandler(void)
{
    MAP_DMA_clearInterruptFlag(4);
    MAP_DMA_clearInterruptFlag(5);


    /* Disable the interrupt to allow execution */
    MAP_Interrupt_disableInterrupt(INT_DMA_INT1);
    MAP_DMA_disableInterrupt(INT_DMA_INT1);
}
