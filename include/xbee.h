/*
 * xbee.h
 *
 *  Created on: 14/08/2015
 *      Author: leandro
 */

#ifndef XBEE_H_
#define XBEE_H_
#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <sys/types.h>
#include <inttypes.h>
#include <sys/time.h>

#define XBEE_RECEIVE_PACKET 	0x90
#define XBEE_TRANSMIT_REQUEST	0x10
#define XBEE_CMD				0x08
#define XBEE_CMDAT				0x88
#define XBEE_CMDATRT			0x17
#define XBEE_CMDATRR			0x97
#define XBEE_STATUS				0x8B
#define XBEE_PAYLOAD_OFFSET		0x0F
#define	XBEE_STATUS_SUCESS		0x08
#define XBEE_OFFSET_SOURCE		0x04

typedef union{
	float val;
	struct{
		char b[4];
    	}byte;
}floatbyte_T;

typedef union{
	uint16_t val;
	struct{
		uint8_t b[2];
	}byte;
}uint16byte_T;
typedef struct{
	floatbyte_T var;
	uint8_t type;
}var_T;

typedef struct{
	int addr[20][8];
	int qtd;
	struct timeval start;
	int flag;
}discoverT;

typedef struct{
	uint8_t type;

	//string addr
	uint8_t myaddr[20];
	//vetor addr
	uint8_t myaddres[8];

	uint8_t source_Address[8];
	uint8_t payload[200];
	uint8_t buf[200];
	uint8_t TamBuf;
	uint8_t size;
	uint8_t flag_myaddr;
	uint8_t id;
	discoverT disc;
}xbee_t;

typedef union{
	uint32_t val;
	struct{
		char b[4];
    	}byte;
}uint32_byte_T;

//uint8_t xbee_broadcast[]={0,0,0,0,0,0,0xFF,0xFF};

void xbee_reciver(xbee_t *xbee);
uint8_t xbee_SendData(int fd,xbee_t *xbee,uint8_t *address,uint8_t *data,uint8_t size);
void xbee_PedRSSI(int fd,xbee_t *xbee,uint8_t *addr);
void xbee_cmdAT(int fd,xbee_t *xbee,uint8_t *str);
void xbee_cmdATR(int fd,xbee_t *xbee,uint8_t *str,uint8_t *addr);
void xbee_addrstr(uint8_t *addr,uint8_t *str);
#endif /* XBEE_H_ */
