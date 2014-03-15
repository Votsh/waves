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

#ifndef _CR_H_
#define _CR_H_


/*- Includes ---------------------------------------------------------------*/

#include <stdint.h>
#include "Arduino.h"
#include "StatusCodes.h"
#include "RadioDriver.h"

/*- Definitions ------------------------------------------------------------*/

/*- Types ------------------------------------------------------------------*/

typedef struct ATP_ChunkResponse_t
{
  char			frameID[3];
  int			frameType;
  long			meshAddress;
  unsigned long long	datetime;
  long			atpCount;
  unsigned int 	version;

  unsigned int  	status;
  unsigned int * 	start;
  unsigned long 	length;
  unsigned int		transferTypes;
  unsigned int		verify;
  unsigned int * 	data;
} ATP_ChunkResponse_t;

/*- Variables --------------------------------------------------------------*/
/*- Prototypes -------------------------------------------------------------*/

class ChunkResponse
{
  public:
	ChunkResponse();
	
	ATP_ChunkResponse_t * getNewRequest();				// Return a new ChunkResponse object
	void setDefaults( ATP_ChunkResponse_t * );		// Sets values for ChunkResponse
	void print( ATP_ChunkResponse_t * );				// Prints ChunkResponse object to logger
	void setRadioDriverType( char * );
	void sendIt( ATP_ChunkResponse_t * );

	/* Getters and Setters for ChunkResponse struct */

	char * getFrameID( ATP_ChunkResponse_t * );
	void setFrameID( ATP_ChunkResponse_t *, char *);

	unsigned int getFrameType( ATP_ChunkResponse_t * );
	void setFrameType( ATP_ChunkResponse_t *, unsigned int);

	unsigned int getMeshAddress( ATP_ChunkResponse_t * );
	void setMeshAddress( ATP_ChunkResponse_t *, unsigned int);

	unsigned long long getDatetime( ATP_ChunkResponse_t * );
	void setDatetime( ATP_ChunkResponse_t *, unsigned long long);

	unsigned long getAtpCount( ATP_ChunkResponse_t * );
	void setAtpCount( ATP_ChunkResponse_t *, unsigned long);

	unsigned int getVersion( ATP_ChunkResponse_t *);
	void setVersion( ATP_ChunkResponse_t *, unsigned int);

	unsigned int getStatus( ATP_ChunkResponse_t * );
	void setStatus( ATP_ChunkResponse_t *, unsigned int);

	unsigned int * getStart( ATP_ChunkResponse_t * );
	void setStart( ATP_ChunkResponse_t *, unsigned int *);

	unsigned long getLength( ATP_ChunkResponse_t * );
	void setLength( ATP_ChunkResponse_t *, unsigned long);

	unsigned int getTransferTypes( ATP_ChunkResponse_t * );
	void setTransferTypes( ATP_ChunkResponse_t *, unsigned int);

	unsigned int getVerify( ATP_ChunkResponse_t * );
	void setVerify( ATP_ChunkResponse_t *, unsigned int);

	unsigned int * getData( ATP_ChunkResponse_t * );
	void setData( ATP_ChunkResponse_t *, unsigned int *);
};

#endif
