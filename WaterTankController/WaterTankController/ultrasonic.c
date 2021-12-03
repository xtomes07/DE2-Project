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
 *           trig_pin Trigger pin designation in the interval 0 to 7
 *           echo_reg Address of Data Direction Register, such as &DDRB
 *           echo_pin Incoming signal pin designation in the interval 
 *                    0 to 7
 * Returns:  none
 **********************************************************************/
void ultrasonic_init(volatile uint8_t *trig_reg, uint8_t trig_pin, volatile uint8_t *echo_reg, uint8_t echo_pin)
{
    signal_pin = echo_pin;
    
    // Configure Trig pin as output
    *trig_reg |= (1<<trig_pin);
    // Set pin low
    ++trig_reg;
    *trig_reg &= ~(1<<trig_pin);
    
    // Configure Echo pin as input 
    *echo_reg &= ~(1<<signal_pin);
    // Enable pull-up resistor
    ++echo_reg;
    *echo_reg |= (1<<signal_pin);
    
    --echo_reg;
    
    if (*echo_reg == DDRB) {
        // Any change on any enabled PCINT[7:0] pin will cause an interrupt
        PCICR |= (1<<PCIE0);
        // Enable pin change interrupt on the corresponding I/O pin
        PCMSK0 |= (1<<signal_pin);
    }
    if (*echo_reg == DDRC) {
        PCICR |= (1<<PCIE1);
        PCMSK1 |= (1<<signal_pin);
    }
    if (*echo_reg == DDRD) {
        if (signal_pin == PIN_INT1) {
            // Rising edge on INT1 generates an interrupt request
            EICRA |= (1<<ISC11) | (1<<ISC10);
            // Enable External Interrupt Requests
            EIMSK |= (1<<INT1);
        }
        else if (signal_pin == PIN_INT0) {
            // Rising edge on INT0 generates an interrupt request
            EICRA |= (1<<ISC01) | (1<<ISC00);
            // Enable External Interrupt Requests
            EIMSK |= (1<<INT0);
        }
        else {
            PCICR |= (1<<PCIE2);
            PCMSK2 |= (1<<signal_pin);
        }
    }

    // 340 m/s sound wave propagates by 1 cm in ~58,8235 us
    // Empirical measurement suggests that 930 clocks of TIM1
    // with prescaler N=1 takes almost the same amount of time
    // Set max TIM1 value to this clock number
    OCR1A = 930;
    // Enable Timer/Counter1 Output Compare A Match interrupt
    TIMSK1 |= (1<<OCIE1A);
    // Enable Timer/Counter1 Clear Timer on Compare Match mode
    TCCR1B |= (1<<WGM12);
}

/**********************************************************************
 * Function: ultrasonic_trigger()
 * Purpose:  Trigger ultrasonic sensor by sending 10us pulse.
 * Input:    reg_name - Address of Port Register, such as &PORTB
 *           pin_num  - Pin connected to trigger input of HC-SR04
 * Returns:  none
 **********************************************************************/
void ultrasonic_trigger(volatile uint8_t *reg_name, uint8_t pin_num)
{
    *reg_name |= (1<<pin_num);
    _delay_us(10);
    *reg_name &= ~(1<<pin_num);
}

/**********************************************************************
 * Function: ultrasonic_start_measuring()
 * Purpose:  Start Timer/Counter1.
 * Input:    none
 * Returns:  none
 **********************************************************************/
void ultrasonic_start_measuring()
{  
    // Clear previous calculated distance before next measurement
    distance = 0;
    
    // Start Timer/Counter1
    TCCR1B &= ~((1<<CS12) | (1<<CS11)); 
    TCCR1B |= (1<<CS10);
    
    if (signal_pin == PIN_INT1) {
        // Detect falling edge of the signal if INT1 is used
        EICRA |= (1<<ISC11);
        EICRA &= ~(1<<ISC10);
    }
    else if (signal_pin == PIN_INT0) {
        // Detect falling edge of the signal if INT0 is used
        EICRA |= (1<<ISC01);
        EICRA &= ~(1<<ISC00);
    }
}

/**********************************************************************
 * Function: ultrasonic_stop_measuring()
 * Purpose:  Stop Timer/Counter1.
 * Input:    none
 * Returns:  none
 **********************************************************************/
void ultrasonic_stop_measuring()
{
    // Stop Timer/Counter1
    TCCR1B &= ~((1<<CS12) | (1<<CS11) | (1<<CS10));
    
    if (signal_pin == PIN_INT1) {
        // Detect rising edge of the signal if INT1 is used
        EICRA |= (1<<ISC11) | (1<<ISC10);
    }
    else if (signal_pin == PIN_INT0) {
        // Detect rising edge of the signal if INT0 is used
        EICRA |= (1<<ISC01) | (1<<ISC00);
    }
}

/**********************************************************************
 * Function: ultrasonic_get_distance()
 * Purpose:  Stop measurement and get measured distance
 * Input:    none
 * Returns:  Distance in cm
 **********************************************************************/
uint16_t ultrasonic_get_distance()
{
    // Stop counting echo
    ultrasonic_stop_measuring();

    return distance;    
}

/* Interrupt service routines ----------------------------------------*/
/**********************************************************************
 * Function: Timer/Counter1 compare match interrupt
 * Purpose:  Update distance every time timer is cleared on compare 
*            match
 **********************************************************************/
ISR(TIMER1_COMPA_vect)
{
    ++distance;
}
