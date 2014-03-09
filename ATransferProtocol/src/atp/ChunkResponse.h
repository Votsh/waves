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

#ifndef _CR_H_
#define _CR_H_


/*- Includes ---------------------------------------------------------------*/

#include "ChunkResponse.h"
#include "ChunkRequest.h"
#include "RadioDriver.h"
#include "StatusCodes.h"
#include "TransferReqest.h"

/*- Definitions ------------------------------------------------------------*/

/*- Types ------------------------------------------------------------------*/

typedef struct ATP_ChunkResponse_t
{
  uint16_t     AUID;
  uint8_t      Status;
  uint32_t     Start;
  uint16_t     Length;
  uint8_t      TransferTypes;
  uint8_t      Verify;
  uint8_t      Data[];
} ATP_ChunkResponse_t;

# AUID - 16 bit value, a unique identifier from Transfer Request, , with the first 2 bits set at 1,0 identifying the response type# Status - Success, Failed during transit, failed checksum# Start byte offset - 32 bit unsigned value, starting address of transfer# Length - 16 bit unsigned value, quantity of 8 bit unsigned values to transfer, if compression is used the length is the quantity of values after compression# Transfer types - Four 2-bit values packed into 1 8-bit unsigned value:CT - Compression technique - compression: 0 = none, 1 = RTE, 2 = LempelET - Encryption technique - 0 = none, 1 = encryption engine 1, 2 engine 2, etc.VT - Verify technique - 0 = 8 bit checksumUnused# Verify - 8 bit unsigned value# Data - series of 8 bit unsigned integer values

typedef enum
{
  ATP_SUCCESS                             = 0x00,
  ATP_FAILED_DURING_TRANSIT               = 0x01,
  ATP_FAILED_CHECKSUM                     = 0x02,
  ATP_FAILED_ENCRYPTION                   = 0x03,
  ATP_FAILED_COMPRESSION                  = 0x04
} ATP_Status_t;

typedef enum
{
  ATP_NONE                                = 0x00,
  ATP_RTE                                 = 0x01,
  ATP_Lempel                              = 0x02	
} ATP_Compression_Technique;

typedef enum
{
  ATP_NONE                                = 0x00,
  ATP_ENGINE1                             = 0x01,
  ATP_ENGINE2                             = 0x02
} ATP_Encryptiong_Technique;

typedef enum
{
  ATP_NONE                                = 0x00,
  ATP_CHECKSUM                            = 0x01	
} ATP_Verify_Technique;

/*- Variables --------------------------------------------------------------*/
/*- Prototypes -------------------------------------------------------------*/


#endif
