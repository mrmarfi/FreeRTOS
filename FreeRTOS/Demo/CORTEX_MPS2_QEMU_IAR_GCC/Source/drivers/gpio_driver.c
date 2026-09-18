#include "gpio_driver.h"
#include "CMSDK_CM3.h"
#include <stdio.h>

static uint8_t ucInitialised   = 0;
static uint8_t ucConfiguredPin = 0;

gpio_status_t gpio_driver_init( const gpio_config_t * pxConfig )
{
    printf( "initializing gpio peripheral\r\n" );

    if( ( pxConfig == NULL ) || ( pxConfig->ucPin > 15 ) )
    {
        return GPIO_ERR_INVALID_PARAM;
    }

    ucConfiguredPin = pxConfig->ucPin;

    /* Configure the pin as output. */
    CMSDK_GPIO0->OUTENABLESET = ( 1UL << ucConfiguredPin );

    /* Start with the output low. */
    CMSDK_GPIO0->DATAOUT &= ~( 1UL << ucConfiguredPin );

    ucInitialised = 1;

    return GPIO_OK;
}

gpio_status_t gpio_driver_write( uint8_t ucValue, TickType_t xTimeout )
{
    /* xTimeout is accepted for API consistency across the drivers in this
     * chapter (Sec. 4.5); this GPIO driver never blocks, so GPIO_ERR_TIMEOUT
     * is part of the contract but is never actually returned here. */
    ( void ) xTimeout;

    if( !ucInitialised )
    {
        return GPIO_ERR_NOT_INITIALISED;
    }

    if( ucValue )
    {
        printf( "trying to turn led on\r\n" );
        CMSDK_GPIO0->DATAOUT |= ( 1UL << ucConfiguredPin );
        printf( "--> led on\r\n" );
    }
    else
    {
        printf( "trying to turn led off\r\n" );
        CMSDK_GPIO0->DATAOUT &= ~( 1UL << ucConfiguredPin );
        printf( "--> led off\r\n" );
    }

    return GPIO_OK;
}

gpio_status_t gpio_driver_deinit( void )
{
    printf( "shutting down gpio peripheral\r\n" );

    if( ucInitialised )
    {
        CMSDK_GPIO0->OUTENABLECLR = ( 1UL << ucConfiguredPin );
        ucInitialised = 0;
    }

    return GPIO_OK;
}