#ifndef ULTRASONIC_H_
#define ULTRASONIC_H_

/***********************************************************************
 * 
 * HC-SR04 Ultrasonic sensor library for AVR-GCC.
 * ATmega328P (Arduino Uno), 16 MHz, AVR 8-bit Toolchain 3.6.2
 *
 * Copyright (c) 2021 Pavlo Shelemba
 * This work is licensed under the terms of the MIT license.
 *
 **********************************************************************/

/**
 * @file 
 * @defgroup shelemba_ultrasonic HC-SR04 Ultrasonic sensor Library 
 *          <ultrasonic.h>
 * @code #include "ultrasonic.h" @endcode
 *
 * @brief HC-SR04 Ultrasonic sensor library for AVR-GCC.
 *
 * The library contains functions for controlling HC-SR04 Ultrasonic
 * Sensor.
 *
 * @author Pavlo Shelemba
 * @copyright (c) 2021 Pavlo Shelemba. 
 * This work is licensed under the terms of the MIT license.
 * @{
 */

/* Defines -----------------------------------------------------------*/
#define PIN_INT0    PIND2   // External interrupt 0 pin on ATmega328P
#define PIN_INT1    PIND3   // External interrupt 1 pin on ATmega328P
#ifndef F_CPU
#define F_CPU 16000000UL    // CPU frequency in Hz for delay.h
#endif

/* Includes ----------------------------------------------------------*/
#include <avr/interrupt.h>  // Interrupts standard C library for AVR-GCC
#include <avr/io.h>         // AVR device-specific IO definitions
#include <util/delay.h>     // Busy-wait delay loops

/* Variables ---------------------------------------------------------*/
// Measured distance in cm
uint16_t distance;
// Incoming echo pin
uint8_t  signal_pin;

/* Function prototypes -----------------------------------------------*/
/**
 * @name Functions
 */

/**
 * @brief  Configure pin and Timer/Counter1 for use with HC-SR04.
 * @param  trig_reg Address of Data Direction Register, such as &DDRB.
 * @param  trig_pin Trigger pin designation in the interval 0 to 7.
 * @param  echo_reg Address of Data Direction Register, such as &DDRB.
 * @param  echo_pin Incoming signal pin designation in the interval
 *                  0 to 7.
 * @return none
 */
void ultrasonic_init(volatile uint8_t *trig_reg, uint8_t trig_pin, volatile uint8_t *echo_reg, uint8_t echo_pin);

/**
 * @brief  Trigger ultrasonic sensor by sending 10us pulse.
 * @param  reg_name Address of Port Register, such as &PORTB.
 *         pin_num  Pin connected to trigger input of HC-SR04.
 * @return none
 */
void ultrasonic_trigger(volatile uint8_t *reg_name, uint8_t pin_num);

/**
 * @brief  Start Timer/Counter1.
 * @param  none
 * @return none
 */
void ultrasonic_start_measuring();

/**
 * @brief  Stop Timer/Counter1.
 * @param  none
 * @return none
 */
void ultrasonic_stop_measuring();

/**
 * @brief  Stop measurement and get measured distance.
 * @param  none
 * @return Distance in cm
 */
uint16_t ultrasonic_get_distance();

/** @} */

#endif /* ULTRASONIC_H_ */
