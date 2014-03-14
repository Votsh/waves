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
#include "ATP.h"
#include "Logging.h"
#include "RadioDriver.h"
#include "StatusCodes.h"
#include "TransferRequest.h"

/*- Variables --------------------------------------------------------------*/

/*- Implementations --------------------------------------------------------*/

/*
 @brief Initializes ATP modules and data structures
*/

ATP::ATP(void)
{
    Init("XBEE");
}

void ATP::Init( char * driverType )
{
    Log.Init(LOGLEVEL, baudrate);
    Log.Info("A Transfer Protocol (ATP)"CR);
}

/*
 @brief Used for testing
*/

void ATP::Test(void)
{
	Log.Info("ATP Test"CR);
	
	RadioDriver rd = RadioDriver( "XBEE" );
	if ( rd.getStatus() ) 
	{
		Log.Debug("RadioDriver: Main, status is 1"CR);	

		Log.Debug("Sending message"CR);
		rd.Send("Hi Frank Here");
		Log.Debug("Message sent"CR);

		TransferRequest tro = TransferRequest();
		ATP_TransferRequest_t * rq = tro.getNewRequest();
		tro.printMe( rq );
		Log.Debug( "-end-" );

	}
	else
	{
		Log.Debug("RadioDriver: Main, status is 0"CR);			
	}	









	/*
	MakeSampleData( 1028 bytes of data );
	ATP_TransferRequest( sampledata, remotexbee );
	waitTillTransferred();
	done();
	getTime(source);
	*/

}




