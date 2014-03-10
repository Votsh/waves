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

#ifndef _TR_H_
#define _TR_H_

/*- Includes ---------------------------------------------------------------*/

#include <stdint.h>
#include "ChunkResponse.h"
#include "ChunkRequest.h"
#include "RadioDriver.h"
#include "StatusCodes.h"
#include "TransferReqest.h"

/*- Definitions ------------------------------------------------------------*/

/*- Types ------------------------------------------------------------------*/

typedef struct ATP_TransferRequest_t
{
  uint16_t     AUID;
  uint8_t      Version;
  uint32_t     Size;
  uint32_t     Time;
  uint16_t     Expires;
  char         Descriptor[6];
  uint64_t     Source;
  uint64_t     Destination;
  char         FileName[17];
} ATP_TransferRequest_t;

/*

# AUID - A Unique ID, 16 bit value, hash of the request type (first 2 bits set at 0,0), PAN ID, current time in milliseconds, and count of TransferRequest objects created. While a hash introduces the possibility of collissions, AUID is a hash for situations where a peer crashed or went offline temporarily during a transfer session.# Version - 8 bit value, version of the request schema# Size - 32 bit unsigned value, number of 8 bit unsigned bytes to transfer# Time - 32 bit unsigned value, milliseconds since January 1, 1970 00:00:00 GMT# Expires - 16 bit unsigned value number of milliseconds after Time value# Descriptor - 5 character array of abbreviated MIME types, see http://www.abbreviations.com/acronyms/MIMETYPES# Source - serving resource (Mesh, Point, URL), if mesh then the Source is the PAN value, used only by the receiving peer# Destination - consuming resource (Mesh, Point, URL), if mesh then the Destination is the PAN value, used only by the sending peer# FileName - 16 character file name with null terminator, or null if store to memory buffer

*/

// Request type for AUID hash bits 0 and 1

typedef enum
{
  ATP_TRANSFER_REQUEST                    = 0x00,
  ATP_CHUNK__REQUEST                      = 0x01,
  ATP_CHUNK_RESPONSE                      = 0x02,	
} ATP_RequestType_t;

/*- Variables --------------------------------------------------------------*/
/*- Prototypes -------------------------------------------------------------*/



#endif

