/* B Knudsen Cc5x C-compiler - not ANSI-C */
#include "16F690.h"
#include "int16Cxx.h"
#pragma config |= 0x00D4

/*
 *  CONFIGURATION
 *  -------------
 *
 *  0bxx.xxxxx.xxx.xxxxxxxxxx.x.00
 *
 *  00 - SR0.0 - "IT"
 *  01 - SR0.1 - "IS"
 *
 *  02 - SR0.2 - "TEN"
 *  03 - SR0.3 - "HALF"
 *  04 - SR0.4 - "QUARTER"
 *  05 - SR0.5 - "TWENTZ"
 *  06 - SR0.6 - "FIVE"
 *
 *  07 - SR0.7 - "MINUTES"
 *  08 - SR1.0 - "PAST"
 *  09 - SR1.1 - "TO"
 *
 *  10 - SR1.2 - "ONE"
 *  11 - SR1.3 - "TWO"
 *  12 - SR1.4 - "THREE"
 *  13 - SR1.5 - "FOUR"
 *  14 - SR1.6 - "FIVE"
 *  15 - SR1.7 - "SIX"
 *  16 - SR2.0 - "SEVEN"
 *  17 - SR2.1 - "EIGHT"
 *  18 - SR2.2 - "NINE"
 *  19 - SR2.3 - "TEN"
 *  20 - SR2.4 - "ELEVEN"
 *  21 - SR2.5 - "TWELVE"
 *  22 - SR2.6 - "OCLOCK"
 *  23 - SR2.7 - undefined pew
 */

#define IT SR0.0 = 1
#define IS SR0.1 = 1
#define MTEN SR0.2 = 1
#define HALF SR0.3 = 1
#define QUARTER SR0.4 = 1
#define TWENTY SR0.5 = 1
#define MFIVE SR0.6 = 1
#define MINUTES SR0.7 = 1
#define PAST SR1.0 = 1
#define TO SR1.1 = 1
#define ONE SR1.2 = 1
#define TWO SR1.3 = 1
#define THREE SR1.4 = 1
#define FOUR SR1.5 = 1
#define HFIVE SR1.6 = 1
#define SIX SR1.7 = 1
#define SEVEN SR2.0 = 1
#define EIGHT SR2.1 = 1
#define NINE SR2.2 = 1
#define HTEN SR2.3 = 1
#define ELEVEN SR2.4 = 1
#define TWELVE SR2.5 = 1
#define OCLOCK SR2.6 = 1
#define PEW SR2.7 = 1
 
/* General */
#define DATA_PORT       PORTC.0
#define CLOCK_PORT      PORTC.1
#define STROBE_PORT     PORTC.2
#define MINUTES_BUTTON  PORTC.3
#define HOURS_BUTTON    PORTC.4

/* Functions */
void shiftOut(char dataPin, char clockPin, char val);
void initPorts(void);
void initTimer0Interrupt(void);
void turnOffLeds(void);

/* Timing */
int state_minutes = 0;
int state_hours = 0;
int SR0, SR1, SR2; // shift register X

/**
 * Interrupt service routine
 */
#pragma origin 4
interrupt isr(void) {
    int_save_registers
    // interrupt routine here
    
    turnOffLeds();
    IT
    IS
    
    // HOURS
    switch (state_hours) {
        case 1:
            ONE
            break;
        case 2:
            TWO
            break;
        case 3:
            THREE
            break;
        case 4:
            FOUR
            break;
        case 5:
            HFIVE
            break;
        case 6:
            SIX
            break;
        case 7:
            SEVEN
            break;
        case 8:
            EIGHT
            break;
        case 9:
            NINE
            break;
        case 10:
            HTEN
            break;
        case 11:
            ELEVEN
            break;
        case 0:
            TWELVE
            break;
        default:
            PEW
    }
    
    // MINUTES
    switch (state_minutes) {
        case 0:
            OCLOCK
            break;
        case 1:
            MFIVE
            PAST
            break;
        case 2:
            MTEN
            PAST
            break;
        case 3:
            QUARTER
            PAST
            break;
        case 4:
            TWENTY
            PAST
            break;
        case 5:
            TWENTY
            MFIVE
            PAST
            break;
        case 6:
            HALF
            PAST
            break;
        case 7:
            TWENTY
            MFIVE
            TO
            break;
        case 8:
            TWENTY
            TO
            break;
        case 9:
            QUARTER
            TO
            break;
        case 10:
            MTEN
            TO
            break;
        case 11:
            MFIVE
            TO
            break;
    }
    
    state_minutes = (state_minutes + 1) % 12;
    state_hours = (state_hours + 1) % 12;
    updateView();
    
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
    TRISC.3 = 0;
    TRISC.4 = 0;
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
 * Each bit is written in turn to a DATA_PORT, after which a CLOCK_PORT is pulsed
 * to indicate that the bit is available.
 */
void shiftOut(char val) {
    int i;
    for (i = 0; i < 8; i++)  {
        DATA_PORT = !!(val & (1 << (7 - i))); // The double bang !! implicitly cast to bool: will it work?
        CLOCK_PORT = 1;
        CLOCK_PORT = 0;      
    }
}

/**
 * Update time.
 */
void updateView(void) {
    shiftOut((char) SR0);
    shiftOut((char) SR1);
    shiftOut((char) SR2);
    STROBE_PORT = 1;
    nop();
    STROBE_PORT = 0;
}

/**
 * Power off all LEDS.
 */
void turnOffLeds(void) {
    SR0 = 0;
    SR1 = 0;
    SR2 = 0;
    updateView();
}