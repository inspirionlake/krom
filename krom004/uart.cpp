/*
 * uart.cpp
 *
 * Created: 06.06.2018 13:59:45
 *  Author: Dmitry
 */ 

#include "uart.h"
#include <stdlib.h>

//driver
uint8_t uart_rcv_buffer[20] = { 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0};
uint8_t u_buf_rcv_cur_pos = 0;
uint8_t u_buf_rcv_over = 0;

uint8_t uart_trm_buffer[20] = { 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0};
uint8_t u_buf_trm_cur_pos = 0;
uint8_t u_buf_trm_over = 0;

void uartInit(/*unsigned int m_uiUbrr*/void) {
	unsigned int m_uiUbrr = MYUBRR;
	UBRR0H = (unsigned char)(m_uiUbrr>>8);						//	set baud rate
	UBRR0L = (unsigned char)m_uiUbrr;
	
	UCSR0B = (1<<RXEN0) | (1<<TXEN0);							//	enable received and transmit
	UCSR0B |= (1<<RXCIE0);										//	enable interrupt when receive complete
	
	UCSR0C = (1<<USBS0) | (3<<UCSZ00);							//	set frame format 8data, 2stop bit
}

void uartTransmit(uint8_t m_ucMsg) {
	while( !(UCSR0A & (1<<UDRE0)) ) {							//	wait for empty transmit buffer
		;
	}
	UDR0 = m_ucMsg;												//	put msg into buffer, sends the msg
}

uint8_t uartReceive(void) {
	while ( !UCSR0A & (1<<RXC0));
	return UDR0;
}

void uartReceiveInInterrupt(void) {
	if (u_buf_rcv_cur_pos >= 20) {
		u_buf_rcv_over = 1;
		//u_buf_rcv_cur_pos = 0;
		return;	//	if receive buffer overload then all receiving bytes will be missed
	}
	uart_rcv_buffer[u_buf_rcv_cur_pos] = UDR0;
	u_buf_rcv_cur_pos++;
}

void uartTransmitInInterrupt(void) {
	if (u_buf_trm_cur_pos > 0) {
		UDR0 = uart_trm_buffer[u_buf_trm_cur_pos];
		--u_buf_trm_cur_pos;
	}
}

uint8_t putInTrmBuf(uint8_t data_byte) {
	if (u_buf_trm_cur_pos >= 20) {
		return 1; //	transmit buffer overload
	}
	if (u_buf_trm_cur_pos == 0) {
		++u_buf_trm_cur_pos;
		uart_trm_buffer[u_buf_trm_cur_pos] = data_byte;
		return 0;	//	data byte was added in transmit buffer;
	}	
	++u_buf_trm_cur_pos;
	uart_trm_buffer[u_buf_trm_cur_pos] = data_byte;
	return 0;	//	data byte was added in transmit buffer
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