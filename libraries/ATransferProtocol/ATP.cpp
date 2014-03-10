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

#include "ATP.h"
#include "ChunkResponse.h"
#include "ChunkTransferRequest.h"
#include "RadioDriver.h"
#include "StatusCodes.h"
#include "TransferReqest.h"
#include "../Logging/Logging.h"

/*- Variables --------------------------------------------------------------*/

/*- Implementations --------------------------------------------------------*/

/*
 @brief Constructor
*/

ATP::ATP(void)
{
	// Coming soon
}

/*
 @brief Initializes ATP modules and data structures
*/

void ATP::Init(void)
{
    Log.Init(LOGLEVEL, baudrate);
    Log.Info("A Transfer Protocol (ATP)"CR);
}

/*
 @brief Used for testing
*/

void ATP::Test(void)
{

	ATP_Init( XBee );
	MakeSampleData( 1028 bytes of data );
	ATP_TransferRequest( sampledata, remotexbee );
	waitTillTransferred();
	done();
	getTime(source);

}
