/*

Scout Syslog LWM Message Driver

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

*/

#ifndef _LWMDriver_H_
#define _LWMDriver_H_

#include "lwm/nwk/nwkDataReq.h"
#include "lwm/nwk/nwk.h"

#ifdef __cplusplus
extern "C"{
#endif

NWK_DataReq_t *message;

void sendConfirm( NWK_DataReq_t *req );

void sendLWMMsg( const char * data, int destination_mesh_id );

bool receiveMessage(NWK_DataInd_t *ind);

char * printStatus( int status );

void LWMDriverInit();

bool LWMMessageWaiting();

char * LWMGetMessage();

void LWMGC();

#ifdef __cplusplus
} // extern "C"
#endif

#endif

