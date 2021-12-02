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
 * Input:    trig_reg Address of Data Direction Register, such as &DDRB
 *           trig_pin Pin designation in the interval 0 to 7
 *           echo_reg Address of Data Direction Register, such as &DDRB
 *           echo_pin Pin designation, should handle either INT0 or INT1
 * Returns:  none
 **********************************************************************/
void ultrasonic_init(volatile uint8_t *trig_reg, uint8_t trig_pin, volatile uint8_t *echo_reg, uint8_t echo_pin)
{
     // Configure Trig pin and set it to low value
    *trig_reg = *trig_reg | (1<<trig_pin);
    ++trig_reg;
    *trig_reg = *trig_reg & ~(1<<trig_pin);
    
    // Configure Echo pin
    *echo_reg = *echo_reg & ~(1<<echo_pin);
    ++echo_reg;
    *echo_reg = *echo_reg | (1<<echo_pin);
    
    if (echo_pin == INT1_PIN)
    {
        // Any logical change on INT1 generates an interrupt request
        EICRA |= (1<<ISC10);
        EICRA &= ~(1<<ISC11);
        // Enable External Interrupt Requests
        EIMSK |= (1<<INT1);
    }
    else if (echo_pin == INT0_PIN)
    {
        // Any logical change on INT0 generates an interrupt request
        EICRA |= (1<<ISC00);
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
    // Enable TIM1 CTC mode
    TCCR1B |= (1<<WGM12);
}
/**********************************************************************
 * Function: ultrasonic_trig()
 * Purpose:  Trigger ultrasonic sensor by sending 10us pulse.
 * Input:    reg_name - Address of Port Register, such as &PORTB
 *           pin_num  - Pin connected to trigger input of HC-SR04
 * Returns:  none
 **********************************************************************/
void ultrasonic_trig(volatile uint8_t *reg_name, uint8_t pin_num)
{
    *reg_name = *reg_name | (1<<pin_num);
    _delay_us(10);
    *reg_name = *reg_name & ~(1<<pin_num);
}

/**********************************************************************
 * Function: ultrasonic_trig()
 * Purpose:  Start Timer/Counter1.
 * Input:    none
 * Returns:  none
 **********************************************************************/
void ultrasonic_start_TIM1()
{  
    TCCR1B &= ~((1<<CS12) | (1<<CS11)); 
    TCCR1B |= (1<<CS10);
    
}

/**********************************************************************
 * Function: ultrasonic_trig()
 * Purpose:  Stop Timer/Counter1.
 * Input:    none
 * Returns:  none
 **********************************************************************/
void ultrasonic_stop_TIM1()
{
    TCCR1B &= ~((1<<CS12) | (1<<CS11) | (1<<CS10));
}