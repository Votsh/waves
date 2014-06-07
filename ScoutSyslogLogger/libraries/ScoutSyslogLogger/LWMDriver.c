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

int messageNumber = 0;
int sending = 0;
char * recentMsg[1000];
int recentStatus = 0;

/*
Send a message across the LWM to another Scout running this Echo sketch
*/

// only needed if you want to do something once you receive the Ack packet back from the receiver
void sendConfirm( NWK_DataReq_t *req ) {
  sending = 0;
  
  if (NWK_SUCCESS_STATUS == req->status)
  {
    //recentMsg = String("Sent successfully.");
    recentStatus = 0;
  }
  else
  {
	/*
	recentMsg = String( "sendConfirm error: );
	recentMsg += printStatus( req->status );
	recentMsg += " " + req->status;
	*/
	recentStatus = 1;
  }
  
  LWMGC( req );
}

/*
Send a message across the LWM to the lead Scout, who is running ScoutSyslogLogger
*/

void sendLWMMsg( const char * data, int destination_mesh_id ) {

  sending = 1;
  
  //Serial.println("sendMsg");
  //Log.Debug("sendMsg to %i, length = %d, sending: %s"CR, destination_mesh_id, strlen(data) + 1, data );

  NWK_DataReq_t * message = (NWK_DataReq_t*) malloc( sizeof( NWK_DataReq_t ) );

  message->dstAddr = destination_mesh_id;
  message->dstEndpoint = 1;
  message->srcEndpoint = 1;
  message->options = NWK_OPT_ACK_REQUEST;   // Example: NWK_OPT_ACK_REQUEST|NWK_OPT_ENABLE_SECURITY
  message->data = (uint8_t *) data;     // (uint8_t *) data.toCharArray();
  message->size = strlen( data ) + 1 ;  // data.length() + 1 ;
  message->confirm = sendConfirm;
  NWK_DataReq(message);  
}

/*
Event handler to receive messages
*/

bool receiveMessage(NWK_DataInd_t *ind) {

  char *recdata = (char*)ind->data;
  
  /*
  Log.Debug("Received message of %d bytes: %s", ind->size, recdata );
  Log.Debug(" from %d ", ind->srcAddr );
  Log.Debug(" lqi %d ", ind->lqi );
  Log.Debug(" rssi %d "CR, abs(ind->rssi) );
  */
	
  NWK_SetAckControl(abs(ind->rssi));

  recentStatus = 1;
  strcpy( recentMsg, recdata);

  return true;
}

bool LWMMessageWaiting()
{
	return recentStatus;	
}

char * LWMGetMessage(){
	recentStatus = 0;
	return recentMsg;
}

/*
	Free the message memory
*/

void LWMGC( NWK_DataReq_t* msg ){
	sending = 0;
	if ( msg != 0 ) 
	{
		free(msg);
	}
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
  NWK_OpenEndpoint(1, receiveMessage);
  sending = 0;
}


