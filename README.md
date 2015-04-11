# nRF24L01 library for PIC microcontrollers

##Modified by ColdKeyboard for PIC18F45K50
+ Fixed infinite loop error caused when nRF24L01 keeps IRQ low or PIC misses interrupt because of higher priority.
+ Still porting and testing this library so there are some debugging functions, outputs and etc.

This library is a port of the [tinkerer.eu MiRF library](http://tinkerer.eu/AVRLib/nRF24L01)
for PIC microcontrollers. 

I have tested this port on the PIC18F45K22 device but it should work on any PIC device. 

The implementation found in **spi.c** is using the internal MSSP peripheral. You might need to 
change the register names and bit values to fit your particular application. Additionally you will need
to configure the used pins for CE and CSN in **wl_module.h** and **wl_module.c**

## Example projects

This repository contains two complete example projects for MPLABX:

+ NRF_Master - a 4-bit binary up counter whose value is sent to the slave via the nRF module
+ NRF_Slave - receives the counter value from the master and displays it on four LEDs connected to GPIO pins

## Licensing
![CC-BY 3.0](http://i.creativecommons.org/l/by/3.0/88x31.png)

This work is licensed under a [Creative Commons Attribution 3.0 Unported License](http://creativecommons.org/licenses/by/3.0/).