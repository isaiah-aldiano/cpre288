#include <stdint.h>
#include <inc/tm4c123gh6pm.h>
#include "Timer.h"
#include "driverlib/interrupt.h"


/*
 * servo.h
 *
 *  Created on: Nov 12, 2024
 *      Author: ialdiano
 */

#ifndef SERVO_H_
#define SERVO_H_

void timer1b_handler(void);
void servo_init(void);
int servo_move(float degrees);
int get_match(void);
int get_match_subtract(void);
void button_init(void);
uint8_t button_getButton(void);
int get_direction(void);
void scan(int pointTo);

#endif /* SERVO_H_ */
