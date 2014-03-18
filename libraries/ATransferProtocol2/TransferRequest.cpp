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

TransferRequest provides a "push" from the lead peer to a peer. On the lead
peer the methods initiate the request to the peer. On the receiving peer, the
methods provide the App with malloc-style access to the transferred data.

See the TransferRequest.h file for definitions of the structs used by the methods.

*/

/*- Includes ---------------------------------------------------------------*/

#include "TransferRequest.h"
#include "RadioDriver.h"

/*- Variables --------------------------------------------------------------*/

long UniqueIDCounter = 0;

/*- Implementations --------------------------------------------------------*/

/*
\brief Constructor
*/

TransferRequest::TransferRequest(){}

/*
\brief Return a new TransferRequest object
*/

ATP_TransferRequest_t * TransferRequest::getNewRequest( int id )
{
	ATP_TransferRequest_t * atr = (ATP_TransferRequest_t *) malloc( sizeof(ATP_TransferRequest_t) );
	//todo: Test for a null response from malloc and handle as exception	
	setDefaults( atr, id );
	return atr;
}

/*
\brief Sets header values for TransferRequest
*/

void TransferRequest::setDefaults( ATP_TransferRequest_t * header, int id ){
	
	setFrameID( header, "ATP" );
	setFrameType( header, ATP_TRANSFER_REQUEST );
	setMeshAddress( header, 0 );			// Pan address for XBee radios
	setDatetime( header, (unsigned long) millis() );
	setAtpID( header, id );
	setVersion( header, 1 );
	setStatus( header, ATP_IDLE );
	// Do not change the above members of the struct. The app.cpp dispatcher requires these.
	
	setSize( header, 0 );
	setExpires( header, 0 );
	setDescriptor( header, "CC");	
	setSource( header, 0 );		//todo Implement for Pinoccio Scouts, right now we're using XBee radios in direct connect mode
	setDestination( header, 0 );
	setFileName( header, "" );
}
/*
\brief Prints ATP_TransferRequest_t type to the logger for debugging
*/
void TransferRequest::print( ATP_TransferRequest_t * frame ){
	Log.Debug("TransferRequest object:"CR);
	Log.Debug("frameID:     %s"CR, getFrameID(frame));
	Log.Debug("frameType:   %d"CR, getFrameType(frame));	
	Log.Debug("meshAddress: %d"CR, getMeshAddress(frame));	
	Log.Debug("datetime:    %l"CR, getDatetime(frame));	
	Log.Debug("atpCount:    %d"CR, getAtpID(frame)); 	
	Log.Debug("version:     %d"CR, getVersion(frame));

	if ( getStatus(frame) == ATP_IDLE ) Log.Debug("status:      ATP_IDLE"CR );
	if ( getStatus(frame) == ATP_SUCCESS ) Log.Debug("status:      ATP_SUCCESS"CR );
	if ( getStatus(frame) == ATP_FAILED_DURING_TRANSIT ) Log.Debug("status:      ATP_FAILED_DURING_TRANSIT"CR );
	if ( getStatus(frame) == ATP_FAILED_CHECKSUM ) Log.Debug("status:      ATP_FAILED_CHECKSUM"CR );
	if ( getStatus(frame) == ATP_FAILED_ENCRYPTION ) Log.Debug("status:      ATP_FAILED_ENCRYPTION"CR );
	if ( getStatus(frame) == ATP_FAILED_COMPRESSION ) Log.Debug("status:      ATP_FAILED_COMPRESSION"CR );
	
	Log.Debug("size:        %d"CR, getSize(frame));
	Log.Debug("expires:     %d"CR, getExpires(frame));	
	Log.Debug("descriptor:  %s"CR, getDescriptor(frame));	
	Log.Debug("source:      %d"CR, getSource(frame));	
	Log.Debug("destination: %d"CR, getDestination(frame));	
	Log.Debug("fileName:    %s"CR, getFileName(frame));
	
	Log.Debug("buffer:      %s"CR, getBuffer(frame));
	Log.Debug("---"CR);
}
/*
\brief Getters and Setters
*/

char * TransferRequest::getFrameID( ATP_TransferRequest_t * frame ){ return frame->frameID; }
void TransferRequest::setFrameID( ATP_TransferRequest_t * frame, char * id)
{	frame->frameID[0]=id[0];
	frame->frameID[1]=id[1];
	frame->frameID[2]=id[2];
}

unsigned int TransferRequest::getFrameType( ATP_TransferRequest_t * frame ){ return frame->frameType; }
void TransferRequest::setFrameType( ATP_TransferRequest_t * frame, unsigned int mytype){ frame->frameType = mytype; }

unsigned int TransferRequest::getMeshAddress( ATP_TransferRequest_t * frame ){ return frame->meshAddress; }
void TransferRequest::setMeshAddress( ATP_TransferRequest_t * frame, unsigned int mymesh){ frame->meshAddress = mymesh; }

unsigned long TransferRequest::getDatetime( ATP_TransferRequest_t * frame ){ return frame->datetime; }
void TransferRequest::setDatetime( ATP_TransferRequest_t * frame, unsigned long mytime){ frame->datetime = mytime; }

unsigned long TransferRequest::getAtpID( ATP_TransferRequest_t * frame ){ return frame->atpID; }
void TransferRequest::setAtpID( ATP_TransferRequest_t * frame, unsigned long myval){ frame->atpID = myval; }

unsigned int TransferRequest::getVersion( ATP_TransferRequest_t * frame ){ return frame->version; }
void TransferRequest::setVersion( ATP_TransferRequest_t * frame, unsigned int myval){ frame->version = myval; }

unsigned int TransferRequest::getStatus( ATP_TransferRequest_t * frame ){ return frame->status; }
void TransferRequest::setStatus( ATP_TransferRequest_t * frame, unsigned int myval){ frame->status = myval; }

unsigned long TransferRequest::getSize( ATP_TransferRequest_t * frame ){ return frame->size; }
void TransferRequest::setSize( ATP_TransferRequest_t * frame, unsigned long myval){ frame->size = myval; }

unsigned long TransferRequest::getExpires( ATP_TransferRequest_t * frame ){ return frame->expires; }
void TransferRequest::setExpires( ATP_TransferRequest_t * frame, unsigned long myval){ frame->expires = myval; }

char * TransferRequest::getDescriptor( ATP_TransferRequest_t * frame ){ return frame->descriptor; }
void TransferRequest::setDescriptor( ATP_TransferRequest_t * frame, char * id){ strcpy( frame->descriptor, id ); }

unsigned long TransferRequest::getSource( ATP_TransferRequest_t * frame ){ return frame->source; }
void TransferRequest::setSource( ATP_TransferRequest_t * frame, unsigned long myval){ frame->source = myval; }

unsigned long TransferRequest::getDestination( ATP_TransferRequest_t * frame ){ return frame->destination; }
void TransferRequest::setDestination( ATP_TransferRequest_t * frame, unsigned long myval){ frame->destination = myval; }

unsigned int * TransferRequest::getBuffer( ATP_TransferRequest_t * frame ){ return frame->buffer; }
void TransferRequest::setBuffer( ATP_TransferRequest_t * frame, unsigned int * thebuffer ){ frame->buffer = thebuffer; }

char * TransferRequest::getFileName( ATP_TransferRequest_t * frame ){ return frame->fileName; }
void TransferRequest::setFileName( ATP_TransferRequest_t * frame, char * id){ strcpy( frame->fileName, id ); }



/*todoTransferRequest::delete() - removes object and related dataTransferRequest::isTransferComplete() - returns true if all data transffered, false if still transferring dataTransferRequest::getTransferStatus() - returns first node of a linked-list of a entries showing which portions of the transfer are complete, meaningful only to the receiving peerTransferRequest::getTransferSize() - returns a 16 bit value defining the number of 8-bit unsigned integer values in the transfer

*/

