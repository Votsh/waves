/*

Scout Syslog LWM Message Driver

by Frank Cohen, fcohen@votsh.com

Provided by Votsh, http://www.votsh.com

Copyright (c) 2014 Frank Cohen. All rights reserved.
GPL v3 license:
This file is part of A Transfer Protocol (ATP)
ATP is free software: you can redistribute it and/or modify
it under the terms of the GNU General Public License as published by
the Free Software Foundation, either version 3 of the License, or
(at your option) any later version.
VOS is distributed in the hope that it will be useful,
but WITHOUT ANY WARRANTY; without even the implied warranty of
MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
GNU General Public License for more details.
You should have received a copy of the GNU General Public License
along with ATP.  If not, see <http://www.gnu.org/licenses/>.

*/

#ifndef _LWMDriver_H_
#define _LWMDriver_H_

#include "lwm/nwk/nwkDataReq.h"
#include "lwm/nwk/nwk.h"

#ifdef __cplusplus
extern "C"{
#endif

struct node {
  char * data;  
  struct node * next;
};
typedef struct node node_t;

node_t * root;
node_t * conductor;      
node_t * prevnode;

struct sendnode{
  struct sendnode * next;
  int stage;
  int status;
  unsigned long timeorigin;
  NWK_DataReq_t * req;
};

typedef struct sendnode sendnode_t;

sendnode_t * sendroot;
sendnode_t * sendconductor;      
sendnode_t * sendprevnode;   
sendnode_t * sendremove;   

#define lwmInit 0
#define lwmSent 1
#define lwmConfirmed 2
#define lwmError 3

#define lwmTimeout 10000

char * recentError;
int * recentErrorInt;

uint8_t cnt;
uint8_t cntsend;

unsigned long bytesused;
unsigned long bytesfreed;

static void sendConfirm( NWK_DataReq_t * req2 );

sendnode_t * LWMGetSendList();

int sendLWMMsg( char * data, int destination_mesh_id, unsigned long time );

uint8_t LWMRecCount();
uint8_t LWMSendCount();
unsigned long LWMSendBytesUsed();
unsigned long LWMSendBytesFreed();

void LWMSendMessages( unsigned long time );

void LWMPruneSendList( unsigned long time );

static bool receiveMessage(NWK_DataInd_t *ind);

char * printStatus( int status );

void LWMDriverInit();


uint8_t LWMgetsize1();
uint8_t LWMgetsize2();
uint8_t LWMgetsize3();
uint8_t size1;
uint8_t size2;
uint8_t size3;

uint8_t LWMgetQueueSize();

int sizeofme();

node_t * LWMGetRoot();
void LWMSetRoot(node_t *);
int LWMCount();

#ifdef __cplusplus
} // extern "C"
#endif

#endif

