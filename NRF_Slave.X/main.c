/*
 *      2013 Elia Ritterbusch
 *      http://eliaselectronics.com
 *
 *      This work is licensed under the Creative Commons Attribution 3.0 Unported License.
 *      To view a copy of this license, visit http://creativecommons.org/licenses/by/3.0/
 */
#include "main.h"

#define MAX_STRLEN 14
char received_string[MAX_STRLEN+1];

void _delay_10ms(int length){
    while(length){
        __delay_ms(10);
        length--;
    }
}

int main(void){
    device_setup();
    device_uart_setup(57600);
    uart_puts("UART is on.\r\n");

    
    unsigned char payload[wl_module_PAYLOAD]; //holds the payload
    unsigned char nRF_status; //STATUS information of nRF24L01+
    unsigned char zaehler = 0;

    uart_puts("wl_module_init()\r\n");
    wl_module_init(); //Init nRF Module
    _delay_10ms(5); //wait for Module
    uart_puts("wl_module_init() done.\r\n");

    // configure output port
    TRISD &= ~(0x0F);
    LATD |= 0x0F;



    wl_module_config();	//config nRF as RX Module, simple Version
    uart_puts("wl_module_config()\r\n");

    INTCONbits.PEIE = 1; // peripheral interrupts enabled
    INTCONbits.GIE = 1;  // global interrupt enable
    
    while(1){
        uart_puts("Waiting for data...\r\n");
        while (!wl_module_data_ready()); //waits for RX_DR Flag in STATUS
        nRF_status = wl_module_get_data(payload); //reads the incomming Data to Array payload
        uart_puts("Data ready!.\r\n");
	zaehler = payload[0];
        //LATD |= 0x0F;
        uart_puts("\r\n\r\n\t\tPayload: ");
        uart_put_hex(payload[0]);
        uart_puts("\r\n\r\n");

        //LATD &= ~(payload[0]);
    }

    return 0;
}

void interrupt ISR(void){
    // external interrupt IRQ pin of NRF24L01
    if( INTCON3bits.INT2IF ){
        uart_puts("Interrupt...\r\n");
        unsigned char status;

        // Read wl_module status
        wl_module_CSN_lo; // Pull down chip select
        status = spi_fast_shift(NOOP); // Read status register
        wl_module_CSN_hi; // Pull up chip select


        if (status & (1<<TX_DS)){ // IRQ: Package has been sent
                    wl_module_config_register(STATUS, (1<<TX_DS)); //Clear Interrupt Bit
                    PTX=0;
        }

	if (status & (1<<MAX_RT)){ // IRQ: Package has not been sent, send again
		wl_module_config_register(STATUS, (1<<MAX_RT));	// Clear Interrupt Bit
		wl_module_CE_hi; // Start transmission
		__delay_us(10);
		wl_module_CE_lo;
	}

	if (status & (1<<TX_FULL)){ //TX_FIFO Full <-- this is not an IRQ
		wl_module_CSN_lo; // Pull down chip select
		spi_fast_shift(FLUSH_TX); // Flush TX-FIFO
		wl_module_CSN_hi; // Pull up chip select
	}
        // reset INT2 flag
        INTCON3bits.INT2IF = 0;
    }
}


