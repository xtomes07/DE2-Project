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
#define INT0_PIN    PD2     // External interrupt 0 pin on ATmega328P
#define INT1_PIN    PD3     // External interrupt 1 pin on ATmega328P
#ifndef F_CPU
#define F_CPU 16000000UL    // CPU frequency in Hz for delay.h
#endif

/* Includes ----------------------------------------------------------*/
#include <avr/io.h>
#include <util/delay.h>

/* Function prototypes -----------------------------------------------*/
/**
 * @name Functions
 */

/**
 * @brief  Configure pin and Timer/Counter1 for use with HC-SR04.
 * @param  trig_reg Address of Data Direction Register, such as &DDRB
 * @param  trig_pin Pin designation in the interval 0 to 7
 * @param  echo_reg Address of Data Direction Register, such as &DDRB
 * @param  echo_pin Pin designation, should handle either INT0 or INT1
 * @return none
 */
void ultrasonic_init(volatile uint8_t *trig_reg, uint8_t trig_pin, volatile uint8_t *echo_reg, uint8_t echo_pin);

/**
 * @brief  Trigger ultrasonic sensor by sending 10us pulse.
 * @param  reg_name Address of Port Register, such as &PORTB.
 * @param  pin_num  Pin connected to trigger input of HC-SR04.
 * @return none
 */
void ultrasonic_trig(volatile uint8_t *reg_name, uint8_t pin_num);

/**
 * @brief  Start Timer/Counter1.
 * @param  none
 * @return none
 */
void ultrasonic_start_TIM1();

/**
 * @brief  Stop Timer/Counter1.
 * @param  none
 * @return none
 */
void ultrasonic_stop_TIM1();

/** @} */

#endif /* ULTRASONIC_H_ */
