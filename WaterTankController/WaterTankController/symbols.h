/***********************************************************************
 * 
 * Custom characters for HD44780 LCD
 * ATmega328P (Arduino Uno), 16 MHz, AVR 8-bit Toolchain 3.6.2
 *
 * Copyright (c) 2021 Tomešek Ji?í
 * Copyright (c) 2021 To?ený Ivo
 * Dept. of Radio Electronics, Brno University of Technology, Czechia
 * This work is licensed under the terms of the MIT license.
 *
 **********************************************************************/

#ifndef SYMBOLS_H_
#define SYMBOLS_H_

uint16_t customChar[] = {
    // Tank is empty
    0B10001,    
    0B10001,
    0B10001,
    0B10001,
    0B10001,
    0B10001,
    0B11111,
    0B11111,
    // 1/5 tank
    0B10001,    
    0B10001,
    0B10001,
    0B10001,
    0B10001,
    0B11111,
    0B11111,
    0B11111,
    // 2/5 tank
    0B10001,    
    0B10001,
    0B10001,
    0B10001,
    0B11111,
    0B11111,
    0B11111,
    0B11111,
    // 3/5 tank
    0B10001,   
    0B10001,
    0B10001,
    0B11111,
    0B11111,
    0B11111,
    0B11111,
    0B11111,
    // 4/5 tank
    0B10001,	
    0B10001,
    0B11111,
    0B11111,
    0B11111,
    0B11111,
    0B11111,
    0B11111,
    // Tank is full
    0B10001,    
    0B11111,
    0B11111,
    0B11111,
    0B11111,
    0B11111,
    0B11111,
    0B11111
};

#endif /* SYMBOLS_H_ */