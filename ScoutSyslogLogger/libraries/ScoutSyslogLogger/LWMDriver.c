/*

Scout Syslog LWM Message Driver

by Frank Cohen, fcohen@votsh.com

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

#include "LWMDriver.h"

/*
Received a send confirmation message on the client. 
Only needed if you want to do something once you receive the Ack packet back from the receiver */

static void sendConfirm( NWK_DataReq_t *req2 ) 
{
    sendconductor = sendroot;
	while ( sendconductor != 0 )
	{
		if ( sendconductor->req == req2 )
		{
			sendconductor->status = req2->status;
			if ( req2->status == NWK_SUCCESS_STATUS )
			{
				sendconductor->stage = lwmConfirmed;
				return;
			}
			else
			{
				sendconductor->stage = lwmError;
				return;
			}
		}
		sendconductor = sendconductor->next;
	}
}

sendnode_t * LWMGetSendList()
{
	return sendroot;
}

void LWMPruneSendList( unsigned long time )
{	
    sendconductor = sendroot;
	sendprevnode = sendroot;		
	while ( sendconductor != 0 )
	{
		if ( 
			( (sendconductor->timeorigin) + lwmTimeout < time ) &&
			( ( sendconductor->stage == lwmSent )  ||
			  ( sendconductor->stage == lwmError ) ||
			  ( sendconductor->stage == lwmConfirmed )
			)
		   )
		{
			// Sent or error, and timed-out
			
			size1++;
				
			if ( sendconductor->req->data != 0 )
			{
				bytesfreed += ( sendconductor->req->size ) + 1;
				free( sendconductor->req->data );
			}
			if ( sendconductor->req != 0 )
			{
				bytesfreed += sizeof( NWK_DataReq_t );
				free( sendconductor->req );
			}
			
			sendremove = sendconductor;
			
			if ( sendconductor == sendroot )
			{
				sendroot=sendconductor->next;
				sendconductor = sendconductor->next;
			}
			else
			{
				sendprevnode->next = sendconductor->next;
				sendconductor = sendconductor->next;
			}
			
			if ( sendremove != 0 )
			{
				size3++;
				bytesfreed += sizeof( sendnode_t );
				free( sendremove );
			}
		}
		else
		{
			sendprevnode = sendconductor;
			sendconductor = sendconductor->next;
		}
	}
}

/*
Send a message across the LWM to the lead Scout, who is running ScoutSyslogLogger
This puts the message into a linked list, LWMSendNextMessage() actually sends the messages
*/

int sendLWMMsg( char * data, int destination_mesh_id, unsigned long time ) {
	
	cntsend++;
		
	NWK_DataReq_t * message = (NWK_DataReq_t*) malloc( sizeof( NWK_DataReq_t ) );
	if ( message == 0 ) 
	{
		return NWK_OUT_OF_MEMORY_STATUS;
	}
	
	bytesused += sizeof( NWK_DataReq_t );
	
	uint8_t * msgbody = (uint8_t *) malloc( strlen( data ) + 1 );
	if ( msgbody == 0 )
	{
		bytesfreed += sizeof( NWK_DataReq_t );
		free( message );
	 	return NWK_OUT_OF_MEMORY_STATUS;  	
	}
	
	bytesused += strlen( data ) + 1;
	
	memcpy(msgbody, data, strlen( data ) );
	
	message->dstAddr = destination_mesh_id;
	message->dstEndpoint = 1;
	message->srcEndpoint = 1;
	message->options = 0; //NWK_OPT_ACK_REQUEST;   // Example: NWK_OPT_ACK_REQUEST|NWK_OPT_ENABLE_SECURITY
	message->data = msgbody;
	message->size = strlen( data );
	message->confirm = &sendConfirm;

	// Add to end of list	
	sendconductor = sendroot;
	if ( sendroot == 0 )
	{
		sendroot = (sendnode_t *) malloc( sizeof(sendnode_t) );
		if ( sendroot == 0 ) return NWK_OUT_OF_MEMORY_STATUS;	//Out of memory

		bytesused += sizeof(sendnode_t);

		sendroot->next = 0;
		sendconductor = sendroot;
	}
	else
	{
		while ( sendconductor->next != 0 )
		{
			sendconductor = sendconductor->next;
		}

		sendconductor->next = (sendnode_t *) malloc( sizeof(sendnode_t) );
		if ( sendconductor->next == 0 ) return NWK_OUT_OF_MEMORY_STATUS;	//Out of memory
		bytesused += sizeof(sendnode_t);
		sendconductor = sendconductor->next;
		sendconductor->next = 0;
	}

	sendconductor->req = message;
	sendconductor->timeorigin = time;
	sendconductor->status = 0;
	sendconductor->stage = lwmInit;
	return 0;
}

/*
* Sends messages waiting on the linked list to the Lead
  Sends only one message at a time
*/

void LWMSendMessages( unsigned long time )
{
	if ( sendroot == 0 ) return;

	int ready = 0;
    sendconductor = sendroot;
	while ( sendconductor != 0 )
	{
		if ( sendconductor->stage == lwmSent )
		{
			ready = 1;				
		}
		sendconductor = sendconductor->next;
	}

	// exit if it is already sending a message
	if ( ready ) { return; }

    sendconductor = sendroot;
	while ( sendconductor != 0 )
	{
		if ( sendconductor->stage == lwmInit )
		{
		  	NWK_DataReq( (NWK_DataReq_t *) ( sendconductor->req ) );		// Send it	
			sendconductor->stage = lwmSent;
			sendconductor->timeorigin = time;
			return;
		}
		sendconductor = sendconductor->next;
	}
}

uint8_t LWMgetsize1() { return size1; }
uint8_t LWMgetsize2() { return size2; }
uint8_t LWMgetsize3() { return size3; }


/*
Returns count of messages received
*/

uint8_t LWMRecCount() { return cnt; }

/*
Returns count of messages sent
*/

uint8_t LWMSendCount() { return cntsend; }

/*
Returns count of bytes of storage used
*/

unsigned long LWMSendBytesUsed() { return bytesused; }

unsigned long LWMSendBytesFreed() { return bytesfreed; }

/*
Returns count of messages in sent linked list
*/

uint8_t LWMgetQueueSize()
{
	uint8_t count = 0;
    sendconductor = sendroot;
	while ( sendconductor != 0 )
	{
		count++;
		sendconductor = sendconductor->next;
	}
	return count;
}

/*
Event handler to receive messages on the LeadScout side
*/

static bool receiveMessage(NWK_DataInd_t *ind) {

  	cnt++;

  	NWK_SetAckControl(abs(ind->rssi));
	
  // Add node to a linked-list
  if ( root == 0 )
  {
  	root = (node_t *) malloc( sizeof(node_t) );
  	if ( root == 0 ) { return false; }		//Out of memory
	root->next = 0;
	unsigned long tlen = ((unsigned long) ind->size ) + 1;
	root->data = malloc( tlen );
	if ( ( root->data ) == 0 ) 
	{
		bytesfreed += sizeof( root );
		free( root );
		return false;	
	}
	memcpy( root->data, ind->data, ind->size );
	( root -> data ) [ tlen - 1 ] = '\0';
  }
  else
  {
    conductor = root;
	while ( conductor->next != 0 )
	{
		conductor = conductor->next;
	}

	conductor->next = (node_t *) malloc( sizeof(node_t) );
	prevnode = conductor->next;
	if ( conductor->next == 0 ) return false;	//Out of memory
	conductor = conductor->next;

	unsigned long tlen = ((unsigned long) ind->size ) + 1;
	conductor->data = malloc( tlen );
	if ( ( conductor->data ) == 0 )
	{
		bytesfreed += sizeof( prevnode );
		free( prevnode );
		return false;
	}	
	memcpy( conductor->data, ind->data, ind->size);
	( conductor -> data ) [ tlen - 1 ] = '\0';

	conductor->next = 0;
  }

  return true;
}

node_t * LWMGetRoot()
{
	return root;	
}

void LWMSetRoot( node_t * node )
{
	root = node;
}

char * printStatus( int status )
{
  if ( status == NWK_SUCCESS_STATUS ) return "NWK_SUCCESS_STATUS";
  if ( status == NWK_ERROR_STATUS ) return "NWK_ERROR_STATUS";
  if ( status == NWK_OUT_OF_MEMORY_STATUS ) return "NWK_OUT_OF_MEMORY_STATUS";
  if ( status == NWK_NO_ACK_STATUS ) return "NWK_NO_ACK_STATUS";
  if ( status == NWK_NO_ROUTE_STATUS ) return "NWK_NO_ROUTE_STATUS";
  if ( status == NWK_PHY_CHANNEL_ACCESS_FAILURE_STATUS ) return "NWK_PHY_CHANNEL_ACCESS_FAILURE_STATUS";
  if ( status == NWK_PHY_NO_ACK_STATUS ) return "NWK_PHY_NO_ACK_STATUS";
  return "Unknown Error";
}

void LWMDriverInit(){
  root = 0;
  conductor = 0;
  prevnode = 0;
  cnt = 0;
  cntsend = 0;
  
  sendroot = 0;
  sendconductor = 0;
  sendprevnode = 0;

  bytesused = 0;
  bytesfreed = 0;
  
  recentError = "None";
  recentErrorInt = 0;  
  NWK_OpenEndpoint(1, receiveMessage);  
}


