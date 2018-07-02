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

// driver
extern uint8_t uart_rcv_buffer[20];
extern uint8_t u_buf_rcv_cur_pos;
extern uint8_t u_buf_rcv_over;

extern uint8_t uart_trm_buffer[20];
extern uint8_t u_buf_trm_cur_pos;
extern uint8_t u_buf_trm_over;

void uartInit(void);
void uartTransmit(uint8_t data);
void uartTransmitInInterrupt(void);
uint8_t uartReceive(void);
void uartReceiveInInterrupt(void);
uint8_t putInTrmBuf(uint8_t data_byte);		//	return 0 if all ok, 1 if transmit buffer overload
uint8_t uartStartTrmInInterrupt(void);		//	return 0 if uart data register empty and 1 if data register is not empty
// driver

void uartTransmitString(char *m_cpStr);
void uartPrintValue(int value);
void uartTransmitFrame(uint8_t frameLength, uint8_t *frame);

#endif /* UART_H_ */