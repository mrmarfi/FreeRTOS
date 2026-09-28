#include "gpio_driver.h"
#include "CMSDK_CM3.h"
#include <stdio.h>

static uint8_t ucInitialised   = 0;
static uint8_t ucConfiguredPin = 0;

gpio_status_t eGPIODriverInit( const gpio_config_t * pxConfig )
{
    printf( "initializing gpio peripheral\r\n" );

    /* each GPIO blocks controls 16 pins: 0-15 */
    if( ( pxConfig == NULL ) || ( pxConfig->ucPin > 15 ) )
    {
        return GPIO_ERR_INVALID_PARAM;
    }

    ucConfiguredPin = pxConfig->ucPin;

    /* << bitshift, 1UL 1 value as unsigned long	 */
    CMSDK_GPIO0->OUTENABLESET = ( 1UL << ucConfiguredPin );

    /* clear only the selected pin’s bit using bitwise AND with an inverted mask */
    CMSDK_GPIO0->DATAOUT &= ~( 1UL << ucConfiguredPin );
    
    ucInitialised = 1;

    return GPIO_OK;
}


gpio_status_t eGPIODriverWrite( uint8_t ucValue, TickType_t xTimeout )
{
    ( void ) xTimeout;

    if( !ucInitialised )
    {
        return GPIO_ERR_NOT_INITIALISED;
    }

    if( ucValue )
    {
        printf( "trying to turn led on\r\n" );
        /* |= bitwise OR to write only selected pin’s bit */
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


gpio_status_t eGPIODriverDeinit( void )
{
    printf( "shutting down gpio peripheral\r\n" );

    if( ucInitialised )
    {
        CMSDK_GPIO0->OUTENABLECLR = ( 1UL << ucConfiguredPin );
        ucInitialised = 0;
    }

    return GPIO_OK;
}