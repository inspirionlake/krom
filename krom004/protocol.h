/*
 * protocol.h
 *
 * Created: 21.06.2018 18:42:10
 *  Author: Dmitry
 */ 
#ifndef PROTOCOL_H_
#define PROTOCOL_H_

#define	ECHO_MSG	2

void makeFrame(uint8_t *frame, uint8_t *number_of_bytes, uint8_t function_code, uint8_t *data, uint8_t number_of_data_bytes);

#endif /* PROTOCOL_H_ */