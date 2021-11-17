#include "Ultrasonic.h"
#include "gpio.h"
uint16_t micros;

Ultrasonic::Ultrasonic(uint8_t trigPin, uint8_t echoPin, unsigned long timeOut) {
  trig = trigPin;
  echo = echoPin;
  threePins = trig == echo ? true : false;
  GPIO_config_output(&DDRD, trig);
  GPIO_config_input_pullup(&DDRD, echo);
  timeout = timeOut;
}

unsigned int Ultrasonic::timing() {
  if (threePins)
    GPIO_config_output(&DDRD, trig);

  GPIO_write_low(&PORTD, trig);
  _delay_us(2);
  GPIO_write_high(&PORTD, trig);
  _delay_us(10);
  GPIO_write_low(&PORTD, trig);

  if (threePins)
    GPIO_config_input_pullup(&DDRD, trig);
  
  previousMicros = micros();
  while(!GPIO_read(echo) && (micros() - previousMicros) <= timeout); // wait for the echo pin HIGH or timeout
  previousMicros = micros();
  while(GPIO_read(echo)  && (micros() - previousMicros) <= timeout); // wait for the echo pin LOW or timeout

  return micros() - previousMicros; // duration
}

/*
 * If the unit of measure is not passed as a parameter,
 * sby default, it will return the distance in centimeters.
 * To change the default, replace CM by INC.
 */
unsigned int Ultrasonic::read(uint8_t und) {
  return timing() / und / 2;  //distance by divisor
}

/*
 * This method is too verbal, so, it's deprecated.
 * Use read() instead.
 */
unsigned int Ultrasonic::distanceRead(uint8_t und) {
  return read(und);
}