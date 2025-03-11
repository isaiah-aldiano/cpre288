/*
 * ping.c
 *
 *  Created on: Oct 30, 2024
 *      Author: lewisc65
 */
#include "timer.h"

void TIMER3B_Handler(void);
void send_pulse();

volatile enum {LOW, HIGH, DONE} state; // set by ISR
volatile unsigned int rising_time; //Pulse start time: Set by ISR
volatile unsigned int falling_time; //Pulse end time: Set by ISR
volatile unsigned int level = 0;
volatile unsigned int overflowCount;

void ping_init(void) {
    overflowCount = 0;

    SYSCTL_RCGCGPIO_R |= 0b10;
    GPIO_PORTB_DEN_R |= 0x8;
    SYSCTL_RCGCTIMER_R |= 0x8;
    GPIO_PORTB_AFSEL_R |= 0x8;
    GPIO_PORTB_PCTL_R &= 0xFFFF0FFF;
    GPIO_PORTB_PCTL_R |= 0x7000;

    TIMER3_CTL_R &= 0xEFF;
    TIMER3_CFG_R |= 0x4;

    TIMER3_TBMR_R |= 0b00111; //
    TIMER3_TBMR_R &= 0b00111;

    TIMER3_CTL_R |= 0xC00; //Set edge detection

    TIMER3_TBILR_R |= 0xFFFF; //Timer start set
    TIMER3_TBPR_R |= 0xFF;

    TIMER3_IMR_R &= ~0x400; //interrupt enable
    NVIC_EN1_R |= 0x10;

    TIMER3_CTL_R |= 0x100;
    TIMER3_ICR_R |= 0x400;

    IntRegister(INT_TIMER3B, TIMER3B_Handler);
}

int ping_read(void){
    send_pulse();

    int timeReturn;

    //timer_waitMicros(1000000);

    while(level < 2){}; //wait for pulse to be recieved

    level = 0; //reset where pulse is at
    timeReturn = rising_time - falling_time; //calculate pulse time

    if(timeReturn < 0){
        overflowCount++;
    }

    return timeReturn;
}

void send_pulse() {
    GPIO_PORTB_AFSEL_R &= 0b11110111;
    TIMER3_ICR_R |= 0x400;
    TIMER3_IMR_R &= 0xDFF;

    GPIO_PORTB_DIR_R |= 0b1000;// Set PB3 as output
    GPIO_PORTB_DATA_R &= ~0x08;// Set PB3 to low 0b1000
    GPIO_PORTB_DATA_R |= 0x08;// Set PB3 to high
    // wait at least 5 microseconds based on data sheet
    timer_waitMicros(6);
    TIMER3_ICR_R |= 0x400;
    GPIO_PORTB_DATA_R &= ~0x08;// Set PB3 to low
    GPIO_PORTB_DIR_R &= 0x7;// Set PB3 as input

    //After Pulse send
    GPIO_PORTB_AFSEL_R |= 0b1000;
    GPIO_PORTB_DIR_R &= 0b0111;
    TIMER3_ICR_R |= 0x400;
    TIMER3_IMR_R |= 0x400;

}

void TIMER3B_Handler(void)
{
    if(level == 0){
        rising_time = TIMER3_TBR_R;
    } if(level == 1){
        falling_time = TIMER3_TBR_R;
    }
    TIMER3_ICR_R |= 0x400;
    level++;

}

