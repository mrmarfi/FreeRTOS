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
    uint32_t ulBaudDiv;   /* Value for BAUDDIV; caller supplies a valid divider. */
} uart_config_t;

uart_status_t uart_driver_init( const uart_config_t * pxConfig );
uart_status_t uart_driver_write( uint8_t ucByte, TickType_t xTimeout );
uart_status_t uart_driver_read( uint8_t * pucByte, TickType_t xTimeout );
uart_status_t uart_driver_deinit( void );

#endif