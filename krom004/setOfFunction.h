/*
 * setOfFunction.h
 *
 * Created: 06.07.2018 15:14:17
 *  Author: Dmitry
 */ 

#include "positioning.h"

#ifndef SETOFFUNCTION_H_
#define SETOFFUNCTION_H_

extern uint8_t function_executing_flag;
extern uint8_t finish_all_steps_flag;

// function's names
#define ECHO	1
#define REPEAT	15
#define OK		16
#define GET_COORDINATE_XY	2	
#define GET_COORDINATE_Z	3
#define SET_STEPS_XY	11
#define SET_COORDINATE_Z	12
#define	SET_ENGINE			4
#define SET_VAL_DIV_X		5
#define SET_VAL_DIV_Y		6
#define SET_VAL_DIV_Z		7
#define GO_HOME				13
#define ANSW_EXE_COM		17
#define ANSW_FIN_COM		18
#define SET_STP_X_UP		19
#define SET_STP_X_DOWN		20
#define SET_STP_Y_UP		21
#define SET_STP_Y_DOWN		22
#define SET_STP_Z_UP		23
#define SET_STP_Z_DOWN		24
#define EMERGENCY_STOP		25
#define ERROR_CRC			26

//function's names end

void functionEcho(void);
void functionOK(void);
void functionRepeat(void);
void functionErrorCRC(void);
void functionGetCoordinateXY(Axis *ax_x, Axis *ax_y);
void functionGetCoordinateZ(Axis *ax_z);
void functionSetStepsXY(Axis *ax_x, Axis *ax_y, uint8_t *data, uint8_t number_of_data_byte);
void functionSetCoordinateZ(Axis *ax_z, uint8_t *data, uint8_t number_of_data_byte);
void functionSetEngine(Axis *ax_x, Axis *ax_y, Axis *ax_z, uint8_t *data, uint8_t number_of_data_byte);
void answerFinishCommand(uint8_t function_code);
void functionSetValueOfDivisionX(Axis *ax_x, uint8_t  *data, uint8_t number_of_data_byte);
void functionSetValueOfDivisionY(Axis *ax_y, uint8_t  *data, uint8_t number_of_data_byte);
void functionSetValueOfDivisionZ(Axis *ax_z, uint8_t  *data, uint8_t number_of_data_byte);
void functionGoHome(Axis *ax_x, Axis *ax_y, Axis *ax_z);
void functionSetStpUpX(Axis *ax_x);
void functionSetStpDownX(Axis *ax_x);
void functionSetStpUpY(Axis *ax_y);
void functionSetStpDownY(Axis *ax_y);
void functionSetStpUpZ(Axis *ax_z);
void functionSetStpDownZ(Axis *ax_z);
void functionEmergencyStop(Axis *ax_x, Axis *ax_y, Axis *ax_z);

#endif /* SETOFFUNCTION_H_ */