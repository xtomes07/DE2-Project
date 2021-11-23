/***********************************************************************
 * 
 * Distance measurement using HC-SR04 Ultrasonic Distance Sensor
 * ATmega328P (Arduino Uno), 16 MHz, AVR 8-bit Toolchain 3.6.2
 *
 * Copyright (c) 2021 Czmelová Zuzana, Czmelová Zuzana, 
 *                    To?ený Ivo, Tomešek Ji?í
 * This work is licensed under the terms of the MIT license
 * 
 **********************************************************************/

/* Defines -----------------------------------------------------------*/
#define trig PD0		    // Trigger Pin
#define echo PD2			// Echo Pin
#ifndef F_CPU
#define F_CPU 16000000UL    // CPU frequency in Hz for delay.h
#endif

/* Includes ----------------------------------------------------------*/
#include <avr/io.h>         // AVR device-specific IO definitions
#include <avr/interrupt.h>  // Interrupts standard C library for AVR-GCC
#include <util/delay.h>     // Busy-wait delay loops
#include "timer.h"          // Timer library for AVR-GCC
#include "lcd.h"            // Peter Fleury's LCD library
#include "gpio.h"           // GPIO library for AVR-GCC
#include <stdlib.h>         // C library. Needed for conversion function

/* Variables ---------------------------------------------------------*/
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
	// Configure Trig PIN
	GPIO_config_output(&DDRD, trig);
	GPIO_write_low(&PORTD, trig);
	
	// Configure Echo PIN
	GPIO_config_input_pullup(&DDRD, echo);
	
    // Initialize LCD display
    lcd_init(LCD_DISP_ON);

    // Put strings on LCD display
    lcd_gotoxy(1, 0);
    lcd_puts("LEVEL:");
    
    lcd_gotoxy(12, 0);
    lcd_puts("cm");
    
	lcd_gotoxy(1, 1);
	lcd_puts("PUMP:");
    
    
    // Any logical change on INT0 generates an interrupt request
    EICRA |= (1 << ISC00); EICRA &= ~((1 << ISC11) | (1 << ISC10) | (1 << ISC01)); 

    // External Interrupt Request Enable
    EIMSK |= (1 << INT0);
    
    // Overflow timer for trigger signal
    TIM1_overflow_33ms();
    TIM1_overflow_interrupt_enable();

    // Enables interrupts by setting the global interrupt mask
    sei();

    // Infinite loop
    while (1)
    {}

    return 0;
}

/* Interrupt service routines ----------------------------------------*/
ISR(INT0_vect)
{
    // Strings for converting numbers
    static char lcd_str[3];
    // Change of state counter
    static uint8_t i = 0;
    
    if(i)
    {
        // Stop counting echo
        TIM0_stop();
        // Reset timer
        TCNT0 = 0;
        
        // Calculate distance in centimeters and put in on LCD
        itoa(distance/4, lcd_str, 10);
        lcd_gotoxy(8, 0);
        lcd_puts("   ");
        lcd_gotoxy(8, 0);
        lcd_puts(lcd_str);
        
        i = 0;
    }
    else
    {
        // Clear previous calculated distance before next measurement
        distance = 0;
        
        // Start counting echo
        TIM0_overflow_16us();     
        TIM0_overflow_interrupt_enable();
        
        i = 1;
    }
}

ISR(TIMER1_OVF_vect)
{
    // Trigger the ultrasonic sensor
    GPIO_write_high(&PORTD, trig);
    _delay_us(10);
    GPIO_write_low(&PORTD, trig);
}

ISR(TIMER0_OVF_vect)
{
    ++distance;
}
