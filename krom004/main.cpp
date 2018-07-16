/*
 * krom004.c
 *
 * Created: 05.06.2018 18:15:08
 * Author : Dmitry
 */ 

//MOVEMENT
#define MAX_STEP_COUNTS_X 240000
#define MAX_STEP_COUNTS_Y 240000
#define MAX_STEP_COUNTS_Z 10000
//MOVEMENT

#define BAUDRATE 115200
#define F_CPU 16000000UL

#define X_PUL_D 2	//PORTD
#define	Y_PUL_D	3	//PORTD
#define Z_PUL_D	4	//PORTD

#define X_ENA_D	5	//PORTD
#define Y_ENA_D	6	//PORTD
#define Z_ENA_D	7	//PORTD

#define X_DIR_D	0	//PORTB
#define Y_DIR_D 1	//PORTB
#define Z_DIR_D	2	//PORTB

#define CLK 3		//PORTB
#define CLK_PUL 4	//PORTB

#include <avr/io.h>
#include <util/delay.h>
#include <avr/interrupt.h>

#include "uart.h"
#include "protocol.h"
#include "setOfFunction.h"
#include "positioning.h"

Axis axis_x(X, 0, 0, 10, 32, 0.00155);	//	uint8_t name, uint8_t en, uint8_t dir, uint16_t step, uint8_t microstep, value_of_division
Axis axis_y(Y, 0, 0, 10, 32, 0.00155);	//	uint8_t name, uint8_t en, uint8_t dir, uint16_t step, uint8_t microstep
Axis axis_z(Z, 0, 0, 10, 32, 0.00155);	//	uint8_t name, uint8_t en, uint8_t dir, uint16_t step, uint8_t microstep

void timerInit(void) {			//	timer for postprocessor refresh
	TCCR0A = 0;					//	normal port operations
	TCCR0A |= (1<<WGM01);		//	CTC mode
	TCCR0B |= (1<<CS01);		//	prescaller set as 8
	TIMSK0 |= (1<<OCIE0A);
	OCR0A = 73;					//	freq = 16MHz/(2*8*(1+73)) = 13.5kHz
	sei();
}

void portsInit(void) {
	DDRD |= (1<<X_ENA_D);
	DDRB |= (1<<X_DIR_D);
	DDRD |= (1<<X_PUL_D);
	
	DDRD |= (1<<Y_ENA_D);
	DDRB |= (1<<Y_DIR_D);
	DDRD |= (1<<Y_PUL_D);
	
	DDRD |= (1<<Z_ENA_D);
	DDRB |= (1<<Z_DIR_D);
	DDRD |= (1<<Z_PUL_D);
	
	DDRB |= (1<<CLK_PUL);
	DDRB |= (1<<CLK);
	
	//portb4 it is arduino 13 pin with led
	DDRB |= (1<<DDB5); // set as output
}

void doCLK(void) {
	PORTB |= (1<<CLK);
	_delay_us(2) ;
	PORTB &= ~(1<<CLK);
	_delay_us(2);
}

void doCLK_PUL(void) {
	if (axis_x.step > 0) {
		PORTD |= (1<<X_PUL_D);
		axis_x.step--;
	}
	else {
		PORTD &= ~(1<<X_PUL_D);
	}
	if (axis_y.step > 0) {
		PORTD |= (1<<Y_PUL_D);
		axis_y.step--;
	}
	else {
		PORTD &= ~(1<<Y_PUL_D);
	}
	if (axis_z.step > 0) {
		PORTD |= (1<<Z_PUL_D);
		axis_z.step--;
	}
	else {
		PORTD &= ~(1<<Z_PUL_D);
	}
	_delay_us(10);				//	maybe this string can be omitted?
	PORTB ^= (1<<CLK_PUL);
}

void refreshOutput() {
	//ENABLE CONFIG
	if (axis_x.enable) {
		PORTD |= (1<<X_ENA_D);
	}
	else {
		PORTD &= ~(1<<X_ENA_D);
	}
	if (axis_y.enable) {
		PORTD |= (1<<Y_ENA_D);
	}
	else {
		PORTD &= ~(1<<Y_ENA_D);
	}
	if (axis_z.enable) {
		PORTD |= (1<<Z_ENA_D);
	}
	else {
		PORTD &= ~(1<<Z_ENA_D);
	}
	
	//DIRECTION CONFIG
	if (axis_x.direction) {
		PORTB &= ~(1<<X_DIR_D);
	}
	else {
		PORTB |= (1<<X_DIR_D);
	}
	if (axis_y.direction) {
		PORTB &= ~(1<<Y_DIR_D);
	}
	else {
		PORTB |= (1<<Y_DIR_D);
	}
	if (axis_z.direction) {
		PORTB &= ~(1<<Z_DIR_D);
	}
	else {
		PORTB |= (1<<Z_DIR_D);
	}
}
	
ISR(TIMER0_COMPA_vect) {	//	interrupt service where postprocessor will be refresh
	refreshOutput();		//	here execute function which refresh outputs on MCU for EN&DIR
	doCLK();				//	data set on driver's outputs EN_DIR (MCU send his commands on postprocessor)
	doCLK_PUL();			//	signal to make step, if it necessary
}


ISR(USART_RX_vect) {		//	when uart received byte. It put byte to uart_buffer
	uartReceiveInInterrupt();
}

ISR(USART_TX_vect) {
	uartTransmitInInterrupt();
}

int main(void)
{
	uartInit();
	portsInit();
	
	timerInit();	
	axis_x.findHome();
	axis_y.findHome();
	axis_z.findHome();
	
	uint8_t rcv_frame[20];
	uint8_t data[20];
	
	uint8_t number_of_data_byte = 0;
	uint8_t function_code = 0;
	
	
	uint8_t trm_frame[20];
	uint8_t number_of_bytes;
	
	uint8_t function_executing_flag = 0;
	uint8_t flag = 0;
	
    while (1) 
    {		
		if (rx_flag) {
			receiveFrame(uart_rcv_buffer, rcv_frame);
			decodeFrame(rcv_frame, &function_code, data, &number_of_data_byte);
			flag = 0;
		}
		
		for (uint32_t i = 0; i < 20000; i++) {
			;
		}
		
		if (flag) {
			flag = 0;
 			makeFrame(trm_frame, &number_of_bytes, function_code, data, number_of_data_byte);
			while (sendFrame(trm_frame, number_of_bytes) != 0) {
				;
			}
		}

		if (!function_executing_flag) {
			switch(function_code) {
				function_executing_flag = 1;
				case 0: {
					//
					break;
				}
				case ECHO: {
					functionEcho();
					function_code = 0;
					function_executing_flag = 0;
					break;
				}
				case REPEAT: {
					functionRepeat();
					function_code = 0;
					function_executing_flag = 0;
					break;
				}
				case GET_COORDINATE_XY: {
					functionGetCoordinateXY(&axis_x, &axis_y);
					function_code = 0;
					function_executing_flag = 0;
					break;
				}
				case GET_COORDINATE_Z: {
					functionGetCoordinateZ(&axis_z);
					function_code = 0;
					function_executing_flag = 0;
					break;
				}
				case SET_COORDINATE_XY: {
					functionSetCoordinateXY(&axis_x, &axis_y, data, number_of_data_byte);
					function_code = 0;
					function_executing_flag = 0;
					break;
				}
				case SET_COORDINATE_Z: {
					functionSetCoordinateZ(&axis_z, data, number_of_data_byte);
					function_code = 0;
					function_executing_flag = 0;
					break;
				}
				case SET_ENGINE: {
					functionSetEngine(&axis_x, &axis_y, &axis_z, data, number_of_data_byte);
					function_code = 0;
					function_executing_flag = 0;
					break;
				}
				case SET_VAL_DIV_X: {
					functionSetValueOfDivisionX(&axis_x, data, number_of_data_byte);
					function_code = 0;
					function_executing_flag = 0;
					break;
				}
				case SET_VAL_DIV_Y: {
					functionSetValueOfDivisionY(&axis_y, data, number_of_data_byte);
					function_code = 0;
					function_executing_flag = 0;
					break;
				}
				case SET_VAL_DIV_Z: {
					functionSetValueOfDivisionZ(&axis_z, data, number_of_data_byte);
					function_code = 0;
					function_executing_flag = 0;
					break;
				}
				case GO_HOME: {
					functionGoHome(&axis_x, &axis_y, &axis_z);
					function_code = 0;
					function_executing_flag = 0;
					break;
				}
				default: {
					functionRepeat();
					function_code = 0;
					function_executing_flag = 0;
					break;
					
				}
			}
		}
	}
}