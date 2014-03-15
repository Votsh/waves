/*

A Transfer Protocol (ATP)

Easy and efficient file transfer between microcontrollers
over a mesh or point-to-point radio network.

by Frank Cohen, fcohen@votsh.com

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

#ifndef _RD_H_
#define _RD_H_


/*- Includes ---------------------------------------------------------------*/

#include "Arduino.h"
#include "StatusCodes.h"
#include "Logging.h"
#include <SoftwareSerial.h>
#include "TransferRequest.h"
#include "ChunkRequest.h"
#include "ChunkResponse.h"

/*- Definitions ------------------------------------------------------------*/
/*- Types ------------------------------------------------------------------*/
/*- Variables --------------------------------------------------------------*/
/*- Prototypes -------------------------------------------------------------*/

struct ATP_TransferRequest_t;		// Forward declaration
struct ATP_ChunkRequest_t;			// Forward declaration
struct ATP_ChunkResponse_t;			// Forward declaration

class RadioDriver
{
  public:
    RadioDriver();
    void serviceRadio();					// Called to service incoming data
	void Send( char * ); 		 	// Send data over the radio
	void SendTransferRequest( ATP_TransferRequest_t * );	// Send a Transfer Request
	void SendChunkRequest( ATP_ChunkRequest_t * );			// Send a ChunkRequest
	void SendChunkResponse( ATP_ChunkResponse_t * );		// Send a ChunkResponse
	char * getReceived(); 			// Get a pointer to the received data
	unsigned int getStatus();
	void setRadioType( char * );	// Sets type of driver: XBEE, LMW
};


#endif
