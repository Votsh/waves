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
char recbuf[16] = "123456789012345";	// Receive buffer

/*- Implementations --------------------------------------------------------*/



/*

Remember that C passes everything by value. so char *myptr(char *dest, char *src)
is going to return the value of a pointer to the return value, and the inputs are
the address of a character array, NOT the a reference to the pointer.

const char *src
tells the compiler that the src array is read only

*ptr++ get the value, then increment the pointer
(*ptr)++ get the value and increment the value by 1

"" defines a character array with /0 as the last character, '' is just the char array
*/


/*
\brief Constructor, pass in XBEE or LMW
*/

RadioDriver::RadioDriver( char * driverType ){

	if ( ! strcmp( driverType, "XBEE") )
	{
		Log.Debug("driverType: XBEE"CR);
		status = 1;		
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

    XBeeOnBreadboard.begin(9600);
}

unsigned int RadioDriver::getStatus(){
	return status;
}

/*
/brief Send data over the radio, this is a block call
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
/brief Service incoming data and oher interrupts
*/

void RadioDriver::serviceRadio()
{
}

int Main() {
	RadioDriver rd = RadioDriver( "XBEE" );
	if ( rd.getStatus() ) 
	{
		Log.Debug("RadioDriver: Main, status is 1");	
	}
	else
	{
		Log.Debug("RadioDriver: Main, status is 0");			
	}	
}

// Get a pointer to the received data
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
 
