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
 * @brief  Configure pin and timer/counter 1 for use with HC-SR04.
 * @param  pin 0 if sensor is connected to INT0;
 *             1 if sensor is connected to INT1.
 * @return none
 */
void ultrasonic_init(uint8_t pin);

/**
 * @brief  Trigger ultrasonic sensor by sending 10us pulse.
 * @param  reg_name Address of Port Register, such as &PORTB.
 * @param  pin_num  Pin connected to trigger input of HC-SR04.
 * @return none
 */
void ultrasonic_trig(volatile uint8_t *reg_name, uint8_t pin_num);

/** @} */

#endif /* ULTRASONIC_H_ */