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

void sendConfirm( NWK_DataReq_t *req ) {	

	// Release the memory for the message and its data
	free( req->data );
	free( req );
}

/*
Send a message across the LWM to the lead Scout, who is running ScoutSyslogLogger
*/

void sendLWMMsg( char * data, int destination_mesh_id ) {

  NWK_DataReq_t * message = (NWK_DataReq_t*) malloc( sizeof( NWK_DataReq_t ) );
  if ( message == 0 ) return;

  uint8_t * msgbody = (uint8_t *) malloc( strlen( data ) + 1 );
  if ( msgbody == 0 ) return;

  memcpy(msgbody, data, strlen( data ) );

  message->dstAddr = destination_mesh_id;
  message->dstEndpoint = 1;
  message->srcEndpoint = 1;
  message->options = NWK_OPT_ACK_REQUEST;   // Example: NWK_OPT_ACK_REQUEST|NWK_OPT_ENABLE_SECURITY
  message->data = msgbody;
  message->size = strlen( data );
  message->confirm = sendConfirm;
  NWK_DataReq(message);  
}



int LWMCount() { return cnt; }

/*
Event handler to receive messages on the LeadScout side
*/

bool receiveMessage(NWK_DataInd_t *ind) {

  	cnt++;

  	NWK_SetAckControl(abs(ind->rssi));
	
  // Add node to a linked-list
  if ( root == 0 )
  {
  	root = (node_t *) malloc( sizeof(node_t) );
  	if ( root == 0 ) { return 0; }		//Out of memory
	root->next = 0;
	root->data = malloc( (ind->size) + 1 );
	memcpy( root->data, ind->data, ind->size);
  }
  else
  {
    conductor = root;
	while ( conductor->next != 0 )
	{
		conductor = conductor->next;
	}

	conductor->next = (node_t *) malloc( sizeof(node_t) );
	if ( conductor->next == 0 ) return 0;	//Out of memory
	conductor = conductor->next;

	conductor->data = malloc( (ind->size) + 1 );
	memcpy( root->data, ind->data, ind->size);
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
  NWK_OpenEndpoint(1, receiveMessage);  
}


