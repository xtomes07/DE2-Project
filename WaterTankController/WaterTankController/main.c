/***********************************************************************
 * 
 * Water tank controller.
 * ATmega328P (Arduino Uno), 16 MHz, AVR 8-bit Toolchain 3.6.2
 *
 * Copyright (c) 2021 Czmelová Zuzana, Shelemba Pavlo, Točený Ivo, 
 *                    Tomešek Jiří
 * This work is licensed under the terms of the MIT license
 * 
 **********************************************************************/

/* Defines -----------------------------------------------------------*/
#define trig         PD0    // Trigger Pin
#define echo         PD2    // Echo Pin
#define servo        PB2    // Servo velve pin
#define led_green    PB6    // Servo velve pin
#define led_red      PB7    // Servo velve pin
#define relay        PC0    // Pin for pump relay control 
#define pump         PC1    // Pin for pump switch 
#define btn_servo    PC2    // Pin for servo valve switch 
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
#include <stdlib.h>         // C library for conversion function
#include <string.h>         // C library for string manipulations

/* Variables ---------------------------------------------------------*/
uint16_t distance_cm = 0;
uint8_t valveIsOpen = 0;

/* Function definitions ----------------------------------------------*/
void send_trigger()
{
    GPIO_write_high(&PORTD, trig);
    _delay_us(10);
    GPIO_write_low(&PORTD, trig);
}

void open_valve()
{
    GPIO_write_high(&PORTB, servo);
    _delay_ms(2);
    GPIO_write_low(&PORTB, servo);
    _delay_ms(18);
    
    valveIsOpen = 1;
}

void close_valve()
{
    GPIO_write_high(&PORTB, servo);
    _delay_ms(1.5);
    GPIO_write_low(&PORTB, servo);
    _delay_ms(18.5);
    
    valveIsOpen = 0;
}

void pump_on()
{
    // Turn relay for Pump on
    GPIO_write_high(&PORTC, relay);
}

void pump_off()
{
    // Turn relay for Pump off
    GPIO_write_low(&PORTC, relay);
}

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
    
    // Configure Control Pump PIN
    GPIO_config_input_nopull(&DDRC, pump);
    
    // Configure Relay Pump PIN
    GPIO_config_output(&DDRC, relay);
    GPIO_write_low(&PORTC, relay);
    
    // Configure Servo PIN
    GPIO_config_output(&DDRB, servo);
    GPIO_write_low(&PORTB, servo);
    
    // Configure Control Servo PIN
    GPIO_config_input_nopull(&DDRC, btn_servo);
    
    // Configure LED PINs
    GPIO_config_output(&DDRB, led_green);
    GPIO_write_low(&PORTB, led_green);
    GPIO_config_output(&DDRB, led_red);
    GPIO_write_low(&PORTB, led_red);
    
    // Initialize LCD display
    lcd_init(LCD_DISP_ON);

    // Put strings on LCD display
    lcd_gotoxy(0, 0);
    lcd_puts("LVL:");
    lcd_gotoxy(6, 0);
    lcd_puts("%");
    lcd_gotoxy(0, 1);
    lcd_puts("PMP:");
    lcd_gotoxy(9, 1);
    lcd_puts("VLV:CLS");
    
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
    // Empirical measurement suggests that 930 clocks of TIM1
    // with prescaler N=1 takes almost the same amount of time
    // Set MAX TIM1 value to this value
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
    // Water tank fill level 
    static uint8_t volume = 0;
    
    if (i)
    {
        // Disable counter
        TCCR1B |= 0;
         
        // Check tank fill level
        if (distance_cm > 399)
        {
            strcpy(lcd_str, "EMPTY");
            
            GPIO_write_low(&PORTB, led_green);
            GPIO_write_high(&PORTB, led_red);
        }      
        else if (distance_cm < 40)
        {
            lcd_gotoxy(8, 0);
            lcd_puts(" ");
            strcpy(lcd_str, "FULL");
            
            GPIO_write_high(&PORTB, led_green);
            GPIO_write_low(&PORTB, led_red);
        }
        else
        {
            volume = 100 - distance_cm / 4;
            
            itoa(volume, lcd_str, 10);
            
            if (volume > 9)
            {
                lcd_gotoxy(6, 0);
                lcd_puts("%  ");
            }
            else
            {
                lcd_gotoxy(5, 0);
                lcd_puts("%   ");
            }
            
            GPIO_write_low(&PORTB, led_green);
            GPIO_write_low(&PORTB, led_red);
        }            
        
        // Put tank fill level on LCD  
        lcd_gotoxy(4, 0);
        lcd_puts(lcd_str);
        
        // Check for water excess
        if (distance_cm < 20 || GPIO_read(&PINC, btn_servo))
        {
            open_valve();
            
            lcd_gotoxy(13, 1);
            lcd_puts("OPN");
        }
        else if (valveIsOpen)
        {
            close_valve();
            
            lcd_gotoxy(13, 1);
            lcd_puts("CLS");
        }
            
        // Check if pump is on
        if (GPIO_read(&PINC, pump) && distance_cm > 40)
        {
            pump_on();
            
            lcd_gotoxy(4, 1);
            lcd_puts("ON ");
        }            
        else
        {
            pump_off();
            
            lcd_gotoxy(4, 1);
            lcd_puts("OFF");
        }            
        
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
    
    // Trigger ultrasonic sensor every ~40 ms
    if (i == 9)
    {  
        send_trigger();
        i = 0;
    }
}

ISR(TIMER1_COMPA_vect)
{
    ++distance_cm;
    
    if (distance_cm > 400)
    {
        distance_cm = 400;
    }
}