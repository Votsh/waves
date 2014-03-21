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

#include "Arduino.h"
#include "ATP.h"
#include "Logging.h"
#include <SoftwareSerial.h>
#include <SD.h>
#include <SPI.h>

ATP atp = ATP();
#define baudrate 9600
int firsttime = 1;

#define TESTType 0

unsigned long trcounter;
unsigned long gccounter;
int dotcount;

void setup() {
  Serial.begin(baudrate);
   // while the serial stream is not open, do nothing:
   while (!Serial) ;
}

void loop() {

  if (firsttime==1)
  {
    trcounter = millis() + 10000;
    gccounter = millis() + 30000;
    firsttime=0;
    dotcount=0;
    delay(3000);
    atp.begin("XBEE");

    delay(3000);
    atp.initiateTransferRequest();

  }

  atp.serviceRequests();

  atp.sendRequests();

  if ( millis() > trcounter )
  {
    //atp.initiateTransferRequest();
    trcounter = millis() + 30000;
  }
  
  // Every 10 seconds
  if ( millis() > gccounter )
  {
    //atp.garbageCollection();
    atp.print();
    gccounter = millis() + 10000;
  }
  
  if (dotcount++>10) { Log.Debug("."CR); dotcount=0;} else Log.Debug(".");
  delay(1000);
}
