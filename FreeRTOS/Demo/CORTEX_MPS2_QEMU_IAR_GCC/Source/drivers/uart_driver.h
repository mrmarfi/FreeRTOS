#ifndef UART_DRIVER_H
#define UART_DRIVER_H

#include "FreeRTOS.h"
#include <stdint.h>

typedef enum
{
    UART_OK = 0,
    UART_ERR_TIMEOUT,
    UART_ERR_NOT_INITIALISED,
    UART_ERR_INVALID_PARAM
} uart_status_t;

typedef struct
{
    /* Value for BAUDDIV, reserved for an instance that does not share UART0 with the console */
    uint32_t ulBaudDivisor;  
} uart_config_t;

uart_status_t eUARTDriverInit( const uart_config_t * pxConfig );
uart_status_t eUARTDriverWrite( uint8_t ucByte, TickType_t xTimeout );
uart_status_t eUARTDriverRead( uint8_t * pucByte, TickType_t xTimeout );
uart_status_t eUARTDriverDeinit( void );

#endif