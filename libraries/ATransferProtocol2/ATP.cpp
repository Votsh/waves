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

	TransferRequest tro = TransferRequest();
	ATP_TransferRequest_t * rq = tro.getNewRequest();
	tro.print( rq );
	tro.setRadioDriverType("XBEE");
	tro.sendIt( rq );
	Log.Debug( "-end-" );



/*

Big picture:
1) Finish coding A Transfer Protocol (this weekend)
2) Define the “show” format (definition of sound, light events)
3) Define the Web interface for a Show Builder app (this creates “shows”)
4) Receive your PC boards for the LEDs/prototype.
5) Receive the Pinoccio scouts, replace my current Arduino Micros, finish the prototype
6) Once I prove the Pinoccio design works, I will work with you to design a prototype PC board based on the Pinoccio design (it’s open source, including Eagle files)
7) Receive the RTC units and wire into Pinoccio-based prototype
8) Assemble 2 Lead Wave units, 4 Wave units (6 total). 2 of them go to show designers I am lining up.
8) Design final prototype with you. I’ll need to replace anything that gets in the way of a $20 total BOM per Wave unit - to keep the retail price below $99.


Daemon - receive and dispatch (both sides, same library)
Malloc stuff
Save to SD card
Show definition
*/






	/*
	MakeSampleData( 1028 bytes of data );
	waitTillTransferred();
	done();
	getTime(source);
	*/

}




