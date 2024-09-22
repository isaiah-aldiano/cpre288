/// Simple 'Hello, world' program
/**
 * This program prints "Hello, world" to the LCD screen
 * @author Chad Nelson
 * @date 06/26/2012
 *
 * updated: phjones 9/3/2019
 * Describtion: Added timer_init call, and including Timer.h
 */

#include "Timer.h"
#include "lcd.h"


int main (void) {

	timer_init(); // Initialize Timer, needed before any LCD screen fucntions can be called 
	              // and enables time functions (e.g. timer_waitMillis)

	lcd_init();   // Initialize the the LCD screen.  This also clears the screen. 

	// Clear the LCD screen and print "Hello, world" on the LCD
//	lcd_printf("John Mayer sucks eggs");

//	lcd_puts("Hello, world");// Replace lcd_printf with lcd_puts
                                 // step through in debug mode
                                 // and explain to TA how it
                                 // works



    
	// NOTE: It is recommended that you use only lcd_init(), lcd_printf(), lcd_putc, and lcd_puts from lcd.h.
    // NOTE: For time fuctions see Timer.h
	char banner[] = "Embedded!";
	int spaces = 20;
	int shows = 0;
    int index = 1;
	char output[21];
	output[20] = '\0';
	while(true) {
	    timer_waitMillis(300);
	    int i;
	    if(spaces >= 0) {
	        for(i = 0; i < spaces; i++) {
	            output[i] = ' ';
	        }
	        for(shows = 0; shows < (20-spaces); shows++, i++) {
	            output[i] = banner[shows];
	        }
	        spaces -=1;
	    }
	    else if(index + 20 <= strlen(banner)) {
	        int j = 0;
	        for(i = index; i < index + 20; i++, j++) {
	           output[j] = banner[i];
	        }
	        index++;
	    }
	    else if(index <= strlen(banner)) {
            int j = 0;
            for(i = index; i < index + 20; i++, j++) {
                if(i < strlen(banner)) {
                   output[j] = banner[i];
                }
                else {
                   output[j] = ' ';
                }
            }
            index++;
	    }
	    else {
	        spaces = 20;
	        shows = 0;
	        index = 1;
	    }

	   lcd_printf(output);
	}
}
