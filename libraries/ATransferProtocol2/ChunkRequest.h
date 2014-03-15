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

#ifndef _CTR_H_
#define _CTR_H_


/*- Includes ---------------------------------------------------------------*/

#include <stdint.h>
#include "Arduino.h"
#include "StatusCodes.h"

/*- Definitions ------------------------------------------------------------*/


/*- Types ------------------------------------------------------------------*/

typedef struct ATP_ChunkRequest_t
{
  char			frameID[3];
  int			frameType;
  long			meshAddress;
  unsigned long long	datetime;
  long			atpCount;
  unsigned int 	version;

  unsigned long start;
  unsigned long length;
  unsigned int	transferTypes;  
} ATP_ChunkRequest_t;

/*- Variables --------------------------------------------------------------*/
/*- Prototypes -------------------------------------------------------------*/

class ChunkRequest
{
  public:
	ChunkRequest();
	
	ATP_ChunkRequest_t * getNewRequest();				// Return a new ChunkRequest object
	void setDefaults( ATP_ChunkRequest_t * );		// Sets values for ChunkRequest
	void print( ATP_ChunkRequest_t * );				// Prints ChunkRequest object to logger
	void setRadioDriverType( char * );
	void sendIt( ATP_ChunkRequest_t * );

	/* Getters and Setters for ChunkRequest struct */

	char * getFrameID( ATP_ChunkRequest_t * );
	void setFrameID( ATP_ChunkRequest_t *, char *);

	unsigned int getFrameType( ATP_ChunkRequest_t * );
	void setFrameType( ATP_ChunkRequest_t *, unsigned int);

	unsigned int getMeshAddress( ATP_ChunkRequest_t * );
	void setMeshAddress( ATP_ChunkRequest_t *, unsigned int);

	unsigned long long getDatetime( ATP_ChunkRequest_t * );
	void setDatetime( ATP_ChunkRequest_t *, unsigned long long);

	unsigned long getAtpCount( ATP_ChunkRequest_t * );
	void setAtpCount( ATP_ChunkRequest_t *, unsigned long);

	unsigned int getVersion( ATP_ChunkRequest_t *);
	void setVersion( ATP_ChunkRequest_t *, unsigned int);

	unsigned long getStart( ATP_ChunkRequest_t *);
	void setStart( ATP_ChunkRequest_t *, unsigned long);

	unsigned long getLength( ATP_ChunkRequest_t * );
	void setLength( ATP_ChunkRequest_t *, unsigned long);

	unsigned int getTransferTypes( ATP_ChunkRequest_t * );
	void setTransferTypes( ATP_ChunkRequest_t *, unsigned int);
};

#endif
