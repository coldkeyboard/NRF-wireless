/*
 *      2013 Elia Ritterbusch
 *      http://eliaselectronics.com
 *
 *      This work is licensed under the Creative Commons Attribution 3.0 Unported License.
 *      To view a copy of this license, visit http://creativecommons.org/licenses/by/3.0/
 */
#include "main.h"

void poolIRQ(void);

#define MAX_STRLEN 14
char received_string[MAX_STRLEN+1];

void _delay_10ms(int length){
    while(length){
        __delay_ms(10);
        length--;
    }
}

void main(){
    device_setup();
    device_uart_setup(57600);
    uart_puts("UART is on.\r\n");

    unsigned char payload[wl_module_PAYLOAD]; //Array for Payload
    unsigned char maincounter =0;
    unsigned char k;

    //configure IRQ pin
    TRISBbits.RB2 = 1;  //input
    

    // status pin to indicate when master
    // sends a message only for debugging
    TRISBbits.TRISB7 = 0;
    LATBbits.LATB7 = 0;

    _delay_10ms(100);

    wl_module_init();	//initialise nRF24L01+ Module
    _delay_10ms(5);	//wait for nRF24L01+ Module
    uart_puts("wl_module_init\r\n");

//    INTCON3bits.INT2IF  = 0;
//    INTCONbits.PEIE     = 1;    // peripheral interrupts enabled
//    INTCONbits.GIE      = 1;    // global interrupt enable

    LATBbits.LATB7  = 1;    //Turn off indicator

    uart_puts("Running wl_module_tx_config...\r\n");
    wl_module_tx_config(wl_module_TX_NR_0); //Config Module
    uart_puts("wl_module_tx_config\r\n");

    wl_module_CSN_lo; // Pull down chip select
    spi_fast_shift(FLUSH_TX); // Flush TX-FIFO
    wl_module_CSN_hi; // Pull up chip select

    wl_module_CSN_lo; // Pull down chip select
    spi_fast_shift(FLUSH_RX); // Flush TX-FIFO
    wl_module_CSN_hi; // Pull up chip select

    uart_puts("Clearing IRQs in Status register\r\n");
    wl_module_CSN_hi; // Pull up chip select
    wl_module_config_register(STATUS, 0x70); //Clear Interrupt Bits
    wl_module_CE_hi; // Start transmission
    __delay_us(10);
    wl_module_CE_lo;




    while(1){
        poolIRQ();
        uart_puts("Transmitting...\r\n");
        LATBbits.LATB7 = 0; // turn indicator LED on
        for (k=0; k<=wl_module_PAYLOAD-1; k++){
            payload[k] = k;
	}

	payload[0] = maincounter;
	payload[1] = maincounter+1;

        uart_puts("Calling wl_module_send\r\n");
	wl_module_send(payload,wl_module_PAYLOAD);

	maincounter++;
	if (maincounter > 0x0F){
            maincounter = 0;
        }
        uart_puts("Transmitting...done.\r\n");
        _delay_10ms(50);
        LATBbits.LATB7 = 1; // turn indicator LED off
        _delay_10ms(50);
    }

}

void poolIRQ(void)
{
    // external interrupt IRQ pin of NRF24L01
    if( LATBbits.LATB2 == 0 ){
        unsigned char status;
        uart_puts("Interrupt\r\n");

        // Read wl_module status
        wl_module_CSN_lo; // Pull down chip select
        status = spi_fast_shift(NOOP); // Read status register
        wl_module_CSN_hi; // Pull up chip select


        if (status & (1<<TX_DS)){ // IRQ: Package has been sent
            uart_puts("IRQ: Package has been sent\r\n");
            wl_module_config_register(STATUS, (1 << TX_DS)); //Clear Interrupt Bit
            PTX = 0;
        }

	if (status & (1<<MAX_RT)){ // IRQ: Package has not been sent, send again
            uart_puts("IRQ: Package has not been sent, send again\r\n");
            wl_module_config_register(STATUS, (1 << MAX_RT)); // Clear Interrupt Bit
            wl_module_CE_hi; // Start transmission
            __delay_us(10);
            wl_module_CE_lo;
	}

	if (status & (1<<TX_FULL)){ //TX_FIFO Full <-- this is not an IRQ
            uart_puts("TX_FIFO Full\r\n");
            wl_module_CSN_lo; // Pull down chip select
            spi_fast_shift(FLUSH_TX); // Flush TX-FIFO
            wl_module_CSN_hi; // Pull up chip select
	}

        uart_puts("Return from interrupt\r\n");
        // reset INT2 flag
        INTCON3bits.INT2IF = 0;

        return;
    }

    return;
}


