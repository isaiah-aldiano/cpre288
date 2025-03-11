/*
 * movement.h
 *
 *  Created on: Sep 9, 2024
 *      Author: ialdiano
 */

#ifndef MOVEMENT_H_
#define MOVEMENT_H_

extern volatile bool mode;

void move_forward(oi_t *sensor, int centimeters);
void move_backward(oi_t *sensor, int centimeters);
void turn_clockwise(oi_t *sensor, int degrees);
void turn_counter_clockwise(oi_t *sensor, int degrees);





#endif /* MOVEMENT_H_ */
