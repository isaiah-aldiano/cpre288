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
	//TODO
    SYSCTL_RCGCGPIO_R |= 0x02;
    SYSCTL_RCGCUART_R |= 0x02;
    GPIO_PORTB_AFSEL_R = 0b00000011;
    GPIO_PORTB_PCTL_R = 0x00000011;
    GPIO_PORTB_DEN_R = 0x03;
    GPIO_PORTB_DIR_R = 0b00000010;

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
	//TODO
    while((UART1_FR_R & 0x20) != 0);

    UART1_DR_R = data;
}

char uart_receive(void){
	char data = 0;

	while((UART1_FR_R & UART_FR_RXFE)) {

	}

	data = (char) (UART1_DR_R & 0xFF);

	return data;
}

void uart_sendStr(const char *data){
	//TODO for reference see lcd_puts from lcd.c file

}

void uart_interrupt_init(void) {
    UART1_CTL_R &= 0xFFFFFFFE;
    UART1_ICR_R = 0xFFFF;
    UART1_ICR_R = 0x0000;
    UART1_IM_R |= 0x00000030;
    NVIC_PRI1_R |= 0x00200000;
    NVIC_EN0_R |= 0b01000000;
    IntRegister(INT_UART1, UART1_Handler);
    IntMasterEnable();
    UART1_CTL_R |= 0x1;
}

void UART1_Handler(void) {
    if(UART1_MIS_R & 0x10) {
        char input;
        input = uart_receive();
//        uart_sendChar(input);
        UART1_ICR_R |= 0x10;
    }
}
