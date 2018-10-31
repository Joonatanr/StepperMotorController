/*
 * spidrv.c
 *
 *  Created on: 28. sept 2018
 *      Author: JRE
 */

#include "spidrv.h"
#include "driverlib.h"
#include "uartmgr.h"
#include "led.h"

/* This file should contain the low-level SPI slave mode driver. */
/* SPI Configuration Parameter */

/* Ports :
 *
 * 3.6 - MOSI We receive data here.
 * 3.7 - MISO We send data here.
 * 3.5 - CLK
 * 6.4 - CS
 * */

#define SPIDRV_DEBUG

/*********************************** Private function forward declarations **********************************/

Private void setupTransfer(void);

/*********************************** Private variable declarations ******************************************/
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
static DMA_ControlTable MSP_EXP432P401RLP_DMAControlTable[32]; //According to documentation the size of this should correspond to the number of available DMA channels, so not buffer size.

uint8_t priv_tx_data[MAP_SPI_MSG_LENGTH] = { 0 };
uint8_t priv_rx_data[MAP_SPI_MSG_LENGTH] = { 0 };

volatile Boolean priv_is_receive_complete = FALSE;

Public void spidrv_init(void)
{
    /* Configure SLAVE CLK, MOSI and SPMI (EUSCI_B2) */
    MAP_GPIO_setAsPeripheralModuleFunctionInputPin(GPIO_PORT_P3,
            GPIO_PIN5 | GPIO_PIN6 | GPIO_PIN7,
            GPIO_PRIMARY_MODULE_FUNCTION);

    /* Configuring SPI module */
    MAP_SPI_initSlave(EUSCI_B2_BASE, &spiSlaveConfig);

    /* Enable the SPI module */
    MAP_SPI_enableModule(EUSCI_B2_BASE);

    /* Configuring DMA module */
    MAP_DMA_enableModule();
    MAP_DMA_setControlBase(MSP_EXP432P401RLP_DMAControlTable);

    MAP_DMA_assignChannel(DMA_CH4_EUSCIB2TX0);
    MAP_DMA_assignChannel(DMA_CH5_EUSCIB2RX0);

    /* Configure CS pin.    */
    /* CS pin -> 6.4        */

    /* Configuring P1.1 as an input and enabling interrupts */
    MAP_GPIO_setAsInputPin(GPIO_PORT_P6, GPIO_PIN4);
    GPIO_interruptEdgeSelect(GPIO_PORT_P6, GPIO_PIN4, GPIO_LOW_TO_HIGH_TRANSITION);

    MAP_GPIO_clearInterruptFlag(GPIO_PORT_P6, GPIO_PIN4);
    MAP_GPIO_enableInterrupt(GPIO_PORT_P6, GPIO_PIN4);
    MAP_Interrupt_enableInterrupt(INT_PORT6);

    /* Enabling SRAM Bank Retention */
    /*Why the heck do we need this??? */
    MAP_SysCtl_enableSRAMBankRetention(SYSCTL_SRAM_BANK1);

    /* General setup is complete from here on. */

}


Public void spidrv_cyclic10ms(void)
{
    Boolean res = FALSE;
    if (priv_is_receive_complete)
    {
        priv_is_receive_complete = FALSE;


        /* The device is configured as SPI slave, so we process commands. */
        if (spidrv_callback != NULL)
        {
            res = spidrv_callback(priv_rx_data);
        }

#ifdef SPIDRV_DEBUG
        if (res)
        {
            led_show_period(LED_TWO_BLUE, 200u);
        }
        else
        {
            led_show_period(LED_TWO_RED, 200u);
        }
#endif
    }
}


Public void spidrv_setResponse(U8 * data, U8 data_len)
{
    U8 ix;

    if (data_len <= MAP_SPI_MSG_LENGTH)
    {
        memcpy(priv_tx_data, data, data_len);
        for (ix = data_len; ix < MAP_SPI_MSG_LENGTH; ix++)
        {
            priv_tx_data[ix] = 0xffu;
        }
    }
}


/*********************************** Private function definitions. **********************************/

Private void setupTransfer(void)
{
    /* Lets disable the channel and hope this flushes existing data. */
    MAP_DMA_disableChannel(5);
    MAP_DMA_disableChannel(4);

    /* Disable the SPI module */
    MAP_SPI_disableModule(EUSCI_B2_BASE);

    /* Delay here??*/
    __delay_cycles(100);
    MAP_SPI_enableModule(EUSCI_B2_BASE);


    /* Slave Settings */
    /* Setup the TX transfer characteristics and buffers */
    MAP_DMA_setChannelControl(DMA_CH4_EUSCIB2TX0 | UDMA_PRI_SELECT, UDMA_SIZE_8 | UDMA_SRC_INC_8 | UDMA_DST_INC_NONE | UDMA_ARB_1);

    MAP_DMA_setChannelTransfer( DMA_CH4_EUSCIB2TX0 | UDMA_PRI_SELECT, UDMA_MODE_BASIC, priv_tx_data,
                                (void *) MAP_SPI_getTransmitBufferAddressForDMA(EUSCI_B2_BASE),
                                MAP_SPI_MSG_LENGTH);

    /* Setup the RX transfer characteristics & buffers */
    MAP_DMA_setChannelControl(DMA_CH5_EUSCIB2RX0 | UDMA_PRI_SELECT, UDMA_SIZE_8 | UDMA_SRC_INC_NONE | UDMA_DST_INC_8 | UDMA_ARB_1);

    MAP_DMA_setChannelTransfer( DMA_CH5_EUSCIB2RX0 | UDMA_PRI_SELECT, UDMA_MODE_BASIC,
                                (void *) MAP_SPI_getReceiveBufferAddressForDMA(EUSCI_B2_BASE),
                                priv_rx_data,
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

    /* Set flag for processing data. */
    priv_is_receive_complete = TRUE;

    /* Disable the interrupt to allow execution */
    MAP_Interrupt_disableInterrupt(INT_DMA_INT1);
    MAP_DMA_disableInterrupt(INT_DMA_INT1);
}

/* GPIO ISR */
void PORT6_IRQHandler(void)
{
    uint32_t status;

    status = MAP_GPIO_getEnabledInterruptStatus(GPIO_PORT_P6);
    MAP_GPIO_clearInterruptFlag(GPIO_PORT_P6, status);

    led_show_period(LED_TWO_GREEN, 300u);

    setupTransfer();
}
