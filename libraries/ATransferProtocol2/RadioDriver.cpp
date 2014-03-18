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



/*- Includes ---------------------------------------------------------------*/

#include "Arduino.h"
#include "Logging.h"
#include "RadioDriver.h"

/*- Variables --------------------------------------------------------------*/

// For XBee mesh and point-to-point radio network
SoftwareSerial XBeeOnBreadboard(11, 9); // RX, TX
int status = 0;		// 1 = normal, 2 = exception
//char recbuf[16] = "123456789012345";	// Receive buffer

/*- Implementations --------------------------------------------------------*/


// Empty constructor

RadioDriver::RadioDriver(){}

/*
\brief Set radio type: XBEE or LMW
*/

void RadioDriver::setRadioType( char * driverType ){

	if ( ! strcmp( driverType, "XBEE") )
	{
		Log.Debug("driverType: XBEE"CR);
		status = 1;		
	    XBeeOnBreadboard.begin(9600);
	}
	else if( ! strcmp( driverType, "LWM") )
	{
		Log.Debug("driverType: LWM"CR);
		status = 1;		
	}
	else
	{
		status = 0;
		Log.Error("Unknown driver type."CR);
		return;
	}

}

unsigned int RadioDriver::getStatus(){
	return status;
}

/*
/brief Send data over the radio, this is a blocking call
*/

void RadioDriver::Send( char * mydata )
{
	for (int i=0; i < strlen( mydata ); i++ )
	{
    	XBeeOnBreadboard.write( mydata[i] );
    	delay(100);
	}
}

/*
/brief Send a TransferRequest struct over the radio, this is a blocking call
*/

void RadioDriver::SendTransferRequest( ATP_TransferRequest_t * frame )
{
	long size = sizeof(ATP_TransferRequest_t);		
    const unsigned char *byte;
    for ( byte = (const unsigned char *) frame; size--; ++byte )                                     
    {   
    	//Log.Debug("size=%d byte=%c"CR,size,*byte);
        XBeeOnBreadboard.write( *byte );
    	delay(100);
    }
    frame->status = ATP_SUCCESS;
    //XBeeOnBreadboard.println( "We are done." );
}

/*
/brief Send a ChunkRequest struct over the radio, this is a blocking call
*/

void RadioDriver::SendChunkRequest( ATP_ChunkRequest_t * frame )
{
	long size = sizeof(ATP_ChunkRequest_t);		
    const unsigned char *byte;
    for ( byte = (const unsigned char *) frame; size--; ++byte )                                     
    {   
    	//Log.Debug("size=%d byte=%c"CR,size,*byte);
        XBeeOnBreadboard.write( *byte );
    	delay(100);
    }
    frame->status = ATP_SUCCESS;
    //XBeeOnBreadboard.println( "We are done." );
}

/*
/brief Send a ChunkResponse struct over the radio, this is a blocking call
*/

//todo Send to specific radio end points

void RadioDriver::SendChunkResponseBuffer( ATP_ChunkResponse_t * frame, unsigned int * buffer )
{
	// Calculate checksum value
	
	unsigned int checksum = 0;
	for ( int c = 0; c < frame->length; c++ )
	{
		checksum = checksum + buffer[c];		
	}
	frame->verify = checksum;

	long size = sizeof(ATP_ChunkResponse_t);		
    const unsigned char *byte;
    for ( byte = (const unsigned char *) frame; size--; ++byte )                                     
    {   
    	//Log.Debug("size=%d byte=%c"CR,size,*byte);
        XBeeOnBreadboard.write( *byte );
    	delay(100);
    }
    
    size = frame->length;
    for ( byte = (const unsigned char *) buffer + frame->start; size--; ++byte )                                     
    {   
    	//Log.Debug("size=%d byte=%c"CR,size,*byte);
        XBeeOnBreadboard.write( *byte );
    	delay(100);
    }
    frame->status = ATP_SUCCESS;
    
    //todo Handle exceptions
    
    //XBeeOnBreadboard.println( "We are done." );
}

/*
/brief Send a ChunkResponse struct over the radio, this is a blocking call
*/

void RadioDriver::SendChunkResponseFile( ATP_ChunkResponse_t * frame, char * thefile )
{
	// Calculate checksum value
	unsigned int * buffer = frame->data;
	
	unsigned int checksum = 0;
	for ( int c = 0; c < frame->length; c++ )
	{
		checksum = checksum + buffer[c];		
	}
	frame->verify = checksum;

	long size = sizeof(ATP_ChunkResponse_t);		
    const unsigned char *byte;
    for ( byte = (const unsigned char *) frame; size--; ++byte )                                     
    {   
    	//Log.Debug("size=%d byte=%c"CR,size,*byte);
        XBeeOnBreadboard.write( *byte );
    	delay(100);
    }
    
    size = frame->length;
    for ( byte = (const unsigned char *) buffer + frame->start; size--; ++byte )                                     
    {   
    	//Log.Debug("size=%d byte=%c"CR,size,*byte);
        XBeeOnBreadboard.write( *byte );
    	delay(100);
    }
    
    frame->status = ATP_SUCCESS;
    //todo Handle exceptions
}

/*
/brief Returns true if the radio received a character
*/

int RadioDriver::isAvailable(){ return XBeeOnBreadboard.available(); }

/*
/brief Get an int received
*/

unsigned int RadioDriver::getReceivedInt(){ return XBeeOnBreadboard.read(); }

/*
/brief Get an int received
*/

char RadioDriver::getReceivedChar(){ return XBeeOnBreadboard.read(); }

/*
/brief Get a pointer to the received data
*/

/*
char * RadioDriver::getReceived()
{
	for (int i=0; i<14; i++ )
	{
		if (XBeeOnBreadboard.available())
		{
			recbuf[i] = XBeeOnBreadboard.read();
			recbuf[i+1] = '/0';
		}
	}
	return recbuf;
}; 
*/
