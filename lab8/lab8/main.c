#include <stdint.h>
#include <stdbool.h>
#include <math.h>
#include "Timer.h"
#include "uart.h"
#include "lcd.h"
#include "adc.h"
#include "driverlib/interrupt.h"
#include "open_interface.h"
#include "movement.h"
#include "cyBot_Scan.h"

// Volatile variables
volatile int doSomething;
volatile bool mode = true;
volatile short hCount;
volatile char input;
volatile bool foundSmallest = false;

//Object Struct
struct someObject{
    short start;
    short finish;
    short size;
    short angle;
    bool fr;
};

void runScan(float scanData[], bool state, short totalScans, cyBOT_Scan_t scan) {
    /*
     * Distance equation stuff y = 14937x^-.933
     * y = raw ir data
     * x = distance
     */
    double distance;
    double denom = 14937.0;
    double exponent = -1/.933;

    hCount = 1;
    char str[] = "\r\nDegrees   Distance(cm)\r\n";
    uart_sendStr(str);

    short i, scans;
    char data[40];
//    cyBOT_Scan_t scan;
    for(i = 0; i <= 90; i++) { // Using IR scan raw IR values and put them in scanData
        if(mode != state) { // Break out using interrupts
            break;
        }
        cyBOT_Scan(i * 2, &scan);

        distance = 0;
        for(scans = 0; scans < totalScans; scans++) {
            distance += adc_read();
        }
        distance /= totalScans * denom;

        distance = pow(distance, exponent);
        if(distance - 5 <= 0) {
            foundSmallest = true;
            break;
        }
        scanData[i] = distance;

        sprintf(data, "%-10d%.2f\n", i*2, distance);
        uart_sendStr(data);
    }
    uart_sendChar('\n');
    free(data);
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
//    short current = -1;
    float range = 4;

    int i;
    for(i = 0; i < 5; i++) {
        objects[i].start = 181;
        objects[i].finish = 181;
        objects[i].size = 0;
        objects[i].angle = 0;
        objects[i].fr = false;
    }


    bool canWrite = false; // Helps determine if an object can be created at the moment
    for(scanDataIterate = 1; scanDataIterate < 91; scanDataIterate++) { // iterate through scanData values
        currScan = scanData[scanDataIterate]; // Current scanData value
        lastScan = scanData[scanDataIterate - 1]; // value behind current scanData value


        if(currScan <= lastScan + range && currScan >= lastScan - range && currScan < 60 && scanDataIterate < 90) { // If current value is in the range of the last value add points
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
    }
}

short determineSmallest(struct someObject objects[]) {
    char data[40];
    short z;
    short smallestObject = 0;
    for(z = 0; z < 5; z++) {
        if(objects[z].size < objects[smallestObject].size && objects[z].fr == true && objects[z].size < 10) {
            smallestObject = z;
        }
        if(objects[z].fr == true) {
            sprintf(data, "\r\nObj:%d s:%hd f:%hd size:%hd", z + 1, objects[z].start * 2, objects[z].finish * 2, objects[z].size);
            uart_sendStr(data);
        }
    }

    free(data);
    return smallestObject;
}

/**
 * main.c
 */
int main(void)
{
    //    int measurements[101];
    //    short measure_counter;

    //Inits
    lcd_init();
    adc_init();
    uart_init();
    uart_interrupt_init();
    cyBOT_init_Scan(0b0011); // Init Servo, Ping, IR (not in order)
//    cyBOT_SERVO_cal(); // For calibration
    right_calibration_value = 343000;
    left_calibration_value = 1335250;
    // Sensor and scan
    oi_t *sensor_data = oi_alloc();
    oi_init(sensor_data);
    cyBOT_Scan_t scan;

    // Arrays
    char data[50];
    float scanData[91];

    // Iterators
    short scans;
    short w; // For ping distance scan
    short dothing; // For part 1 lab8



    //Stoppers
    short totalScans = 10;

    // Interrupt helper
    bool state = false;

    // Objects array
    struct someObject objects[5];

    // Smallest object stuff
    short smallestObjIndex;
    float smallestObjDist;
    short smallestObjAngle;
    short realScans = 0;
    float pingScan = 0;
    double denom = 14937.0;
    double exponent = -1/.933;

    // Driving to object stuff
    /*
     * true = turn clockwise
     * false = turn counter clockwise
     */
    bool dir = true;




    while(1) {
//        input = uart_receive();
//        uart_sendChar(input);
        uart_sendChar(doSomething);
        switch(doSomething) {
        case 1:
            if(mode) {
                state = mode;
                doSomething = 0;
                uart_sendChar('\r');
                uart_sendChar('\n');
                runScan(scanData, state, totalScans, scan);
                timer_waitMicros(100);
                createObjects(scanData, objects);
                if(mode) {
                    smallestObjIndex = determineSmallest(objects);
                    smallestObjAngle = objects[smallestObjIndex].angle;
                    cyBOT_Scan(smallestObjAngle, &scan);
                    smallestObjDist = scanData[objects[smallestObjIndex].angle / 2] - 13;

                    if(smallestObjDist < 5) {
                        smallestObjDist = 0;
                    } else if (foundSmallest) {
                        break;
                    }

//                    for(w = 0; w < 10; w++) { // scan the smallest object with ping to determine distance
//                        cyBOT_Scan(smallestObjAngle, &scan);
//
//                        if(scan.sound_dist <= 60) {
//                            pingScan += scan.sound_dist;
//                            realScans++;
//                        }
//                    }
//                    cyBOT_Scan(smallestObjAngle, &scan);
//
//                    smallestObjDist = 0.0;
//                    for(scans = 0; scans < totalScans; scans++) {
//                        smallestObjDist += adc_read();
//                    }
//                    smallestObjDist /= totalScans * denom;
//
//                    smallestObjDist = pow(smallestObjDist, exponent);

                    if(objects[smallestObjIndex].angle < 90) { // Calculate move
                        sprintf(data, "\r\nTurn Right %d degrees, Mtove Forward %.2f cm.", 90 - objects[smallestObjIndex].angle, smallestObjDist);
                        dir = true;
                    } else if (objects[smallestObjIndex].angle >= 90) {
                        sprintf(data, "\r\nTurn Left %d degrees, Move Forward %.2f cm.", objects[smallestObjIndex].angle - 90, smallestObjDist);
                        dir = false;
                    }
                    uart_sendStr(data);
                    while(hCount % 2 != 0) {

                    }

                    if(mode) {
                        if(dir) { // Move bot
                            turn_clockwise(sensor_data, 90 - objects[smallestObjIndex].angle);
                            move_forward(sensor_data, (short) smallestObjDist);
                        } else {
                            turn_counter_clockwise(sensor_data, objects[smallestObjIndex].angle - 90);
                            move_forward(sensor_data, (short) smallestObjDist);
                        }
                    }
                }
            }
            break;
        case 2:
            doSomething = 0;
            state = mode;
            runScan(scanData, state, totalScans, scan);
            cyBOT_Scan(179, &scan);
            createObjects(scanData, objects);
            smallestObjIndex = determineSmallest(objects);
            smallestObjAngle = objects[smallestObjIndex].angle;
            cyBOT_Scan(smallestObjAngle, &scan);

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
        case 7:
            doSomething = 0;
            for(dothing = 0; dothing < 100; dothing++) {
                sprintf(data, "\r\n%-10d", adc_read());
                uart_sendStr(data);
            }
            break;
        case 8:
            doSomething = 0;
            float distance = 0.0;
            double denom = 14937.0;
            double exponent = -1/.933;

            for(scans = 0; scans < totalScans; scans++) {
                distance += adc_read();
            }
            distance /= totalScans * denom;

            distance = pow(distance, exponent);
            sprintf(data, "\r\n%-10.2f", distance);
            uart_sendStr(data);

            break;
        }
    }
}



/*
 * Prelab
 * 1: GPIO port B registers for uart and adc init
 * 2: A/D registers activate sampler, event mux sel, sample seq mux sel, set SS control, access FIFO register of a/d
 * 3. Took scans at different distances and made a line of best fit
 *
 *
 *
 */
