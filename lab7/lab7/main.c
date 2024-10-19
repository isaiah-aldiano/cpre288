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
    uart_interrupt_init();
	volatile char input;
	int i;
    oi_t *sensor_data = oi_alloc();
    oi_init(sensor_data);
    cyBOT_Scan_t scan;
    char str[] = "\r\nDegrees   Distance(m)\r\n";

    volatile float ir_raw;
    volatile float b;
    volatile short j;
    volatile short k;
    volatile short scanDataIterate;

    char data[30];
    volatile float scanData[91];
    short scans = 1;
    float range = .03;
    volatile float currScan = 0.0;
    volatile float lastScan = 0.0;

    volatile short objectCount = 0;
    volatile short points = 0;
    volatile short start = -1;

    cyBOT_init_Scan(0b0111);
//        cyBOT_SERVO_cal();
    right_calibration_value = 222250;
    left_calibration_value = 1225000;


    uart_sendChar('\r');
	uart_sendChar('\n');

	struct someObject objects[5];

	while(1) {
	    input = uart_receive();
	    uart_sendChar(input);
	    switch(input) {
	    case 'm':
	        for(j = 0; j < strlen(str); j++) { // Send "\r\nDegrees   Distance(m)\r\n" message
	            uart_sendChar(str[j]);
	        }

	        for(i = 0; i <= 90; i++) { // Using IR scan raw IR values and put them in scanData
	            b = 0;
	            for (j = 0; j < scans; j++) {
	                cyBOT_Scan(i * 2, &scan);
	                b += scan.IR_raw_val;
	            }

	            ir_raw = b / scans;
	            b = 8830.2 * pow(ir_raw, -1.421);

	            scanData[i] = b;

	            sprintf(data, "\r\n%-10d%.2f",i*2, b); // print degree IR value was taken at and averaged IR value
                for (k = 0; k < strlen(data); k++) {
                     uart_sendChar(data[k]);
                }
	        }

	        bool canWrite = false; // Helps determine if an object can be created at the moment
            for(scanDataIterate = 1; scanDataIterate < 90; scanDataIterate++) { // iterate through scanData values
                currScan = scanData[scanDataIterate]; // Current scanData value
                lastScan = scanData[scanDataIterate - 1]; // value behind current scanData value

                if(currScan < 1.0) { // filters out IR values over 1 meter because our IR measurement isn't accurate beyond 50 cm
                    if(currScan <= lastScan + range && currScan >= lastScan - range) { // If current value is in the range of the last value add points
                        if(start == -1) {
                            start = scanDataIterate;
                            canWrite = true;
                        }
                        points++; // If points >= 5 then its probably an object
                    } else if (points >= 5 && canWrite == true) { // probably and object and can write object then create a struct
                        points = 0;
                        objects[objectCount].start = start - 1;
                        objects[objectCount].finish = scanDataIterate;
                        objects[objectCount].size = objects[objectCount].finish - objects[objectCount].start;
                        objects[objectCount].angle =  (objects[objectCount].finish + objects[objectCount].start);
                        objects[objectCount].fr = true;
                        objectCount++;
                        start = -1; // reset values
                        canWrite = false;
                    } else {
                        start = -1; // if not within range then reset all values
                        points = 0;
                        canWrite = false;
                    }
                } else {
                    start = -1; // if beyond 1 meter then reset values
                    points = 0;
                    canWrite = false;
                }
            }

            objectCount = 0; // reset object count for future scan
            short z;
            short smallest = 100;
            short smallestAngle = 180;
            short smallestObject = 0;
            /*
             * Iterate through struct array
             * If an object is written and its the smallest object encountered and its size is less than 10 then its probably a small object
             * If the object is written then print out the object
             */
            for(z = 0; z < sizeof(objects) / sizeof(objects[0]); z++) {
                if(objects[z].size < smallest && objects[z].fr == true && objects[z].size < 10) {
                    smallest = objects[z].size;
                    smallestAngle = objects[z].angle;
                    smallestObject = z;
                }
                if(objects[z].fr == true) {
                    sprintf(data, "\r\nObj:%d s:%hd f:%hd size:%hd", z + 1, objects[z].start * 2, objects[z].finish * 2, objects[z].size);
                    for (k = 0; k < strlen(data); k++) {
                        uart_sendChar(data[k]);
                    }
                }

            }

            cyBOT_Scan(smallestAngle * 2, &scan); // Point sensors towards smallest object

            short w;
            short realScans = 0;
            float pingScan = 0;
            for(w = 0; w < 10; w++) { // scan the smallest object with ping to determine distance
                cyBOT_Scan(smallestAngle, &scan);

                if(scan.sound_dist <= 60) {
                    pingScan += scan.sound_dist;
                    realScans++;
                }
            }

            float shortestScanDistance = .9; // ssd
            short smallestObjectStart = objects[smallestObject].start;
            short smallestObjectFinish = objects[smallestObject].finish;
            short iter_ssd;
            short angleOfSmallest;
            /*
             * Finds smallest distance to smallest object of IR values
             */
            for(iter_ssd = smallestObjectStart; iter_ssd < smallestObjectFinish; iter_ssd++) {
                if(scanData[iter_ssd] < shortestScanDistance) {
                    shortestScanDistance = scanData[iter_ssd];
                    angleOfSmallest = iter_ssd * 2;
                }
            }

            /*
             * Checks if IR or Ping have a smaller distance to smallest object
             * Casts real smallest distance to int for movement functions
             */
            int realDistance;
            if(shortestScanDistance < pingScan / realScans) {
                realDistance = (shortestScanDistance * 100.0);
            } else {
                realDistance = ((pingScan / realScans) * 100.0);
            }

            realDistance -= 10; // Adjusts for bot length

            if(angleOfSmallest < 90) { // Move bot
                turn_clockwise(sensor_data, 90 - angleOfSmallest);
                move_forward(sensor_data, realDistance);
            } else if (angleOfSmallest >= 90) {
                turn_counter_clockwise(sensor_data, angleOfSmallest- 90);
                move_forward(sensor_data, realDistance);
            }

            oi_setWheels(0,0);

            lcd_printf("%hd %.2f %.2f %d %hd", smallestAngle, pingScan / realScans, shortestScanDistance, realDistance, angleOfSmallest);

            for(i = 0; i < 5; i++) {
                    objects[i].start = 181;
                    objects[i].finish = 181;
                    objects[i].fr = false;
            }
            break;
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
}
