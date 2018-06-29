/*
 * uart.cpp
 *
 * Created: 06.06.2018 13:59:45
 *  Author: Dmitry
 */ 

#include "uart.h"
#include <stdlib.h>

void uartInit(/*unsigned int m_uiUbrr*/void) {
	unsigned int m_uiUbrr = MYUBRR;
	UBRR0H = (unsigned char)(m_uiUbrr>>8);						//	set baud rate
	UBRR0L = (unsigned char)m_uiUbrr;
	
	UCSR0B = (1<<RXEN0) | (1<<TXEN0);							//	enable received and transmit
	UCSR0B |= (1<<RXCIE0);										//	enable interrupt when receive complete
	
	UCSR0C = (1<<USBS0) | (3<<UCSZ00);							//	set frame format 8data, 2stop bit
}

void uartTransmit(unsigned char m_ucMsg) {
	while( !(UCSR0A & (1<<UDRE0)) ) {							//	wait for empty transmit buffer
		;
	}
	UDR0 = m_ucMsg;												//	put msg into buffer, sends the msg
}

void uartTransmitString(char *m_cpStr) {
	unsigned char i = 0;
	while (m_cpStr[i] != '\0') {
		uartTransmit(m_cpStr[i]);
		i++;
	}
}

unsigned char uartReceive(void) {
	while ( !UCSR0A & (1<<RXC0));
	return UDR0;
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