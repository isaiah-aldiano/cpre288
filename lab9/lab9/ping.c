#include "ping.h"

/*
 * ping.c
 *
 *  Created on: Nov 2, 2024
 *      Author: ialdiano
 */



void timer3b_handler() {
    uint32_t timer = TIMER3_TBR_R;

    if(state == DONE) {
        state = LOW;
    }
    if (state == LOW) {
        rising_time = timer;
        state = HIGH;
    } else if (state == HIGH) {
        falling_time = timer;
        state = DONE;
    }

    TIMER3_ICR_R |= 0x400;
}

void ping_init(void) {
    SYSCTL_RCGCGPIO_R |= 0x02;
    GPIO_PORTB_AFSEL_R |= 0x08;
    GPIO_PORTB_DEN_R |= 0x08;

    SYSCTL_RCGCTIMER_R |= 0x08;

    TIMER3_CTL_R &= 0xEFF; // Disable Timer 3
    TIMER3_CFG_R |= 0x04; // Set as 16 bit timer config
    TIMER3_TBMR_R |= 0x7; // Capture mode, Edge Time Mode
    TIMER3_TBMR_R &= 0xEF; // Count Down mode
    TIMER3_TBILR_R |= 0xFFFF; // Max Value for 16 bit register
    TIMER3_TBPR_R |= 0xFF; // Max value for 8 bit prescaler

    TIMER3_ICR_R |= 0x400; // Clear Timer 3B Capture mode event Interrupt
    TIMER3_IMR_R |= 0x400; // Enable Timber 3B capture mode event interrupts

    NVIC_EN1_R |= 0b10000; // Enable Pin 36 in the NVIC

    IntRegister(INT_TIMER3B, timer3b_handler); // Bind interrupt to handler
    IntMasterEnable();

    TIMER3_CTL_R |= 0xC00; // Enable Timer 3B with dual edge detection
    //1100
}

int ping_read(void) {
    send_pulse();

    int cycles;

    cycles = rising_time - falling_time;

    cycles = abs(cycles); // Rising - falling = echo pulse width in clock cycles

    if(falling_time > rising_time) {
        overflow++;
    }


    return cycles;
}

void send_pulse() {
    TIMER3_CTL_R &= 0xFEFF;

    GPIO_PORTB_AFSEL_R &= 0xF7;
    GPIO_PORTB_PCTL_R &= 0x0000;
    GPIO_PORTB_DIR_R |= 0x08;
    GPIO_PORTB_DATA_R |= 0x08;

    timer_waitMicros(5);
    TIMER3_ICR_R |= 0x400; // Clear Timer 3B Capture mode event Interrupt


    GPIO_PORTB_DATA_R &= 0xF7;
    GPIO_PORTB_DIR_R &= 0xF7;
    GPIO_PORTB_AFSEL_R |= 0x08;
    GPIO_PORTB_PCTL_R |= 0x00007000;

    TIMER3_CTL_R |= 0x0100;

    state = DONE;
}



