#ifdef _18F45K50

#include "main.h"

void device_setup()
{
    //Initialise Oscillator
    OSCTUNE = 0x80; //3X PLL ratio mode selected
    OSCCON = 0x70;  //Switch to 16MHz HFINTOSC
    OSCCON2 = 0x10; //Enable PLL, SOSC, PRI OSC drivers turned off
    while(OSCCON2bits.PLLRDY != 1);   //Wait for PLL lock
    ACTCON = 0x90;  //Enable active clock tuning for USB operation

    ANSELB = 0xFF;  //Port B as digital

//    //Configure RA0 as LED activity
//    TRISBbits.TRISB0    = OUTPUT;
//    LATBbits.LATB0      = 1;

}

void device_uart_setup(uint32_t baudrate)
{
    PMD0bits.UARTMD     = 0;    //0 = Module is enabled, clock source is connected, module draws digital power
    BAUDCONbits.BRG16   = 1;    //1 = 16-bit Baud Rate Generator is used (SPBRGHx:SPBRGx)
    TXSTAbits.BRGH      = 1;    //BRGH: High Baud Rate Select bit

    //Interrupts
    INTCONbits.PEIE     = 1;
    PIE1bits.RCIE       = 1;    //Enable Receive interrupt
    IPR1bits.RCIP       = 1;    //RCIP: EUSART Receive Interrupt Priority bit   1 = High priority

    //Baudrate
    uint16_t CalcBaud   = (((_XTAL_FREQ/4)/baudrate)-1);
    SPBRGH    = (CalcBaud >> 8);
    SPBRG     = (CalcBaud & 0xFF);

    //Enable
    ANSELCbits.ANSC7    = 0;
    ANSELCbits.ANSC6    = 0;
    TRISCbits.TRISC7    = 1;    //RX as input
    TRISCbits.TRISC6    = 0;    //TX as output

    TXSTAbits.TXEN      = 1;    //1 = Transmit enabled
    RCSTAbits.CREN      = 1;    //1 = Enables receiver
    RCSTAbits.SPEN      = 1;
}

void uart_putc(uint8_t c)
{
    while(!TXSTAbits.TRMT);
    TXREG1 = c;
    while(!TXSTAbits.TRMT);
}

void uart_puts(const uint8_t *s)
{
    while(*s){
        uart_putc(*s);
        s++;
    }
}

void putch(char txData)
{
    while(!TXSTA1bits.TRMT);
    TXREG1 = txData;    // Write the data byte to the USART.
}

void Delay_ms(uint16_t time)
{
    while(--time)
    {
        _delay_ms(1);
    }
}

void delayms(uint16_t time)
{
    while(--time)
    {
        _delay_ms(1);
    }
}




void Timer3_Setup()
{
    T3CONbits.RD16      = 1;    //1 = Enables register read/write of Timer1/3 in one 16-bit operation
    T3CONbits.T3CKPS    = 0b00; //00 = 1:1 Prescale value
    T3CONbits.SOSCEN    = 0;    //0 = Dedicated secondary oscillator circuit disabled
    T3CONbits.T3CKPS    = 0b00; //00 =Timer1/3 clock source is instruction clock (FOSC/4)

    PIE2bits.TMR3IE     = 1;    //Enable Timer3 Interrupt
}

void Timer3_On()     { T3CONbits.TMR3ON    = 1; }
void Timer3_Off()    { T3CONbits.TMR3ON    = 0; }


uint8_t spi_transfer(uint8_t data) {
    SSP1BUF = data;
    while(!PIR1bits.SSP1IF);
    return SSP1BUF;
}



void uart_put_hex(uint8_t value) {
    uint8_t temp, hex[2], i=0;
    uart_putc('0');
    uart_putc('x');
    if(value == 0) { uart_putc(48); uart_putc(48);  return; }
    if(value < 16) { uart_putc(48); }

    while(value!=0){
        temp = value % 16;

        //To convert integer into character
        if( temp < 10)
        {
            temp = temp + 48;
        }
        else
        {
            temp = temp + 55;
        }
        hex[i] = temp;
        i++;

        value = value / 16;
    }

    uart_putc(hex[1]);
    uart_putc(hex[0]);

}

#endif