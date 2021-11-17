/* Defines -----------------------------------------------------------*/
#define trig PD3		    // Trigger Pin
#define echo PD2			// Echo Pin


/* Includes ----------------------------------------------------------*/
#include <avr/io.h>         // AVR device-specific IO definitions
#include <avr/interrupt.h>  // Interrupts standard C library for AVR-GCC
#include "timer.h"          // Timer library for AVR-GCC
#include "lcd.h"            // Peter Fleury's LCD library
#include "gpio.h"           // GPIO library for AVR-GCC
#include <stdlib.h>         // C library. Needed for conversion function
#include "Ultrasonic.h"

/* Variables ---------------------------------------------------------*/
uint16_t duration = 0;
uint16_t distance = 0;

/* Function definitions ----------------------------------------------*/
/**********************************************************************
 * Function: Main function where the program execution begins
 * Purpose:  Update stopwatch value on LCD display when 8-bit 
 *           Timer/Counter2 overflows.
 * Returns:  none
 **********************************************************************/

int main(void)
{
	//Configure Trig PIN
	GPIO_config_output(&DDRD, trig);
	GPIO_write_low(&PORTD, trig);
	
	//Configure Echo PIN
	GPIO_config_input_pullup(&DDRD, echo);
	
    // Initialize LCD display
    lcd_init(LCD_DISP_ON);
	
	// Set pointer to beginning of CGRAM memory
	lcd_command(1 << LCD_CGRAM);

	// Set DDRAM address
	lcd_command(1 << LCD_DDRAM);

    // Put string(s) at LCD display
    lcd_gotoxy(0, 0);
    lcd_puts("LEVEL:");
	
	lcd_gotoxy(0, 1);
	lcd_puts("PUMP:");
	

    // Configure 8-bit Timer/Counter2 for Stopwatch
	TIM2_overflow_16384us();
    // Set the overflow prescaler to 16 ms and enable interrupt
	TIM2_overflow_interrupt_enable();

    // Enables interrupts by setting the global interrupt mask
    sei();

    // Infinite loop
    while (1)
    {
        /* Empty loop. All subsequent operations are performed exclusively 
         * inside interrupt service routines ISRs */
    }

    // Will never reach this
    return 0;
}

/* Interrupt service routines ----------------------------------------*/
ISR(TIMER2_OVF_vect)
{
   /* GPIO_write_low(&PORTD, trig);
	_delay_us(2);
	GPIO_write_high(&PORTD, trig);
	_delay_us(10);
	GPIO_write_low(&PORTD, trig);
	
	int neco = GPIO_read(&PORTD, echo);	//musime zacit citat kdyz echo bude na high
	
	duration = pulseIn(echoPin, HIGH);
	distanceCm= duration*0.034/2;
	distanceInch = duration*0.0133/2;
	lcd.setCursor(0,0); // Sets the location at which subsequent text written to the LCD will be displayed
	lcd.print("Distance: "); // Prints string "Distance" on the LCD
	lcd.print(distanceCm); // Prints the distance value from the sensor
	lcd.print(" cm");
	delay(10);
	lcd.setCursor(0,1);
	lcd.print("Distance: ");
	lcd.print(distanceInch);
	lcd.print(" inch");
	delay(10);*/
	distance = Ultrasonic.read();
}

