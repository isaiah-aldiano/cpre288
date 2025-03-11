#include <stdint.h>
#include <stdbool.h>
#include "timer.h"
#include "lcd.h"
#include <inc/tm4c123gh6pm.h>
#include "driverlib/interrupt.h"
#include "uart.h"
#include "cyBot_Scan.h"
#include "open_interface.h"
#include "movement.h"
#include "string.h"

/**
 * main.c
 */
int main(void)
{
    lcd_init();
    uart_init();
	volatile char input;
	int i;
    oi_t *sensor_data = oi_alloc();
    oi_init(sensor_data);
    cyBOT_Scan_t scan;
    char str[] = "\r\nDegrees   Distance(m)\r\n";

    float ir_raw;
    float b;
    short j;
    char data[20];
    short scans = 3;

        cyBOT_init_Scan(0b0111);
//        cyBOT_SERVO_cal();
        right_calibration_value = 343000;
    left_calibration_value = 1351000;


    uart_sendChar('\r');
	uart_sendChar('\n');

//	uart_interrupt_init();

	while(1) {
	    input = uart_receive();
	    uart_sendChar(input);
	    switch(input) {
	    case 'm':


	        for(j = 0; j < strlen(str); j++) {
	            uart_sendChar(str[j]);
	        }
	        for(i = 0; i < 91; i++) {
	            b = 0;
	            for (j = 0; j < scans; j++) {
	                cyBOT_Scan(i * 2, &scan);
	                b += scan.IR_raw_val;
	            }
	            ir_raw = b / 3;
	            b = 8830.2 * pow(ir_raw, -1.421);
	            sprintf(data, "\r\n%-10d%.2f",i*2, b);
	            short k;
	            for (k = 0; k < strlen(data); k++) {
	                uart_sendChar(data[k]);
	            }
	        }
	        break;
	    case 'w':
	        move_forward(sensor_data, 2);
	        break;
	    case 's':
	        move_backward(sensor_data, 2);
	        break;
	    case 'a':
	        turn_counter_clockwise(sensor_data, 2);
	        break;
	    case 'd':
	        turn_clockwise(sensor_data, 2);
	        break;
	    }
	    oi_setWheels(0,0);
   	}

//	while(1) {
//	   input = uart_receive();
////	   if(input == '\r') {
////	       data[i] = '\0';
////	       break;
////	   }
//	   if(input == '\r') {
//          uart_sendChar('\n');
//	   }
//
//	   uart_sendChar(input);
//
//	   data[i] = input;
//	   i++;
//
////	   lcd_printf("%c", input);
//	}
//	lcd_clear();
//	lcd_puts(data);


}
