/*
*
*   uart.c
*
*
*
*
*
*   @author
*   @date
*/

#include "uart.h"
#include "math.h"



void uart_init(void){
    SYSCTL_RCGCGPIO_R |= 0x02;
    SYSCTL_RCGCUART_R |= 0x02;
    GPIO_PORTB_AFSEL_R |= 0b00000011;
    GPIO_PORTB_PCTL_R |= 0x00000011;
    GPIO_PORTB_DEN_R |= 0x03;
    GPIO_PORTB_DIR_R |= 0b00000010;

//    baud rate = 115200, #Date bits = 8, #Stop bits =1, Parity = None

    double fbrd;
    int ibrd;

    fbrd = 16000000.0 / (16.0 * 115200.0);
    ibrd = floor(fbrd); // 8
    fbrd = (fbrd - ibrd) * 64 + .5;

    UART1_CTL_R &= 0xFFFFFFFE;
    UART1_IBRD_R = ibrd;
    UART1_FBRD_R = fbrd;
    UART1_LCRH_R = 0b01100000;
    UART1_CC_R = 0x0;
    UART1_CTL_R |= 0x1;
}

void uart_sendChar(char data){

    while((UART1_FR_R & 0x20) != 0) {

    }

    UART1_DR_R = data;
}

char uart_receive(void){
	char data = 0;
//	short read = 0;

	while((UART1_FR_R & UART_FR_RXFE)) {

	}

	data = (char) (UART1_DR_R & 0xFF);

	return data;
}

void uart_sendStr(const char *data){
    while(*data != '\0'){

            uart_sendChar(*data);
            data++;
        }
    timer_waitMicros(10000);

}

void uart_interrupt_init(void) {
    UART1_CTL_R &= 0xFFFFFFFE;
    UART1_ICR_R = 0xFFFF;
    UART1_ICR_R = 0x0000;
    UART1_IM_R |= 0x00000010;
    NVIC_PRI1_R |= 0x00200000;
    NVIC_EN0_R |= 0b01000000;
    IntRegister(INT_UART1, UART1_Handler);
    IntMasterEnable();
    UART1_CTL_R |= 0x1;
}

void UART1_Handler(void) {

    if(UART1_MIS_R & 0x10) {
        char input;
        input = UART1_DR_R & 0xFF;
        uart_sendChar(input);

        if(input == 't') {
            mode = !mode;
        }
        else if(input == 'h') {
            doSomething = 1;
            hCount++;
        }
        else if(input == 'm') {
            doSomething = 2;
        }
        else if(input == 'w') {
            doSomething = 3;
        }
        else if(input == 's') {
            doSomething = 4;
        }
        else if(input == 'a') {
            doSomething = 5;
        }
        else if(input == 'd') {
            doSomething = 6;
        }
        else if(input == 'v') {
            doSomething = 7;
        }
        else if(input == 'b') {
            doSomething = 8;
        }



        UART1_ICR_R |= 0x10;
    }

//    doSomething = input;
}
