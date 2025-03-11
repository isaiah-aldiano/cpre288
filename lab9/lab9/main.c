#include <stdint.h>
#include <stdbool.h>
#include <inc/tm4c123gh6pm.h>
#include "driverlib/interrupt.h"
#include "lcd.h"
#include "Timer.h"
#include "ping.h"




/**
 * main.c
 */
void main()
{
    lcd_init();
    ping_init();

    int cycles; // Rising - Falling time of ping
    float cycles_in_milliseconds;
    float distance_cm; // distance to object in cm
    char data[50];


    while(1) {
//        send_pulse();
        cycles = ping_read();

        cycles_in_milliseconds = (float) cycles / 16000;

        distance_cm = (float) cycles / 16000000.0; // cycles / clockMHz = seconds between pulse send and receive
        distance_cm /= 2.0; // Only account for seconds to object
        distance_cm *= 34000.0; // Seconds to object * Speed of sound (~340m/s) * 100(cm in m) = cm to object

        sprintf(data, "CYC:%d\nMILLI:%.7f\nCM:%.2f\nOverflow:%d", cycles, cycles_in_milliseconds, distance_cm, overflow);



//        sprintf(data, "%d %d\n%d\n%d", rising_time, falling_time, rising_time - falling_time, falling_time - rising_time);
        lcd_printf(data);
        timer_waitMillis(500);
    }

}
