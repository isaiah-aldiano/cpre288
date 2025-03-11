#include <stdint.h>
#include "adc.h"
#include "Timer.h"
#include "lcd.h"
/*
 * adc.c
 *
 *  Created on: Oct 28, 2024
 *      Author: ialdiano
 */


void adc_init(void) {
    SYSCTL_RCGCGPIO_R |= 0x02;
    SYSCTL_RCGCADC_R |= 0x01;
    GPIO_PORTB_AFSEL_R |= 0x10;
    GPIO_PORTB_DEN_R &= 0xEF;
    GPIO_PORTB_DIR_R &= 0xEF;
    GPIO_PORTB_AMSEL_R |= 0x10;
    GPIO_PORTB_ADCCTL_R = 0x00;


    ADC0_ACTSS_R &= 0xE;
    ADC0_EMUX_R &= 0xFFF0;
    ADC0_SSMUX0_R |= 0xA;
    ADC0_SAC_R |= 0x3;
    ADC0_SSCTL0_R |= 0x6;
    ADC0_ACTSS_R |= 0b00000001;

}
int adc_read(void) {
    ADC0_PSSI_R |= 0x01;
    ADC0_IM_R &= 0x1;


    while((ADC0_RIS_R & ADC0_IM_R) == 1) {

    }

    ADC0_ISC_R = 0x1;
    return ADC0_SSFIFO0_R;
}

