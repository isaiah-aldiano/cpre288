#include "open_interface.h";

/*
 * movement.c
 *
 *  Created on: Sep 9, 2024
 *      Author: ialdiano
 */

// Moves

int base = 0;
const double ERROR = .3; // Physically tested approximate error in roomba rotation

// base allows the distance given to be reached from a "start"
void resetBase() {
    base = 0;
}

// Positive -> Forward
// Negative -> Backward
void move_forward(oi_t *sensor, int centimeters){
    int travel_distance = centimeters * 10; // Convert to mm
    oi_setWheels(250, 250); // Set wheel speed
    while(base < travel_distance) {
        oi_update(sensor);
        base += sensor->distance; // Add distance traveled to base
//        printf("%f %d\n", sensor->distance, base);
    }

    resetBase();
}

void move_backward(oi_t *sensor, int centimeters){
    int travel_distance = centimeters * 10; // Convert to mm
    oi_setWheels(-250, -250); // Set wheel speed
    while(base < travel_distance) {
        oi_update(sensor);
        base -= sensor->distance; // Subtract distance reversed to base
    }

    resetBase();
}

// Positive -> Counter-clockwise
// Negative -> Clockwise
void turn_clockwise(oi_t *sensor, int degrees) {
    oi_setWheels(-100, 100);
    while(base > (degrees * -1) + (degrees * ERROR)) {
        oi_update(sensor);
        base += sensor->angle;
    }

    resetBase();
}

void turn_counter_clockwise(oi_t *sensor, int degrees) {
    oi_setWheels(100, -100);
    while(base < degrees - (degrees * ERROR)) {
        oi_update(sensor);
        base += sensor->angle;
    }

    resetBase();
}


void go_the_distance(oi_t *sensor, int distance) {
    int distance_traveled = 0;
    while(distance_traveled < distance - 30) {
        move_forward(sensor, 1);
        distance_traveled++;
        if (sensor->bumpLeft) {
            move_backward(sensor, 15);
            distance_traveled -= 15;
            turn_clockwise(sensor, 90);
            move_forward(sensor, 25);
            turn_counter_clockwise(sensor, 90);
        } else if(sensor->bumpRight) {
            move_backward(sensor, 15);
            distance_traveled -= 15;
            turn_counter_clockwise(sensor, 90);
            move_forward(sensor, 25);
            turn_clockwise(sensor, 90);
        }
        printf("%d\n", distance_traveled);

    }

}




