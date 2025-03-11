/*
 * adc.c
 *
 *  Created on: Oct 23, 2024
 *      Author: lewisc65
 */

#include "adc.h"
#include "timer.h"
#include "math.h"

void ADC_init(void)
{
    //Module initialization
    SYSCTL_RCGCADC_R |= 0x1;        //Set ADC Clock
    timer_waitMillis(20);

    SYSCTL_RCGCGPIO_R |= 0b011010;    //Set port clock
    timer_waitMillis(20);

    GPIO_PORTB_AFSEL_R |= 0x10;     //Set alternate function
    timer_waitMillis(20);

    GPIO_PORTB_DEN_R |= 0x10;        //Set pins as input
    timer_waitMillis(20);

    GPIO_PORTB_DIR_R |= 0x10;
    timer_waitMillis(20);

    GPIO_PORTB_AMSEL_R |= 0x10;    //Set analog
    timer_waitMillis(20);

    //Sample Sequencer
    ADC0_ACTSS_R &= 0;
    timer_waitMillis(20);

    ADC0_EMUX_R |= 0xF;
    timer_waitMillis(20);

    ADC0_SSMUX0_R = 0xA;
    timer_waitMillis(20);

    ADC0_SSCTL0_R = 0b0110;
    timer_waitMillis(20);

    ADC0_ACTSS_R |= 0x1;
    timer_waitMillis(20);

    ADC0_PSSI_R |= 0x1;
    timer_waitMillis(20);

    ADC0_SAC_R = 0x4;


}


int ADC_read(void){
    float av;
    float IR = 0;

    while(((ADC0_RIS_R) & 0x1) == 0){};
    IR = ADC0_SSFIFO0_R;


    av = 45493* (pow(IR, -1.069));
    return av;
}
