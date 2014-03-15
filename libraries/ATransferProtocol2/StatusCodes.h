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

#include "RadioDriver.h"

/*- Definitions ------------------------------------------------------------*/

/*
ATP_SUCCESS - Operation successfulATP_ERROR_STATUS - Unknown errorATP_OUT_OF_MEMORY_STATUS - Buffer allocation failedATP_NO_ACK_STATUS - Network level acknoledgement not receivedATP_NO_ROUTE_STATUS - Route to destination address not foundATP_PHY_CHANNEL_ACCESS_FAILURE_STATUS - Radio failed to gain access to channelATP_PHY_NO_ACK_STATUS - Physical level acknowledgement was not received
*/

typedef enum
{
  ATP_IDLE								  = 0x00,
  ATP_SUCCESS                             = 0x01,
  ATP_FAILED_DURING_TRANSIT               = 0x02,
  ATP_FAILED_CHECKSUM                     = 0x03,
  ATP_FAILED_ENCRYPTION                   = 0x04,
  ATP_FAILED_COMPRESSION                  = 0x05
} ATP_Status_t;

typedef enum
{
  ATP_NONE_1                              = 0x00,
  ATP_RTE                                 = 0x01,
  ATP_Lempel                              = 0x02	
} ATP_Compression_Technique;

typedef enum
{
  ATP_NONE_2                              = 0x00,
  ATP_ENGINE1                             = 0x01,
  ATP_ENGINE2                             = 0x02
} ATP_Encryptiong_Technique;

typedef enum
{
  ATP_NONE_3                              = 0x00,
  ATP_CHECKSUM                            = 0x01	
} ATP_Verify_Technique;

/*- Types ------------------------------------------------------------------*/
/*- Variables --------------------------------------------------------------*/
/*- Prototypes -------------------------------------------------------------*/

#endif
