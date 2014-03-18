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
ATP_linkNode * rootnode = 0;       // Linked list of transfers, chunk requests, chunk responses
ATP_linkNode * conductor = 0;
ATP_linkNode * prevnode = 0;
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

    if (!SD.begin( SD_CARD_CS )) {
      Log.Error("SD fail"CR);
      SDStatus = 0;
      return;
    }
    else
    {
      Log.Debug("SD: OK"CR);
      SDStatus = 1;
    }

    timeout=0;
}

/*
 @brief Services requests coming from the mesh network
*/

void ATP::serviceRequests(void)
{
	if ( rd.isAvailable() )
	{
		Log.Debug("t%d"CR,timeout);

		// Handle timeouts
		timeout++;
		if ( timeout > 60 )
		{
			timeout = 0;
			recIndex = 0;
			Log.Debug("+");
		}
		
		unsigned int rc = rd.getReceivedInt();
		
		Log.Debug("!%c-%c-%i"CR,rc,atpHeader[ recIndex ], recIndex);

		
		if ( recIndex < 3 )
		{
			if ( rc == (int) atpHeader[ recIndex ] )
			{
				recIndex++;
							Log.Debug("#");

			}
			else
			{
				recIndex=0;			Log.Debug("@");

				return;
			}
		}
		else
		{
			recIndex = 0;
			if ( rc == ATP_TRANSFER_REQUEST )
			{
				// You received a TransferRequest
				Log.Debug("Rec TR"CR);
				
				// Add the TR to the linked list

				ATP_linkNode * lnknode = addNode();
				if ( lnknode == 0 ) { return; }
								
				int i = 4;
				char * head = (char *)(lnknode->transfer);
				*head++ = 'A';
				*head++ = 'T';
				*head++ = 'P';
				*head++ = ATP_TRANSFER_REQUEST;

				// get the rest of the frame from the radio

				Log.Debug("sizeof %d"CR, sizeof( ATP_TransferRequest_t ));
				
				timeout = millis() + 10000;		// Wait up to 10 seconds
				while ( millis() < timeout )
				{
					if ( rd.isAvailable() )
					{
						if (i > sizeof( ATP_TransferRequest_t ) ) { break; }
						*head++ = rd.getReceivedInt();
						Log.Debug("i=%d"CR,i);
					}
				}
				
				if ( millis() >= timeout )
				{
					Log.Error("TR Timeout"CR);
					timeout = 0;
					return;	
				}
				
				// create a buffer (malloc) or a file (and fill it with zeroes), close file
				
				if ( ( (ATP_TransferRequest_t *) (lnknode->transfer) ) -> fileName[0]==0 )
				{
					// We're going to transfer into an in-memory buffer
					
					Log.Debug( "Allocate %d"CR, ( (ATP_TransferRequest_t *) (lnknode->transfer) ) -> size );
					( (ATP_TransferRequest_t *) (lnknode->transfer) ) -> buffer = 
					(unsigned int *) malloc( ( (ATP_TransferRequest_t *) (lnknode->transfer) ) -> size );
					if ( ( (ATP_TransferRequest_t *) (lnknode->transfer) ) -> buffer == 0 )
					{
						Log.Error("TR fail"CR);
						return;						
					}
				}
				else
				{
					// We're going to transfer into a file on the SD card
					// create the file and populate it with 0 bytes of the TR size

					if ( SDStatus == 1 )
					{
						File myFile = SD.open( ( (ATP_TransferRequest_t *) (lnknode->transfer) ) -> 
						fileName, FILE_WRITE);
						
						if ( myFile == 0 ){
							Log.Error("File error %s"CR, ( (ATP_TransferRequest_t *) (lnknode->transfer) ) -> 
							fileName );
							return;
						}
											
						for ( unsigned long j=0; j < ( (ATP_TransferRequest_t *) (lnknode->transfer) ) ->
						size; j++ )
						{
							myFile.print( 0 );	
						}
	
	  					myFile.close();
					}
					else
					{
						Log.Error("SD unavailable"CR);
					}
				}
				
				// Calculate the chunk size
				// and put the correct number of chunks onto the linked list
				
				double chunk_count = ( ( (ATP_TransferRequest_t *) (lnknode->transfer) ) -> size ) / CHUNKSIZE;

				Log.Debug("TR: %d chunks, %d bytes"CR, chunk_count, CHUNKSIZE );
				
				//Add the number of ChunkRequests to the linked list

				for ( int k = 0; k<chunk_count; k++ )
				{
					ATP_ChunkRequest_t * ncr = creqo.getNewRequest( ( (ATP_TransferRequest_t *) (lnknode->transfer) ) -> atpID );
					ATP_linkNode * lnknode = addNode();
					if ( lnknode == 0 ) { return; }
					lnknode->transfer = ncr;
					Log.Debug("ATP: New CRQ"CR);
				}

				//todo Back out of the ChunkRequests if a malloc fails

			}
			else if ( rc = ATP_CHUNK_REQUEST )
			{
				// Received a ChunkRequest
				Log.Debug("Rec CRq"CR);
								
				timeout = 0;
				
				ATP_linkNode * lnknode = addNode();
				if ( lnknode == 0 ) { return; }
				
				//todo: For LWM the frame comes in at once, no getting the rest
				
				int i = 0;
				char * head = (char *)(lnknode->transfer);
				*head++ = 'A';
				*head++ = 'T';
				*head++ = 'P';
				*head++ = ATP_CHUNK_REQUEST;
				
				// get the rest of it
				while ( timeout++ < ( 1000 * 10 ) )
				{
					if ( rd.isAvailable() )
					{
						if (i > sizeof( ATP_ChunkRequest_t ) - 4 ) { break; }
						*head++ = rd.getReceivedInt();
					}
				}
				
				if ( timeout = ( 1000 * 10 ) )
				{
					Log.Error("CRQ timeout"CR);
					timeout = 0;
					return;	
				}
				
				// Create a new ChunkResponse
				
				ATP_ChunkResponse_t * ncr = crespo.getNewRequest( ( (ATP_TransferRequest_t *) 
				(lnknode->transfer) ) -> atpID );
				ATP_linkNode * lnknodeResponse = addNode();
				if ( lnknodeResponse == 0 ) { return; }
				lnknodeResponse->transfer = ncr;
				Log.Debug("Added new ChunkResponse"CR);

				// populate the header
				
				( (ATP_ChunkResponse_t *) (lnknode->transfer) ) -> start = 
				( (ATP_ChunkRequest_t *) (lnknode->transfer) ) -> start;
				
				( (ATP_ChunkResponse_t *) (lnknode->transfer) ) -> length = 
				( (ATP_ChunkRequest_t *) (lnknode->transfer) ) -> length;

				// Find the TransferRequest and buffer or FileName
				
				ATP_linkNode * theTR = findNode( ( (ATP_TransferRequest_t *) (lnknode->transfer)) -> atpID );
				if ( theTR == 0 ) { return; }
				if ( ( (ATP_TransferRequest_t *) (lnknode->transfer) ) -> fileName[0] == 0 )
				{
					// We are transfering from an in-memory buffer
					rd.SendChunkResponseBuffer( ncr, ( (ATP_TransferRequest_t *) theTR -> transfer ) -> buffer );				
				}
				else
				{
					// We are transfering from a file on the SD card
					rd.SendChunkResponseFile( ncr, ( (ATP_TransferRequest_t *) theTR -> transfer ) -> fileName );				
				}
								
				//todo Support various TransferTypes, right now its just plain/text
				
			}
			else if ( rc = ATP_CHUNK_RESPONSE )
			{
				// Received a ChunkResponse, save the data to the buffer or file
				Log.Debug("Rec CRsp"CR);
				
				timeout = 0;
				
				Log.Debug("ATP: Recv CRSP"CR);
				
				ATP_linkNode * lnknode = addNode();
				if ( lnknode == 0 ) { return; }
				
				//todo: For LWM the frame comes in at once, no getting the rest
				
				int i = 0;
				char * head = (char *)(lnknode->transfer);
				*head++ = 'A';
				*head++ = 'T';
				*head++ = 'P';
				*head++ = ATP_CHUNK_RESPONSE;

				// get the rest of it
				while ( timeout++ < ( 1000 * 10 ) )
				{
					if ( rd.isAvailable() )
					{
						if (i > sizeof( ATP_ChunkResponse_t ) - 4 ) { break; }
						*head++ = rd.getReceivedInt();
					}
				}
				
				if ( timeout = ( 1000 * 10 ) )
				{
					Log.Error("CRSP Timeout"CR);
					timeout = 0;
					return;	
				}
				
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

	if ( rootnode != 0 )
	{
		conductor = rootnode;
		while ( conductor->next != 0)
        {
            conductor = conductor->next;
             
			if ( ( (ATP_TransferRequest_t *) (conductor->transfer) ) -> frameType == 
			ATP_TRANSFER_REQUEST && ( (ATP_TransferRequest_t *) (conductor->transfer) ) -> status == ATP_IDLE )
			{
				rd.SendTransferRequest( (ATP_TransferRequest_t * ) conductor -> transfer );
			}
            
			if ( ( (ATP_TransferRequest_t *) (conductor->transfer) ) -> frameType == 
			ATP_CHUNK_REQUEST && ( (ATP_TransferRequest_t *) (conductor->transfer) ) -> status == ATP_IDLE )
			{
				rd.SendChunkRequest( (ATP_ChunkRequest_t *) conductor -> transfer );
			}
			          
			if ( ( (ATP_TransferRequest_t *) (conductor->transfer) ) -> frameType == 
			ATP_CHUNK_RESPONSE && ( (ATP_TransferRequest_t *) (conductor->transfer) ) -> status == ATP_IDLE )
			{
				ATP_linkNode * theTR = findNode( ( (ATP_TransferRequest_t *) conductor->transfer ) -> atpID );
				if ( theTR == 0 ) { return; }
				if ( ( (ATP_TransferRequest_t *) theTR->transfer ) -> fileName[0]==0 )
				{
					// We are transfering from an in-memory buffer
					rd.SendChunkResponseBuffer( (ATP_ChunkResponse_t *) conductor->transfer, 
					((ATP_TransferRequest_t *) theTR->transfer ) -> buffer );				
				}
				else
				{
					// We are transfering from a file on the SD card
					rd.SendChunkResponseFile( (ATP_ChunkResponse_t *) conductor->transfer, 
					((ATP_TransferRequest_t *) theTR->transfer ) -> fileName );				
				}
			}
        }
	}
}

/*
 @brief Shows transfers and status in log
*/

void ATP::print(void){

	int i = 1;
	conductor = rootnode;
	while ( conductor != NULL ) {
		
		Log.Info("ATP::print %d"CR, i++);
		
		void * transobj = (ATP_TransferRequest_t *) conductor->transfer;
		if ( ( (ATP_TransferRequest_t *) transobj ) ->frameType == ATP_TRANSFER_REQUEST )
		{
			tro.print( (ATP_TransferRequest_t *) conductor->transfer ); 	
		}
		else if ( ( (ATP_ChunkRequest_t *) transobj ) ->frameType == ATP_CHUNK_REQUEST )
		{
			creqo.print( (ATP_ChunkRequest_t *) conductor->transfer );			
		}
		else if ( ( (ATP_ChunkResponse_t *) transobj ) ->frameType == ATP_CHUNK_RESPONSE )
		{
			crespo.print( (ATP_ChunkResponse_t *) conductor->transfer );
		}
		else
		{
			Log.Error("Unknown %d"CR,((ATP_TransferRequest_t *) transobj ) ->frameType);
		}

	    conductor = conductor->next;
	}	
	
}

/*
 @brief Delete transfers older than ten minutes
*/
void ATP::garbageCollection(void){
	/*
	go through linked list and remove anything over 10 minutes old
	*/
	
	int i = 1;
	conductor = rootnode;
	prevnode = 0;
	while ( conductor != NULL ) {
		
		Log.Info("ATP: GC %d"CR, i++);
		
		//todo remove only TransferRequests, and then remove all associated ChunkRequests
		//and if we are a receiver then delete the file or buffer
		
		
		ATP_TransferRequest_t * transobj = (ATP_TransferRequest_t *) conductor->transfer;
		if ( transobj->datetime < millis() + (1000*60*10) )
		{
			Log.Debug("GC: Removing %d"CR, ((ATP_TransferRequest_t *) transobj ) -> atpID );
			if (prevnode==0)
			{
				// Deleting the rootnode
				free( rootnode );
				rootnode = 0;
				return;	
			}			
			else
			{
				// Deleting the conductors node
				prevnode->next = conductor->next;				
				//todo free the data buffer in the request object
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
	Log.Info("initiateTransferRequest"CR);

	ATP_linkNode * lnknode = addNode();
	if ( lnknode == 0 ) { return; }

	ATP_TransferRequest_t * rq = tro.getNewRequest( transID++ );
	tro.setExpires( rq, 60000 );	// expires in 60 seconds
	tro.setSize( rq, 16 );
	tro.setFileName( rq, "frank.txt" );
	tro.setBuffer( rq, (unsigned int *) "Greetings oh brave new world" );
	
	// If you are sending a buffer over the radio, use this:
	
	/*
	unsigned int * thebuffer = (unsigned int *) malloc( CHUNKSIZE * 3 );
	if ( thebuffer == 0 )
	{
		Log.Error( "Allocate buffer fail"CR );
		return;
	}
	rq.setBuffer( thebuffer );
	*/
		
	// Otherwise, just set the FileName and go
	// rq.setFileName("myfilename.txt");
	tro.print( rq );

	lnknode->transfer = rq;
	lnknode->next = 0;

	rd.SendTransferRequest( rq );
	timeout=0;

	Log.Debug("ATP: TRQ sent"CR);	
}

/*
 @brief Add the new node to the end of the list
*/

ATP_linkNode * ATP::addNode(){

	// Create a new node for the linked list
	ATP_linkNode * lnknode = (ATP_linkNode *) malloc( sizeof(ATP_linkNode) );
	if (lnknode == 0)
	{
		Log.Error("OOM"CR);
		return 0;
	}
	
	if ( rootnode == 0 )
	{
		rootnode=lnknode;
	}
	else
	{
		conductor = rootnode;
		while ( conductor->next != 0)
        {
            conductor = conductor->next;
        }
        conductor->next = lnknode;
	}
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



