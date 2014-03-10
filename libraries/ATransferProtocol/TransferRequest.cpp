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

/*

TransferRequest provides a "push" from the lead peer to a peer. On the lead
peer the methods initiate the request to the peer. On the receiving peer, the
methods provide the App with malloc-style access to the transferred data.

See the TransferRequest.h file for definitions of the structs used by the methods.

*/


/*- Includes ---------------------------------------------------------------*/
/*- Variables --------------------------------------------------------------*/
/*- Implementations --------------------------------------------------------*/


getAUID()setAUID(value)setAUID() - automatically creates AUID based on time, random, and PAN addresssetSize()getSize()setTime()getTime()setExpires()getExpires()setVersion()getVersion()setContentDescriptor() - description of content 0 = binary, 1 = video, 2 = ogg, 4 = text, 5 = JPG, 6 = PNGgetContentDescriptor() - returns description of content 0 = binary, 1 = video, 2 = ogg, 4 = text, 5 = JPG, 6 = PNGsetSource()getSource()setDestination()getDestination()getStatus() - returns status codegetError() - returns error codeisTransferToBuffer() - returns true if transfer is to in-memory buffer, returns false if transfer is to cardsetFileName() - 16 character file name, or null if store to memory buffergetFileName()send() - sends request to the destinationdelete() - removes object and related datagetBufferAddress() - returns address of the in-memory buffer, meaningful only to the peerisTransferComplete() - returns true if all data transffered, false if still transferring datagetTransferStatus() - returns first node of a linked-list of a entries showing which portions of the transfer are complete, meaningful only to the receiving peergetTransferSize() - returns a 16 bit value defining the number of 8-bit unsigned integer values in the transfer



