/* B Knudsen Cc5x C-compiler - not ANSI-C */
#include "16F690.h"
#include "int16Cxx.h"
#pragma config |= 0x00D4

/* General */
#define LSBFIRST		1
#define DATA_PORT 		PORTC.0
#define CLOCK_PORT 		PORTC.1
#define STROBE_PORT		PORTC.2

void shiftOut(char dataPin, char clockPin, char val);
void initPorts(void);

/**
 * Interrupt service routine
 */
#pragma origin 4
interrupt isr(void) {
    int_save_registers
    // interrupt routine here
    //RABIF = 0;
    int_restore_registers
}

/**
 * Main routine
 */
void main( void)
{
    initPorts();
    //IOCA.5 = 1; // interrupt on RA5 pin enable
    //IOCA.4 = 1; // interrupt on RA4 pin enable
    RABIE = 1; // local interrupt enable
    GIE = 1; // global interrupt enable

    while(1) {
        // do something between interrupts
    }
}

/**
 * Initialise PIC16F690 general purpose I/O ports.
 */
void initPorts(void) {
	TRISC.0 = 0;
	TRISC.1 = 0;
	TRISC.2 = 0;
}

/**
 * Shifts out a byte of data one bit at a time. Starts from the most significant bit. 
 * Each bit is written in turn to a dataPin, after which a clockPin is pulsed
 * to indicate that the bit is available.
 */
void shiftOut(char dataPin, char clockPin, char val) {
    int i;
    for (i = 0; i < 8; i++)  {
    	DATA_PORT = !!(val & (1 << (7 - i))); // The double bang !! implicitly cast to bool: will it work?
        CLOCK_PORT = 1;
        CLOCK_PORT = 0;      
    }
}