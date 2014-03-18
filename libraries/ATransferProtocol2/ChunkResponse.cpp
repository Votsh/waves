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

/*

ChuckResponse provides a "pull" from the lead peer to a peer, in response
to the peer receiving a TransferRequest. On the receiving peer, the
methods provide the App with malloc-style access to the transferred data.

See the ChunkResponse.h file for definitions of the structs used by the methods.

*/

/*- Includes ---------------------------------------------------------------*/
#include "RadioDriver.h"
#include "ChunkResponse.h"

/*- Implementations --------------------------------------------------------*/

/*
\brief Constructor
*/

ChunkResponse::ChunkResponse(){}

/*
\brief Return a new ChunkResponse object
*/

ATP_ChunkResponse_t * ChunkResponse::getNewRequest( int id )
{
	ATP_ChunkResponse_t * acr = (ATP_ChunkResponse_t *) malloc( sizeof(ATP_ChunkResponse_t) );
	//todo: Test for a null response from malloc and handle as exception
	setDefaults( acr, id );
	return acr;
}

/*
\brief Sets header values for ChunkResponse
*/

void ChunkResponse::setDefaults( ATP_ChunkResponse_t * header, int id){
	
	setFrameID( header, "ATP" );
	setFrameType( header, ATP_CHUNK_RESPONSE );
	setMeshAddress( header, 0 );			// Pan address for XBee radios
	setDatetime( header, millis() );	//todo: Replace this with the Time library or when you get a Real Time Clock
	setAtpID( header, id );
	setVersion( header, 1 );	
	setStatus( header, ATP_IDLE );
	
	setStart( header, 0 );
	setLength( header, 1);	
	setTransferTypes( header, 0 );
	setVerify( header, 0 );
	setData( header, (unsigned int * ) "12345678" );
}
/*
\brief Prints ATP_ChunkResponse_t type to the logger for debugging
*/
void ChunkResponse::print( ATP_ChunkResponse_t * frame ){
	Log.Debug("ChunkResponse object:"CR);
	Log.Debug("frameID:       %s"CR, getFrameID(frame));
	Log.Debug("frameType:     %d"CR, getFrameType(frame));	
	Log.Debug("meshAddress:   %d"CR, getMeshAddress(frame));	
	Log.Debug("datetime:      %d"CR, getDatetime(frame));	
	Log.Debug("atpCount:      %d"CR, getAtpID(frame)); 	
	Log.Debug("version:       %d"CR, getVersion(frame));
	Log.Debug("status:  	  %d"CR, getStatus(frame));
	
	Log.Debug("start:         %d"CR, getStart(frame));	
	Log.Debug("length:        %s"CR, getLength(frame));	
	Log.Debug("transferTypes: %d"CR, getTransferTypes(frame));	
	Log.Debug("verify:        %d"CR, getVerify(frame));	
	Log.Debug("data:          %s"CR, getData(frame));
}
/*
\brief Getters and Setters
*/

char * ChunkResponse::getFrameID( ATP_ChunkResponse_t * frame ){ return frame->frameID; }
void ChunkResponse::setFrameID( ATP_ChunkResponse_t * frame, char * id)
{	frame->frameID[0]=id[0];
	frame->frameID[1]=id[1];
	frame->frameID[2]=id[2];
}

unsigned int ChunkResponse::getFrameType( ATP_ChunkResponse_t * frame ){ return frame->frameType; }
void ChunkResponse::setFrameType( ATP_ChunkResponse_t * frame, unsigned int mytype){ frame->frameType = mytype; }

unsigned int ChunkResponse::getMeshAddress( ATP_ChunkResponse_t * frame ){ return frame->meshAddress; }
void ChunkResponse::setMeshAddress( ATP_ChunkResponse_t * frame, unsigned int mymesh){ frame->meshAddress = mymesh; }

unsigned long long ChunkResponse::getDatetime( ATP_ChunkResponse_t * frame ){ return frame->datetime; }
void ChunkResponse::setDatetime( ATP_ChunkResponse_t * frame, unsigned long long mytime){ frame->datetime = mytime; }

unsigned long ChunkResponse::getAtpID( ATP_ChunkResponse_t * frame ){ return frame->atpID; }
void ChunkResponse::setAtpID( ATP_ChunkResponse_t * frame, unsigned long myval){ frame->atpID = myval; }

unsigned int ChunkResponse::getVersion( ATP_ChunkResponse_t * frame ){ return frame->version; }
void ChunkResponse::setVersion( ATP_ChunkResponse_t * frame, unsigned int myval){ frame->version = myval; }

unsigned int ChunkResponse::getStatus( ATP_ChunkResponse_t * frame ){ return frame->status; }
void ChunkResponse::setStatus( ATP_ChunkResponse_t * frame, unsigned int myval){ frame->status = myval; }

unsigned long ChunkResponse::getStart( ATP_ChunkResponse_t * frame ){ return frame->start; }
void ChunkResponse::setStart( ATP_ChunkResponse_t * frame, unsigned long myval){ frame->start = myval; }

unsigned long ChunkResponse::getLength( ATP_ChunkResponse_t * frame ){ return frame->length; }
void ChunkResponse::setLength( ATP_ChunkResponse_t * frame, unsigned long myval ){ frame->length = myval; }

unsigned int ChunkResponse::getTransferTypes( ATP_ChunkResponse_t * frame ){ return frame->transferTypes; }
void ChunkResponse::setTransferTypes( ATP_ChunkResponse_t * frame, unsigned int myval){ frame->transferTypes = myval; }

unsigned int ChunkResponse::getVerify( ATP_ChunkResponse_t * frame ){ return frame->verify; }
void ChunkResponse::setVerify( ATP_ChunkResponse_t * frame, unsigned int myval){ frame->verify = myval; }

unsigned int * ChunkResponse::getData( ATP_ChunkResponse_t * frame ){ return frame->data; }
void ChunkResponse::setData( ATP_ChunkResponse_t * frame, unsigned int * myval){ frame->data = myval; }


/*
verify() - verifies data using Verify valuegetStatus() - returns status codegetError() - returns error codedelete() - removes object and related data
getNetworkTime(source);
*/

