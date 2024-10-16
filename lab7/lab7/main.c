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

struct someObject{
    short start;
    short finish;
    short size;
    short angle;
    bool fr;
};

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
    short k;
    char data[30];
    short scanData[90];
    short scans = 4;
    float range = .02;
    float lastScan = 0.0;
    bool objectMade = false;
    short objectCount = 0;
    short points = 0;
    short start = -1;
    short finish;

    cyBOT_init_Scan(0b0111);
//        cyBOT_SERVO_cal();
    right_calibration_value = 211750;
    left_calibration_value = 1167250;


    uart_sendChar('\r');
	uart_sendChar('\n');

	struct someObject objects[5];
	for(i = 0; i < 5; i++) {
	    objects[i].start = 181;
	    objects[i].finish = 181;
	    objects[i].fr = false;
	}

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

	            ir_raw = b / scans;
	            b = 8830.2 * pow(ir_raw, -1.421);

//	            scanData[i] = b;


	            if(b > 1.0) {

	            } else if(lastScan == 0 || b > lastScan + range || b < lastScan - range) {
	                lastScan = b;
	                if(objectMade && points >= 5) {
	                    objects[objectCount].start = start;
	                    objects[objectCount].finish = i;
	                    objects[objectCount].size = objects[objectCount].finish - objects[objectCount].start;
	                    objects[objectCount].angle = (objects[objectCount].finish + objects[objectCount].start) / 2;
	                    objects[objectCount].fr = true;
	                    start = -1;
	                    objectMade = false;
	                    objectCount++;
	                    points = 0;
	                } else {
	                    objectMade = false;
	                    points = 0;
	                    start = -1;
	                }
	            } else {
	                objectMade = true;
	                if(start == -1) {
	                    start = i;
	                }
	                points++;
	            }

	            sprintf(data, "\r\n%-10d%.2f",i*2, b);

	            for (k = 0; k < strlen(data); k++) {
	                uart_sendChar(data[k]);
	            }
	        }

	        short z;
	        short smallest = 100;
	        short smallestAngle = 180;
	        for(z = 0; z < sizeof(objects) / sizeof(objects[0]); z++) {
	            if(objects[z].size < smallest && objects[z].fr == true) {
	                smallest = objects[z].size;
	                smallestAngle = objects[z].angle;
	            }
	            sprintf(data, "\r\nObj:%d s:%hd f:%hd", z, objects[z].start, objects[z].finish);
	            for (k = 0; k < strlen(data); k++) {
                    uart_sendChar(data[k]);
                }
	        }

	        cyBOT_Scan(smallestAngle * 2, &scan);

	        short w;
	        short pingScan;
	        for(w = 0; w < scans; w++) {
	            cyBOT_Scan(smallestAngle, &scan);
	             pingScan += scan.sound_dist;
	        }


	        lcd_printf("%hd %.2f", smallestAngle, pingScan / scans);
	    }


//	        break;
//	    case 'w':
//	        move_forward(sensor_data, 2);
//	        break;
//	    case 's':
//	        move_backward(sensor_data, 2);
//	        break;
//	    case 'a':
//	        turn_counter_clockwise(sensor_data, 2);
//	        break;
//	    case 'd':
//	        turn_clockwise(sensor_data, 2);
//	        break;
//	    }
//	    oi_setWheels(0,0);
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
