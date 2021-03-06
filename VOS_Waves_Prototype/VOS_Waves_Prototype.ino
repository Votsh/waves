/*

Votsh Operation System (VOS), Version: Waves

This is the Waves release of the Votsh Operating System. Waves is 
a table top computing platform that uses color, light, and animation.
VOS provides a distributed engine to connect and operate Waves devices.
Details on Waves is found at http://www.votsh.com

VOS Waves Prototype delivers these objects:

Main sketch for Waves to run on Arduino Micro

Copyright (c) 2014 Frank Cohen. All rights reserved.
GPL v3 license:
This file is part of Votsh Operating System (VOS).
VOS is free software: you can redistribute it and/or modify
it under the terms of the GNU General Public License as published by
the Free Software Foundation, either version 3 of the License, or
(at your option) any later version.
VOS is distributed in the hope that it will be useful,
but WITHOUT ANY WARRANTY; without even the implied warranty of
MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
GNU General Public License for more details.
You should have received a copy of the GNU General Public License
along with VOS.  If not, see <http://www.gnu.org/licenses/>.

This version of VOS includes use of the following libraries:

This version of VOS links to Adafruit_NeoPixel library. Adafruit_NeoPixel
library is distributed by Adafruit under a GPL v3 license. VOS makes no
modifications to Adafruit_NeoPixel. Adafruit_NeoPixelis an Arduino library
to control a wide variety of WS2811- and WS2812-based RGB LED devices such 
as Adafruit FLORA RGB Smart Pixels and NeoPixel strips. Details at:
http://learn.adafruit.com/adafruit-neopixel-uberguide/overview

Arduino Logging Library, similar to log4j
http://playground.arduino.cc/Code/Logging
with a modification to support %f
*/

#include <Logging.h>
#include <Adafruit_NeoPixel_Votsh.h>
#include <SPI.h>
#include <SD.h>
#include <Adafruit_VS1053.h>
#include <SoftwareSerial.h>

#define baudrate 9600

#define LOGLEVEL LOG_LEVEL_DEBUG

// Not all pins on the Leonardo support change interrupts,
// so only the following can be used for RX:
// 8, 9, 10, 11, 14 (MISO), 15 (SCK), 16 (MOSI).

// For XBee mesh and point-to-point radio network
SoftwareSerial XBeeOnBreadboard(11, 9); // RX, TX

// For VS1053 Codec audio Adafruit breakout board

// define the pins used
//#define CLK 13       // SPI Clock, shared with SD card
//#define MISO 12      // Input data, from VS1053/SD card
//#define MOSI 11      // Output data, to VS1053/SD card
//#define RESET 13      // VS1053 reset pin (output)
//#define CS 10        // VS1053 chip select pin (output)
//#define DCS 8        // VS1053 Data/command select pin (output)
//#define DREQ 2       // VS1053 Data request pin (into Arduino)
//#define CARDCS 6     // Card chip select pin
#define RESET 9      // VS1053 reset pin (output)
#define CS 10        // VS1053 chip select pin (output)
#define DCS 8        // VS1053 Data/command select pin (output)
#define DREQ 3       // VS1053 Data request pin (into Arduino) 
#define CARDCS 4     // Card chip select pin

Adafruit_VS1053_FilePlayer musicPlayer = Adafruit_VS1053_FilePlayer(RESET, CS, DCS, DREQ, CARDCS);

// change this to match your SD shield or module;
// Arduino Ethernet shield: pin 4
// Adafruit SD shields and modules: pin 10
// Sparkfun SD shield: pin 8
const int chipSelect = CARDCS;

// Parameter 1 = number of pixels in strip
// Parameter 2 = pin number (most are valid)
// Parameter 3 = pixel type flags, add together as needed:
//   NEO_KHZ800  800 KHz bitstream (most NeoPixel products w/WS2812 LEDs)
//   NEO_KHZ400  400 KHz (classic 'v1' (not v2) FLORA pixels, WS2811 drivers)
//   NEO_GRB     Pixels are wired for GRB bitstream (most NeoPixel products)
//   NEO_RGB     Pixels are wired for RGB bitstream (v1 FLORA pixels, not v2)

#define WS2812_PIN 22

Adafruit_NeoPixel_Votsh strip = Adafruit_NeoPixel_Votsh(16, WS2812_PIN, NEO_GRB + NEO_KHZ800);

int firsttime = 1;

void setup() {
  Serial.begin(baudrate);
   // while the serial stream is not open, do nothing:
   while (!Serial) ;
}

int myval = 260;

void loop() {

  // We do our out setup method to allow the setup functions 
  // to send debugging messages to the serial monitor
  if (firsttime==1){
    firsttime=0;
    mySetup();
    setupMusic();    
  }
  
  //Serial.print("looping ");
  //Serial.println(myval);

  myval = myval + 10;
  for (int i=0; i<8; i++){
    strip.setPixelColor(i*2, myval);
  }
  strip.show();
  
  //XBeeOnBreadboard.println("Hello, world.  ");

/*

  if (XBeeOnBreadboard.available())
    Serial.write(XBeeOnBreadboard.read());
  if (Serial.available())
    XBeeOnBreadboard.write(Serial.read());
*/
  delay(100);
}

void mySetup(){
    delay(5000);  // Wait for faux USB driver to catch up

    //Serial.println(F("Waiting for 1 character"));
    //while(Serial.read() == -1);

    Log.Init(LOGLEVEL, baudrate);
    Log.Info("---------------------------------"CR);
    Log.Info("VOS Waves"CR);
    Log.Info("---------------------------------"CR);
    
    pinMode(WS2812_PIN, OUTPUT);
    strip.begin();
    Log.Debug(CR"Started WS2812 lights"CR);
    /*
    setupMusic();
    
    XBeeOnBreadboard.begin(9600);
    XBeeOnBreadboard.println("Hello, world?");
    */
  }
  
  void setupMusic(){
    Log.Info("Initialize the music player"CR);
    if (!musicPlayer.begin()) {
      Serial.println("VS1053 not found");
      return;
    }

    Log.Debug("Make a tone to indicate VS1053 is working"CR);
    musicPlayer.sineTest(0x44, 500);
    delay(4000);

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
      
    Log.Info("Playing music file"CR);
    if (! musicPlayer.startPlayingFile("FRANK.OGG")) {
      Log.Error("Could not open file"CR);
    }
      
  }

