#ifndef GPIO_DRIVER_H
#define GPIO_DRIVER_H

#include "FreeRTOS.h"
#include <stdint.h>

typedef enum
{
    GPIO_OK = 0,
    GPIO_ERR_TIMEOUT,
    GPIO_ERR_NOT_INITIALISED,
    GPIO_ERR_INVALID_PARAM
} gpio_status_t;

typedef struct
{
    uint8_t ucPin;   /* Pin number within GPIO0, 0-15. */
} gpio_config_t;

gpio_status_t gpio_driver_init( const gpio_config_t * pxConfig );
gpio_status_t gpio_driver_write( uint8_t ucValue, TickType_t xTimeout );
gpio_status_t gpio_driver_deinit( void );

#endif /* GPIO_DRIVER_H */