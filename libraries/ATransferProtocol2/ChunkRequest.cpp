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

ChuckRequest provides a "pull" from the lead peer to a peer, in response
to the peer receiving a TransferRequest. On the receiving peer, the
methods provide the App with malloc-style access to the transferred data.

See the ChunkResponse.h file for definitions of the structs used by the methods.

*/


/*- Includes ---------------------------------------------------------------*/

#include "ChunkRequest.h"
#include "RadioDriver.h"

/*- Variables --------------------------------------------------------------*/

//todo: These variables and object instances are ugly. Should be sharing a single RadioDriver and UniqueIDCounter
long UniqueIDCounter3 = 0;
RadioDriver myrd2 = RadioDriver();

/*- Implementations --------------------------------------------------------*/

ChunkRequest::ChunkRequest(){}

/*
\brief Return a new ChunkRequest object
*/

ATP_ChunkRequest_t * ChunkRequest::getNewRequest()
{
	ATP_ChunkRequest_t * acr = (ATP_ChunkRequest_t *) malloc( sizeof(ATP_ChunkRequest_t) );
	//todo: Test for a null response from malloc and handle as exception	
	setDefaults( acr );
	return acr;
}

/*
\brief Sets header values for ChunkRequest
*/

void ChunkRequest::setDefaults( ATP_ChunkRequest_t * header){	

	setFrameID( header, "FCC" );
	setFrameType( header, ATP_TRANSFER_REQUEST );
	setMeshAddress( header, 0 );			// Pan address for XBee radios
	setDatetime( header, millis() );	//todo: Replace this with the Time library or when you get a Real Time Clock
	setAtpCount( header, UniqueIDCounter3++ );
	setVersion( header, 1 );
	
	setStart( header, 0);
	setLength( header, 0);
	setTransferTypes( header, 0);
}
/*
\brief Prints ATP_ChunkRequest_t type to the logger for debugging
*/
void ChunkRequest::print( ATP_ChunkRequest_t * frame ){
	Log.Debug("ChunkRequest object:"CR);
	Log.Debug("frameID:       %s"CR, getFrameID(frame));
	Log.Debug("frameType:     %d"CR, getFrameType(frame));	
	Log.Debug("meshAddress:   %d"CR, getMeshAddress(frame));	
	Log.Debug("datetime:      %d"CR, getDatetime(frame));	
	Log.Debug("atpCount:      %d"CR, getAtpCount(frame)); 	
	Log.Debug("version:       %d"CR, getVersion(frame));
	
	Log.Debug("start:         %d"CR, getStart(frame));	
	Log.Debug("length:        %s"CR, getLength(frame));	
	Log.Debug("transferTypes: %d"CR, getTransferTypes(frame));	
}
/*
\brief Getters and Setters
*/

char * ChunkRequest::getFrameID( ATP_ChunkRequest_t * frame ){ return frame->frameID; }
void ChunkRequest::setFrameID( ATP_ChunkRequest_t * frame, char * id)
{	frame->frameID[0]=id[0];
	frame->frameID[1]=id[1];
	frame->frameID[2]=id[2];
}

unsigned int ChunkRequest::getFrameType( ATP_ChunkRequest_t * frame ){ return frame->frameType; }
void ChunkRequest::setFrameType( ATP_ChunkRequest_t * frame, unsigned int mytype){ frame->frameType = mytype; }

unsigned int ChunkRequest::getMeshAddress( ATP_ChunkRequest_t * frame ){ return frame->meshAddress; }
void ChunkRequest::setMeshAddress( ATP_ChunkRequest_t * frame, unsigned int mymesh){ frame->meshAddress = mymesh; }

unsigned long long ChunkRequest::getDatetime( ATP_ChunkRequest_t * frame ){ return frame->datetime; }
void ChunkRequest::setDatetime( ATP_ChunkRequest_t * frame, unsigned long long mytime){ frame->datetime = mytime; }

unsigned long ChunkRequest::getAtpCount( ATP_ChunkRequest_t * frame ){ return frame->atpCount; }
void ChunkRequest::setAtpCount( ATP_ChunkRequest_t * frame, unsigned long myval){ frame->atpCount = myval; }

unsigned int ChunkRequest::getVersion( ATP_ChunkRequest_t * frame ){ return frame->version; }
void ChunkRequest::setVersion( ATP_ChunkRequest_t * frame, unsigned int myval){ frame->version = myval; }


unsigned long ChunkRequest::getStart( ATP_ChunkRequest_t * frame ){ return frame->start; }
void ChunkRequest::setStart( ATP_ChunkRequest_t * frame, unsigned long myval){ frame->start = myval; }

unsigned long ChunkRequest::getLength( ATP_ChunkRequest_t * frame ){ return frame->length; }
void ChunkRequest::setLength( ATP_ChunkRequest_t * frame, unsigned long myval){ frame->length = myval; }

unsigned int ChunkRequest::getTransferTypes( ATP_ChunkRequest_t * frame ){ return frame->transferTypes; }
void ChunkRequest::setTransferTypes( ATP_ChunkRequest_t * frame, unsigned int myval){ frame->transferTypes = myval; }

/*
\brief Instantiate a new RadioDriver instance
*/

void ChunkRequest::setRadioDriverType(char * mytype)
{
	myrd2.setRadioType( mytype );
	if ( myrd2.getStatus() ) 
	{
		Log.Debug("RadioDriver: getNewRadioDriver, status is 1"CR);	
	}
	else
	{
		Log.Debug("RadioDriver: getNewRadioDriver, status is 0"CR);	
	}	
}

/*
\brief Send a ChunkRequest to a remote host
*/

void ChunkRequest::sendIt( ATP_ChunkRequest_t * frame )
{
	Log.Debug("Sending message"CR);
	myrd2.SendChunkRequest( frame );
	Log.Debug("Message sent"CR);
}


/*getError() - returns error codedelete() - removes object and related data
getNetworkTime(source);
*/
