/*
 * hall_handler.h
 *
 * Created: 16.07.2018 18:35:06
 *  Author: Dmitry
 */ 
#include <stdint.h>
#include "positioning.h"

#ifndef HALL_HANDLER_H_
#define HALL_HANDLER_H_

void initAdc(void);
void refreshHallValue(uint8_t axis, uint8_t *hall_val);


#endif /* HALL_HANDLER_H_ */