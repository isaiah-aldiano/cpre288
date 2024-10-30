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
        scanData[i] = distance;

        sprintf(data, "\r\n%-10d%.2f\0", i*2, distance);
        uart_sendStr(data);
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
    for(scanDataIterate = 1; scanDataIterate < 90; scanDataIterate++) { // iterate through scanData values
        currScan = scanData[scanDataIterate]; // Current scanData value
        lastScan = scanData[scanDataIterate - 1]; // value behind current scanData value


//        if(currScan < 60) { // filters out IR values over 1 meter because our IR measurement isn't accurate beyond 50 cm
            if(currScan <= lastScan + range && currScan >= lastScan - range && currScan < 60) { // If current value is in the range of the last value add points
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
//        }
//
//        else {
//                   if (points >= 4 && canWrite == true) { // probably and object and can write object then create a struct
//                       points = 1;
//                       objects[objectCount].start = start - 1;
//                       objects[objectCount].finish = scanDataIterate;
//                       objects[objectCount].size = objects[objectCount].finish - objects[objectCount].start;
//                       objects[objectCount].angle =  (objects[objectCount].finish + objects[objectCount].start);
//                       objects[objectCount].fr = true;
//                       objectCount++;
//                       start = -1; // reset values
//                       canWrite = false;
//                   }
//                   start = -1; // if beyond 1 meter then reset values
//                   points = 1;
//                   canWrite = false;
//               }

//        if(currScan < 60 && lastScan < 60 && currScan <= lastScan + range && currScan >= lastScan - range) {
//            points = 2;
//            start = scanDataIterate - 1;
//            current = scanDataIterate;
//
//
//            while(currScan <= lastScan + range && currScan >= lastScan - range) {
//                points++;
//
//            }
//        }




    }
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
    cyBOT_init_Scan(0b0111); // Init Servo, Ping, IR (not in order)
//    cyBOT_SERVO_cal();
    right_calibration_value = 337750;
    left_calibration_value = 1324750;
    // Sensor and scan
    oi_t *sensor_data = oi_alloc();
    oi_init(sensor_data);
    cyBOT_Scan_t scan;

    // Arrays
    char data[50];
    float scanData[91];

    // Iterators
    short scans;

    //Stoppers
    short totalScans = 10;

    // Interrupt helper
    bool state = false;

    // Objects array
    struct someObject objects[5];




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
            }
            break;
        }
    }
}
