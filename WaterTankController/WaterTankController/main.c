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

// Max water height in cm 
uint16_t HEIGHT = 400;
// Gap between sensor and max water height in cm
uint16_t AIR    = 20;

// Max water level before valve opens
uint16_t MAX;

// Measured distance in cm
uint16_t DISTANCE = 0;
// Booleans for electromechanics
uint8_t  valveIsOpen = 0;
uint8_t  pumpIsOn    = 0;

/* Symbols	----------------------------------------------------------*/
uint16_t customChar[] = {
	0B10001,	//tank is empty
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
	
	0B10001,	// tank is full
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
    
    TIM2_overflow_16ms();
}

void close_valve()
{
    GPIO_write_high(&PORTB, servo);
    _delay_ms(1.5);
    GPIO_write_low(&PORTB, servo);
    _delay_ms(18.5);
    
    valveIsOpen = 0;
    
    if (!pumpIsOn)
        TIM2_stop();
}

void pump_on()
{
    // Turn relay for Pump on
    GPIO_write_high(&PORTC, relay);
    
    pumpIsOn = 1;

    TIM2_overflow_16ms();
}

void pump_off()
{
    // Turn relay for Pump off
    GPIO_write_low(&PORTC, relay);
    
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
    AIR -= 1;
    // Set max water level halfway between sensor and water
    MAX = AIR / 2;
    
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
	
	// Set pointer to beginning of CGRAM memory
	lcd_command(1 << LCD_CGRAM);
	for (uint8_t i = 0; i < 48; i++)
	{
		// Store all new chars to memory line by line
		lcd_data(customChar[i]);
	}
	// Set DDRAM address
	lcd_command(1 << LCD_DDRAM);

    // Put strings on LCD display
    lcd_gotoxy(0, 0);
    lcd_puts("LVL:");
    lcd_gotoxy(6, 0);
    lcd_puts("%");
	lcd_gotoxy(10, 0);
	lcd_putc(0);
    lcd_gotoxy(0, 1);
    lcd_puts("PMP:");
    lcd_gotoxy(9, 1);
    lcd_puts("VLV:CLS");

    
    // Any logical change on INT0 generates an interrupt request
    // The rising edge of INT1 generates an interrupt request
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
    // Water tank fill level 
    static uint16_t volume = 0;
    
    if (i) {
        // Disable counter
        TCCR1B |= 0;
        
        // Calculate the volume of water in the tank in %
        volume = 100 - ((DISTANCE - AIR) * 100 / HEIGHT);

        if (volume > 99) {
            strcpy(lcd_str, "FULL");
            
            lcd_gotoxy(8, 0);
            lcd_puts(" ");
			
			lcd_gotoxy(10, 0);
			lcd_putc(5);
            
            if (!pumpIsOn)
                GPIO_write_high(&PORTB, led_green);
            if (!valveIsOpen)
                GPIO_write_low(&PORTB, led_red);
        }
        else if (volume > 9) {
            itoa(volume, lcd_str, 10);
            
            lcd_gotoxy(6, 0);
            lcd_puts("%  ");
			
			//custom char
			if (volume > 0 && volume <= 20)
			{
				lcd_gotoxy(10, 0);
				lcd_putc(1);
			}
			else if (volume > 20 && volume <= 40)
			{
				lcd_gotoxy(10, 0);
				lcd_putc(2);
			}
			else if (volume > 40 && volume <= 60)
			{
				lcd_gotoxy(10, 0);
				lcd_putc(3);
			}
			else if (volume > 60 && volume <= 80)
			{
				lcd_gotoxy(10, 0);
				lcd_putc(4);
			}
			else if (volume > 80 && volume <= 100)
			{
				lcd_gotoxy(10, 0);
				lcd_putc(5);
			}
            
            if (!pumpIsOn)
                GPIO_write_low(&PORTB, led_green);
            if (!valveIsOpen)
                GPIO_write_low(&PORTB, led_red);
        }
        else if (volume > 0) {
            itoa(volume, lcd_str, 10);
            
            lcd_gotoxy(5, 0);
            lcd_puts("%   ");
            
            if (!pumpIsOn)
                GPIO_write_low(&PORTB, led_green);
            if (!valveIsOpen)
                GPIO_write_low(&PORTB, led_red);
        }
        else {
            strcpy(lcd_str, "EMPTY");
            
			lcd_gotoxy(10, 0);
			lcd_putc(0);
			
            if (!pumpIsOn)
                GPIO_write_low(&PORTB, led_green);
            if (!valveIsOpen)
                GPIO_write_high(&PORTB, led_red);
        } 
        
        // Put tank fill level on LCD  
        lcd_gotoxy(4, 0);
        lcd_puts(lcd_str);
        
        // Check for water excess (level is greater than max allowed value)
        if (DISTANCE < MAX || GPIO_read(&PINC, btn_servo)) {
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
        if (DISTANCE > AIR && GPIO_read(&PINC, pump)) {
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
        DISTANCE = 0;
        
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
    ++DISTANCE;
    
    if (DISTANCE >= HEIGHT + AIR)
        TIM1_stop();
}

ISR(TIMER2_OVF_vect)
{
    static uint8_t number_of_overflows = 0;
    
     // Toggle LED(s) every ~500ms
    if (number_of_overflows >= 31) {
        if (valveIsOpen)
            GPIO_toggle(&PORTB, led_red);
        if (pumpIsOn)
            GPIO_toggle(&PORTB, led_green);
        
        number_of_overflows = 0;
    }        
    
    ++number_of_overflows;
}