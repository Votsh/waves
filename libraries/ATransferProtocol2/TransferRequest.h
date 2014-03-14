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

#ifndef _TR_H_
#define _TR_H_

/*- Includes ---------------------------------------------------------------*/

#include <stdint.h>
#include "Arduino.h"
#include "StatusCodes.h"
#include "RadioDriver.h"

/*- Definitions ------------------------------------------------------------*/

/*- Types ------------------------------------------------------------------*/

typedef struct ATP_TransferRequest_t
{
  char			frameID[3];
  int			frameType;
  long			meshAddress;
  unsigned long long	datetime;
  long			atpCount;
  unsigned int 	version;

  unsigned long size;
  unsigned long expires;  
  char			descriptor[6];
  unsigned long source;
  unsigned long	destination;
  char			fileName[17];
} ATP_TransferRequest_t;

// Request type for AUID hash bits 0 and 1

typedef enum
{
  ATP_TRANSFER_REQUEST                    = 0x00,
  ATP_CHUNK__REQUEST                      = 0x01,
  ATP_CHUNK_RESPONSE                      = 0x02,	
} ATP_RequestType_t;

/*- Variables --------------------------------------------------------------*/
/*- Prototypes -------------------------------------------------------------*/

class TransferRequest
{
  public:
    TransferRequest();
	ATP_TransferRequest_t * getNewRequest();	// Return a new TransferRequest object
	void setHeaderDefaults( ATP_TransferRequest_t * );	// Sets dalues for TransferRequest
	void print( ATP_TransferRequest_t * );		// Prints TransferRequest object to logger
	void setRadioDriverType( char * );

	char * getFrameID( ATP_TransferRequest_t * );
	void setFrameID( ATP_TransferRequest_t *, char *);

	unsigned int getFrameType( ATP_TransferRequest_t * );
	void setFrameType( ATP_TransferRequest_t *, unsigned int);

	unsigned int getMeshAddress( ATP_TransferRequest_t * );
	void setMeshAddress( ATP_TransferRequest_t *, unsigned int);

	unsigned long long getDatetime( ATP_TransferRequest_t * );
	void setDatetime( ATP_TransferRequest_t *, unsigned long long);

	unsigned long getAtpCount( ATP_TransferRequest_t * );
	void setAtpCount( ATP_TransferRequest_t *, unsigned long);

	unsigned int getVersion( ATP_TransferRequest_t *);
	void setVersion( ATP_TransferRequest_t *, unsigned int);

	unsigned long getSize( ATP_TransferRequest_t * );
	void setSize( ATP_TransferRequest_t *, unsigned long);

	unsigned long getExpires( ATP_TransferRequest_t * );
	void setExpires( ATP_TransferRequest_t *, unsigned long);

	char * getDescriptor( ATP_TransferRequest_t * );
	void setDescriptor( ATP_TransferRequest_t *, char *);

	unsigned long getSource( ATP_TransferRequest_t * );
	void setSource( ATP_TransferRequest_t *, unsigned long);

	unsigned long getDestination( ATP_TransferRequest_t * );
	void setDestination( ATP_TransferRequest_t *, unsigned long);

	char * getFileName( ATP_TransferRequest_t * );
	void setFileName( ATP_TransferRequest_t *, char *);

	void sendIt( ATP_TransferRequest_t * );
	
};



#endif

