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
#include <Time.h>

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
	header->frameID[0]='A';
	header->frameID[1]='T';
	header->frameID[2]='P';
	
	header->frameType = ATP_TRANSFER_REQUEST;
	header->meshAddress = 0;		// Pan address for XBee radios
	header->datetime = millis();	//todo: Replace this with the Time library or when you get a Real Time Clock
	header->atpCount = UniqueIDCounter++;
	header->version = 1;
	header->size=1000;
	header->expires=6789;
	strcpy( header->descriptor, "CC" ); // text/plain
	header->source = 0;
	header->destination = 0;
	strcpy( header->fileName, "frankolo.ogg" );
}
/*
\brief Prints ATP_TransferRequest_t type to the logger for debugging
*/
void TransferRequest::printMe( ATP_TransferRequest_t * frame ){
	Log.Debug("TransferRequest object:"CR);
	Log.Debug("frameID: %s"CR, getFrameID(frame));	
	
	
}
/*  int			frameType;
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
*//*
\brief Getters and Setters
*/

char * TransferRequest::getFrameID( ATP_TransferRequest_t * frame ){ return frame->frameID; }


/*TransferRequest::setSize()TransferRequest::getSize()TransferRequest::setTime()TransferRequest::getTime()TransferRequest::setExpires()TransferRequest::getExpires()TransferRequest::setVersion()TransferRequest::getVersion()TransferRequest::setContentDescriptor()
TransferRequest::getContentDescriptor()
TransferRequest::setSource()TransferRequest::getSource()TransferRequest::setDestination()TransferRequest::getDestination()TransferRequest::getStatus() - returns status codeTransferRequest::getError() - returns error codeTransferRequest::isTransferToBuffer() - returns true if transfer is to in-memory buffer, returns false if transfer is to cardTransferRequest::setFileName() - 16 character file name, or null if store to memory bufferTransferRequest::getFileName()TransferRequest::send() - sends request to the destinationTransferRequest::delete() - removes object and related dataTransferRequest::getBufferAddress() - returns address of the in-memory buffer, meaningful only to the peerTransferRequest::isTransferComplete() - returns true if all data transffered, false if still transferring dataTransferRequest::getTransferStatus() - returns first node of a linked-list of a entries showing which portions of the transfer are complete, meaningful only to the receiving peerTransferRequest::getTransferSize() - returns a 16 bit value defining the number of 8-bit unsigned integer values in the transfer

*/

