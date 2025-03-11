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

volatile int doSomething = 0;
volatile bool mode = true;
volatile int hCount = 0;

void runScan(volatile float scanData[], bool state) {
    hCount = 1;
    int j;
    char str[] = "\r\nDegrees   Distance(m)\r\n";
    short strLength = strlen(str);
    for(j = 0; j < strLength; j++) { // Send "\r\nDegrees   Distance(m)\r\n" message
        uart_sendChar(str[j]);
    }

    int i, k;
    float ir_raw, b;
    char data[30];
    cyBOT_Scan_t scan;
    for(i = 0; i <= 90; i++) { // Using IR scan raw IR values and put them in scanData
        if(mode != state) {
            break;
        }
        b = 0;

        cyBOT_Scan(i * 2, &scan);
        b += scan.IR_raw_val;

        ir_raw = b;
        b = 8830.2 * pow(ir_raw, -1.421);

        scanData[i] = b;

        sprintf(data, "\r\n%-10d%.2f",i*2, b); // print degree IR value was taken at and averaged IR value
        for (k = 0; k < strlen(data); k++) {
            uart_sendChar(data[k]);
        }
    }
}

void displayObjects(float scanData[], struct someObject objects[]) {
    unsigned short i;
    unsigned short j;
    unsigned short k;
    bool printed = false;


    for(k = 0; k < 7; k++) {
        uart_sendChar('\r');
        uart_sendChar('\n');
        for(i = 90; i > 0; i--) {
            for(j = 0; j < 5; j++) {
                if(i >= objects[j].start && i <= objects[j].finish ) {
                   uart_sendChar('#');
                   printed = true;
                   break;
                }
            }
            if(printed) {
                printed = false;
            } else {
                uart_sendChar(' ');
            }
        }
    }

}

void createObjects(float scanData[], struct someObject objects[]) {
    short scanDataIterate;
    float currScan = 0.0;
    float lastScan = 0.0;
    short objectCount = 0;
    short points = 0;
    short start = -1;
    float range = .04;

    int i;
    for(i = 0; i < 5; i++) {
        objects[i].start = 181;
        objects[i].finish = 181;
        objects[i].fr = false;
    }


    bool canWrite = false; // Helps determine if an object can be created at the moment
    for(scanDataIterate = 1; scanDataIterate < 90; scanDataIterate++) { // iterate through scanData values
        currScan = scanData[scanDataIterate]; // Current scanData value
        lastScan = scanData[scanDataIterate - 1]; // value behind current scanData value

        if(currScan < .6) { // filters out IR values over 1 meter because our IR measurement isn't accurate beyond 50 cm
            if(currScan <= lastScan + range && currScan >= lastScan - range) { // If current value is in the range of the last value add points
                if(start == -1) {
                    start = scanDataIterate;
                    canWrite = true;
                }
                points++; // If points >= 5 then its probably an object
            } else if (points >= 4 && canWrite == true) { // probably and object and can write object then create a struct
                points = 1;
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
                points = 1;
                canWrite = false;
            }
        } else {
            start = -1; // if beyond 1 meter then reset values
            points = 1;
            canWrite = false;
        }
    }
}

int main(void)
{
    lcd_init();
    uart_init();
    uart_interrupt_init();
    volatile char input;
//    int i;
    oi_t *sensor_data = oi_alloc();
    oi_init(sensor_data);
    cyBOT_Scan_t scan;

    int notVHCount;
     float ir_raw;
     float b;
     short j;
     short k;

    bool state = false;

    char data[40];
    float scanData[91];

    cyBOT_init_Scan(0b0111);
//        cyBOT_SERVO_cal();
    right_calibration_value = 337750;
    left_calibration_value = 1324750;


    uart_sendChar('\r');
    uart_sendChar('\n');

    struct someObject objects[5];


    while(1) {
//        input = doSomething;
//        input = uart_receive();
        uart_sendChar(doSomething);
        switch(doSomething) {
        case 1:
            if(mode) {
                doSomething = 0;
                state = mode;
                runScan(scanData, state);
                createObjects(scanData, objects);

                short z;
                short smallest = 100;
                short smallestAngle = 180;
                short smallestObject = 0;
                /*
                 * Iterate through struct array
                 * If an object is written and its the smallest object encountered and its size is less than 10 then its probably a small object
                 * If the object is written then print out the object
                 */
                if(mode) {

                    cyBOT_Scan(smallestAngle * 2, &scan); // Point sensors towards smallest object

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
                    int realDistance = (pingScan / realScans) - 11;

                    if(objects[smallestObject].angle < 90) { // Calculate move
                        sprintf(data, "\r\nTurn Right %d degrees, Move Forward %d cm.", 90 - objects[smallestObject].angle, realDistance);
                        for (k = 0; k < strlen(data); k++) {
                            uart_sendChar(data[k]);
                        }
                    } else if (objects[smallestObject].angle >= 90) {
                        sprintf(data, "\r\nTurn Left %d degrees, Move Forward %d cm.", objects[smallestObject].angle - 90, realDistance);
                        for (k = 0; k < strlen(data); k++) {
                            uart_sendChar(data[k]);
                        }
                    }

                    //Insert break here to wait for second h, if second h, set doSomething to 1 again and keep going
                    //Else, sit and wait, or if t switch to manual
                    notVHCount = hCount;
                    while(hCount % 2 != 0) {

                    }
                    notVHCount = hCount;

                    // Possible way to abort the movement??

                    if(objects[smallestObject].angle < 90) { // Move bot
                        turn_clockwise(sensor_data, 90 - objects[smallestObject].angle);
                        move_forward(sensor_data, realDistance);
                    } else if (objects[smallestObject].angle >= 90) {
                        turn_counter_clockwise(sensor_data, objects[smallestObject].angle - 90);
                        move_forward(sensor_data, realDistance);
                    }

                    oi_setWheels(0,0);

                    lcd_printf("Angle:%hd Dist:%.2f", smallestAngle, pingScan / realScans);
                }
            }
            break;

        case 2:
            doSomething = 0;
            state = mode;
            runScan(scanData, state);
            cyBOT_Scan(179, &scan);
            createObjects(scanData, objects);
            displayObjects(scanData, objects);

            break;
        case 3:
            doSomething = 0;
            move_forward(sensor_data, 2);
            break;
        case 4:
            doSomething = 0;
            move_backward(sensor_data, 2);
            break;
        case 5:
            doSomething = 0;
            turn_counter_clockwise(sensor_data, 2);
            break;
        case 6:
            doSomething = 0;
            turn_clockwise(sensor_data, 2);
            break;

        }
    }
}

//    short scans = 1;
//    float range = .03;
//    volatile float currScan = 0.0;
//    volatile float lastScan = 0.0;
//
//    volatile short objectCount = 0;
//    volatile short points = 0;
//    volatile short start = -1;
//    volatile short scanDataIterate;

//                bool canWrite = false; // Helps determine if an object can be created at the moment
//                for(scanDataIterate = 1; scanDataIterate < 90; scanDataIterate++) { // iterate through scanData values
//                    currScan = scanData[scanDataIterate]; // Current scanData value
//                    lastScan = scanData[scanDataIterate - 1]; // value behind current scanData value
//
//                    if(currScan < .6) { // filters out IR values over 1 meter because our IR measurement isn't accurate beyond 50 cm
//                        if(currScan <= lastScan + range && currScan >= lastScan - range) { // If current value is in the range of the last value add points
//                            if(start == -1) {
//                                start = scanDataIterate;
//                                canWrite = true;
//                            }
//                            points++; // If points >= 5 then its probably an object
//                        } else if (points >= 4 && canWrite == true) { // probably and object and can write object then create a struct
//                            points = 1;
//                            objects[objectCount].start = start - 1;
//                            objects[objectCount].finish = scanDataIterate;
//                            objects[objectCount].size = objects[objectCount].finish - objects[objectCount].start;
//                            objects[objectCount].angle =  (objects[objectCount].finish + objects[objectCount].start);
//                            objects[objectCount].fr = true;
//                            objectCount++;
//                            start = -1; // reset values
//                            canWrite = false;
//                        } else {
//                            start = -1; // if not within range then reset all values
//                            points = 1;
//                            canWrite = false;
//                        }
//                    } else {
//                        start = -1; // if beyond 1 meter then reset values
//                        points = 1;
//                        canWrite = false;
//                    }
//                }

/**Autonomous mode:
 * first h - scan and print planned movement
 * second h - move and scan and print again
 * repeat
 *
 * Manual mode: Switch whenever ....
 * hit m for a scan
 * wasd for movement
 *
 *
 * H steps
 * not WASD in automatic
 */






