/*
 * protocol.h
 *
 * Created: 21.06.2018 18:42:10
 *  Author: Dmitry
 */ 
#ifndef PROTOCOL_H_
#define PROTOCOL_H_

const uint8_t start_code = 0b10101010;
const uint8_t end_code = 0b01010101;

extern uint8_t frame_buffer_state_trm;
extern uint8_t frame_buffer_state_rcv;

void makeFrame(uint8_t *frame, uint8_t *number_of_bytes, uint8_t function_code, uint8_t *data, uint8_t number_of_data_bytes);

uint8_t sendFrame(uint8_t *frame, uint8_t number_of_bytes);

uint8_t receiveFrame(uint8_t *buffer, uint8_t *frame);

uint8_t decodeFrame(uint8_t *frame, uint8_t *function_code, uint8_t *data, uint8_t *number_of_data_byte);

#endif /* PROTOCOL_H_ */