/*
 * movement.h
 *
 *  Created on: Sep 11, 2024
 *      Author: lewisc65
 */

#ifndef MOVEMENT_H_
#define MOVEMENT_H_
#include "open_interface.h"

void move_forward(oi_t*sensor, int centimeters);

void turn_clockwise(oi_t *sensor, int degrees);

void move_backwards(oi_t*sensor, int centimeters);

void turn_counterclockwise(oi_t*sensor, int centimeters);

void bump (oi_t*sensor_data, int sum);

#endif /* MOVEMENT_H_ */
