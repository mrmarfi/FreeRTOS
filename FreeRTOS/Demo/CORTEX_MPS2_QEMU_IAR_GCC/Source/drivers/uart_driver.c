#include "uart_driver.h"
#include "CMSDK_CM3.h"
#include "queue.h"
#include "semphr.h"
#include <stdio.h>

static QueueHandle_t     xRxQueue    = NULL;
static SemaphoreHandle_t xTxMutex    = NULL;
static uint8_t            ucInitialised = 0;

void vUARTDriverRXHandler( void )
{
    BaseType_t xHigherPriorityTaskWoken = pdFALSE;
    uint8_t ucByte;

	 /* Reading DATA clears RXBF */
    ucByte = ( uint8_t ) CMSDK_UART0->DATA;   
	 
	 /* minimal interrupt work -> add byte read to the queue */
    xQueueSendFromISR( xRxQueue, &ucByte, &xHigherPriorityTaskWoken );

	 /* acknowledge the interrupt */
    CMSDK_UART0->INTCLEAR = CMSDK_UART_CTRL_RXIRQ_Msk;

	 /* yield if a higher priority task was unblocked */
    portYIELD_FROM_ISR( xHigherPriorityTaskWoken );
}


uart_status_t eUARTDriverInit( const uart_config_t * pxConfig )
{
    if( pxConfig == NULL )
    {
        return UART_ERR_INVALID_PARAM;
    }

	 /* create synchronization objects */
    xRxQueue = xQueueCreate( 16, sizeof( uint8_t ) );
    xTxMutex = xSemaphoreCreateMutex();
    configASSERT( xRxQueue != NULL );
    configASSERT( xTxMutex != NULL );

	 /* write control using predefined hardware bit indicator constants */
    CMSDK_UART0->CTRL |= ( CMSDK_UART_CTRL_RXEN_Msk | CMSDK_UART_CTRL_RXIRQEN_Msk );
	
	 /* register interrupt */
    NVIC_SetPriority( UARTRX0_IRQn, configMAX_SYSCALL_INTERRUPT_PRIORITY );
    NVIC_EnableIRQ( UARTRX0_IRQn );

    ucInitialised = 1;

    return UART_OK;
}


uart_status_t eUARTDriverWrite( uint8_t ucByte, TickType_t xTimeout )
{
    if( !ucInitialised )
    {
        return UART_ERR_NOT_INITIALISED;
    }

    if( xSemaphoreTake( xTxMutex, xTimeout ) != pdTRUE )
    {
        return UART_ERR_TIMEOUT;
    }

	  /* wait until TX buffer is free */
    while( ( CMSDK_UART0->STATE & CMSDK_UART_STATE_TXBF_Msk ) != 0 )
    {
    }
    CMSDK_UART0->DATA = ucByte;

    xSemaphoreGive( xTxMutex );
    
    return UART_OK;
}

uart_status_t eUARTDriverRead( uint8_t * pucByte, TickType_t xTimeout )
{
    if( !ucInitialised )
    {
        return UART_ERR_NOT_INITIALISED;
    }
    if( pucByte == NULL )
    {
        return UART_ERR_INVALID_PARAM;
    }

    /* blocks up to xTimeout waiting for a byte from the ISR */
    if( xQueueReceive( xRxQueue, pucByte, xTimeout ) != pdTRUE )
    {
        return UART_ERR_TIMEOUT;
    }

    return UART_OK;
}



uart_status_t eUARTDriverDeinit( void )
{
	 printf( "shutting down uart peripheral\r\n" );

    if( ucInitialised )
    {
		/* disable interrupt */
        NVIC_DisableIRQ( UARTRX0_IRQn );

		/* Clears RXEN/RXIRQEN only, TXEN is left untouched */
        CMSDK_UART0->CTRL &= ~( CMSDK_UART_CTRL_RXEN_Msk | CMSDK_UART_CTRL_RXIRQEN_Msk );

        vQueueDelete( xRxQueue );
        vSemaphoreDelete( xTxMutex );
        xRxQueue = NULL;
        xTxMutex = NULL;

        ucInitialised = 0;
    }

    return UART_OK;
}
