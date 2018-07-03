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
#include <stdlib.h>

#include "uart.h"
#include "protocol.h"

enum axis_name {X = 0, Y, Z};

class Axis {
	private:
	int32_t coordinate;
	uint8_t microstep;
	uint8_t name;		// 0 - x, 1 - y, 2 - z
	uint8_t busy;
	double value_of_division;	
	public:
	uint8_t direction;
	uint8_t enable;
	uint32_t step;		//	example: 1 step = 10um
	Axis(uint8_t nm, uint8_t en, uint8_t dir, uint16_t stp, uint8_t microstp, double val_of_div) {
		step = stp;
		microstep = microstp;
		name = nm;
		enable = en;
		direction = dir;
		value_of_division = val_of_div;
		busy = 0;
	}
	int32_t getCoordinate(void) {
		return coordinate;
	}
	void findHome(void) {
		//finding home...
		coordinate = 0;
	}
	void setCoordinate(int32_t value) {
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
	void release() {
		busy = 0;
	}
	uint8_t status(void) {
		return busy;
	}
	protected:
};

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

ISR(USART_UDRE_vect) {
	uartTransmitInInterrupt();
}

ISR(USART_TX_vect) {
	uartTransmitInInterrupt();
}

int main(void)
{
	uartInit();
	portsInit();
	
	char version[11] = "Krom v004\n";
	uartTransmitString(version);
	
	timerInit();
	axis_x.findHome();
	axis_y.findHome();
	axis_z.findHome();
	
	//test of make&send frame
	uint8_t frame[20];

	uint8_t function_code = 5;
	uint8_t number_of_data_bytes = 2;
	uint8_t data[number_of_data_bytes] = {1, 4};
	uint8_t number_of_bytes;
	makeFrame(frame, &number_of_bytes, function_code, data, number_of_data_bytes);
	sendFrame(frame, number_of_bytes);
	
	for (uint32_t i = 0; i < 2000000; i++) {
		;
	}
	
	while (sendFrame(frame, number_of_bytes) != 0) {
		;
	}
	
	for (uint32_t i = 0; i < 2000000; i++) {
		;
	}
	
	
	while (sendFrame(frame, number_of_bytes) != 0) {
		;
	}
	
	_delay_ms(1000);
	
	while (sendFrame(frame, number_of_bytes) != 0) {
		;
	}
	//end of test make&send frame
    while (1) 
    {
		//_delay_ms(1000);
		//uartTransmit('\n');
// 		axis_x.setCoordinate(760);
// 		_delay_ms(5000);
// 		axis_x.setCoordinate(0);
// 		_delay_ms(5000);
	}
}