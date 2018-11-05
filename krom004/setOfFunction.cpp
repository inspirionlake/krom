/*
 * setOfFunction.cpp
 *
 * Created: 06.07.2018 15:14:49
 *  Author: Dmitry
 */ 
#include "setOfFunction.h"
#include "uart.h"
#include "protocol.h"
#include <avr/interrupt.h>

uint8_t function_executing_flag = 0;
uint8_t finish_all_steps_flag = 0;

void answerExecuteCommand(uint8_t function_code) {	//	notified the master of the execution command <function_code>
	uint8_t frame_ExecuteCommand[6];
	uint8_t number_of_bytes_ExecuteCommand;
	uint8_t data_ExecuteCommand[1];
	uint8_t number_of_data_bytes_ExecuteCommand = 1;
	
	data_ExecuteCommand[0] = function_code;
	
	makeFrame(frame_ExecuteCommand, &number_of_bytes_ExecuteCommand, ANSW_EXE_COM, data_ExecuteCommand, number_of_data_bytes_ExecuteCommand);
	sendFrame(frame_ExecuteCommand, number_of_bytes_ExecuteCommand);
}

void answerFinishCommand(uint8_t function_code) {
	uint8_t frame_FinishCommand[6];
	uint8_t number_of_bytes_FinishCommand;
	uint8_t data_FinishCommand[1];
	uint8_t number_of_data_bytes_FinishCommand = 1;
	
	data_FinishCommand[0] = function_code;
	
	makeFrame(frame_FinishCommand, &number_of_bytes_FinishCommand, ANSW_FIN_COM, data_FinishCommand, number_of_data_bytes_FinishCommand);
	sendFrame(frame_FinishCommand, number_of_bytes_FinishCommand);
}

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

void functionGetCoordinateXY(Axis *ax_x, Axis *ax_y) {	//	check on hardware 15.07.18
	uint8_t frame_getCoordinateXY[13];
	uint8_t number_of_bytes_getCoordinateXY;
	uint8_t data_getCoordinateXY[8];
	uint8_t number_of_data_bytes_getCoordinateXY = 8;
		
	int32_t coordinate_x = ax_x->getCoordinate();
	int32_t coordinate_y = ax_y->getCoordinate();
	int8_t index = 0;
	int32_t mask = 255;
	for (index = 3; index >= 0; index--) {
		data_getCoordinateXY[index] = coordinate_x & mask;
		coordinate_x =  coordinate_x >> 8;
	}
	
	for (index = 7; index >= 4; index--) {
		data_getCoordinateXY[index] = coordinate_y & mask;
		coordinate_y = coordinate_y >> 8;
	}
	
	makeFrame(frame_getCoordinateXY, &number_of_bytes_getCoordinateXY, GET_COORDINATE_XY, data_getCoordinateXY, number_of_data_bytes_getCoordinateXY);
	sendFrame(frame_getCoordinateXY, number_of_bytes_getCoordinateXY);
}

void functionGetCoordinateZ(Axis *ax_z) {
	uint8_t frame_getCoordinateZ[9];
	uint8_t number_of_bytes_getCoordinateZ;
	uint8_t data_getCoordinateZ[4];
	uint8_t number_of_data_bytes_getCoordinateZ = 4;
	
	int32_t coordinate_z = ax_z->getCoordinate();
	int8_t index = 0;
	int32_t mask = 255;
	for (index = 3; index >= 0; index--) {
		data_getCoordinateZ[index] = coordinate_z & mask;
		coordinate_z =  coordinate_z >> 8;
	}
	
	makeFrame(frame_getCoordinateZ, &number_of_bytes_getCoordinateZ, GET_COORDINATE_Z, data_getCoordinateZ, number_of_data_bytes_getCoordinateZ);
	sendFrame(frame_getCoordinateZ, number_of_bytes_getCoordinateZ);
}

void functionSetStepsXY(Axis *ax_x, Axis *ax_y, uint8_t *data, uint8_t number_of_data_byte) {	
	cli();
	int32_t steps_x = 0;
	int32_t steps_y = 0;
	int8_t dir_x = 0;
	int8_t dir_y = 0;
	int8_t index = 0;
	
	dir_x = data[0];
	if (dir_x == 1) {
		dir_x = 0;
	}
	else {
		dir_x = 1;
	}
	for (index = 1; index < 4; index++) {
		steps_x = steps_x << 8;
		steps_x |= data[index];
	}
	
	dir_y = data[4];
	for (index = 5; index < 8; index++) {
		steps_y = steps_y << 8;
		steps_y |= data[index];
	}
	
	ax_x->setDirection(dir_x);
	ax_y->setDirection(dir_y);
	
	ax_x->setStep(steps_x);
	ax_y->setStep(steps_y);
	sei();
	//answerExecuteCommand(SET_COORDINATE_XY);	
} 

void functionSetCoordinateZ(Axis *ax_z, uint8_t *data, uint8_t number_of_data_byte) {
	int32_t coordinate_z = 0;
	int8_t index = 0;
	
	for (index = 0; index < 4; index++) {
		coordinate_z = coordinate_z << 8;
		coordinate_z |= data[index];
	}
	
	ax_z->setCoordinate(coordinate_z);
	
	answerExecuteCommand(SET_COORDINATE_Z);	
}

void functionSetEngine(Axis *ax_x, Axis *ax_y, Axis *ax_z, uint8_t *data, uint8_t number_of_data_byte) {	
	uint8_t engine_state = data[0];
	
	uint8_t mask = 0b11111110;
	ax_x->enable = mask & engine_state;
	
	mask = 0b11111101;
	ax_y->enable = mask & engine_state;
	
	mask = 0b11111011;
	ax_z->enable = mask & engine_state;
	
	answerExecuteCommand(SET_ENGINE);
}

void functionSetValueOfDivisionX(Axis *ax_x, uint8_t  *data, uint8_t number_of_data_byte) {	
	double newValOfDiv = data[0] * 0.00001;
	
	ax_x->setValueOfDivision(newValOfDiv);
	
	answerFinishCommand(SET_VAL_DIV_X);
}

void functionSetValueOfDivisionY(Axis *ax_y, uint8_t  *data, uint8_t number_of_data_byte) {	
	double newValOfDiv = data[0] * 0.00001;
	
	ax_y->setValueOfDivision(newValOfDiv);
	
	answerFinishCommand(SET_VAL_DIV_Y);
}

void functionSetValueOfDivisionZ(Axis *ax_z, uint8_t  *data, uint8_t number_of_data_byte) {	
	double newValOfDiv = data[0] * 0.00001;
	
	ax_z->setValueOfDivision(newValOfDiv);
	
	answerFinishCommand(SET_VAL_DIV_Z);
}

void functionGoHome(Axis *ax_x, Axis *ax_y, Axis *ax_z) {
	ax_x->findHome();
	//ax_y->findHome();
	//ax_z->findHome();
	answerFinishCommand(GO_HOME);
}

void functionSetStpUpX(Axis *ax_x) {
	cli();
	ax_x->setDirection(0);
	ax_x->setStep(6000);
	sei();
}

void functionSetStpDownX(Axis *ax_x) {
	cli();
	ax_x->setDirection(1);
	ax_x->setStep(6000);
	sei();
}

void functionSetStpUpY(Axis *ax_y) {
	cli();
	ax_y->setDirection(1);
	ax_y->setStep(6000);
	sei();
}

void functionSetStpDownY(Axis *ax_y) {
	cli();
	ax_y->setDirection(0);
	ax_y->setStep(6000);
	sei();
}

void functionSetStpUpZ(Axis *ax_z) {
	cli();
	ax_z->setDirection(1);
	ax_z->setStep(24000);
	sei();
}

void functionSetStpDownZ(Axis *ax_z) {
	cli();
	ax_z->setDirection(0);
	ax_z->setStep(24000);
	sei();
}

void functionEmergencyStop(Axis *ax_x, Axis *ax_y, Axis *ax_z) {
	TIMSK0 &= ~(1<<OCIE0A);		//	turn off timer interrupts
	cli();
	
	ax_x->setStep(0);
	ax_x->setDirection(1);
	
	ax_y->setStep(0);
	ax_y->setDirection(0);
	
	ax_z->setStep(0);
	ax_z->setDirection(1);
	
	TIMSK0 |= (1<<OCIE0A);		//	turn on timer interrupts
	sei();
}