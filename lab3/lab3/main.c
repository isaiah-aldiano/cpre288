#include "open_interface.h"
#include "cyBot_uart.h"
#include "Timer.h"
#include "lcd.h"
#include "string.h"
#include "stdio.h"
#include "cyBot_Scan.h"
#include "ctype.h"
#include "math.h"
#include <movement.h>

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
    // Bot 12
    right_calibration_value = 243250;
    left_calibration_value = 1246000;//  Use if using new bot
    //  cyBOT_SERVO_cal();

    float distances[90];

    cyBOT_Scan_t scan;
    char input = '0';

    cyBOT_Scan(0, &scan);
    timer_waitMillis(100);


    while (true)
    {
        input = cyBot_getByte();
        cyBot_sendByte(input);
        int j;

        if (input == 'm')
        {
//            char str[] = "\033cDegrees   Distance(m)\r\n";
//            for(j = 0; j < strlen(str); j++) {
//                cyBot_sendByte(str[j]);
//            }
            int i;
            float a;
            int k;
            char buffer[30];
            char scans = 3;
            for (i = 0; i <= 180; i+=2)
            {
                a = 0.0;
                for (j = 0; j < scans; j++)
                {
                    cyBOT_Scan(i, &scan);
                    a += scan.sound_dist / 100.0;
                }
                distances[i/2] = a / scans;

                sprintf(buffer, "%-10d%-10.2f\r\n", i, distances[i/2]);
                for (k = 0; k < strlen(buffer); k++)
                {
                    cyBot_sendByte(buffer[k]);
                }
            }
        }

        short q, w;
        short pointTo = 0;
        short shortestAngle = 180;
        short currentAngle = 0;
        short points = 0;
        short minimumPoints = 10;
        float threshold = .03;
        short currentIndex = 0;
        short startIndex = 0;

        for(q = 0; q < 90; q++) {
            startIndex = q;

            for(w = q; w < 90; w++) {
//              if(distances[currentIndex] > 1.0) {
//                  break;
//              }
              if(distances[w] <= distances[startIndex] + threshold && distances[w] >= distances[startIndex] - threshold) {
                points++;
                if(points > minimumPoints) {
                    currentAngle = w - startIndex;
                }
              } else {
                  if(currentAngle < shortestAngle && points > minimumPoints) {
                      shortestAngle = currentAngle;
                      pointTo = w + startIndex;

                      if(pointTo % 2 != 0) {
                          pointTo += 1;
                      }
                  }
                  points = 0;
                  q = w;
                  break;
              }
            }
        }

        cyBOT_Scan(pointTo, &scan);

        oi_t *sensor = oi_alloc();
        oi_init(sensor);
        int botLength = 10;
        int moveIt = (distances[pointTo] * 100.0 * .93);

        lcd_printf("%hu %d\n", pointTo, moveIt);


        if(pointTo < 90) {
            turn_clockwise(sensor, 90 - pointTo);
            move_forward(sensor, moveIt - botLength);
        } else if (pointTo >= 90) {
            turn_counter_clockwise(sensor, pointTo - 90);
            move_forward(sensor, moveIt - botLength);
        }

        oi_setWheels(0,0);
        oi_free(sensor);




//        cyBOT_Scan(0, &scan);

        /*Run 1 scan
         * Pinging 3 times per angle
         * Average the 3 pings
         * Save average in array
         * Run f function of the array
         */

        //Point at smallest object.
//        int i = 0;
//
//        float input = 0.00;
//        float prior = 0.00;
//        int cont = 0;
//        int writtenOBJ = 0;
//
//        struct obj
//        {
//            bool written;
//            float angle;
//            float distance;
//            float width;
//        };
//
//        struct obj listOfOBJ[5];
//        for (i = 0; i < 5; i++)
//        {
//            listOfOBJ[i].written = false;
//        }
//
//        for (i = 0; i < 91; i++)
//        {
//            input = distances[i];
//
//            if (input <= prior + 0.05 && input >= prior - 0.05)
//            {
//                cont++;
//                if (cont == 5)
//                {
//                    listOfOBJ[writtenOBJ].written = true;
//                    writtenOBJ++;
//                }
//            }
//            else
//            {
//                if (cont >= 5)
//                {
//                      listOfOBJ[writtenOBJ - 1].angle = i * 2 - cont;
//                      listOfOBJ[writtenOBJ - 1].distance = distances[i * 2 - cont];
//
////                    listOfOBJ[writtenOBJ - 1].distance = distAvg;
//                      listOfOBJ[writtenOBJ - 1].width = (listOfOBJ[writtenOBJ - 1].distance * sin(cont)) * 2;
////                    listOfOBJ[writtenOBJ - 1].angle = i * 2 - listOfOBJ[writtenOBJ - 1].width / 2;
//                }
//                cont = 0;
//            }
//            prior = input;
//        }
//
//        float smallAngle = 0;
//        int min = 200;
//
//        printf("\nObject#   Angle     Distance  Width\n");
//
//        for (i = 0; i < 5; i++)
//        {
//            if (listOfOBJ[i].written)
//            {
//                printf("%-10d%-10f%-10f%-10f\n", i + 1, listOfOBJ[i].angle, listOfOBJ[i].distance, listOfOBJ[i].width);
//
//                if (listOfOBJ[i].width < min)
//                {
//                    min = listOfOBJ[i].width;
//                    smallAngle = listOfOBJ[i].angle;
//                }
//            }
//        }
//        printf("%d",  smallAngle);
//        cyBOT_Scan(smallAngle, &scan);
    }
}
