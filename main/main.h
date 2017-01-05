#ifndef INCLUDE_MAIN_H_
#define INCLUDE_MAIN_H_

#include <stdio.h>
#include <stdlib.h>
#include <pthread.h>
#include <sys/time.h>
#include "fila.h"
#include "serial.h"
#include "xbee.h"
#include "cod.h"
#include "arctl.h"
#include "db.h"

//Definies

//#define DEBUG_INIT
//#define DEBUG_SER_BUF		0
#define DEBUG_XBEE_PACK
//#define DEBUG_POST
//#define DEBUG_SEND
//#define DEBUG_XBEE_INIT
#define DEBUG_ARCTL_INIT

#define START 	1
#define END		2

//variaveis globais
typedef struct{
	TTFila FilaPacket;
	TTFila FilaPost;
	TTFila FilaAtu;
	}thread_args;

xbee_t xbee;
//serial
int fd;
//uint8_t xbee_broadcast[]={0,0,0,0,0,0,0xFF,0xFF};
//pthread_mutex_t lock_Atu = PTHREAD_MUTEX_INITIALIZER;
//pthread_mutex_t lock_Post = PTHREAD_MUTEX_INITIALIZER;
//pthread_mutex_t lock_Packet = PTHREAD_MUTEX_INITIALIZER;

long timeout(struct timeval start);


#endif
