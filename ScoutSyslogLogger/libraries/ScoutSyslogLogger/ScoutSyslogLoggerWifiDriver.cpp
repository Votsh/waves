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
		int procid, char * machinename, char * appname )
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

void ScoutSyslogLoggerWifiDriver::handleLWM()
{	
	if ( isLeadScout  )
	{		
	    if ( ! connected )
		{
			Serial.println("Not connected. Could not send message.");
			return;	
		}
		
		// Pull the msg from the last node, then delete the node

		if ( LWMGetRoot() == 0 ) return;
		
		node_t * conductor = LWMGetRoot();
		node_t * prevnode = 0;
		
		while ( conductor->next != 0 )
		{
			prevnode = conductor;
			conductor = conductor->next;
		}

		if ( prevnode != 0 )
		{
			// Send the message
		    client.print( conductor->data );
		    client.flush();
		    
		    delay(2000);
		    
		    Serial.println(" ");
		    Serial.print("Sending ");
		    Serial.print( LWMCount() );
		    Serial.print(": ");
		    Serial.println( conductor->data );
		    Serial.println(" ");		    

			// Delete the node from the list
			prevnode->next = 0;
			
			// Free the data
			free( conductor->data );
		
			// Free the node
			free( conductor );
		}
		else
		{
			// Send the message
		    client.print( conductor->data );
		    client.flush();

		    delay(2000);

		    Serial.println(" ");
		    Serial.print("Sending ");
		    Serial.print( LWMCount() );
		    Serial.print(": ");
		    Serial.println( conductor->data );
		    Serial.println(" ");		    

			// Free the data
			free( conductor->data );
		
			// Free the node
			free( conductor );
						
			// Remove the root
			LWMSetRoot( 0 );
		}
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
		  	      
	      msg+= String(message);
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
		
		sendLWMMsg( msgbuf, 1 );
	}
}





