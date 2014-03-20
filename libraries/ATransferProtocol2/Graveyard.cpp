/*
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
					
					memset( ( (ATP_TransferRequest_t *) (lnknode->transfer) ) -> buffer, 0, 
					(ATP_TransferRequest_t *) (lnknode->transfer) ) -> size );
					
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
*/