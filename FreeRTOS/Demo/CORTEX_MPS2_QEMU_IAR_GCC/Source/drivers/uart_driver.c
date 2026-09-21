#include "uart_driver.h"
#include "CMSDK_CM3.h"
#include "queue.h"
#include "semphr.h"
#include <stdio.h>

static QueueHandle_t     xRxQueue    = NULL;
static SemaphoreHandle_t xTxMutex    = NULL;
static uint8_t            ucInitialised = 0;

void UART0RX_Handler( void )
{
    BaseType_t xHigherPriorityTaskWoken = pdFALSE;
    uint8_t ucByte;

    ucByte = ( uint8_t ) CMSDK_UART0->DATA;   /* Reading DATA clears RXBF. */

    xQueueSendFromISR( xRxQueue, &ucByte, &xHigherPriorityTaskWoken );

    CMSDK_UART0->INTCLEAR = CMSDK_UART_CTRL_RXIRQ_Msk;

    portYIELD_FROM_ISR( xHigherPriorityTaskWoken );
}

uart_status_t uart_driver_init( const uart_config_t * pxConfig )
{
    if( pxConfig == NULL )
    {
        return UART_ERR_INVALID_PARAM;
    }

    xRxQueue = xQueueCreate( 16, sizeof( uint8_t ) );
    xTxMutex = xSemaphoreCreateMutex();
    configASSERT( xRxQueue != NULL );
    configASSERT( xTxMutex != NULL );

    CMSDK_UART0->CTRL |= ( CMSDK_UART_CTRL_RXEN_Msk | CMSDK_UART_CTRL_RXIRQEN_Msk );

    NVIC_SetPriority( UARTRX0_IRQn, configMAX_SYSCALL_INTERRUPT_PRIORITY );
    NVIC_EnableIRQ( UARTRX0_IRQn );

    ucInitialised = 1;

    return UART_OK;
}

uart_status_t uart_driver_write( uint8_t ucByte, TickType_t xTimeout )
{
    if( !ucInitialised )
    {
        return UART_ERR_NOT_INITIALISED;
    }

    if( xSemaphoreTake( xTxMutex, xTimeout ) != pdTRUE )
    {
        return UART_ERR_TIMEOUT;
    }

    while( ( CMSDK_UART0->STATE & CMSDK_UART_STATE_TXBF_Msk ) != 0 )
    {
    }
    CMSDK_UART0->DATA = ucByte;

    xSemaphoreGive( xTxMutex );

    return UART_OK;
}

uart_status_t uart_driver_read( uint8_t * pucByte, TickType_t xTimeout )
{
    if( !ucInitialised )
    {
        return UART_ERR_NOT_INITIALISED;
    }
    if( pucByte == NULL )
    {
        return UART_ERR_INVALID_PARAM;
    }

    if( xQueueReceive( xRxQueue, pucByte, xTimeout ) != pdTRUE )
    {
        return UART_ERR_TIMEOUT;
    }

    return UART_OK;
}

uart_status_t uart_driver_deinit( void )
{
    if( ucInitialised )
    {
        NVIC_DisableIRQ( UARTRX0_IRQn );
        CMSDK_UART0->CTRL &= ~( CMSDK_UART_CTRL_RXEN_Msk | CMSDK_UART_CTRL_RXIRQEN_Msk );

        vQueueDelete( xRxQueue );
        vSemaphoreDelete( xTxMutex );
        xRxQueue = NULL;
        xTxMutex = NULL;

        ucInitialised = 0;
    }

    return UART_OK;
}