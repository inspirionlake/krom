/*
 * setOfFunction.cpp
 *
 * Created: 06.07.2018 15:14:49
 *  Author: Dmitry
 */ 
#include "setOfFunction.h"
#include "uart.h"
#include "protocol.h"

class Axis;

void functionEcho(void) {
	uint8_t frame_echo[5];
	uint8_t number_of_bytes_echo;
	uint8_t data_echo[1];
	uint8_t number_of_data_bytes_echo = 0;
	makeFrame(frame_echo, &number_of_bytes_echo, 255, data_echo, number_of_data_bytes_echo);
	sendFrame(frame_echo, number_of_bytes_echo);
}

void functionOK(void) {
	uint8_t frame_ok[5];
	uint8_t number_of_bytes_ok;
	uint8_t data_ok[1];
	uint8_t number_of_data_bytes_ok = 0;
	makeFrame(frame_ok, &number_of_bytes_ok, OK, data_ok, number_of_data_bytes_ok);
	sendFrame(frame_ok, number_of_bytes_ok);
}

void functionRepeat(void) {
	uint8_t frame_repeat[5];
	uint8_t number_of_bytes_repeat;
	uint8_t data_repeat[1];
	uint8_t number_of_data_bytes_repeat = 0;
	// HERE program must repeat previously frame...
	makeFrame(frame_repeat, &number_of_bytes_repeat, REPEAT, data_repeat, number_of_data_bytes_repeat);
	sendFrame(frame_repeat, number_of_bytes_repeat);
}

void functionGetCoordinateXY(/*Axis *ax_x, Axis *ax_y*/) {	//	check on hardware 15.07.18
	uint8_t frame_getCoordinateXY[13];
	uint8_t number_of_bytes_getCoordinateXY;
	uint8_t data_getCoordinateXY[8];
	uint8_t number_of_data_bytes_getCoordinateXY = 8;
		
	// You should put coordinates of axises into data_getCoordinateXY... 
// 	int32_t coordinate_x = ax_x->getCoordinate();
// 	int32_t coordinate_y = ax_y->getCoordinate();
// 	
// 	uint8_t index = 0;
// 	int32_t mask = 255;
// 	for (index = 3; index >= 0; index--) {
// 		data_getCoordinateXY[index] = coordinate_x | mask;
// 		coordinate_x =  coordinate_x >> 8;
// 	}
// 	
// 	for (index = 7; index >= 4; index--) {
// 		data_getCoordinateXY[index] = coordinate_y | mask;
// 		coordinate_y = coordinate_y >> 8;
// 	}
	
	makeFrame(frame_getCoordinateXY, &number_of_bytes_getCoordinateXY, GET_COORDINATE_XY, data_getCoordinateXY, number_of_data_bytes_getCoordinateXY);
	sendFrame(frame_getCoordinateXY, number_of_bytes_getCoordinateXY);
}