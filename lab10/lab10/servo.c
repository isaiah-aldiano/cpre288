#include "servo.h"
int match_subtract;
int degrees = 90;
int position = 4000;
short switch_direction = 1;
int base_1_millisecond = 16000;
int right_calibration_value = -10000;
int left_calibration_value = 18700;
/*
 * servo.c
 *
 *  Created on: Nov 12, 2024
 *      Author: ialdiano
 */

void timer1b_handler() {
    TIMER1_ICR_R |= 0x800;
    printf("%-10d\n", TIMER1_TBR_R);
}

int servo_move_clock_cycles(int clock_cycles) {

    match_subtract = clock_cycles + base_1_millisecond; //Clock cycles + base 1 millisecond to register movement

    if(match_subtract < right_calibration_value + base_1_millisecond) {
        match_subtract = right_calibration_value + base_1_millisecond;
        degrees = 0;
        position = right_calibration_value;
    } else if (match_subtract > left_calibration_value + base_1_millisecond) {
        match_subtract = left_calibration_value + base_1_millisecond;
        degrees = 180;
        position = left_calibration_value;
    }

    TIMER1_TBMATCHR_R = TIMER1_TBILR_R - match_subtract;

    return match_subtract;
}

/*
 *  1 millisecond = 16_000 clock cycles = 0 degrees
 *  1.5 milliseconds = 24_000 clock cycles = 90 degrees
 *  2 milliseconds = 32_000 clock cycles = 180 degrees
 */
//int servo_move_degrees(float degrees) {
////    int match_subtract; // in clock cycles
//    float denom = .01125; // relationship between degrees and clock cycles
//
//    match_subtract = (degrees / denom);
//    if(match_subtract < right_calibration_value) {
//        servo_move_clock_cycles(right_calibration_value);
//        return 1;
//    } else if (match_subtract > left_calibration_value) {
//        servo_move_clock_cycles(left_calibration_value);
//        return 0;
//    }
//
//    match_subtract += base_1_millisecond;
//
//    TIMER1_TBMATCHR_R = TIMER1_TBILR_R - match_subtract;
//
//    return match_subtract;
//}

void servo_init() {
    SYSCTL_RCGCGPIO_R |= 0x2;
    GPIO_PORTB_AFSEL_R |= 0x20;
    GPIO_PORTB_PCTL_R |= 0x00700000;
    GPIO_PORTB_DEN_R |= 0x20;
    GPIO_PORTB_DIR_R |= 0x20;


    SYSCTL_RCGCTIMER_R |= 0x2;
    TIMER1_CTL_R &= 0xEFF; // Disable that bih
    TIMER1_CFG_R |= 0x4;
    TIMER1_TBMR_R |= 0x2; // Match interrupts needed? Why not
    TIMER1_TBMR_R |= 0xE; // Periodic, Edge-Time, PWM
    TIMER1_TBMR_R &= 0xEF; // Count Down
    TIMER1_TBILR_R |= 0xFFFF;

    TIMER1_ICR_R |= 0x800;
    TIMER1_IMR_R |= 0x800;

    NVIC_EN0_R |= (1 << 22); //Nvic for 16/32-Bit Timer 1B (page 104)
    IntRegister(INT_TIMER1B, timer1b_handler);
    IntMasterEnable();

    TIMER1_CTL_R |= 0x100;

    servo_move_clock_cycles(position);
}



//void button_init() {
//    static uint8_t initialized = 0;
//
//    if(initialized){
//        return;
//    }
//
//    SYSCTL_RCGCGPIO_R |= 0x10;
//    GPIO_PORTE_DIR_R &= 0xC0;
//    GPIO_PORTE_DEN_R |= 0x3F;
//
//    initialized = 1;
//}
//
//uint8_t button_getButton() {
//
//
//    if (!((GPIO_PORTE_DATA_R >> 2)&1)) {
//        switch_direction *= -1;
//    }
//
//    if (!((GPIO_PORTE_DATA_R >> 3)&1) ) { // button 4
//        if(switch_direction == 1) {
//            servo_move_clock_cycles(right_calibration_value);
//            degrees = 0;
//            position = right_calibration_value;
//
//        } else if (switch_direction == -1){
//            servo_move_clock_cycles(left_calibration_value);
//            degrees = 180;
//            position = left_calibration_value;
//
//        }
//    } else if (!((GPIO_PORTE_DATA_R >> 1)&1)) {
//        position += (775 * switch_direction);
//        degrees += 5 * switch_direction;
//        servo_move_clock_cycles(position);
//    }else if (!((GPIO_PORTE_DATA_R)&1)) {
//        position += (155 * switch_direction);
//        degrees += 1 * switch_direction;
//        servo_move_clock_cycles(position);
//    }
//
//    while(!((GPIO_PORTE_DATA_R)&1) || !((GPIO_PORTE_DATA_R >> 1)&1) || !((GPIO_PORTE_DATA_R >> 3)&1) || !((GPIO_PORTE_DATA_R >> 2)&1)) {
//
//    }
//
//}

void scan(int pointTo) {
    servo_move_clock_cycles((pointTo * 155) + right_calibration_value);
}

int get_match() {
    return TIMER1_TBMATCHR_R;
}

int get_match_subtract() {
    return match_subtract;
}

int get_direction() {
    return switch_direction;
}

int get_angle() {
    return degrees;
}



