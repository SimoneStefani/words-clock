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
 *  00 - "IT" - Hardwired
 *  01 - "IS" - Hardwired
 *
 *  02 - VSR0.0 - "TEN"
 *  03 - VSR0.1 - "HALF"
 *  04 - VSR0.2 - "QUARTER"
 *  05 - VSR0.3 - "TWENTZ"
 *  06 - VSR0.4 - "FIVE"
 *
 *  07 - VSR0.5 - "MINUTES"
 *  08 - VSR0.6 - "PAST"
 *  09 - VSR1.0 - "TO"
 *
 *  10 - VSR1.1 - "ONE"
 *  11 - VSR1.2 - "TWO"
 *  12 - VSR1.3 - "THREE"
 *  13 - VSR1.4 - "FOUR"
 *  14 - VSR1.5 - "FIVE"
 *  15 - VSR1.6 - "SIX"
 *  16 - VSR2.0 - "SEVEN"
 *  17 - VSR2.1 - "EIGHT"
 *  18 - VSR2.2 - "NINE"
 *  19 - VSR2.3 - "TEN"
 *  20 - VSR2.4 - "ELEVEN"
 *  21 - VSR2.5 - "TWELVE"
 *  22 - VSR2.6 - "OCLOCK"
 */

#define MTEN VSR0.0 = 1
#define HALF VSR0.1 = 1
#define QUARTER VSR0.2 = 1
#define TWENTY VSR0.3 = 1
#define MFIVE VSR0.4 = 1
#define MINUTES VSR0.5 = 1
#define PAST VSR0.6 = 1
#define PEW0 VSR0.7 = 1
#define TO VSR1.0 = 1
#define ONE VSR1.1 = 1
#define TWO VSR1.2 = 1
#define THREE VSR1.3 = 1
#define FOUR VSR1.4 = 1
#define HFIVE VSR1.5 = 1
#define SIX VSR1.6 = 1
#define PEW1 VSR1.7 = 1
#define SEVEN VSR2.0 = 1
#define EIGHT VSR2.1 = 1
#define NINE VSR2.2 = 1
#define HTEN VSR2.3 = 1
#define ELEVEN VSR2.4 = 1
#define TWELVE VSR2.5 = 1
#define OCLOCK VSR2.6 = 1
#define PEW2 VSR2.7 = 1
 
/* General */
#define DATA_PORT       PORTC.0
#define CLOCK_PORT      PORTC.1
#define STROBE_PORT     PORTC.2
#define MINUTES_BUTTON  PORTC.3
#define HOURS_BUTTON    PORTC.4

/* Functions */
void shiftOut(char val);
void shiftOutSingle(int b);
void initPorts(void);
void initTimer0Interrupt(void);
void updateView(void);
void turnOffLeds(void);

/* Timing */
int unsigned postScaler;
int unsigned state_minutes;
int unsigned state_hours;
int VSR0, VSR1, VSR2; // shift register X
int hour;
int minute; 
int second;

/**
 * Interrupt service routine
 */
#pragma origin 4
interrupt Timer0_ISR(void) {
    int_save_registers
    // interrupt routine here
	
	postScaler += 1;
	postScaler = postScaler % 50;
	
	/*if (postScaler == 0) {
		PORTC.1 = 1;
	}
	else if (postScaler == 25) {
		PORTC.1 = 0;
	}*/
	
	int unsigned temp = 0;
	if (postScaler == 0) {
		state_minutes += 1;
		temp = state_minutes % 12;
		if (temp == 0) {
			state_hours += 1;
			state_hours = state_hours % 12;
		}
		state_minutes = temp;
		
		
		turnOffLeds();
		
		// HOURS
		switch (state_hours) {
			case 1:
				ONE;
				break;
			case 2:
				TWO;
				break;
			case 3:
				THREE;
				break;
			case 4:
				FOUR;
				break;
			case 5:
				HFIVE;
				break;
			case 6:
				SIX;
				break;
			case 7:
				SEVEN;
				break;
			case 8:
				EIGHT;
				break;
			case 9:
				NINE;
				break;
			case 10:
				HTEN;
				break;
			case 11:
				ELEVEN;
				break;
			case 0:
				TWELVE;
				// Eventual time adjustments here
				break;
			default:
				PEW0;
		}
		
		// MINUTES
		switch (state_minutes) {
			case 0:
				OCLOCK;
				break;
			case 1:
				MFIVE;
				PAST;
				break;
			case 2:
				MTEN;
				PAST;
				break;
			case 3:
				QUARTER;
				PAST;
				break;
			case 4:
				TWENTY;
				PAST;
				break;
			case 5:
				TWENTY;
				MFIVE;
				PAST;
				break;
			case 6:
				HALF;
				PAST;
				break;
			case 7:
				TWENTY;
				MFIVE;
				TO;
				break;
			case 8:
				TWENTY;
				TO;
				break;
			case 9:
				QUARTER;
				TO;
				break;
			case 10:
				MTEN;
				TO;
				break;
			case 11:
				MFIVE;
				TO;
				break;
		}
		
		updateView();
	}

    int_restore_registers
	T0IF = 0; // clear interrupt flag
}

/**
 * Main routine
 */
void main( void)
{
    initPorts();
    initTimer0Interrupt();
	
	state_minutes = 0;
	state_hours = 0;
	hour = 5;
	minute = 55; 
	second = 00;

	postScaler = 0;
	PORTC.1 = 0;
    
    // global interrupt enable
    GIE = 1;
	
	while(1) {}
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
    //PSA = 0; 

    // prescaling 111 = 1:256
    //PS0 = 1; 
    //PS1 = 1;
    //PS2 = 1;
	OPTION = 7;

    // set timer value
    TMR0 = 125;

    // enable timer0 overflow interrupt
    T0IE = 1;
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

	// NOT COMPILABLE
    /*for (i = 7; i <= 0; i--)  {
		mask = 1 << i;
        DATA_PORT = val & mask;
		
        CLOCK_PORT = 1;
		nop();
        CLOCK_PORT = 0;      
    }*/
}

/**
 * Update time.
 */
void updateView(void) {
    shiftOut((char) VSR0);
    shiftOut((char) VSR1);
    shiftOut((char) VSR2);
    STROBE_PORT = 1;
    nop2();
    STROBE_PORT = 0;
}

/**
 * Power off all LEDS.
 */
void turnOffLeds(void) {
    VSR0 = 0;
    VSR1 = 0;
    VSR2 = 0;
    updateView();
}