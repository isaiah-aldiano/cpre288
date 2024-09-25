#include <stdio.h>
#include <stdlib.h>
#include <ctype.h>
#include <stdbool.h>
#include <math.h>
#include "data.h"

void main()
{
    int i = 0;

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
    for(int i = 0; i < 5; i++) {
        listOfOBJ[i].written = false;
    }

    for(int x = 0; x < 91; x++) {
        input = sensor_data_array[x];

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
                listOfOBJ[writtenOBJ-1].distance = sensor_data_array[listOfOBJ[writtenOBJ-1].angle / 2] * 100;
                listOfOBJ[writtenOBJ-1].angle += cont;
            }
            cont = 0;
            }
        prior = input;
    }


    int smallAngle = 0;
    int min = 200;

    printf("Object#   Angle     Distance  Width\n");
    for (int o = 0; o < 5; o++) {
        if(listOfOBJ[o].written) {
            printf("%-10d%-10d%-10d%-10d\n", o+1,listOfOBJ[o].angle, listOfOBJ[o].distance, listOfOBJ[o].width );
            

            if(listOfOBJ[o].width < min) {
                min = listOfOBJ[o].width;
                smallAngle = listOfOBJ[o].angle;
            }
        }
    }

    printf("%d", smallAngle);
}
