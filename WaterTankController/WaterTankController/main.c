/***********************************************************************
 * 
 * Water tank controller.
 * ATmega328P (Arduino Uno), 16 MHz, AVR 8-bit Toolchain 3.6.2
 *
 * Copyright (c) 2021 Shelemba Pavlo
 * Copyright (c) 2021 Tomešek Jiří
 * Copyright (c) 2021 Točený Ivo
 * This work is licensed under the terms of the MIT license
 * 
 **********************************************************************/

/* Defines -----------------------------------------------------------*/
#define TRIG     PD0     // Trigger Pin
#define ECHO     PD2     // Echo Pin
#define SERVO    PB4     // Servo valve pin
#define LED_G    PB6     // Servo valve pin
#define LED_R    PB7     // Servo valve pin
#define RELAY    PC0     // Pin for pump relay control
#define SW_PUMP  PC1     // Pin for pump switch
#define SW_SERVO PC2     // Pin for servo valve switch
#ifndef F_CPU
#define F_CPU 16000000UL // CPU frequency in Hz for delay.h
#endif

/* Includes ----------------------------------------------------------*/
#include <avr/interrupt.h> // Interrupts standard C library for AVR-GCC
#include <avr/io.h>        // AVR device-specific IO definitions
#include <stdlib.h>        // C library for conversion function
#include <string.h>        // C library for string manipulations
#include <util/delay.h>    // Busy-wait delay loops
#include "gpio.h"          // GPIO library for AVR-GCC
#include "lcd.h"           // Peter Fleury's LCD library
#include "symbols.h"       // Custom characters for HD44780 LCD
#include "timer.h"         // Timer library for AVR-GCC
#include "ultrasonic.h"    // Ultrasonic sensor library for AVR-GCC

/* Variables ---------------------------------------------------------*/
// Max water height in cm
uint16_t water_height = 400;
// Gap between sensor and max water height in cm
uint16_t air_gap = 20;
// Total height of the system
uint16_t total_height;

// Measured distance in cm
uint16_t distance;
// Max water level before valve opens
uint16_t max_level;

// Water tank fill level
uint8_t volume = 0;
// Booleans for electromechanics
uint8_t valveIsOpen = 0;
uint8_t pumpIsOn = 0;
// Custom character number
uint8_t char_num = 0;

/* Function definitions ----------------------------------------------*/
/**********************************************************************
 * Function: Pump configuration
 * Purpose:  Start-up pump configuration for control and relay pins.
 * Input:    none 
 * Returns:  none
 **********************************************************************/
void configure_pump()
{
    // Configure Pump Control pin
    GPIO_config_input_nopull(&DDRC, SW_PUMP);

    // Configure Pump Relay pin
    GPIO_config_output(&DDRC, RELAY);
    GPIO_write_low(&PORTC, RELAY);
}
/**********************************************************************
 * Function: Servo configuration
 * Purpose:  Start-up servo configuration for pin and control pin.
 * Input:    none	 
 * Returns:  none
 **********************************************************************/
void configure_servo()
{
    // Configure Servo pin
    GPIO_config_output(&DDRB, SERVO);
    GPIO_write_low(&PORTB, SERVO);

    // Configure Servo Control pin
    GPIO_config_input_nopull(&DDRC, SW_SERVO);
}
/**********************************************************************
 * Function: LEDs configuration
 * Purpose:  Start-up LEDs configuration and storage of custom chars.
 * Input:    none		 
 * Returns:  none
 **********************************************************************/
void configure_leds()
{
    // Configure LED pins
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
}
/**********************************************************************
 * Function: Initialization of start-up LCD values
 * Purpose:  Initializes LCD values at start of device to default
 *           values.
 * Input:    none	 
 * Returns:  none
 **********************************************************************/
void set_initial_lcd_values()
{
    // Put strings on LCD display
    lcd_show(0, 0, "LVL:");
    lcd_showc(15, 0, char_num);
    lcd_show(0, 1, "PMP:");
    lcd_show(9, 1, "VLV:CLS");
}
/**********************************************************************
 * Function: Initialization of timer overflows
 * Purpose:  Set start-up initialization of timer overflow interrupts
 *           and global interrupt mask.
 * Input:    none
 * Returns:  none
 **********************************************************************/
void set_timer_overflows()
{
    // Overflow timer for trigger signal
    TIM0_overflow_4ms();
    TIM0_overflow_interrupt_enable();

    // Set overflow flag for LED timer
    TIM2_overflow_interrupt_enable();

    // Enables interrupts by setting the global interrupt mask
    sei();
}
/**********************************************************************
 * Function: Initializes configurations
 * Purpose:  Initial configuration of essential components and values
 *           at the start of the program.
 * Input:    none
 * Returns:  none
 **********************************************************************/
void init_configurations()
{
    // Set max water level halfway between sensor and max water level
    max_level = air_gap / 2;
    total_height = water_height + air_gap;

    // Initialize ultrasonic sensor
    ultrasonic_init(&DDRD, TRIG, &DDRD, ECHO);

    configure_pump();

    configure_servo();

    configure_leds();
}
/**********************************************************************
 * Function: Open valve
 * Purpose:  Send 2 ms pulse to open valve and notify it by blinking 
 *           LED.
 * Input:    none	 
 * Returns:  none
 **********************************************************************/
void open_valve()
{
    GPIO_write_high(&PORTB, SERVO);
    _delay_ms(2);
    GPIO_write_low(&PORTB, SERVO);
    _delay_ms(18);

    valveIsOpen = 1;

    // Start blinking LED
    TIM2_overflow_16ms();
}
/**********************************************************************
 * Function: Close valve
 * Purpose:  Send 1.5 ms pulse to close valve and stop blinking LED.
 * Input:    none 
 * Returns:  none
 **********************************************************************/
void close_valve()
{
    GPIO_write_high(&PORTB, SERVO);
    _delay_ms(1.5);
    GPIO_write_low(&PORTB, SERVO);
    _delay_ms(18.5);

    valveIsOpen = 0;

    // Stop blinking LED
    if (!pumpIsOn)
        TIM2_stop();
}
/**********************************************************************
 * Function: Turns on the pump
 * Purpose:  If tank is not overflowing, switch pump on automatically.
 * Input:    none
 * Returns:  none
 **********************************************************************/
void pump_on()
{
    // Turn relay for Pump on
    GPIO_write_high(&PORTC, RELAY);

    pumpIsOn = 1;

    // Start blinking LED
    TIM2_overflow_16ms();
}
/**********************************************************************
 * Function: Turns off the pump
 * Purpose:  If tank is overflowing, switch pump off automatically.	
 * Input:    none
 * Returns:  none
 **********************************************************************/
void pump_off()
{
    GPIO_write_low(&PORTC, RELAY);

    pumpIsOn = 0;

    // Stop blinking LED
    if (!valveIsOpen)
        TIM2_stop();
}
/**********************************************************************
 * Function: Shows prepared LCD values
 * Purpose:  After preparation of LCD values based on water tank level,
 *           display values on LCD display.
 * Input:    lcd_str    - string for water level status
 *           lcd_smiley - string for smiley :^)
 *           char_num   - char for tank fill level icon
 * Returns:  none
 **********************************************************************/
void show_final_lcd_values(const char *lcd_str, const char *lcd_smiley, const uint8_t char_num)
{
    // Put tank fill level in % on LCD
    lcd_show(4, 0, lcd_str);
    // Put smiley on LCD
    lcd_show(12, 0, lcd_smiley);
    // Put cute tank fill level icon on LCD
    lcd_show(15, 0, char_num);
}
/**********************************************************************
 * Function: Resolves tank overflow and fill status
 * Purpose:  If tank is filled too much based on distance, function
 *           prepares LCD with OVERFLOW text and surprised smiley,
 *           otherwise tank is full and text FULL is prepared.
 * Input:    lcd_str    - string for water level status
 *           lcd_smiley - string for smiley :^)
 * Returns:  none
 **********************************************************************/
void resolve_full_water_level_or_overflow(char *lcd_str, char *lcd_smiley)
{
    if (distance < max_level)
    {
        strcpy(lcd_str, "OVERFLOW");
        strcpy(lcd_smiley, ":^O");
    }
    else
    {
        strcpy(lcd_str, "FULL    ");
        strcpy(lcd_smiley, ":^)");
    }

    char_num = 5;

    // Signal full level on LED if valve is not open
    if (!pumpIsOn)
        GPIO_write_high(&PORTB, LED_G);

    if (!valveIsOpen)
        GPIO_write_low(&PORTB, LED_R);
}
/**********************************************************************
 * Function: Resolves custom character selection based on water levels
 * Purpose:  Custom characters show user how filled the tank is.
 *           Appropriate char number is chosen based on water level.
 * Input:    none
 * Returns:  none
 **********************************************************************/
void resolve_char_num()
{
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
}
/**********************************************************************
 * Function: Resolves LCD values for filled tank in norm
 * Purpose:  If tank is filled with atleast 10% and less than 100%,
 *           function shows happy smiley and prepares LCD values.
 * Input:    lcd_str    - string for water level status
 *           lcd_smiley - string for smiley :^)
 * Returns:  none
 **********************************************************************/
void resolve_two_digit_water_level(char *lcd_str, char *lcd_smiley)
{
    itoa(volume, lcd_str, 10);
    strcpy(lcd_smiley, ":^)");

    lcd_show(6, 0, "%     ");

    resolve_char_num();

    if (!pumpIsOn)
    {
        GPIO_write_low(&PORTB, LED_G);
    }

    if (!valveIsOpen)
    {
        GPIO_write_low(&PORTB, LED_R);
    }
}
/**********************************************************************
 * Function: Resolves LCD values for almost empty tank
 * Purpose:  If tank is almost empty, function shows user custom chars with
 *           stressed smiley face and EMPTY water tank char.
 * Input:    lcd_str    - string for water level status
 *           lcd_smiley - string for smiley :^)
 * Returns:  none
 **********************************************************************/
void resolve_single_digit_water_level(char *lcd_str, char *lcd_smiley)
{
    itoa(volume, lcd_str, 10);
    strcpy(lcd_smiley, ":^I");

    lcd_show(5, 0, "%      ");

    char_num = 1;

    if (!pumpIsOn)
    {
        GPIO_write_low(&PORTB, LED_G);
    }

    if (!valveIsOpen)
    {
        GPIO_write_low(&PORTB, LED_R);
    }
}
/**********************************************************************
 * Function: Resolves LCD values for empty water tank 
 * Purpose:  If tank is empty, function shows user custom chars with
 *           sad smiley face and EMPTY text.
 * Input:    lcd_str    - string for water level status
 *           lcd_smiley - string for smiley :^)
 * Returns:  none
 **********************************************************************/
void resolve_empty(char *lcd_str, char *lcd_smiley)
{
    strcpy(lcd_str, "EMPTY   ");
    strcpy(lcd_smiley, ":^(");

    char_num = 0;

    if (!pumpIsOn)
    {
        GPIO_write_low(&PORTB, LED_G);
    }

    if (!valveIsOpen)
    {
        GPIO_write_high(&PORTB, LED_R);
    }
}
/**********************************************************************
 * Function: Resolves LCD values based on tank water percentage 
 * Purpose:  Prepares LCD values based on tank water level percentage.
 * Input:    lcd_str    - string for water level status
 *           lcd_smiley - string for smiley :^)
 * Returns:  none
 **********************************************************************/
void resolve_tank_fill_percentage(char *lcd_str, char *lcd_smiley)
{
    if (volume > 99)
        resolve_full_water_level_or_overflow(lcd_str, lcd_smiley);
    else if (volume > 9)
        resolve_two_digit_water_level(lcd_str, lcd_smiley);
    else if (volume > 0)
        resolve_single_digit_water_level(lcd_str, lcd_smiley);
    else
        resolve_empty(lcd_str, lcd_smiley);
}
/**********************************************************************
 * Function: Checks water overflow or if valve is turned on
 * Purpose:  Automated proccess of opening and closing valve
 *           based on water level.
 * Input:    none
 * Returns:  none
 **********************************************************************/
void check_valve_on_or_water_overflow()
{
    // Check for water excess (level is greater than max allowed value)
    // or if valve switch is on
    if (distance < max_level || GPIO_read(&PINC, SW_SERVO))
    {
        lcd_show(13, 1, "OPN");
        if (!valveIsOpen)
            open_valve();
    }
    else if (valveIsOpen)
    {
        lcd_show(13, 1, "CLS");
        close_valve();
    }
}
/**********************************************************************
 * Function: Checks if pump is on and water level is ok 
 * Purpose:  Based on water level turns pump on or off.
 * Input:    none
 * Returns:  none
 **********************************************************************/
void check_pump_on_and_water_level_ok()
{
    if (distance > air_gap && GPIO_read(&PINC, SW_PUMP)) 
    {
        lcd_show(4, 1, "ON ");

        pump_on();
    }
    else 
    {
        lcd_show(4, 1, "OFF");
        pump_off();
    }
}
/**********************************************************************
 * Function: Calculates measured distance from sensor 
 * Purpose:  Sensor tells us distance between water and sensor.
 * Input:    none
 * Returns:  none
 **********************************************************************/
void get_measured_distance()
{
    if ((distance = ultrasonic_get_distance()) > total_height)
        distance = total_height;
}
/**********************************************************************
 * Function: Calculates water level from measured distance from sensor
 * Purpose:  Volume is needed to show fill percentage of tank
 * Input:    none
 * Returns:  none
 **********************************************************************/
void calculate_water_volume()
{
    get_measured_distance();

    volume = 100 - ((distance - air_gap) * 100 / water_height);
}
/**********************************************************************
 * Function: Main function where the program execution begins
 * Purpose:  Update stopwatch value on LCD display when 8-bit 
 *           Timer/Counter2 overflows.
 * Returns:  none
 **********************************************************************/
int main(void)
{
    init_configurations();

    set_initial_lcd_values();

    set_timer_overflows();

    while (1)
    {
    }

    return 0;
}
/* Interrupt service routines ----------------------------------------*/
/**********************************************************************
 * Function: External Interrupt 0
 * Purpose:  Register echo signal from sensor and calculate distance
 *           based on its length
 **********************************************************************/
ISR(INT0_vect)
{
    // State counter
    static uint8_t echoIsHigh = 1;
    // String for water level status
    static char lcd_str[16];
    // String for smiley :^)
    static char lcd_smiley[8];

    if (echoIsHigh)
    {
        // Begin measuring echo signal
        ultrasonic_start_measuring();

        echoIsHigh = 0;
    }
    else
    {
        calculate_water_volume();

        resolve_tank_fill_percentage(lcd_str, lcd_smiley);

        check_valve_on_or_water_overflow();

        check_pump_on_and_water_level_ok();

        show_final_lcd_values(lcd_str, char_num, lcd_smiley);

        echoIsHigh = 1;
    }
}
/**********************************************************************
 * Function: Timer/Counter0 overflow interrupt
 * Purpose:  Trigger ultrasonic sensor every ~40 ms
 **********************************************************************/
ISR(TIMER0_OVF_vect)
{
    static uint8_t timerCounter = 0;
    ++timerCounter;

    if (timerCounter == 9)
    {
        ultrasonic_trigger(&PORTD, TRIG);

        timerCounter = 0;
    }
}
/**********************************************************************
 * Function: Timer/Counter2 compare match interrupt
 * Purpose:  Toggle LED(s) every ~500ms
 **********************************************************************/
ISR(TIMER2_OVF_vect)
{
    static uint8_t number_of_overflows = 0;

    ++number_of_overflows;

    if (number_of_overflows >= 31)
    {
        if (valveIsOpen)
            GPIO_toggle(&PORTB, LED_R);

        if (pumpIsOn)
            GPIO_toggle(&PORTB, LED_G);

        number_of_overflows = 0;
    }
}
