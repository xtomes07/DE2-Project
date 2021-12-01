/***********************************************************************
 * 
 * Water tank controller.
 * ATmega328P (Arduino Uno), 16 MHz, AVR 8-bit Toolchain 3.6.2
 *
 * Copyright (c) 2021 Czmelová Zuzana       
 * Copyright (c) 2021 Shelemba Pavlo
 * Copyright (c) 2021 Točený Ivo
 * Copyright (c) 2021 Tomešek Jiří
 * This work is licensed under the terms of the MIT license
 * 
 **********************************************************************/

/* Defines -----------------------------------------------------------*/
#define TRIG       PD0    // Trigger Pin
#define ECHO       PD2    // Echo Pin
#define SERVO      PB2    // Servo valve pin
#define LED_G      PB6    // Servo valve pin
#define LED_R      PB7    // Servo valve pin
#define RELAY      PC0    // Pin for pump relay control 
#define SW_PUMP    PC1    // Pin for pump switch 
#define SW_SERVO   PC2    // Pin for servo valve switch 
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

// Max water height in cm 
uint16_t water_height = 400;
// Gap between sensor and max water height in cm
uint16_t air_gap      = 20;

// Max water level before valve opens
uint16_t max_level;

// Measured distance in cm
uint16_t distance    = 0;
// Water tank fill level
uint16_t volume      = 0;
// Booleans for electromechanics
uint8_t  valveIsOpen = 0;
uint8_t  pumpIsOn    = 0;
// Custom character number
uint8_t char_num     = 0;

/* Symbols	----------------------------------------------------------*/
uint16_t customChar[] = {
	0B10001,	// Tank is empty
	0B10001,
	0B10001,
	0B10001,
	0B10001,
	0B10001,
	0B11111,
	0B11111,
	
	0B10001,	// 1/5 tank
	0B10001,
	0B10001,
	0B10001,
	0B10001,
	0B11111,
	0B11111,
	0B11111,
	
	0B10001,	// 2/5 tank
	0B10001,
	0B10001,
	0B10001,
	0B11111,
	0B11111,
	0B11111,
	0B11111,
	
	0B10001,	// 3/5 tank
	0B10001,
	0B10001,
	0B11111,
	0B11111,
	0B11111,
	0B11111,
	0B11111,
	
	0B10001,	// 4/5 tank
	0B10001,
	0B11111,
	0B11111,
	0B11111,
	0B11111,
	0B11111,
	0B11111,
	
	0B10001,	// Tank is full
	0B11111,
	0B11111,
	0B11111,
	0B11111,
	0B11111,
	0B11111,
	0B11111
};
/* Function definitions ----------------------------------------------*/
void send_trigger()
{
    GPIO_write_high(&PORTD, TRIG);
    _delay_us(10);
    GPIO_write_low(&PORTD, TRIG);
}

void open_valve()
{
    GPIO_write_high(&PORTB, SERVO);
    _delay_ms(2);
    GPIO_write_low(&PORTB, SERVO);
    _delay_ms(18);
    
    valveIsOpen = 1;
    
    TIM2_overflow_16ms();
}

void close_valve()
{
    GPIO_write_high(&PORTB, SERVO);
    _delay_ms(1.5);
    GPIO_write_low(&PORTB, SERVO);
    _delay_ms(18.5);
    
    valveIsOpen = 0;
    
    if (!pumpIsOn)
        TIM2_stop();
}

void pump_on()
{
    // Turn relay for Pump on
    GPIO_write_high(&PORTC, RELAY);
    
    pumpIsOn = 1;

    TIM2_overflow_16ms();
}

void pump_off()
{
    // Turn relay for Pump off
    GPIO_write_low(&PORTC, RELAY);
    
    pumpIsOn = 0;

    if (!valveIsOpen)
        TIM2_stop();
}

/**********************************************************************
 * Function: Main function where the program execution begins
 * Purpose:  Update stopwatch value on LCD display when 8-bit 
 *           Timer/Counter2 overflows.
 * Returns:  none
 **********************************************************************/
int main(void)
{
    // Set max water level halfway between sensor and water
    max_level = air_gap / 2;
    
    // Configure Trig PIN
    GPIO_config_output(&DDRD, TRIG);
    GPIO_write_low(&PORTD, TRIG);
    
    // Configure Echo PIN
    GPIO_config_input_pullup(&DDRD, ECHO);
    
    // Configure Control Pump PIN
    GPIO_config_input_nopull(&DDRC, SW_PUMP);
    
    // Configure Relay Pump PIN
    GPIO_config_output(&DDRC, RELAY);
    GPIO_write_low(&PORTC, RELAY);
    
    // Configure Servo PIN
    GPIO_config_output(&DDRB, SERVO);
    GPIO_write_low(&PORTB, SERVO);
    
    // Configure Control Servo PIN
    GPIO_config_input_nopull(&DDRC, SW_SERVO);
    
    // Configure LED PINs
    GPIO_config_output(&DDRB, LED_G);
    GPIO_write_low(&PORTB, LED_G);
    GPIO_config_output(&DDRB, LED_R);
    GPIO_write_low(&PORTB, LED_R);
    
    // Initialize LCD display
    lcd_init(LCD_DISP_ON);
	
	// Set pointer to beginning of CGRAM memory
	lcd_command(1 << LCD_CGRAM);
    // Store all new chars to memory line by line
	for (uint8_t i = 0; i < 48; i++)
		lcd_data(customChar[i]);
        
	// Set DDRAM address
	lcd_command(1 << LCD_DDRAM);

    // Put strings on LCD display
    lcd_gotoxy(0, 0);
    lcd_puts("LVL:");
    lcd_gotoxy(6, 0);
    lcd_puts("%");
	lcd_gotoxy(12, 0);
	lcd_putc(char_num);
    lcd_gotoxy(0, 1);
    lcd_puts("PMP:");
    lcd_gotoxy(9, 1);
    lcd_puts("VLV:CLS");

    // Any logical change on INT0 generates an interrupt request
    EICRA |= (1 << ISC00); 
    // Not strictly necessary, as register default values are already 0
    EICRA &= ~((1 << ISC01) | (1 << ISC11) | (1 << ISC10)); 
    // External Interrupt Request Enable
    EIMSK |= (1 << INT0); EIMSK  &= ~(1 << INT1);
    
    // Overflow timer for trigger signal
    TIM0_overflow_4ms();
    TIM0_overflow_interrupt_enable();
    
    // Set overflow flag for LED timer
    TIM2_overflow_interrupt_enable();

    // 340 m/s sound wave propagates by 1 cm in ~58,8235 us
    // Empirical measurement suggests that 930 clocks of TIM1
    // with prescaler N=1 takes almost the same amount of time
    // Set max TIM1 value to this clock number
    OCR1A = 930;
    // Enable Timer/Counter1 Output Compare A Match interrupt    
    TIMSK1 |= (1 << OCIE1A);
    
    // Enables interrupts by setting the global interrupt mask
    sei();

    // Infinite loop
    while (1) {}

    return 0;
}

/* Interrupt service routines ----------------------------------------*/
ISR(INT0_vect)
{
    // Strings for converting numbers
    static char lcd_str[16];
    // Change of state counter
    static uint8_t i = 0;
    
    if (i) {
        // Disable counter
        TCCR1B |= 0;
        
        // Calculate the volume of water in the tank in %
        volume = 100 - ((distance - air_gap) * 100 / water_height);

        if (volume > 99) {
            strcpy(lcd_str, "FULL");
            
            lcd_gotoxy(8, 0);
            lcd_puts(" ");
			
			char_num = 5;
            
            if (!pumpIsOn)
                GPIO_write_high(&PORTB, LED_G);
            if (!valveIsOpen)
                GPIO_write_low(&PORTB, LED_R);
        }
        else if (volume > 9) {
            itoa(volume, lcd_str, 10);
            
            lcd_gotoxy(6, 0);
            lcd_puts("%  ");
			
			// Water level icon
            lcd_gotoxy(12, 0);
            
			if (volume > 80)
				char_num = 5;
			else if (volume > 60)
				char_num = 4;
			else if (volume > 40)
				char_num = 3;
			else if (volume > 20)
				char_num = 2;
			else
				char_num = 1;
            
            if (!pumpIsOn)
                GPIO_write_low(&PORTB, LED_G);
            if (!valveIsOpen)
                GPIO_write_low(&PORTB, LED_R);
        }
        else if (volume > 0) {
            itoa(volume, lcd_str, 10);
            
            lcd_gotoxy(5, 0);
            lcd_puts("%   ");
            
            if (!pumpIsOn)
                GPIO_write_low(&PORTB, LED_G);
            if (!valveIsOpen)
                GPIO_write_low(&PORTB, LED_R);
        }
        else {
            strcpy(lcd_str, "EMPTY");
            
			char_num = 0;
			
            if (!pumpIsOn)
                GPIO_write_low(&PORTB, LED_G);
            if (!valveIsOpen)
                GPIO_write_high(&PORTB, LED_R);
        } 
        
        // Put tank fill level on LCD  
        lcd_gotoxy(4, 0);
        lcd_puts(lcd_str);
        // Put cute tank fill icon on LCD  
        lcd_gotoxy(12, 0);
        lcd_putc(char_num);
        
        // Check for water excess (level is greater than max allowed value)
        if (distance < max_level || GPIO_read(&PINC, SW_SERVO)) {
            if (!valveIsOpen)
                open_valve();

            lcd_gotoxy(13, 1);
            lcd_puts("OPN");
        }
        else if (valveIsOpen) {
            close_valve();
            
            lcd_gotoxy(13, 1);
            lcd_puts("CLS");
        }
            
        // Check whether pump is on and water level is OK
        if (distance > air_gap && GPIO_read(&PINC, SW_PUMP)) {
            pump_on();
            
            lcd_gotoxy(4, 1);
            lcd_puts("ON ");
        } 
        else {
            pump_off();
            
            lcd_gotoxy(4, 1);
            lcd_puts("OFF");
        }            
        
        i = 0;
    }
    else {
        // Clear previous calculated distance before next measurement
        distance = 0;
        
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
    if (i == 9) {  
        send_trigger();
        
        i = 0;
    }          
}

ISR(TIMER1_COMPA_vect)
{
    ++distance;
    
    if (distance >= water_height + air_gap)
        TIM1_stop();
}

ISR(TIMER2_OVF_vect)
{
    static uint8_t number_of_overflows = 0;
    
    // Toggle LED(s) every ~500ms
    if (number_of_overflows >= 31) {
        if (valveIsOpen) {
            GPIO_toggle(&PORTB, LED_R);
        }            
        if (pumpIsOn) {
            GPIO_toggle(&PORTB, LED_G);
        }
                
        number_of_overflows = 0;
    }        
    
    ++number_of_overflows;
}