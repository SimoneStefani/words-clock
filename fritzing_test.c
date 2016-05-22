/*  
 *  WORD CLOCK - fritzing_test.c
 *  ----------------------------
 *  Program for testing the functionality of the
 *  processor. To be distributed with Fritzing
 *  schematics and PCB layout of the electronic
 *  circuit. The program lights up the words 
 *  TWELVE OCLOCK and the green test LED in an
 *  alternating pattern.
 *
 *  To be used in Fritzing, the code file must be
 *  saved with .ino extension and in a suitable folder. 
 *
 *  by:
 *  - Simone Stefani, @SimoneStefani
 *  - Marcel Eschmann, @eschmar
 *
 *  Compiler:
 *  B Knudsen Cc5x C-compiler - not ANSI-C
 */

#include "16F690.h"
#include "int16Cxx.h"
#pragma config |= 0x00D4

#define MTEN    VSR0.0 = 1
#define HALF    VSR0.1 = 1
#define QUARTER VSR0.2 = 1
#define TWENTY  VSR0.3 = 1
#define MFIVE   VSR0.4 = 1
#define MINUTES VSR0.5 = 1
#define PAST    VSR0.6 = 1
#define PEW0    VSR0.7 = 1
#define TO      VSR1.0 = 1
#define ONE     VSR1.1 = 1
#define TWO     VSR1.2 = 1
#define THREE   VSR1.3 = 1
#define FOUR    VSR1.4 = 1
#define HFIVE   VSR1.5 = 1
#define SIX     VSR1.6 = 1
#define PEW1    VSR1.7 = 1
#define SEVEN   VSR2.0 = 1
#define EIGHT   VSR2.1 = 1
#define NINE    VSR2.2 = 1
#define HTEN    VSR2.3 = 1
#define ELEVEN  VSR2.4 = 1
#define TWELVE  VSR2.5 = 1
#define OCLOCK  VSR2.6 = 1
#define PEW2    VSR2.7 = 1
 
/**
 * Shift Register Pins
 */
#define DATA_PORT       PORTC.0
#define CLOCK_PORT      PORTC.1
#define STROBE_PORT     PORTC.2


/**
 * Functions
 */
void shiftOut(char val);
void shiftOutSingle(int b);
void initPorts(void);
void updateShiftRegisters(void);
void updateView(void);
void turnOffLeds(void);
void delay10(char n);

int VSR0, VSR1, VSR2; // shift register X



/**
 * Main routine
 * Test program to check processor functionality:
 * lights up the words TWELVE OCLOCK and the test
 * LED in an alternating pattern with 1 sec dealy.
 */
void main(void) {
    initPorts();
    
    while(1) {
        TWELVE;
        OCLOCK;
        updateView();
        PORTB.5 = 0;
        delay10(100);

        turnOffLeds();
        PORTB.5 = 1;
        delay10(100);
    }
}

/**
 * Initialise PIC16F690 general purpose I/O ports.
 */
void initPorts(void) {
    TRISC.0 = 0; // DATA
    TRISC.1 = 0; // CLOCK
    TRISC.2 = 0; // STROBE
    TRISB.5 = 0; // TEST LED
}


/**
 * Helper function for shiftOut.
 */
void shiftOutSingle(int b) {
    DATA_PORT = b;
    CLOCK_PORT = 1;
    nop();
    CLOCK_PORT = 0;
}

/**
 * Shifts out a byte of data one bit at a time. Starts from the most significant bit. 
 * Each bit is written in turn to a DATA_PORT, after which a CLOCK_PORT is pulsed
 * to indicate that the bit is available.
 */
void shiftOut(char val) {
    shiftOutSingle(val.7);
    shiftOutSingle(val.6);
    shiftOutSingle(val.5);
    shiftOutSingle(val.4);
    shiftOutSingle(val.3);
    shiftOutSingle(val.2);
    shiftOutSingle(val.1);
    shiftOutSingle(val.0);
}

/**
 * Update clock face to show new time.
 */
void updateView(void) {
    shiftOut((char) VSR2);
    shiftOut((char) VSR1);
    shiftOut((char) VSR0);
    
    STROBE_PORT = 1;
    nop2();
    STROBE_PORT = 0;
}

/**
 * Power off all LEDs.
 */
void turnOffLeds(void) {
    VSR0 = 0;
    VSR1 = 0;
    VSR2 = 0;
    updateView();
}

/**
 * Delay function
 * Delays a multiple of 10 milliseconds using the TMR0 timer
 * error: 0.16 percent. B Knudsen.
 */
void delay10(char n) {
    char i;
    OPTION = 7;
    do {
        i = TMR0 + 39;
        while (i != TMR0)
            ;
    } while ( --n > 0);
}