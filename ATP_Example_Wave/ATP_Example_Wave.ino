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
#include <Adafruit_VS1053.h>

ATP atp = ATP();
#define baudrate 9600
int firsttime = 1;

#define TESTType 0

// Adafruit VS1053 Music Codec breakout board
//#define CLK 13       // SPI Clock, shared with SD card
//#define MISO 12      // Input data, from VS1053/SD card
//#define MOSI 11      // Output data, to VS1053/SD card
#define RESET 9      // VS1053 reset pin (output)
#define CS 10        // VS1053 chip select pin (output)
#define DCS 8        // VS1053 Data/command select pin (output)
#define DREQ 3       // VS1053 Data request pin (into Arduino) 
#define CARDCS 4     // Card chip select pin

Adafruit_VS1053_FilePlayer musicPlayer = Adafruit_VS1053_FilePlayer(RESET, CS, DCS, DREQ, CARDCS);

unsigned long trcounter;
unsigned long gccounter;
int dotcount;

void setup() {
  Serial.begin(baudrate);
   // while the serial stream is not open, do nothing:
   while (!Serial) ;
   Serial.println("WAVE");
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
    setupMusic();

    delay(3000);
    //atp.initiateTransferRequest();

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

void setupMusic(){
    Log.Info("Initialize the music player"CR);
    if (!musicPlayer.begin()) {
      Serial.println("VS1053 not found");
    }

    Log.Debug("Make a tone to indicate VS1053 is working"CR);
    musicPlayer.sineTest(0x44, 500);
    delay(40);

    Log.Debug("Initialize SD card on the VS1053"CR);   
    if (!SD.begin(CARDCS)) {
      Log.Error("SD failed, or not present"CR);
      while (1);  // don't do anything more
    }
    Log.Debug("SD card ok"CR);
    
    // Set volume for left, right channels. lower numbers == louder volume!
    Log.Debug("Setting volume"CR);
    musicPlayer.setVolume(20,20);

    if (! musicPlayer.useInterrupt( VS1053_FILEPLAYER_PIN_INT ))  //DREQ int, VS1053_FILEPLAYER_PIN_INT
      Log.Error("DREQ pin is not an interrupt pin"CR);
  }
