#include "open_interface.h"
#include "cyBot_uart.h"
#include "Timer.h"
#include "lcd.h"
#include "string.h"
#include "stdio.h"
#include "cyBot_Scan.h"
#include "ctype.h"
/*
 * Part 1:
 *
 * 1.1) 7 - 9?
 * 1.2)*2.5*, 2.0, 2.5, 2.0, 2.5, .5, 2.5, 1.5 *2.5*
 * 1.3)       40,  8,   15,  12,  36, 26,  20
 *
 * Part 3:
 *
 * 3. besttestsofar.txt
 * 4. cleanup.txt, for each scan of the sensor program the sensor to scan 4 - 5 times and send the averaged data to the buffer for data logging
 *
 *
 * Manually:
 *  Pro - Understands sensor capabilities on a deeper level
 *  Con - Time consuming
 * Automatically:
 *  Pro: Fast and simple
 *  Con: Have to create the visual generation program
 */

/**
 * main.c
 */
void main()
{
    oi_t *sensor_data = oi_alloc();
    oi_init(sensor_data);

    cyBot_uart_init();
    int features = 0b0111;

    timer_init();
    lcd_init();
//    cyBOT_init_Scan(features);
//    cyBOT_SERVO_cal();


    cyBOT_init_Scan(features);
    // Bot 15
    right_calibration_value = 280000;
    left_calibration_value = 1256500;
//  Use if using new bot
//  cyBOT_SERVO_cal();


    char str[] = "\033cDegrees   Distance(m)\r\n";
    int j;
    for(j = 0; j < strlen(str); j++) {
        cyBot_sendByte(str[j]);
    }



    int i, k;
    cyBOT_Scan_t scan;
    char buffer[20];
    for(i = 0; i <= 180; i+=2) {
        timer_waitMillis(100);
        cyBOT_Scan(i, &scan);
        sprintf(buffer, "%-10d%-10.2f\r\n", i, scan.sound_dist / 100);
        for(k = 0; k < strlen(buffer); k++) {
            cyBot_sendByte(buffer[k]);
        }
    }

    cyBOT_Scan(0, &scan);

    FILE *fptr;
    fptr = fopen("cleanup.txt", "r");

    i = 0;
    int angle[91];
    double distance[91];

    char line[27];

    while(i <=90) {
        char ang[4];
        char dis[5];

        fgets(line, 27, fptr);
        int c;
        for(c = 0; c < 3; c++) {
            if(isdigit(line[c])) {
                ang[c] = line[c];
            }
        }
        int d;
        for(d = 0; d < 5; d++) {
            if(isdigit(line[d+10]) || line[d+10] == '.') {
                dis[d] = line[d+10];
            }
        }

        angle[i] = atoi(ang);
        distance[i] = atof(dis);

        i++;
    }
    double input = 0.00;
    double prior = 0.00;
    int cont = 0;
    int writtenOBJ = 0;

    struct obj{
        bool written;
        int angle;
        int distance;
        int width;
    };

    struct obj listOfOBJ[5];
    for(i = 0; i < 5; i++) {
        listOfOBJ[i].written = false;
    }
    int x;
    for(x = 0; x < 91; x++) {
        input = distance[x];

        if(input <= prior+0.05 && input >= prior-0.05) {
            cont++;
            if(cont == 5) {
                listOfOBJ[writtenOBJ].written = true;
                listOfOBJ[writtenOBJ].angle = x * 2 - 10;
                writtenOBJ++;
            }
        }
        else {
            if(cont >= 5) {
                listOfOBJ[writtenOBJ-1].width = cont * 2;
                listOfOBJ[writtenOBJ-1].distance = distance[listOfOBJ[writtenOBJ-1].angle / 2] * 100;
                listOfOBJ[writtenOBJ-1].angle += cont;
            }
            cont = 0;
            }
        prior = input;
    }
    int smallAngle = 0;
    int min = 200;
    int o;
    for (o = 0; o < 5; o++) {
        if(listOfOBJ[o].written) {
            if(listOfOBJ[o].width < min) {
                min = listOfOBJ[o].width;
                smallAngle = listOfOBJ[o].angle;
            }
        }
    }

    cyBOT_Scan(smallAngle, &scan);




//    return 0;
}