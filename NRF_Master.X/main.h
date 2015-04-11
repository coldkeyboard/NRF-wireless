#ifndef MAIN_H
#define	MAIN_H



#define _XTAL_FREQ 48000000UL

#if 1 // to fix stupid IDE error issues with __delay_ms
    #ifndef _delay_ms(x)
    #define _delay_ms(x) __delay_ms(x)
    #endif

    #ifndef _delay_us(x)
    #define _delay_us(x) __delay_us(x)
    #endif
#endif


#include <xc.h>
#include <stdlib.h>
#include <stdint.h>
#include "pic18f45k50.h"
#include "spi.h"
#include "nRF24L01.h"
#include "wl_module.h"

#endif	/* MAIN_H */

