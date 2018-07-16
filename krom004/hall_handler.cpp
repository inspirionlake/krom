/*
 * hall_handler.cpp
 *
 * Created: 16.07.2018 18:35:25
 *  Author: Dmitry
 */ 
#define F_CPU	16000000UL

#include "hall_handler.h"
#include <avr/io.h>
#include <stdint.h>
#include <util/delay.h>

void initAdc(void) {
	ADMUX |= (1<<REFS0); //reference voltage set as AVCC
	ADMUX |= (1<<ADLAR); //left adjust result of conversion
	ADCSRA |= (1<<ADEN); //Enable ADC
	ADCSRA |= (1<<ADPS2) | (1<<ADPS1) | (1<<ADPS0); //prescaller = 128 it is means that frequency of conversion will be 125kHz
}

void refreshHallValue(uint8_t axis, uint8_t *hall_val) {
	switch(axis) {
		case X: {
			//adc0 - axis X
			ADMUX &= ~(1<<MUX3); ADMUX &= ~(1<<MUX2); ADMUX &= ~(1<<MUX1); ADMUX &= ~(1<<MUX0); //adc0 0000
			ADCSRA |= (1<<ADSC); //start conversion
			_delay_ms(1);
			*hall_val = ADCH;
			break;
		}
		case Y: {
			//adc1 - axis Y
			ADMUX &= ~(1<<MUX3); ADMUX &= ~(1<<MUX2); ADMUX &= ~(1<<MUX1); ADMUX |= (1<<MUX0); //adc1 0001
			ADCSRA |= (1<<ADSC); //start conversion
			_delay_ms(1);
			*hall_val = ADCH;
			break;
		}
		case Z: {
			//adc2 - axis Z
			ADMUX &= ~(1<<MUX3); ADMUX &= ~(1<<MUX2); ADMUX |= (1<<MUX1); ADMUX &= ~(1<<MUX0); //adc2 0010
			ADCSRA |= (1<<ADSC); //start conversion
			_delay_ms(1);
			*hall_val = ADCH;
			break;
		}
		default: return;
	}
	
	
}