/*
 * protocol.cpp
 *
 * Created: 21.06.2018 18:45:41
 *  Author: Dmitry
 */ 

//protocol
#include <avr/io.h>
#include <avr/interrupt.h>

#include "uart.h"
#include "protocol.h"
	
/*
  Name  : CRC-8
  Poly  : 0x31    x^8 + x^5 + x^4 + 1
  Init  : 0xFF
  Revert: false
  XorOut: 0x00
  Check : 0xF7 ("123456789")
  MaxLen: 15
*/

const unsigned char Crc8Table[256] = {
    0x00, 0x31, 0x62, 0x53, 0xC4, 0xF5, 0xA6, 0x97,
    0xB9, 0x88, 0xDB, 0xEA, 0x7D, 0x4C, 0x1F, 0x2E,
    0x43, 0x72, 0x21, 0x10, 0x87, 0xB6, 0xE5, 0xD4,
    0xFA, 0xCB, 0x98, 0xA9, 0x3E, 0x0F, 0x5C, 0x6D,
    0x86, 0xB7, 0xE4, 0xD5, 0x42, 0x73, 0x20, 0x11,
    0x3F, 0x0E, 0x5D, 0x6C, 0xFB, 0xCA, 0x99, 0xA8,
    0xC5, 0xF4, 0xA7, 0x96, 0x01, 0x30, 0x63, 0x52,
    0x7C, 0x4D, 0x1E, 0x2F, 0xB8, 0x89, 0xDA, 0xEB,
    0x3D, 0x0C, 0x5F, 0x6E, 0xF9, 0xC8, 0x9B, 0xAA,
    0x84, 0xB5, 0xE6, 0xD7, 0x40, 0x71, 0x22, 0x13,
    0x7E, 0x4F, 0x1C, 0x2D, 0xBA, 0x8B, 0xD8, 0xE9,
    0xC7, 0xF6, 0xA5, 0x94, 0x03, 0x32, 0x61, 0x50,
    0xBB, 0x8A, 0xD9, 0xE8, 0x7F, 0x4E, 0x1D, 0x2C,
    0x02, 0x33, 0x60, 0x51, 0xC6, 0xF7, 0xA4, 0x95,
    0xF8, 0xC9, 0x9A, 0xAB, 0x3C, 0x0D, 0x5E, 0x6F,
    0x41, 0x70, 0x23, 0x12, 0x85, 0xB4, 0xE7, 0xD6,
    0x7A, 0x4B, 0x18, 0x29, 0xBE, 0x8F, 0xDC, 0xED,
    0xC3, 0xF2, 0xA1, 0x90, 0x07, 0x36, 0x65, 0x54,
    0x39, 0x08, 0x5B, 0x6A, 0xFD, 0xCC, 0x9F, 0xAE,
    0x80, 0xB1, 0xE2, 0xD3, 0x44, 0x75, 0x26, 0x17,
    0xFC, 0xCD, 0x9E, 0xAF, 0x38, 0x09, 0x5A, 0x6B,
    0x45, 0x74, 0x27, 0x16, 0x81, 0xB0, 0xE3, 0xD2,
    0xBF, 0x8E, 0xDD, 0xEC, 0x7B, 0x4A, 0x19, 0x28,
    0x06, 0x37, 0x64, 0x55, 0xC2, 0xF3, 0xA0, 0x91,
    0x47, 0x76, 0x25, 0x14, 0x83, 0xB2, 0xE1, 0xD0,
    0xFE, 0xCF, 0x9C, 0xAD, 0x3A, 0x0B, 0x58, 0x69,
    0x04, 0x35, 0x66, 0x57, 0xC0, 0xF1, 0xA2, 0x93,
    0xBD, 0x8C, 0xDF, 0xEE, 0x79, 0x48, 0x1B, 0x2A,
    0xC1, 0xF0, 0xA3, 0x92, 0x05, 0x34, 0x67, 0x56,
    0x78, 0x49, 0x1A, 0x2B, 0xBC, 0x8D, 0xDE, 0xEF,
    0x82, 0xB3, 0xE0, 0xD1, 0x46, 0x77, 0x24, 0x15,
    0x3B, 0x0A, 0x59, 0x68, 0xFF, 0xCE, 0x9D, 0xAC
};

uint8_t Crc8(uint8_t *pcBlock, uint8_t len)
{
    uint8_t crc = 0xFF;

    while (len--)
        crc = Crc8Table[crc ^ *pcBlock++];

    return crc;
}

uint8_t checkCRC(uint8_t rec_crc, uint8_t frameLength, uint8_t *frame) {
	uint8_t calc_crc = Crc8(frame, frameLength);
	if (calc_crc == rec_crc) {
		return 0;
	}
	else {
		return 1;
	}
}

void makeFrame(uint8_t *frame, uint8_t *number_of_bytes, uint8_t function_code_l, uint8_t *data, uint8_t number_of_data_bytes) {
	*number_of_bytes = 5 + number_of_data_bytes;	//	start_code(1) + function_code(1) + number_of_data_bytes + CRC(1) + end_code(1) + data = 5 + data
	uint8_t index = 0;
	frame[index] = start_code;
	
	++index;
	frame[index] = function_code_l;
	
	++index;
	frame[index] = number_of_data_bytes;
	
	uint8_t i = 0;
	for (i = 0; i < number_of_data_bytes; i++) {
		++index;
		frame[index] = data[i];
	}
	
	++index;
	frame[index] = end_code;
	
	uint8_t crc = Crc8(frame, *number_of_bytes - 1);
	frame[index] = crc;
	
	++index;
	frame[index] = end_code;
}

uint8_t frame_buffer_state_trm = 0;

uint8_t sendFrame(uint8_t *frame, uint8_t number_of_bytes) {
	uint8_t i = 0;
	if (frame_buffer_state_trm == 0) {
		frame_buffer_state_trm = 1;
		for (i = 0; i < number_of_bytes; i++) {
			if (putInTrmBuf(frame[i])) {
				return 1;	//	frame was loaded in buffer not completely
			}
		}
		uartStartTrmInInterrupt();
		return 0;	//	frame was loaded in buffer completely
	}
	return 2;	//	frame buffer is busy. try load later
}

uint8_t rx_flag = 0;
uint8_t receiveFrame(uint8_t *buffer, uint8_t *frame) {
	uint8_t i = 0;
	for (i = 0; i < MAX_BUFFER_SIZE; i++) {
		if (buffer[i] == start_code) {
			break;
		}
	}
	uint8_t j = i + 2;	//	set index on byte with value which describe number of data bytes
	uint8_t frame_length = 5 + buffer[j];
	
	for (j = 0; j < frame_length; j++) {	//	writing all frame in *frame
		frame[j] = buffer[i];
		++i;
	}
	u_buf_rcv_cur_pos = 0;
	rx_flag = 0;
	return 0;		//	all ok
}

uint8_t decodeFrame(uint8_t *frame, uint8_t *function_code, uint8_t *data, uint8_t *number_of_data_bytes) {
	uint8_t index = 0;
	if (frame[index] != start_code) {
		return 1;	//	frame is not correct
	}
	
	++index;	//	set index on function code
	*function_code = frame[index];
	
	++index;	//	set index on number of data bytes
	uint8_t j = 0;
	*number_of_data_bytes = frame[index];
	
	++index;	//	set index on first data byte
	for (j = 0; j < *number_of_data_bytes; j++) {
		data[j] = frame[index];
		++index;
	}
	
	//	now index on crc byte
	uint8_t crc = frame[index];
	
	frame[index] = end_code;	//	Calculating crc without crc byte, only: start code, function code, number of data bytes, data, end code
	
	if (checkCRC(crc, 4 + *number_of_data_bytes, frame) == 0) {
		return 1;	//	crc error
	}
	
	++index;
	if (frame[index] == end_code) {
		return 0;	//	frame is correct
	}
	else {
		return 1;	//	end_code of frame not found
	}
}


//protocol