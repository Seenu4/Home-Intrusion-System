#include <lpc214x.h>
#include <stdio.h>
#include <stdint.h>
#include <stdlib.h>
#include "LCD.H"

// Define peripherals and pins
#define LCD (0xFFFF00FF)
#define RS (1 << 4)
#define RW (1 << 5)
#define EN (1 << 6)

// Function prototypes
void delay_ms(int count);
void initGPIO(void);
void initRTC(void);
void getTime(int* hour, int* minute);

int main() {
    int count_entry = 0;
    int count_exit = 0;
    int is_daytime = 0;

    int entry_sensor, exit_sensor;
    int hour, minute;

    char buffer[10];

    // Function calls to initialize peripherals
    initGPIO();
    initRTC();
    LCD_Init(); // Initialize LCD

    while (1) {
        getTime(&hour, &minute);
        if (hour >= 5 && hour < 17) {
            is_daytime = 0; // Daytime
        } else {
            is_daytime = 1; // Nighttime
        }

        entry_sensor = (IO0PIN & (1 << 10)) >> 10;
        exit_sensor = (IO0PIN & (1 << 11)) >> 11;

        if (entry_sensor == 1) {
            count_entry++;
        }

        if (exit_sensor == 1) {
            count_exit++;
        }
        if (count_entry > count_exit && !is_daytime) {
	    IO1SET |= (1 << 20); // Activate LED
	    delay_ms(2000);
        } else {
            IO1CLR |= (1 << 20); // Deactivate LED
        }
	LCD_Command(0x01);      // Display clear
        sprintf(buffer, "Count: %d", count_entry - count_exit);
        LCD_Data(count_entry - count_exit);
    }
    return 0;
}

// Function implementations

void delay_ms(int count) {
    int j = 0, i = 0;
    for (j = 0; j < count; j++) {
        // At 60Mhz, the below loop introduces delay of 1 milli sec
        for (i = 0; i < 1250; i++);
    }
}

void initGPIO(void) {
    PINSEL0 = 0x00000000;
    IO0DIR &= ~((1 << 10) | (1 << 11));
    IO1DIR |= (1 << 20);
    // Implement GPIO initialization here
}

void initRTC(void) {
    CCR = 0x11;
    CIIR = 0x01;
    // Implement RTC initialization here
}

void getTime(int* hour, int* minute) {
    *hour = (HOUR & 0x1F);
    *minute = (MIN & 0x3F);
}