/**
 * lab5_template.c
 * 
 * Template file for CprE 288 lab 5
 *
 * @author Zhao Zhang, Chad Nelson, Zachary Glanz
 * @date 08/14/2016
 *
 * @author Phillip Jones, updated 6/4/2019
 */

#include "button.h"
#include "timer.h"
#include "lcd.h"
#include "math.h"
#include "open_interface.h"
#include "movement.h"


#include "cyBot_uart.h"  // Functions for communiticate between CyBot and Putty (via UART)
                         // PuTTy: Buad=115200, 8 data bits, No Flow Control, No Party,  COM1

#include "cyBot_Scan.h"  // For scan sensors 


//#warning "Possible unimplemented functions"
#define REPLACEME 0


// Defined in button.c : Used to communicate information between the
// the interupt handeler and main.
extern volatile int button_event;
extern volatile int button_num;


int main(void) {
	button_init();
	lcd_init();
	oi_t *sensor_data = oi_alloc();
	oi_init(sensor_data);

	cyBot_uart_init();   // Don't forget to initialze the cyBot UART before trying to use it
	

//     (Uncomment ME for PuTTy to CyBot UART init part of lab)
//	cyBot_uart_init_clean();  // Clean UART initialization, before running your UART GPIO init code

	// Complete this code for configuring the  (GPIO) part of UART initialization
//      SYSCTL_RCGCGPIO_R |= 0x02;
//      timer_waitMillis(1);            // Small delay before accessing device after turning on clock
//      GPIO_PORTB_AFSEL_R |= 0b00000011;
     // GPIO_PORTB_PCTL_R &= FIXME;     // Force 0's in the disired locations
//      GPIO_PORTB_PCTL_R |= 0x00000011;     // Force 1's in the disired locations
//      GPIO_PORTB_DEN_R |= 0xFF;
     // GPIO_PORTB_DIR_R &= FIXME;      // Force 0's in the disired locations
//      GPIO_PORTB_DIR_R |= 0x3F;      // Force 1's in the disired locataions
    
    // (Uncomment ME for UART init part of lab) cyBot_uart_init_last_half();  // Complete the UART device initialization part of configuration
//	cyBot_uart_init_last_half();
	// Initialze scan sensors
//     cyBOT_init_Scan(0b0111);



	// YOUR CODE HERE
//    cyBOT_SERVO_cal();
    right_calibration_value = 337750;
    left_calibration_value = 1345750;

//	char buffer[10];
//	init_button_interrupts();
//	short k;
//
//    float distances_ir[90];
//    float distances_ping[90];

    char input;
    char command[100];
    short index = 0;


//    cyBOT_Scan_t scan;
//    char input ='0';
//    float a;
//    float b;
//    short i;
//    short k;
//    float ir_raw;
//    char buffer[50];
//    char scans = 3;

    lcd_printf("Running");

    while(1) {
        index = 0;
        input = cyBot_getByte_blocking();
//        cyBot_sendByte(input);

//        if (input == 'm')
//        {
//            short j;
//            float b;
//            char str[] = "\r\nDegrees   Distance(m)\r\n";
//            for(j = 0; j < strlen(str); j++) {
//                cyBot_sendByte(str[j]);
//            }
//
//            a = 0;
//            b = 0;
//                for (j = 0; j < scans; j++)
//
//                    {
//                        cyBOT_Scan(90, &scan);
//                        b += scan.IR_raw_val;
//                    }
//                    ir_raw = b / 3;
//                    b = 8830.2 * pow(ir_raw, -1.421);
//
//                    for (k = 0; k < strlen(buffer); k++)
//                    {
//                        cyBot_sendByte(buffer[k]);
//                    }
//
//
//                     lcd_printf("%.2f %.2f ", b, ir_raw);
//        }

        while(input != '\n') {
            command[index] = input;
            index++;
            input = cyBot_getByte_blocking();
        }

        command[index] = '\n';
        command[index + 1] = 0;
        lcd_printf("Got: %s", command);  // Print received command to the LCD screen

        switch(command[0]) {

             case 'w':
                 move_forward(sensor_data, 50);
                 break;
             case 's':
                 move_backward(sensor_data, 50);
                 break;
             case 'a':
                 turn_counter_clockwise(sensor_data, 50);
                 move_forward(sensor_data, 50);
                 break;
             case 'd':
                 turn_clockwise(sensor_data, 50);
                 move_forward(sensor_data, 50);
                 break;
             case 'q':
                 oi_free(sensor_data);
                 return 0;


        }
        oi_setWheels(0,0); // stop

        if(command[0] != '\n')
        {
          cyBot_sendByte('\n');
        }
    }


}
