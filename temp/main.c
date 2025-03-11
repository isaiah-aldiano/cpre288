#include "uart_extra_help.h"
#include "math.h"
#include "movement.h"
#include "adc.h"
#include "ping.h"
#include "servo.h"

void Ascan(void);

/**
 * main.c
 */
int main(void)
{
    oi_t *sensor_data = oi_alloc();
    oi_init(sensor_data);
    timer_init();
    lcd_init();
    servo_init();
    ping_init();
    ADC_init();

    int i = 0;
    int roombaWidth = 32; //Roomba width in centimeters, used to determine turn distance
    int fieldWidth = 64; //In cm
    int fieldLength = 100; //In cm
    int cycles = fieldWidth / roombaWidth;
    int distTrav = 0;
    char turnDir = 'r'; //Keeps track of which way to turn

    for(i = 0; i < cycles; i++){
        //Travel small distance and scan until reaching full field length
        while(distTrav < fieldLength) {
            distTrav += 40;
            move_forward(sensor_data, 40);

            //Scan field
            Ascan();
        }

        move_forward(sensor_data, fieldLength - distTrav);
        distTrav = 0;

        if(turnDir == 'r'){
            turnDir = 'l';
            turn_clockwise(sensor_data, 90);
            move_forward(sensor_data, roombaWidth);
            turn_clockwise(sensor_data, 90);
        } else {
            turnDir = 'r';
            turn_counterclockwise(sensor_data, 90);
            move_forward(sensor_data, roombaWidth);
            turn_counterclockwise(sensor_data, 90);
        }
    }


    oi_free(sensor_data);
}


//TODO: detect only larger objects to avoid
void Ascan(void){
    int i = 0;
    int n = 0; //Number of objects
    int objectStart[25];
    int objectEnd[25];
    float data[91];
    int s1 = 0;
    int s2 = 0;

    //Scan for data
    servo_move(0);
    timer_waitMillis(400);
    for(i = 0; i <= 90; i++){
        servo_move(i*2);
        s1 = ADC_read();
        servo_move(i*2);
        s2 = ADC_read();

        data[i] = (s1 + s2) / 2;
//        sprintf(str, "%.3f \r\n", data[i]);
//        helpSend(str);
    }

    data[90] = 70;
    data[0] = 70;

    //Clean data
    for(i = 1; i <= 90; i++){
        float diffBelow = abs(data[i-1] - data[i]);
        float diffAbove = abs(data[i+1] - data[i]);

        //if difference of data above and below is too much, the data is set to the average of above and below
        if((diffBelow > 5) && (diffAbove > 5)){
            data[i] = (data[i+1] + data[i-1]) / 2;
        }
    }

    //Collect object data
    if(data[0] < 45 && data[1] < 45){
       objectStart[n] = 0;
    }

    for(i = 1; i <= 60; i++){
        if(data[i] < 45){
            if(data[i-1] > 45){
                //helpSend("Object detected \n\r");
                objectStart[n] = i * 2;
            } else if(data[i+1] > 45){
                objectEnd[n] = i * 2;
                n++;
            }
        }
    }

    int midAngle[25];
    int objectDist[25];
    float linearWidth [25];
    int avoidObjects = 0;

    //Determine width of objects
    for(i = 0; i < n; i++){

        midAngle[i] = (objectEnd[i] + objectStart[i])/2; //Calculate the middle angle of each object

        servo_move(midAngle[i]);
        timer_waitMillis(400);
        servo_move(midAngle[i]);

        objectDist[i] = ping_read();

        int width = objectEnd[i] - objectStart[i];
        linearWidth[i] = objectDist[i] * width * (3.1415926/ 180); //radius * angle (in radians)
        ///sprintf(str, "%.3f \r\n", linearWidth[i]);
        //helpSend(str);

        //If object is to be avoided, put to front of array and new count of avoid objects created
        if(linearWidth[i] >= 10){ //TODO: figure out measurement of large objects
            midAngle[avoidObjects] = midAngle[i];
            objectDist[avoidObjects] = objectDist[i];
            linearWidth[avoidObjects] = linearWidth[i];

            avoidObjects++;
        }
    }

    //TODO: avoidance of large objects



}
