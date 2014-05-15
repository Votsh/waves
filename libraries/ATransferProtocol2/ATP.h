/*

A Transfer Protocol (ATP)

Easy and efficient file transfer between microcontrollers
over a mesh or point-to-point radio network.

ATP specification document is at:
https://github.com/Votsh/waves/tree/master/ATransferProtocol/ATransferProtocolSpecV1.html

Github repository for ATP:
https://github.com/Votsh/waves/tree/master/ATransferProtocol

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

#ifndef _ATP_H_
#define _ATP_H_


/*- Includes ---------------------------------------------------------------*/

#include "Arduino.h"
#include "StatusCodes.h"
#include "RadioDriver.h"
#include "TransferRequest.h"
#include "ChunkRequest.h"
#include "ChunkResponse.h"
#include "Logging.h"
#include <SD.h>

/*- Definitions ------------------------------------------------------------*/

#define LOGLEVEL LOG_LEVEL_DEBUG
#define baudrate 9600
#define CHUNKSIZE 2000	// Default chunk size
#define TIMEOUTVAL 30	// 30 seconds, presuming the main sketch loop repeats ever 1000 milliseconds

// Linked List definition

typedef struct ATP_linkNode {
  void *transfer;
  struct ATP_linkNode *next;
} ATP_linkNode;

class ATP
{
  public:
    ATP( void );
    void begin( char * );
    void print(void);
    void garbageCollection(void);
    void initiateTransferRequest(void);
    void serviceRequests(void);
    void sendRequests(void);
	ATP_linkNode * addNode( unsigned long );
	ATP_linkNode * findNode( unsigned long );
  private:
};

#endif
