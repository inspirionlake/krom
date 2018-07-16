/*
 * positioning.h
 *
 * Created: 14.07.2018 2:45:11
 *  Author: Dmitry
 */ 
#ifndef POSITIONING_H_
#define POSITIONING_H_

#include <stdint.h>

enum axis_name {X = 0, Y, Z};
	
class Axis {
	private:
	int32_t coordinate;
	uint8_t microstep;
	uint8_t name;
	uint8_t busy;
	double value_of_division;
	
	public:
	uint8_t direction;
	uint8_t enable;
	uint32_t step;		//	example: 1 step = 10um
	
	Axis(uint8_t nm, uint8_t en, uint8_t dir, uint16_t stp, uint8_t microstp, double val_of_div);
	
	int32_t getCoordinate(void);
	
	void findHome(void);
	
	void setCoordinate(int32_t value);
	
	void release(void);
	
	uint8_t status(void);
	
	void setValueOfDivision(double newValOfDiv);
	};

#endif /* POSITIONING_H_ */