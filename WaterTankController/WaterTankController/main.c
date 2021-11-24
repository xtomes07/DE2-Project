/***********************************************************************
 * 
 * Distance measurement using HC-SR04 Ultrasonic Distance Sensor
 * ATmega328P (Arduino Uno), 16 MHz, AVR 8-bit Toolchain 3.6.2
 *
 * Copyright (c) 2021 Czmelová Zuzana, Shelemba Pavlo, Točený Ivo, 
 *                    Tomešek Jiří
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
uint16_t distance_cm = 0;

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
    EICRA |= (1 << ISC00); 
    // Not strictly necessary, as register default values are already 0
    EICRA &= ~((1 << ISC11) | (1 << ISC10) | (1 << ISC01)); 
    // External Interrupt Request Enable
    EIMSK |= (1 << INT0); EIMSK &= ~(1 << INT1);
    
    // Overflow timer for trigger signal
    TIM0_overflow_4ms();
    TIM0_overflow_interrupt_enable();

    // 340 m/s sound wave propagates by 1 cm in ~58,8235 us
    // Empirical measurement suggests that 905 clocks of TIM1
    // with prescaler N=1 takes almost the same amount of time
    // This value will probably change with growing complexity
    // of the program
    OCR1A = 930;
    // Enable Timer/Counter1 Output Compare A Match interrupt    
    TIMSK1 |= (1 << OCIE1A);
    
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
    static char lcd_str[16];
    // Change of state counter
    static uint8_t i = 0;
    
    if(i)
    {
        // Disable counter
        TCCR1B |= 0;
        
        // Calculate distance in centimeters and put in on LCD
        itoa(distance_cm, lcd_str, 10);
        lcd_gotoxy(8, 0);
        lcd_puts("    ");
        lcd_gotoxy(8, 0);
        lcd_puts(lcd_str);
        
        i = 0;
    }
    else
    {
        // Clear previous calculated distance before next measurement
        distance_cm = 0;
        
        // Start counting echo using 16-bit counter with prescaler N=1
        TIM1_overflow_4ms();  
        // Enable TIM1 CTC mode   
        TCCR1B |= (1 << WGM12);
        
        i = 1;
    }
}

ISR(TIMER0_OVF_vect)
{
    static uint8_t i = 0;
    
    ++i;
    
    // Trigger the ultrasonic sensor every ~40 ms
    if (i == 9)
    {  
        GPIO_write_high(&PORTD, trig);
        _delay_us(10);
        GPIO_write_low(&PORTD, trig);
        
        i = 0;
    }
}

ISR(TIMER1_COMPA_vect)
{
    ++distance_cm;
}