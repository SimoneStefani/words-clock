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
void initTimer0Interrupt(void);

/**
 * Interrupt service routine
 */
#pragma origin 4
interrupt isr(void) {
    int_save_registers
    // interrupt routine here

    T0IF = 0; // clear interrupt flag
    int_restore_registers
}

/**
 * Main routine
 */
void main( void)
{
    initPorts();
    initTimer0Interrupt();
    
    // global interrupt enable
    GIE = 1;

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
 * Initialise timer0 with interrupts
 */
void initTimer0Interrupt(void) {
    // prescaler is assigned to the Timer0 module
    PSA = 0; 

    // prescaling 111 = 1:256
    PS0 = 1; 
    PS1 = 1;
    PS2 = 1;

    // set timer value
    TMR0 = 5;

    // enable timer0 overflow interrupt
    T0IE = 1;
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