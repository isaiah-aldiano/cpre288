/*
 * servo.c
 *
 *  Created on: Nov 6, 2024
 *      Author: lewisc65
 */
#include "lcd.h"
#include "timer.h"


void servo_init(void){
    SYSCTL_RCGCGPIO_R |= 0b10;
    GPIO_PORTB_DEN_R |= 0b100000;
    SYSCTL_RCGCTIMER_R |= 0b10;
    GPIO_PORTB_AFSEL_R |= 0b100000;
    GPIO_PORTB_PCTL_R &= ~0x800000;
    GPIO_PORTB_PCTL_R |= 0x700000;

    TIMER1_CTL_R &= ~0x100; //disable timer
    TIMER1_CFG_R |= 0x4;
    TIMER1_TBMR_R |= 0b01010; //Set periodic
    //TIMER1_TBMR_R &= 0b01010;

    TIMER1_CTL_R &= ~0x400;

    TIMER1_TBILR_R = 0x4E200 & 0xFFFF; //4E200 = 320000; //Timer start set
    TIMER1_TBPR_R = 0x4E200 >> 16;

    TIMER1_TBMATCHR_R = 0x4BED8 & 0xFFFF; //304000 = 0x4A380
    TIMER1_TBPMR_R = 0x4BED8 >> 16;

    TIMER1_CTL_R |= 0x100;

}

int servo_move(int degrees){

    TIMER1_TBILR_R = 0x4E200 & 0xFFFF; //4E200 = 320000; //Timer start set
    TIMER1_TBPR_R = 0x4E200 >> 16;

    int slope = (285298 - 311710)/180;   //0x45B00
    int match = (311710 + (slope*degrees));

    TIMER1_TBMATCHR_R = match & 0xFFFF; //304000 = 0x4A380
    TIMER1_TBPMR_R = match >> 16;

    timer_waitMillis(20);

    return match;
}


