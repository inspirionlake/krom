/*
 * positioning.cpp
 *
 * Created: 14.07.2018 2:45:23
 *  Author: Dmitry
 */ 
#include <avr/io.h>
#include <stdlib.h>
#include "positioning.h"

Axis::Axis(uint8_t nm, uint8_t en, uint8_t dir, uint16_t stp, uint8_t microstp, double val_of_div) {
	step = stp;
	microstep = microstp;
	name = nm;
	enable = en;
	direction = dir;
	value_of_division = val_of_div;
	busy = 0;
}

int32_t Axis::getCoordinate(void) {
	return coordinate;
}

void Axis::findHome(void) {
	//finding home...
	coordinate = 0;
}

void Axis::setCoordinate(int32_t value) {
	if (coordinate < value) {
		direction = 0;
	}
	if (coordinate > value) {
		direction = 1;
	}
	int32_t different = abs(value - coordinate);
	//printValue(different);
	double tmp = different / value_of_division;
	//char buff[10];
	//dtostrf(tmp,6,9,buff);
	//uartTransmitString(buff);
	//uartTransmit('\n');
	uint32_t steps = (uint32_t)tmp;
	//printValue(steps);
	step = steps;
	//printValue(step);
	coordinate = value;
	//printValue(coordinate);
}

void Axis::release(void) {
	busy = 0;
}

uint8_t Axis::status(void) {
	return busy;
}
