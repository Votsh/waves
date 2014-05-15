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

/*- Variables --------------------------------------------------------------*/
ATP_linkNode * rootnode;       // Linked list of transfers, chunk requests, chunk responses
ATP_linkNode * conductor;
ATP_linkNode * prevnode;
void * transobj = 0;
int recIndex = 0;
int SDStatus = 0;					// SD card status, 1 = ok, 2 = failed
unsigned long timeout = 0;
unsigned int transID = 1;
char atpHeader[] = "ATP";
TransferRequest tro = TransferRequest();
ChunkRequest creqo = ChunkRequest();
ChunkResponse crespo = ChunkResponse();
RadioDriver rd = RadioDriver();

/*- Implementations --------------------------------------------------------*/

/*
 @brief Initializes ATP modules and data structures
*/

ATP::ATP(void){}

void ATP::begin( char * driverType )
{
    Log.Init(LOGLEVEL, baudrate);
    Log.Info("A Transfer Protocol (ATP)"CR);
    
	rd.setRadioType( "XBEE" );
	if ( rd.getStatus() ) 
	{
		Log.Debug("RD: 1"CR);	
	}
	else
	{
		Log.Debug("RD: 0"CR);	
	}

    timeout=0;
	rootnode = 0;
	conductor = 0;
	prevnode = 0;    
}

/*
 @brief Services requests coming from the mesh network
*/

void ATP::serviceRequests(void)
{
	if ( rd.isAvailable() )
	{
		// Handle timeouts
		timeout++;
		if ( timeout > 60 )
		{
			timeout = 0;
			recIndex = 0;
		}
		
		unsigned int rc = rd.getReceivedInt();
		
		if ( recIndex < 3 )
		{
			if ( rc == (int) atpHeader[ recIndex ] )
			{
				recIndex++;
			}
			else
			{
				recIndex=0;
				return;
			}
		}
		else
		{
			recIndex = 0;
			if ( rc == ATP_TRANSFER_REQUEST )
			{
				// You received a TransferRequest
				Log.Debug(CR"Rec TR"CR);
				
				// Add the TR to the linked list

				ATP_linkNode * lnknode = addNode( sizeof( ATP_TransferRequest_t ) );
				if ( lnknode == 0 ) { return; }
				
				ATP_TransferRequest_t * frame = (ATP_TransferRequest_t *) lnknode->transfer;
				tro.setDefaults( frame, 0 );
				
				int i = 4;
				char * head = (char *)(lnknode->transfer);
				*head++ = 'A';
				*head++ = 'T';
				*head++ = 'P';
				*head++ = ATP_TRANSFER_REQUEST;

				// get the rest of the frame from the radio
				
				timeout = millis() + 10000;		// Wait up to 10 seconds
				while ( millis() < timeout )
				{
					if ( rd.isAvailable() )
					{
						if (i >= sizeof( ATP_TransferRequest_t ) ) { break; }
						*head++ = rd.getReceivedInt();
						i++;
					}
				}
				
				if ( i != sizeof( ATP_TransferRequest_t ) )
				{
					Log.Error("TR Timeout %d of %d"CR, i, sizeof( ATP_TransferRequest_t ) );
					tro.setStatus( frame, ATP_FAILED_DURING_TRANSIT );
					timeout = 0;
					return;
				}
				
				tro.setStatus( frame, ATP_WORKING );
						
				print();
			}
			else if ( rc = ATP_CHUNK_REQUEST )
			{
				// Received a ChunkRequest
				Log.Debug(CR"Rec CRq"CR);
								
				ATP_linkNode * lnknode = addNode( sizeof( ATP_ChunkRequest_t ) );
				if ( lnknode == 0 ) { return; }

				ATP_ChunkRequest_t * frame = (ATP_ChunkRequest_t *) lnknode->transfer;
				creqo.setDefaults( frame, 0 );
				
				//todo: For LWM the frame comes in at once, no getting the rest
				
				int i = 4;
				char * head = (char *)(lnknode->transfer);
				*head++ = 'A';
				*head++ = 'T';
				*head++ = 'P';
				*head++ = ATP_CHUNK_REQUEST;
				
				// get the rest of it
				timeout = millis() + 10000;		// Wait up to 10 seconds
				while ( millis() < timeout )
				{
					if ( rd.isAvailable() )
					{
						if (i > sizeof( ATP_ChunkRequest_t ) ) { break; }
						*head++ = rd.getReceivedInt();
						i++;
					}
				}
				
				if ( i != sizeof( ATP_ChunkRequest_t ) )
				{
					Log.Error("CRQ Timeout %d of %d"CR, i, sizeof( ATP_ChunkRequest_t ) );
					creqo.setStatus( frame, ATP_FAILED_DURING_TRANSIT );
					timeout = 0;
					return;	
				}

				creqo.setStatus( frame, ATP_RECEIVED );
				
				// Create a new ChunkResponse
				
				Log.Debug("New CR"CR);
				ATP_linkNode * lnknodeResponse = addNode( sizeof(ATP_ChunkResponse_t) );
				if ( lnknodeResponse == 0 ) { return; }
				crespo.setDefaults( (ATP_ChunkResponse_t *) lnknodeResponse->transfer, 
				( (ATP_ChunkRequest_t *) lnknode->transfer ) -> atpID );

				// populate the header
				
				( (ATP_ChunkResponse_t *) (lnknode->transfer) ) -> start = 
				( (ATP_ChunkRequest_t *) (lnknode->transfer) ) -> start;
				
				( (ATP_ChunkResponse_t *) (lnknode->transfer) ) -> length = 
				( (ATP_ChunkRequest_t *) (lnknode->transfer) ) -> length;
				
				print();
						
				//todo Support various TransferTypes, right now its just plain/text
				
			}
			else if ( rc = ATP_CHUNK_RESPONSE )
			{
				// Received a ChunkResponse, save the data to the buffer or file
				Log.Debug(CR"Rec CRsp"CR);
			
				//todo if tr = failed, then skip cr
									
				ATP_linkNode * lnknode = addNode( sizeof( ATP_ChunkResponse_t ) );
				if ( lnknode == 0 ) { return; }

				ATP_ChunkResponse_t * frame = (ATP_ChunkResponse_t *) lnknode->transfer;
				crespo.setDefaults( frame, 0 );
				
				//todo: For LWM the frame comes in at once, no getting the rest
				
				int i = 4;
				char * head = (char *)(lnknode->transfer);
				*head++ = 'A';
				*head++ = 'T';
				*head++ = 'P';
				*head++ = ATP_CHUNK_RESPONSE;

				// get the rest of it
				timeout = millis() + 10000;		// Wait up to 10 seconds
				while ( millis() < timeout )
				{
					if ( rd.isAvailable() )
					{
						if (i > sizeof( ATP_ChunkResponse_t ) ) { break; }
						*head++ = rd.getReceivedInt();
						i++;
					}
				}
				
				if ( i != sizeof( ATP_TransferRequest_t ) )
				{
					Log.Error("CRQ Timeout %d of %d"CR, i, sizeof( ATP_ChunkRequest_t ) );
					crespo.setStatus( frame, ATP_FAILED_DURING_TRANSIT );
					timeout = 0;
					return;	
				}
				
				crespo.setStatus( frame, ATP_RECEIVED );
				
				// Find the TransferRequest and buffer or FileName
				int isFile = 1;
				File theFile;
				
				ATP_linkNode * theTR = findNode( ( (ATP_TransferRequest_t *) (lnknode->transfer) ) -> atpID );
				if ( theTR == 0 ) { return; }
				if ( ( (ATP_TransferRequest_t *) (theTR->transfer) ) -> fileName[0]==0 )
				{
					// We are transferring to a buffer
					isFile = 0;
				}
				else
				{
					// We are transfering from a file on the SD card
					isFile = 1;
					theFile = SD.open( ( (ATP_TransferRequest_t *) (theTR->transfer) ) -> fileName, FILE_WRITE );
					theFile.seek( ( (ATP_ChunkResponse_t *) (lnknode->transfer) ) -> start );
				}

				// get the data

				int checksum = 0;
				unsigned long j = 0;
				unsigned int * buf = ( (ATP_TransferRequest_t *) (lnknode->transfer) ) -> buffer;

				while ( timeout++ < ( 1000 * 10 ) )
				{
					if ( rd.isAvailable() )
					{
						if (j > ( ( (ATP_ChunkResponse_t *) (lnknode->transfer) ) -> length ) ) { break; }
						unsigned int v = rd.getReceivedInt();
						checksum = checksum + v;
						
						if ( isFile )
						{
							theFile.write( v );
						}
						else
						{
							*buf++ = v;
						}
					}
				}

				if ( isFile ) theFile.close();
							
				if ( timeout = ( 1000 * 10 ) )
				{
					Log.Error("CRSP timeout 2"CR);
					timeout = 0;
					return;	
				}
				

				// Verify the checksum

				if ( ( (ATP_ChunkResponse_t *) (lnknode->transfer) ) -> verify != checksum )			
				{
					Log.Error("Checksum verify fail"CR);
				}
				//todo if the checksum fails, retry
				
				print();
			}
			else
			{
				Log.Error("Unknown type"CR);
				return;
			}
		}				
	}
}

/*
 @brief Sends requests from linked list
*/

void ATP::sendRequests(void){

	ATP_linkNode * conductor2;

	conductor2 = rootnode;
	
	while ( conductor2 != 0 )
    {
    	ATP_TransferRequest_t * frame = (ATP_TransferRequest_t *) (conductor2->transfer);

		if ( ( tro.getFrameType( frame ) == ATP_TRANSFER_REQUEST ) && ( tro.getStatus( frame ) == ATP_UNSENT ) )
		{
			rd.SendTransferRequest( frame );
			Log.Debug(CR"Send TR %d"CR, frame -> atpID );
		}

		// See if a TransferRequest can send some more requests, from the topchunk value
		// Add throttling code here, for example maximum of 3 chunk requests at a time

		if ( ( tro.getFrameType( frame ) == ATP_TRANSFER_REQUEST ) && ( tro.getStatus( frame ) == ATP_WORKING ) )
		{
			while ( tro.getTopChunk( frame ) < 2 )
			{
				Log.Debug("New CRQ, topchunk = %d"CR, tro.getTopChunk( frame ) );
				ATP_linkNode * lnknode_cr = addNode( sizeof(ATP_ChunkRequest_t) );
				if ( lnknode_cr == 0 )
				{ 
					return;
					//todo: back of this request 
				}
				creqo.setDefaults( (ATP_ChunkRequest_t *) lnknode_cr->transfer, tro.getAtpID( frame ) );
				
				tro.setTopChunk( frame, tro.getTopChunk( frame ) + 1 );

     			//todo: if retrycount++ > maxretry, tr= failed on retry			
			}
		}
	    
		if ( ( (ATP_ChunkRequest_t *) (conductor2->transfer) ) -> frameType == ATP_CHUNK_REQUEST 
		&& ( (ATP_ChunkRequest_t *) (conductor2->transfer) ) -> status == ATP_UNSENT )
		{
			// todo: Add logic to see if chunk request has a matching response within a timeout
			// and if not, retry sending another request			
			// If a threashold of requests did not succeed, then timeout the entire transfer with an error
			
			rd.SendChunkRequest( (ATP_ChunkRequest_t *) conductor2 -> transfer );
			Log.Debug(CR"Send CR %d"CR, ( (ATP_ChunkRequest_t * ) conductor2 -> transfer ) -> atpID );
		}
		          
		if ( ( (ATP_ChunkResponse_t *) (conductor2->transfer) ) -> frameType == ATP_CHUNK_RESPONSE 
		&& ( (ATP_ChunkResponse_t *) (conductor2->transfer) ) -> status == ATP_UNSENT )
		{
			ATP_linkNode * theTR = findNode( ( (ATP_TransferRequest_t *) conductor2->transfer ) -> atpID );
			if ( theTR == 0 ) { return; }
			if ( ( (ATP_TransferRequest_t *) theTR->transfer ) -> fileName[0]==0 )
			{
				// We are transfering from an in-memory buffer
				rd.SendChunkResponseBuffer( (ATP_ChunkResponse_t *) conductor2->transfer, 
				((ATP_TransferRequest_t *) theTR->transfer ) -> buffer );				
				Log.Debug(CR"Send CR (Buffer) %d"CR, ( (ATP_ChunkRequest_t * ) conductor2 -> transfer ) -> atpID );
			}
			else
			{
				// We are transfering from a file on the SD card
				rd.SendChunkResponseFile( (ATP_ChunkResponse_t *) conductor2->transfer, 
				((ATP_TransferRequest_t *) theTR->transfer ) -> fileName );				
				Log.Debug(CR"Send CR (File) %d"CR, ( (ATP_ChunkRequest_t * ) conductor2 -> transfer ) -> atpID );
			}
		}
        conductor2 = conductor2->next;
    }
}

/*
 @brief Shows transfers and status in log
*/

void ATP::print(void){

	int i = 1;
	conductor = rootnode;
		
	while ( conductor != 0 )
	{
		Log.Info(CR"ATP::print %d"CR, i++);
		
		ATP_TransferRequest_t * transobj = (ATP_TransferRequest_t *) conductor->transfer;
		
		if ( transobj->frameType == ATP_TRANSFER_REQUEST )
		{
			tro.print( (ATP_TransferRequest_t *) conductor->transfer ); 	
		}
		else if ( transobj->frameType == ATP_CHUNK_REQUEST )
		{
			creqo.print( (ATP_ChunkRequest_t *) conductor->transfer );			
		}
		else if ( transobj->frameType == ATP_CHUNK_RESPONSE )
		{
			crespo.print( (ATP_ChunkResponse_t *) conductor->transfer );
		}
		else
		{
			Log.Error("Unknown %d"CR,((ATP_TransferRequest_t *) transobj ) -> frameType);
		}
	    conductor = conductor->next;
	}
}

/*
 @brief Go through linked list and remove anything over 10 minutes old
*/
void ATP::garbageCollection(void){
	int i = 1;
	conductor = rootnode;
	prevnode = 0;
	while ( conductor != 0 ) {
		
		Log.Info(CR"ATP: GC %d"CR, i++);
		
		//todo remove only TransferRequests, and then remove all associated ChunkRequests
		//and if we are a receiver then delete the file or buffer
		
		
		ATP_TransferRequest_t * transobj = (ATP_TransferRequest_t *) conductor->transfer;
		if ( transobj->datetime < millis() + (1000*60*10) )
		{
			Log.Debug("GC: Removing %d"CR, ((ATP_TransferRequest_t *) transobj ) -> atpID );
			if (prevnode==0)
			{
				// Deleting the rootnode
				free( (ATP_TransferRequest_t *) conductor->transfer );
				free( rootnode );
				rootnode = 0;
				return;	
			}
			else
			{
				// Deleting the conductors node
				prevnode->next = conductor->next;
				//todo free the data buffer in the request object
				free( (ATP_TransferRequest_t *) conductor->transfer );
				free( conductor );
				conductor=prevnode->next;
			}
		}
		else
		{
			prevnode = conductor;
		    conductor = conductor->next;
		}
	}	
	
}

/*
 @brief Initiate transfer requests randomly
*/

void ATP::initiateTransferRequest(void)
{
	Log.Info("New TR"CR);

	ATP_linkNode * lnknode = addNode( sizeof(ATP_TransferRequest_t) );
	if ( lnknode == 0 ) { return; }
	ATP_TransferRequest_t * frame = (ATP_TransferRequest_t *) lnknode->transfer;
	tro.setDefaults( frame, transID++ );
	tro.setFileName( frame, "FRANK.OGG" );
	tro.setSizeFromFile( frame );	
	tro.setChunkCount( frame, tro.getSize( frame ) / CHUNKSIZE );
	Log.Debug("TR: %d chunks, %d bytes"CR, tro.getChunkCount( frame ), CHUNKSIZE );	
	
	print();
	
	timeout = 0;
}

/*
 @brief Add the new node to the end of the list
*/

ATP_linkNode * ATP::addNode( unsigned long size ){

	// Create a new node for the linked list
	ATP_linkNode * lnknodex = (ATP_linkNode *) malloc( sizeof(ATP_linkNode) );
	if (lnknodex == 0)
	{
		Log.Error("OOM"CR);
		return 0;
	}
	memset( lnknodex, 0, sizeof(ATP_linkNode) ); 

	// Create a new request frame for the linked list
	ATP_TransferRequest_t * framex = (ATP_TransferRequest_t *) malloc( size );
	if (framex == 0)
	{
		Log.Error("OOM-2"CR);
		return 0;
	}
	memset( framex, 0, size ); 

	lnknodex->transfer = framex;
	lnknodex->next = 0;
	
	if ( rootnode == 0 )
	{
		rootnode = lnknodex;
	}
	else
	{
		conductor = rootnode;
		prevnode = conductor;
		while ( conductor != 0 )
        {
	        prevnode = conductor;
            conductor = conductor->next;
        }
        prevnode->next = lnknodex;
	}
	return lnknodex;	
}

/*
 @brief Find a TransferRequest node by atpID
*/

ATP_linkNode * ATP::findNode( unsigned long id ){
	
	if ( rootnode == 0 )
	{
		return 0;
	}
	else
	{
		conductor = rootnode;
		while ( conductor->next != 0)
        {
        	if ( ( ( (ATP_TransferRequest_t *) conductor ) -> atpID == id ) &&
        	( ( (ATP_TransferRequest_t *) conductor ) -> frameType == ATP_TRANSFER_REQUEST ) )
        	{ return conductor; }
            conductor = conductor->next;
        }
		return 0;
	}

}



