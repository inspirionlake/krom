/*
 * positioning.cpp
 *
 * Created: 14.07.2018 2:45:23
 *  Author: Dmitry
 */ 

#include <avr/io.h>
#include <stdlib.h>
#include "positioning.h"
#include "hall_handler.h"
#include <avr/interrupt.h>
#include <math.h>

Axis::Axis(uint8_t nm, uint8_t en, uint8_t dir, uint16_t stp, uint8_t microstp, double val_of_div) {
	step = stp;
	microstep = microstp;
	name = nm;
	enable = en;
	enable_changed = 1;
	direction = dir;
	direction_changed = 1;
	value_of_division = val_of_div;
	busy = 0;
	coordinate = 0;
}

int32_t Axis::getCoordinate(void) {
	return coordinate;
}

void Axis::findHome(void) {
	coordinate = 0;
	uint8_t hall_value = 0;
	uint8_t flag_finded = 0;
	refreshHallValue(name, &hall_value);
	if (hall_value < 125) {
		return;
	}
	while (!flag_finded) {
		refreshHallValue(name, &hall_value);
		if (hall_value > 50) {
			switch(name) {
				case X: {
					setDirection(1);
					setStep(1000);
					break;
				}
				case Y: {
					setDirection(0);
					setStep(1000);
					break;
				}
				case Z: {
					//setDirection();
					//setStep(10);
					break;
				}
			}
		}
		else {
			setStep(0);
			while(!flag_finded) {
				refreshHallValue(name, &hall_value);
				if (hall_value < 50) {
					switch(name) {
						case X: {
							setDirection(0);
							setStep(1);
							break;
						}
						case Y: {
							setDirection(1);
							setStep(1);
							break;
						}
						case Z: {
							//setDirection();
							//setStep(10);
							break;
						}
					}
				}
				else {
					flag_finded = 1;
					setStep(0);
				}
			}
		}
	}
}

void Axis::setCoordinate(int32_t value) {
	cli();
	if (coordinate < value) {
		switch(name) {
			case X: {
				setDirection(0);
				break;
			}
			case Y: {
				setDirection(1);
				break;
			}
			case Z: {
				setDirection(1);
				break;
			}
		}
	}
	if (coordinate > value) {
		switch(name) {
			case X: {
				setDirection(1);
				break;
			}
			case Y: {
				setDirection(0);
				break;
			}
			case Z: {
				setDirection(0);
				break;
			}
		}
	}
	int32_t different = abs(value - coordinate);
	double tmp;
	if (different <= 5000) {
		tmp = round(different / 0.150);	
	}
	if ((different > 5000) && (different <= 10000)) {
		tmp = round(different / 0.152);
	}
	if ((different > 10000) && (different <= 15000)) {
		tmp = round(different / 0.153);
	}
	if ((different > 15000) && (different <= 20000)) {
		tmp = round(different / 0.154);
	}
	if ((different > 20000) && (different <= 25000)) {
		tmp = round(different / 0.155);
	}
	if ((different > 25000) && (different <= 30000)) {
		tmp = round(different / 0.156);
	}
	if ((different > 30000) && (different <= 35000)) {
		tmp = round(different / 0.157);
	}
// 	else {
// 		tmp = round(different / 0.159);
// 	}
	//printValue(different);
	
	//char buff[10];
	//dtostrf(tmp,6,9,buff);
	//uartTransmitString(buff);
	//uartTransmit('\n');
	uint64_t steps = (uint64_t)tmp;
	//printValue(steps);
	step = steps;
	//printValue(step);
	coordinate = value;
	//printValue(coordinate);
	sei();
}

void Axis::setStep(uint32_t value) {
	step = value*2;
}

void Axis::setDirection(uint8_t dir) {
	if (dir != direction) {
		direction_changed = 1;
		direction = dir;
	}
}

void Axis::setEnable(uint8_t en) {
	if (en != enable) {
		enable_changed = 1;
		enable = en;
	}
}

void Axis::release(void) {
	busy = 0;
}

uint8_t Axis::status(void) {
	return busy;
}

void Axis::setValueOfDivision(double newValOfDiv) {
	value_of_division = newValOfDiv;
}
