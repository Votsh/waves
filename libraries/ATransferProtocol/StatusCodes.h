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


#ifndef _SC_H_
#define _SC_H_

/*- Includes ---------------------------------------------------------------*/

#include "ChunkResponse.h"
#include "ChunkTransferRequest.h"
#include "RadioDriver.h"
#include "StatusCodes.h"
#include "TransferReqest.h"

/*- Definitions ------------------------------------------------------------*/

typedef enum
{
  ATP_SUCCESS_STATUS                      = 0x00,
  ATP_ERROR_STATUS                        = 0x01,
  ATP_OUT_OF_MEMORY_STATUS                = 0x02,

  ATP_NO_ACK_STATUS                       = 0x10,
  ATP_NO_ROUTE_STATUS                     = 0x11,

  ATP_PHY_CHANNEL_ACCESS_FAILURE_STATUS   = 0x20,
  ATP_PHY_NO_ACK_STATUS                   = 0x21,
} ATP_Status_t;

/*
ATP_SUCCESS - Operation successfulATP_ERROR_STATUS - Unknown errorATP_OUT_OF_MEMORY_STATUS - Buffer allocation failedATP_NO_ACK_STATUS - Network level acknoledgement not receivedATP_NO_ROUTE_STATUS - Route to destination address not foundATP_PHY_CHANNEL_ACCESS_FAILURE_STATUS - Radio failed to gain access to channelATP_PHY_NO_ACK_STATUS - Physical level acknowledgement was not received
*/

/*- Types ------------------------------------------------------------------*/
/*- Variables --------------------------------------------------------------*/
/*- Prototypes -------------------------------------------------------------*/

#endif
