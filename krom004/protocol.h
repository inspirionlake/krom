/*
 * protocol.h
 *
 * Created: 21.06.2018 18:42:10
 *  Author: Dmitry
 */ 
#ifndef PROTOCOL_H_
#define PROTOCOL_H_

#define	ECHO_MSG	2

extern uint8_t dataBuffer[5];

void checkBuffer(uint8_t *buf);

uint8_t *makeFrame(uint8_t function_code, uint8_t data_counts, uint8_t *data);

#endif /* PROTOCOL_H_ */