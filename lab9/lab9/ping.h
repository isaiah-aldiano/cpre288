#include <stdint.h>
#include <inc/tm4c123gh6pm.h>
#include "Timer.h"
#include "driverlib/interrupt.h"
#include <math.h>


/*
 * ping.h
 *
 *  Created on: Nov 2, 2024
 *      Author: ialdiano
 */

#ifndef PING_H_
#define PING_H_

volatile enum {LOW, HIGH, DONE} state;
volatile unsigned int rising_time;
volatile unsigned int falling_time;
volatile int overflow;

void ping_init(void);
int ping_read();
void send_pulse();
void timer3b_handler();




#endif /* PING_H_ */
