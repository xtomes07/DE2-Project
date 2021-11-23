/*
 * Ultrasonic.h
 *
 * Created: 17.11.2021 0:09:35
 *  Author: Jirka_NT
 */ 


#ifndef ULTRASONIC_H_
#define ULTRASONIC_H_



#define CM 28;
#define INC 71;

class Ultrasonic {
	Ultrasonic(uint8_t sigPin) : Ultrasonic(sigPin, sigPin) {};
	Ultrasonic(uint8_t trigPin, uint8_t echoPin, unsigned long timeOut = 20000UL);
	unsigned int read(uint8_t und = CM);
	unsigned int distanceRead(uint8_t und = CM) __attribute__ ((deprecated ("This method is deprecated, use read() instead.")));
	void setTimeout(unsigned long timeOut) {timeout = timeOut;}
	void setMaxDistance(unsigned long dist) {timeout = dist*CM*2;}

	private:
	uint8_t trig;
	uint8_t echo;
	bool threePins = false;
	unsigned long previousMicros;
	unsigned long timeout;
	unsigned int timing();
};


#endif /* ULTRASONIC_H_ */