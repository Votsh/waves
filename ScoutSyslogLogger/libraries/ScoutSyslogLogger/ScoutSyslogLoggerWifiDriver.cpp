/*

Scout Syslog Logger Wifi Driver

A Web-based log4j-style logger for Arduino Pinoccio Scout apps
Driver communicates to a syslog service over Wifi

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

Implements syslog protocols:
http://tools.ietf.org/html/rfc5424

References to Gainspan UDP example at:
https://github.com/Pinoccio/library-gainspan-s2w/blob/master/examples/GSUdpClient/GSUdpClient.ino

Uses multiple simultaneous sockets from the Gainspan to the web while Pinoccio HQ runs:
https://pinocc.io/forum#!/pinoccio-software-api:outgoing-connections

*/

#include "ScoutSyslogLoggerWifiDriver.h"
#include "ScoutSyslogLogger.h"
#include "LWMDriver.h"

GSModule gs;
GSUdpClient client(Scout.wifi.gs);

ScoutSyslogLoggerWifiDriver::ScoutSyslogLoggerWifiDriver(void){}

void ScoutSyslogLoggerWifiDriver::setup( int ip0, int ip1, int ip2, int ip3,
		int port, int driver, int facility, 
		int procid, char * machinename, char * appname,
		int max_queue_size )
{
    _ip0 = ip0;
    _ip1 = ip1;
    _ip2 = ip2;
    _ip3 = ip3;
    _port = port;
    _driver = driver;
    _facility = facility;
    _procid = procid;
    _appname = String( appname );
    _machinename = String( machinename );
    _max_queue_size = max_queue_size;
    
    Serial.println("Machinename: " + _machinename );
    Serial.print("Facility: ");
    Serial.println( _facility );
    Serial.print("ProcID: " );
    Serial.println( _procid );
    
    if ( Scout.getAddress() == 1 ) { isLeadScout = 1; } else { isLeadScout = 0; }

	if ( isLeadScout )
	{
		// Make the UDP connection to the syslog server
		
		delay(5000);	// Gives time for the Pinoccio Wifi backpack to get online
		
		if ( client.connect(IPAddress(_ip0,_ip1,_ip2,_ip3), 514) )
		{
		  connected = 1;
		  Serial.println("Connected to server");
		}
		else  
		{
		  Serial.println("Connect failed");
		}

		sendMsg( "ScoutSyslogLogger by Frank Cohen of Votsh.com v1", info );			

		// Set up for LWM reception, send to sendMsg( msg ), 1 = Lead Scout
	  	LWMDriverInit();
	}
}

void ScoutSyslogLoggerWifiDriver::loop()
{			
	if ( isLeadScout  )
	{		
	    if ( ! connected )
		{
			Serial.println("Not connected. Could not send message.");
			return;	
		}
		
		// You are the Lead Scout and received messages from the Scouts over LWM
		// Now send, the message to the Syslog service on the server
		
		conductor = LWMGetRoot();
		if ( conductor == 0 ) return;
		
		prevnode = 0;
		while ( conductor->next != 0 )
		{
			prevnode = conductor;
			conductor = conductor->next;
		}

		/*
		Serial.print("===");
		Serial.print( conductor->data );
		Serial.println("===");
		*/
		
		// Send the message
	    client.print( conductor->data );
	    client.flush();

		// Delete the node from the list
		if ( prevnode != 0 )
		{
			prevnode->next = 0;
		}
		else
		{
			// Remove the root
			LWMSetRoot( 0 );
		}
		
		// Free the data
		if ( conductor->data != 0 ) free( conductor->data );		
		// Free the node
		if ( conductor != 0 ) free( conductor );
	}
	else
	{
		LWMSendMessages( millis() );   	// Send messages to the Lead over LWM
		LWMPruneSendList( millis() );	// Prune linked lists of messages already sent or timed-out
	}
}

void ScoutSyslogLoggerWifiDriver::sendMsg( String message, int msgtype )
{
	if ( isLeadScout  )
	{
	    if ( connected )
	    {
	      String msg = String("<");
	      msg+= String( msgtype + ( _facility * 8 ) );
	      msg+= String("> ");
	      msg+= _machinename;
	      msg+= String(" ");
	      
		  switch( msgtype )
		  {
		  	case err:
	          msg+= String("ERR ");
		  	break;
		  	case info:
	          msg+= String("INFO ");
		  	break;
		  	case debug:
	          msg+= String("DEBUG ");
		  	break;
		  	default:
	      		msg+= String("DEFAULT ");
		  	break;
		  }
		  
	      msg += message;

	      client.print( msg );
	      client.flush();
	      
		  Serial.println("Sent to syslog server: " + msg );
	    }
	    else
	    {
	    	Serial.println("Not connected. Could not send message.");	
	    }

	}
	else
	{
		if ( LWMgetQueueSize() >= _max_queue_size ) 
		{
			Serial.print( "Queue at or above max_queue_size. Did not send message:");
			Serial.println( message );
			return;	
		}
		
		// LWM this message to the WifiDriver on the LeadScout

	    String msg = String("<");
	    msg+= String( msgtype + ( _facility * 8 ) );
	    msg+= String("> ");
	    msg+= _machinename;
	    msg+= String(" ");
	      
	  	switch( msgtype )
		{
			case err:
	        msg+= String("ERR ");
		  	break;
		  	case info:
	          msg+= String("INFO ");
		  	break;
		  	case debug:
	          msg+= String("DEBUG ");
		  	break;
		  	default:
	      	  msg+= String("DEFAULT ");
		  	break;
		}
		  	      
	    msg+= String(message);

		msg.toCharArray(msgbuf, 1000);
				
		int myerr = sendLWMMsg( msgbuf, 1, millis() );
		if ( myerr != 0 )
		{
			Serial.print("Send failed, error ");
			if ( myerr == NWK_OUT_OF_MEMORY_STATUS )
			{
				Serial.println("Out of memory");
			}	
			else
			{
				Serial.println( myerr );
			}
		}
		PrintSendList(); // Debug
	}
}

void ScoutSyslogLoggerWifiDriver::PrintSendList()
{
	//Serial.println("PrintSendList:");
	Serial.print("time>");
	Serial.print( millis() );
	Serial.print(" rec ");
	Serial.print( LWMRecCount() );
	Serial.print(" send ");
	Serial.print( LWMSendCount() );
	Serial.print(" allocated ");
	Serial.print( LWMSendBytesUsed() );
	Serial.print(" freed ");
	Serial.print( LWMSendBytesFreed() );
	Serial.print(" size1 ");
	Serial.print( LWMgetsize1() );
	Serial.print(" size2 ");
	Serial.print( LWMgetsize2() );
	Serial.print(" size3 ");
	Serial.println( LWMgetsize3() );

	sendnode * sendconductor = LWMGetSendList();
	while ( sendconductor != 0 ) {

		//Serial.print(" stage = ");
		Serial.print(sendconductor->stage);
		if (sendconductor->stage == lwmInit) { Serial.print(     " lwmInit      " ); }
		if (sendconductor->stage == lwmSent) { Serial.print(     " lwmSent      " ); }
		if (sendconductor->stage == lwmConfirmed) { Serial.print(" lwmConfirmed " ); }
		if (sendconductor->stage == lwmError) { Serial.print(    " lwmError     " ); }
		
		//Serial.print(" status = ");
		Serial.print(sendconductor->status);

		if (sendconductor->status == NWK_SUCCESS_STATUS) {       Serial.print(" NWK_SUCCESS_STATUS   " ); }
		if (sendconductor->status == NWK_ERROR_STATUS) {         Serial.print(" NWK_ERROR_STATUS     " ); }
		if (sendconductor->status == NWK_OUT_OF_MEMORY_STATUS) { Serial.print(" NWK_OUT_OF_MEMORY_STATUS" ); }
		if (sendconductor->status == NWK_NO_ACK_STATUS) {        Serial.print(" NWK_NO_ACK_STATUS    " ); }
		if (sendconductor->status == NWK_NO_ROUTE_STATUS) {      Serial.print(" NWK_NO_ROUTE_STATUS  " ); }
		if (sendconductor->status == NWK_PHY_CHANNEL_ACCESS_FAILURE_STATUS) { Serial.print(" NWK_PHY_CHANNEL_ACCESS_FAILURE_STATUS" ); }
		if (sendconductor->status == NWK_PHY_NO_ACK_STATUS) { Serial.print(   " NWK_PHY_NO_ACK_STATUS" ); }

		//Serial.print(" time = ");
		Serial.print(" ");
		Serial.print( (unsigned long) sendconductor->timeorigin);		
		Serial.print(" ");

		unsigned long tlen = ((unsigned long) sendconductor->req->size ) + 1;
		char msg[ ( (uint8_t) ( sendconductor->req ) -> size ) + 1 ];
		memcpy( msg, sendconductor->req->data, sendconductor->req->size);
		msg[ tlen - 1 ] = '\0';
		Serial.println( msg );

		/*
		char * mydata = (char *) ( sendconductor->req ) -> data;
		//Serial.print(" message = ");		
  		char mydata2[ ( (uint8_t) ( sendconductor->req ) -> size ) + 1];
		memset(mydata2, 0, (uint8_t) ( sendconductor->req ) -> size);
		memcpy(mydata2, mydata, (uint8_t) ( sendconductor->req ) -> size );
		Serial.println( mydata2 );
		*/
		
	/*
		Serial.print("  size=");
		Serial.println( ( sendconductor->req ) -> size );

		Serial.print("  dstAddr=");
		Serial.println( ( sendconductor->req ) -> dstAddr );
		
		Serial.print("  dstEndpoint =");
		Serial.println( ( sendconductor->req ) -> dstEndpoint );
		
		Serial.print("  srcEndpoint =");
		Serial.println( ( sendconductor->req ) -> srcEndpoint );
		
		Serial.print("  options =");
		Serial.println( ( sendconductor->req ) -> options );
*/
		
		//Serial.println(" ");
	    sendconductor = sendconductor->next;
	}
}

void ScoutSyslogLoggerWifiDriver::PrintReceiveList ()
{
}


