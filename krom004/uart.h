/*
 * uart.h
 *
 * Created: 06.06.2018 13:48:01
 *  Author: Dmitry
 */ 


#ifndef UART_H_
#define UART_H_

#include <avr/io.h>

#ifndef FOSC
#define FOSC 16000000UL
#endif

#ifndef BAUD
#define BAUD 9600
#endif

#define MYUBRR FOSC/16/BAUD-1

void uartInit(void);
void uartTransmit(unsigned char data);
void uartTransmitString(char *m_cpStr);
unsigned char uartReceive(void);

void uartPrintValue(int value);

void uartTransmitFrame(uint8_t frameLength, uint8_t *frame);

#endif /* UART_H_ */