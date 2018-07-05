/*
 * uart.cpp
 *
 * Created: 06.06.2018 13:59:45
 *  Author: Dmitry
 */ 

#include "uart.h"
#include <stdlib.h>
#include <avr/interrupt.h>
#include "protocol.h"

//driver

uint8_t uart_rcv_buffer[MAX_BUFFER_SIZE] = { 0, 1, 1, 1, 1, 1, 1, 1, 1, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0};
uint8_t u_buf_rcv_cur_pos = 0;
uint8_t u_buf_rcv_over = 0;

uint8_t uart_trm_buffer[MAX_BUFFER_SIZE] = { 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0};
uint8_t u_buf_trm_cur_pos = 0;
uint8_t u_buf_trm_over = 0;

void uartInit(/*unsigned int m_uiUbrr*/void) {
	unsigned int m_uiUbrr = MYUBRR;
	UBRR0H = (unsigned char)(m_uiUbrr>>8);						//	set baud rate
	UBRR0L = (unsigned char)m_uiUbrr;
	
	UCSR0B = (1<<RXEN0);										//	enable received and transmit
	UCSR0B |= (1<<RXCIE0);										//	enable receive complete interrupt
	
	UCSR0C = (1<<USBS0) | (3<<UCSZ00);							//	set frame format 8data, 2stop bit
}

void uartTransmit(uint8_t m_ucMsg) {
	while( !(UCSR0A & (1<<UDRE0)) ) {							//	wait for empty transmit buffer
		;
	}
	UDR0 = m_ucMsg;							//	put msg into buffer, sends the msg
}

uint8_t uartReceive(void) {
	while ( !UCSR0A & (1<<RXC0));
	return UDR0;
}

void uartReceiveInInterrupt(void) {
	if (u_buf_rcv_cur_pos >= 20) {
		u_buf_rcv_over = 1;
		u_buf_rcv_cur_pos = 0;
		return;	//	if receive buffer overload then all receiving bytes will be missed
	}
	uart_rcv_buffer[u_buf_rcv_cur_pos] = UDR0;
	u_buf_rcv_cur_pos++;
}

void uartTransmitInInterrupt(void) {
	UCSR0B &= ~(1<<UDRIE0);
	UCSR0B &= ~(1<<TXEN0);
	static uint8_t sending_pos = 0;
	if (sending_pos < u_buf_trm_cur_pos) {
		UDR0 = uart_trm_buffer[sending_pos];
		++sending_pos;
		UCSR0B |= (1<<TXCIE0);
		UCSR0B |= (1<<TXEN0);
	}
	else {
		sending_pos = 0;
		u_buf_trm_cur_pos = 0;
		frame_buffer_state_trm = 0;
		UCSR0B &= ~(1<<TXCIE0);
		UCSR0B &= ~(1<<TXEN0);
	}
}

uint8_t putInTrmBuf(uint8_t data_byte) {
	if (u_buf_trm_cur_pos >= MAX_BUFFER_SIZE) {
		return 1; //	transmit buffer overload
	}
	uart_trm_buffer[u_buf_trm_cur_pos] = data_byte;
	++u_buf_trm_cur_pos;
	return 0;	//	data byte was added in transmit buffer
}

uint8_t uartStartTrmInInterrupt(void) {
	if (UDRE0) {
		uartTransmitInInterrupt();
		return 0;		//	uart data register was empty, transmitting is starting
	}
	return 1;	//	uart data register is not empty
}
//	driver


void uartTransmitString(char *m_cpStr) {
	unsigned char i = 0;
	while (m_cpStr[i] != '\0') {
		uartTransmit(m_cpStr[i]);
		i++;
	}
}

void uartPrintValue(int value) {
	char buf[10];
	itoa(value,buf, 20);
	uartTransmitString(buf);
	uartTransmit('\n');
}

void uartTransmitFrame(uint8_t frameLength, uint8_t *frame) {
	for (uint8_t i = 0; i < frameLength; i++) {
		uartTransmit(frame[i]);
	}
}