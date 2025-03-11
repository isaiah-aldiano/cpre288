/*
*
*   uart_extra_help.c
* Description: This is file is meant for those that would like a little
*              extra help with formatting their code, and following the Datasheet.
*/

#include "uart_extra_help.h"
#define REPLACE_ME 0x00

volatile char uart_data;
volatile char flag = 0;

void uart_init(int baud)
{
    SYSCTL_RCGCGPIO_R |= 0b00000010;      // enable clock GPIOB (page 340)
    SYSCTL_RCGCUART_R |= 0b00000010;      // enable clock UART1 (page 344)
    GPIO_PORTB_DEN_R   |= 0x03;        // enables pb0 and pb1
    GPIO_PORTB_AFSEL_R |= 0x03;  // sets PB0 and PB1 as peripherals (page 671)

    GPIO_PORTB_PCTL_R  &= 0xFFFFFF00;
    GPIO_PORTB_PCTL_R  |= 0x00000011;       // pmc0 and pmc1       (page 688)  also refer to page 650

    GPIO_PORTB_DIR_R   |= 0b01;        // sets pb0 as output, pb1 as input
    GPIO_PORTB_DIR_R   &= ~0b10;

    //compute baud values [UART clock= 16 MHz] 
    //BRD = 104.167
    //IBRD = 104
    //FBRD = 11
    int fbrd;
    int    ibrd;

    fbrd = 44; //16000000/(baud * 16);
    ibrd = 8; //(int) fbrd; // page 903
    //fbrd = (((fbrd-ibrd) * 64)- 0.5);

    UART1_CTL_R &= 0xFFFFFFFE;      // disable UART1 (page 918)
    UART1_IBRD_R = ibrd;        // write integer portion of BRD to IBRD
    UART1_FBRD_R = fbrd;   // write fractional portion of BRD to FBRD
    UART1_LCRH_R = 0b01100000;        // write serial communication parameters (page 916) * 8bit and no parity
    UART1_CC_R   = 0x0;          // use system clock as clock source (page 939)
    UART1_CTL_R = 0x301;        // enable UART1

    //uart_interrupt_init();
}

void uart_sendChar(char data)
{
   while((UART1_FR_R & 0x20)){};

   UART1_DR_R = data;

   
}

char uart_receive(void)
{
   char rdata;

   while((UART1_FR_R & 0x10)){};
   rdata = UART1_DR_R;

   return rdata;
 
}

void uart_sendStr(const char *data)
{
    //TODO
	
}

// _PART3


void uart_interrupt_init()
{
    // Enable interrupts for receiving bytes through UART1
    UART1_IM_R |= 0x10; //enable interrupt on receive - page 924

    // Find the NVIC enable register and bit responsible for UART1 in table 2-9
    // Note: NVIC register descriptions are found in chapter 3.4
    NVIC_EN0_R |= (1 << 6); //enable uart1 interrupts - page 104 1000000

    // Find the vector number of UART1 in table 2-9 ! UART1 is 22 from vector number page 104
    IntRegister(INT_UART1, uart_interrupt_handler); //give the microcontroller the address of our interrupt handler - page 104 22 is the vector number

    //IntMasterEnable();

}

void uart_interrupt_handler()
{
// STEP1: Check the Masked Interrup Status

//STEP2:  Copy the data 

//STEP3:  Clear the interrup   
    if(UART1_MIS_R & 0x10){
        uart_data = UART1_DR_R;
        flag = 1;
        UART1_ICR_R |= 0x10;
    }

}
