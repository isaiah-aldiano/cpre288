#include <stdio.h>
#include <stdlib.h>
#include <ctype.h>
#include <stdbool.h>

void main()
{
    FILE *fptr;
    fptr = fopen("cleanup.txt", "r");

    int i = 0;
    int angle[91];
    double distance[91];

    char line[27];

    while(i <=90) {
        char ang[4];
        char dis[5];

        fgets(line, 27, fptr);

        for(int c = 0; c < 3; c++) {
            if(isdigit(line[c])) {
                ang[c] = line[c];
            }
        }

        for(int d = 0; d < 5; d++) {
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
    for(int i = 0; i < 5; i++) {
        listOfOBJ[i].written = false;
    }

    for(int x = 0; x < 91; x++) {
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
