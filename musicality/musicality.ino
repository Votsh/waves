#include "Arduino.h"
#include "ATP.h"
#include "Logging.h"
#include <SoftwareSerial.h>
#include <SD.h>
#include <SPI.h>
#include <Adafruit_VS1053.h>

#define baudrate 9600
int firsttime = 1;

// Adafruit VS1053 Music Codec breakout board
//#define CLK 13       // SPI Clock, shared with SD card
//#define MISO 12      // Input data, from VS1053/SD card
//#define MOSI 11      // Output data, to VS1053/SD card
#define RESET 7      // VS1053 reset pin (output)
#define CS 8        // VS1053 chip select pin (output)
#define DCS 6        // VS1053 Data/command select pin (output)
#define DREQ 4       // VS1053 Data request pin (into Arduino) 
#define CARDCS 5     // Card chip select pin

Adafruit_VS1053_FilePlayer musicPlayer = Adafruit_VS1053_FilePlayer(RESET, CS, DCS, DREQ, CARDCS);

void setup() {
   Serial.begin(baudrate);  
   Serial.println("Musicality");
   
   Log.Init(LOGLEVEL, baudrate);
   Log.Info(CR"---------------------------------"CR);
   Log.Info("VOS Waves"CR);
   setupMusic();
   
}

void loop() {

  if (firsttime==1)
  {
    firsttime=0;
  }
}
 
void setupMusic(){
    Log.Info("Initialize the music player"CR);
    if (!musicPlayer.begin()) {
      Serial.println("VS1053 not found");
    }

    // Set volume for left, right channels. lower numbers == louder volume!
    Log.Debug("Setting volume"CR);
    musicPlayer.setVolume(100,100);

    Log.Debug("Make a tone to indicate VS1053 is working"CR);
    musicPlayer.sineTest(0x44, 500);
    delay(40);

    Log.Debug("Initialize SD card on the VS1053"CR);   
    if (!SD.begin(CARDCS)) {
      Log.Error("SD failed, or not present"CR);
      while (1);  // don't do anything more
    }
    Log.Debug("SD card ok"CR);
    
    if (! musicPlayer.useInterrupt( VS1053_FILEPLAYER_PIN_INT ))  //DREQ int, VS1053_FILEPLAYER_PIN_INT
      Log.Error("DREQ pin is not an interrupt pin"CR);
      
Log.Info("Playing music file"CR);
    if (! musicPlayer.startPlayingFile("FRANK.OGG")) {
      Log.Error("Could not open file"CR);
    }      
      
  }
  
