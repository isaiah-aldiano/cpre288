/**
 * lab4_template.c
 * 
 * Template file for CprE 288 lab 4
 *
 * @author Zhao Zhang, Chad Nelson, Zachary Glanz
 * @date 08/14/2016
 */


/*
 * 1a) GPIODATA
 * 1b) GPIO_PORTE_DATA_R
 * 1c) 0x400243FC
 */

#include <stdio.h>
#include "button.h"
#include "Timer.h"
#include "lcd.h"
#include "cyBot_uart.h"  // Functions for communiticate between CyBot and Putty (via UART)
                         // PuTTy: Buad=115200, 8 data bits, No Flow Control, No Party,  COM1

//#warning "Possible unimplemented functions"
#define REPLACEME 0



int main(void) {
	button_init();
	lcd_init();
	cyBot_uart_init();
	// Don't forget to initialze the cyBot UART before trying to use it
	
	// YOUR CODE HERE
	
	char buffer[5];
	int i;
	uint8_t pushed;
	while(1)
	{

        pushed = button_getButton();
	    if(pushed > 0) {
            lcd_printf("%u", pushed);

            sprintf(buffer, "%u\r\n", pushed);
            for(i = 0; i < sizeof(buffer); i++) {
                cyBot_sendByte(buffer[i]);
            }
	    } else {
	        lcd_printf("");
	    }
	}
	
}
