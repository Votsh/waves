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
RadioDriver myrd = RadioDriver();

/*- Implementations --------------------------------------------------------*/

/*
\brief Constructor
*/

TransferRequest::TransferRequest(){}

/*
\brief Return a new TransferRequest object
*/

ATP_TransferRequest_t * TransferRequest::getNewRequest()
{
	ATP_TransferRequest_t * atr = (ATP_TransferRequest_t *) malloc( sizeof(ATP_TransferRequest_t) );
	//todo: Test for a null response from malloc and handle as exception	
	setHeaderDefaults( atr );
	return atr;
}

/*
\brief Sets header values for TransferRequest
*/

void TransferRequest::setHeaderDefaults( ATP_TransferRequest_t * header){
	
	setFrameID( header, "FCC" );
	setFrameType( header, ATP_TRANSFER_REQUEST );
	setMeshAddress( header, 0 );			// Pan address for XBee radios
	setDatetime( header, millis() );	//todo: Replace this with the Time library or when you get a Real Time Clock
	setAtpCount( header, UniqueIDCounter++ );
	setVersion( header, 1 );
	setSize( header, 1000 );
	setExpires( header, 6789 );
	setDescriptor( header, "CC");	
	setSource( header, 0 );
	setDestination( header, 0 );
	setFileName( header, "frankolo.ogg" );
}
/*
\brief Prints ATP_TransferRequest_t type to the logger for debugging
*/
void TransferRequest::print( ATP_TransferRequest_t * frame ){
	Log.Debug("TransferRequest object:"CR);
	Log.Debug("frameID:     %s"CR, getFrameID(frame));
	Log.Debug("frameType:   %d"CR, getFrameType(frame));	
	Log.Debug("meshAddress: %d"CR, getMeshAddress(frame));	
	Log.Debug("datetime:    %d"CR, getDatetime(frame));	
	Log.Debug("atpCount:    %d"CR, getAtpCount(frame)); 	
	Log.Debug("version:     %d"CR, getVersion(frame));	
	Log.Debug("size:        %d"CR, getSize(frame));
	Log.Debug("expires:     %d"CR, getExpires(frame));	
	Log.Debug("descriptor:  %s"CR, getDescriptor(frame));	
	Log.Debug("source:      %d"CR, getSource(frame));	
	Log.Debug("destination: %d"CR, getDestination(frame));	
	Log.Debug("fileName:    %s"CR, getFileName(frame));
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

unsigned long long TransferRequest::getDatetime( ATP_TransferRequest_t * frame ){ return frame->datetime; }
void TransferRequest::setDatetime( ATP_TransferRequest_t * frame, unsigned long long mytime){ frame->datetime = mytime; }

unsigned long TransferRequest::getAtpCount( ATP_TransferRequest_t * frame ){ return frame->atpCount; }
void TransferRequest::setAtpCount( ATP_TransferRequest_t * frame, unsigned long myval){ frame->atpCount = myval; }

unsigned int TransferRequest::getVersion( ATP_TransferRequest_t * frame ){ return frame->version; }
void TransferRequest::setVersion( ATP_TransferRequest_t * frame, unsigned int myval){ frame->version = myval; }

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

char * TransferRequest::getFileName( ATP_TransferRequest_t * frame ){ return frame->fileName; }
void TransferRequest::setFileName( ATP_TransferRequest_t * frame, char * id){ strcpy( frame->fileName, id ); }

/*
\brief Instantiate a new RadioDriver instance
*/

void TransferRequest::setRadioDriverType(char * mytype)
{
	myrd.setRadioType( mytype );
	if ( myrd.getStatus() ) 
	{
		Log.Debug("RadioDriver: getNewRadioDriver, status is 1"CR);	
	}
	else
	{
		Log.Debug("RadioDriver: getNewRadioDriver, status is 0"CR);	
	}	
}

/*
\brief Send a TransferRequest to a remote host
*/

void TransferRequest::sendIt( ATP_TransferRequest_t * frame )
{
	Log.Debug("Sending message"CR);
	myrd.SendTransferRequest( frame );
	Log.Debug("Message sent"CR);
}





/*TransferRequest::delete() - removes object and related dataTransferRequest::getBufferAddress() - returns address of the in-memory buffer, meaningful only to the peerTransferRequest::isTransferComplete() - returns true if all data transffered, false if still transferring dataTransferRequest::getTransferStatus() - returns first node of a linked-list of a entries showing which portions of the transfer are complete, meaningful only to the receiving peerTransferRequest::getTransferSize() - returns a 16 bit value defining the number of 8-bit unsigned integer values in the transfer

*/

