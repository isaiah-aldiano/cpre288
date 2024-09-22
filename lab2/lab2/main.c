#include "open_interface.h"
#include "movement.h"
#include "cyBot_uart.h"
#include "string.h"

/**
 * main.c
 */mmww
void main()
{

 oi_t *sensor_data = oi_alloc();
 oi_init(sensor_data);
// double sum = 0;
// oi_setWheels(500, 500); // move forward; full speed
// while (sum < 1000) {
// oi_update(sensor_data);
// sum += sensor_data->distance;
// }
// oi_setWheels(0, 0); // stop
// oi_free(sensor_data);


//    move_forward(sensor_data, 50); // ^
//    turn_clockwise(sensor_data, 90); // ->
//    move_forward(sensor_data, 50); //  -->
//    turn_clockwise(sensor_data, 90); // down
//    move_forward(sensor_data, 50); // bus down
//    turn_clockwise(sensor_data, 90); // <-
//    move_forward(sensor_data, 50); // <--

//      move_forward(sensor_data, 50);
//      turn_clockwise(sensor_data, 90);
//      turn_counter_clockwise(sensor_data,90);
//      turn_clockwise(sensor_data, 90);
//      move_backward(sensor_data, 50);


//      go_the_distance(sensor_data, 200);
         cyBot_uart_init();
         while(true) {
             char input = cyBot_getByte();
             cyBot_sendByte(input);

             if(input == 'm') {
                 char str[] = "Got an m";
                 int i;
                 for(i = 0; i < strlen(str); i++) {
                     cyBot_sendByte(str[i]);
                 }
             }
             switch(input) {

             case 'w':
                 move_forward(sensor_data, 50);
                 break;
             case 's':
                 move_backward(sensor_data, 50);
                 break;
             case 'a':
                 turn_counter_clockwise(sensor_data, 50);
                 move_forward(sensor_data, 50);
                 break;
             case 'd':
                 turn_clockwise(sensor_data, 50);
                 move_forward(sensor_data, 50);
                 break;
             }
             oi_setWheels(0,0); // stop

         }
      oi_setWheels(0,0); // stop
      oi_free(sensor_data);


}
