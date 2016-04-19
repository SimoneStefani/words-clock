
#define LSBFIRST		1
#define DATA_PORT 		PORTC.0
#define CLOCK_PORT 		PORTC.1
#define STROBE_PORT		PORTC.2

void shiftOut(char dataPin, char clockPin, char val);
void initPorts(void);


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