/***********************************************************************
 * 
 * HC-SR04 Ultrasonic sensor library for AVR-GCC.
 * ATmega328P (Arduino Uno), 16 MHz, AVR 8-bit Toolchain 3.6.2
 *
 * Copyright (c) 2021 Pavlo Shelemba
 * This work is licensed under the terms of the MIT license.
 *
 **********************************************************************/


/* Includes ----------------------------------------------------------*/
#include "ultrasonic.h"

/* Function definitions ----------------------------------------------*/
/**********************************************************************
 * Function: ultrasonic_init()
 * Purpose:  Configure pin and timer/counter 1 for use with HC-SR04
 * Input:    pin - 0 if sensor is connected to INT0
 *                 1 if sensor is connected to INT1
 * Returns:  none
 **********************************************************************/
void ultrasonic_init(uint8_t pin)
{
    if (pin)
    {
        // Any logical change on INT1 generates an interrupt request
        EICRA |= (1 << ISC10);
        EICRA &= ~(1 << ISC11);
        // Enable External Interrupt Requests
        EIMSK |= (1 << INT1);
    }
    else
    {
        // Any logical change on INT0 generates an interrupt request
        EICRA |= (1<< ISC00);
        EICRA &= ~(1<<ISC01);
        // Enable External Interrupt Requests
        EIMSK |= (1<<INT0);
    }
    
    // 340 m/s sound wave propagates by 1 cm in ~58,8235 us
    // Empirical measurement suggests that 930 clocks of TIM1
    // with prescaler N=1 takes almost the same amount of time
    // Set max TIM1 value to this clock number
    OCR1A = 930;
    // Enable Timer/Counter1 Output Compare A Match interrupt
    TIMSK1 |= (1<<OCIE1A);
}
/**********************************************************************
 * Function: ultrasonic_trig()
 * Purpose:  Trigger ultrasonic sensor by sending 10us pulse.
 * Input:    reg_name - Address of Port Register, such as &PORTB
 *           pin_num - Pin connected to trigger input of HC-SR04
 * Returns:  none
 **********************************************************************/
void ultrasonic_trig(volatile uint8_t *reg_name, uint8_t pin_num)
{
    *reg_name = *reg_name | (1<<pin_num);
    _delay_us(10);
    *reg_name = *reg_name & ~(1<<pin_num);
}