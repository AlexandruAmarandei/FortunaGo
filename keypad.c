#include "keypad.h"
#include <avr/io.h>

void init_keypad(){
	DDRE &= ~_BV(PE7);
	PORTE |= _BV(PE7);
	DDRC &= ~(_BV(PC2)|_BV(PC3)|_BV(PC4)|_BV(PC5)); 
	PORTC |= (_BV(PC2)|_BV(PC3)|_BV(PC4)|_BV(PC5)); 

	EICRB |= _BV(ISC40) | _BV(ISC50) | _BV(ISC71);
}

uint16_t getPressed(){
	if((~PINE & _BV(PE7))){
		return 1;
	}
	if((~PINC & _BV(PC2))){
		return 2;
	}
	if((~PINC & _BV(PC3))){
		return 3;
	}
	if((~PINC & _BV(PC4))){
		return 4;
	}
	if((~PINC & _BV(PC5))){
		return 5;
	}
	return 6;

}
