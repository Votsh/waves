/*

Votsh Operation System (VOS), Version: Waves

This is the Waves release of the Votsh Operating System. Waves is 
a table top computing platform that uses color, light, and animation.
VOS provides a distributed engine to connect and operate Waves devices.
Details on Waves is found at http://www.votsh.com

VOS Waves Sprites delivers these objects:

Main - operates several demonstrations of the Waves device
WavesFactory - instantiates rendering and motion engines
DynamicMotionProvider - Motion and animation for sprites
GridRenderProvider - Render Sprits to a PixelGrid
DeviceRenderProvider - Render grid to hardware devices
WaveProperties - Values used to configure the Providers

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
#include <SPI.h>
#include <SD.h>
#include <Adafruit_NeoPixel_Votsh.h>

#define DEMO 3

#define LOGLEVEL LOG_LEVEL_DEBUG

// Parameter 1 = number of pixels in strip
// Parameter 2 = pin number (most are valid)
// Parameter 3 = pixel type flags, add together as needed:
//   NEO_KHZ800  800 KHz bitstream (most NeoPixel products w/WS2812 LEDs)
//   NEO_KHZ400  400 KHz (classic 'v1' (not v2) FLORA pixels, WS2811 drivers)
//   NEO_GRB     Pixels are wired for GRB bitstream (most NeoPixel products)
//   NEO_RGB     Pixels are wired for RGB bitstream (v1 FLORA pixels, not v2)

#define WS2812_PIN 22

Adafruit_NeoPixel_Votsh strip = Adafruit_NeoPixel_Votsh(16, WS2812_PIN, NEO_GRB + NEO_KHZ800);

#define BlueColor 240
#define YellowColor 10

#define NULL 0

// For sparkles

#define valuesPerSparkle 11;

#define sparkle_speed 0
#define sparkle_timer 1
#define sparkle_current_luminence 2
#define sparkle_maximum_luminence 3
#define sparkle_luminence_step 4
#define sparkle_color_red 5
#define sparkle_color_green 6
#define sparkle_color_blue 7
#define sparkle_updown 8
#define sparkle_x 9
#define sparkle_y 10

int * sparkles = NULL;   // Sparkles
int sparkleCount = 16;

int priorInput = 1;   // Previous potentiometer position
int sensorPercentage = 0;   // Potentiometer position

int * pixels = NULL;   // Grid of pixels
int * spriteMask;      // Grid for sprite mask definition

/* One time initialization of VOS */
void setup() 
{
    Serial.begin(57600);

    pinMode(WS2812_PIN, OUTPUT);
    strip.begin();
        
    Log.Init(LOGLEVEL, 57600);
    Log.Info(CR"---------------------------------"CR);
    Log.Info("VOS Waves"CR);
    Log.Info("---------------------------------"CR);
    
    Show_Controller();  // Sets-up sparkles    
}

/* Main operation
Main - operates several demonstrations of the Waves device
*/
void loop(){
  Serial.println("looping");
  
   if ( DEMO == 1 )
   {
   if ( sensorPercentage>100) sensorPercentage=0;
     sensorPercentage+=1;
   }
   else if (DEMO == 2)
   {
     // Read a potentiometer for sprite positioning
     int sensorValue = analogRead(A0) & 1023;   
     sensorPercentage = sensorValue / 10;
     if ( sensorPercentage>99 ) sensorPercentage = 100;
   }
   else if (DEMO == 3)
   {
      Sparkle_Sprite();  // Animates sparkles
      Render_Sprites();  // Renders sparkles to lights
      sensorPercentage = priorInput;
   }
   
   if (! (sensorPercentage == priorInput) )
   {
      priorInput = sensorPercentage;
      GridRenderProvider();
      DeviceRenderProvider();
   }  
  delay(10);
}

//   Sparkles lights, good for the start of a show

/*
    Show_Controller - Starts one or more Sparkle_Controllers
*/
void Show_Controller(){  

  Sparkle_Controller();  // Start 1 sparkle  
}

/*
    Sparkle_Controller - determines how many sparkles and where they start and end in the pixel grid

    Each sparkle has a definition in the Sparkle_Sprite matrix, a memory structure with the following values:

    sparkle_speed speed
    sparkle_timer timer
    sparkle_current_luminence current luminence
    sparkle_maximum_luminence maximum luminence
    sparkle_luminence_step luminence step
    sparkle_color_red color red
    sparkle_color_green color green
    sparkle_color_blue color blue
    sparkle_updown luminence direction up/down
    sparkle_x pixel x
    sparkle_y pixel y
*/

void Sparkle_Controller(){
  
    int SparkleRows = 4;
    int SparkleRowSize = 4;
    int sparklesTotal = SparkleRows * valuesPerSparkle;
    sparklesTotal = sparklesTotal * SparkleRowSize;
    if ( sparkles == NULL ) sparkles = (int *)malloc( sparklesTotal );
    Log.Debug("lights memory size is myInt1 %d"CR,sparklesTotal);

    for (int i=0; i<sparkleCount; i++)
    {
      int scursor = i * valuesPerSparkle;
      sparkles[ scursor + sparkle_speed ] = 10;   
      sparkles[ scursor + sparkle_timer ] = rand() % 30;
      sparkles[ scursor + sparkle_current_luminence ] = 0;   
      sparkles[ scursor + sparkle_maximum_luminence ] = 255;   
      sparkles[ scursor + sparkle_luminence_step ] = 35;
   
      int mycolor = rand() % 4;
      int myred, myblue, mygreen;
      if (mycolor==0) {         // Votsh Yellow
        myred=254;
        mygreen=211;
        myblue=80; 
      }
      
      if (mycolor==1) {         // Votsh Gray
        myred=122;
        mygreen=139;
        myblue=132; 
      }
         
      if (mycolor==2) {         // Votsh Yellow
        myred=167;
        mygreen=51;
        myblue=42; 
      }

      if (mycolor==3) {         // Votsh Yellow
        myred=0;
        mygreen=46;
        myblue=184; 
      }
      
      sparkles[ scursor + sparkle_color_red ] = myred;       // 254 211 80 is Votsh Yellow  
      sparkles[ scursor + sparkle_color_green ] = mygreen;   
      sparkles[ scursor + sparkle_color_blue ] = myblue;   
      sparkles[ scursor + sparkle_updown ] = 0;
      sparkles[ scursor + sparkle_x ] = ( i % SparkleRowSize );
      sparkles[ scursor + sparkle_y ] = ( i / SparkleRowSize );
      Log.Debug("Init Sparkles: %d %d"CR, sparkles[ scursor + sparkle_x ], sparkles[ scursor + sparkle_y ]);
      Log.Debug("init lum=%d %d %d %d"CR, sparkles[ scursor + sparkle_current_luminence], sparkles[ scursor + sparkle_color_red ], sparkles[ scursor + sparkle_color_green ], sparkles[ scursor + sparkle_color_blue ]);
      
    }
}

/*
    Sparkle_Sprite - Animates the ligiht luminence and color value in a set of sprites
*/

void Sparkle_Sprite(){
  
    // Sparkle algorithm: current_luminence at 0, timer bumps over 100, step up to max_luminence, then down to zero, then restart the timer

    for (int i=0; i<sparkleCount; i++)
    {
      int scursor = i * valuesPerSparkle;

      if ( sparkles[ scursor + sparkle_speed ] > 0 ) 
      {

        if ( sparkles[ scursor + sparkle_timer ] < 1000 )
        {
           // We're still waiting for the timer, before we sparkle
           sparkles[ scursor + sparkle_timer ] += sparkles[ scursor + sparkle_speed ];
        }
        else
        {
          if ( sparkles[ scursor + sparkle_updown ] == 0 ) 
          {
            // We're sparkeling up in luminosity  
            if ( sparkles[ scursor + sparkle_current_luminence ] > ( sparkles[ scursor + sparkle_maximum_luminence ] - sparkles[ scursor + sparkle_luminence_step ] ) )
            {
              sparkles[ scursor + sparkle_updown ] = 1;
              sparkles[ scursor + sparkle_current_luminence ] = sparkles[ scursor + sparkle_maximum_luminence ];
            }
            else
            {            
              sparkles[ scursor + sparkle_current_luminence ] += sparkles[ scursor + sparkle_luminence_step ];
            }
          }
          else
          {
            // We're sparkeling down in luminosity
            if ( sparkles[ scursor + sparkle_current_luminence ] <= sparkles[ scursor + sparkle_luminence_step ] )
            {
              sparkles[ scursor + sparkle_updown ] = 0;
              sparkles[ scursor + sparkle_current_luminence ] = 0;
              
              sparkles[ scursor + sparkle_speed ] = ( rand() % 20 ) + 1;
              sparkles[ scursor + sparkle_timer ] = 0;
              
              
            }
           else
            {            
              sparkles[ scursor + sparkle_current_luminence ] -= sparkles[ scursor + sparkle_luminence_step ];
            }           
          }
        }
      }
    }
}

/*
    Render_Sprites - Renders sprites to lights
*/
void Render_Sprites(){

    int myred, mygreen, myblue;  
    int scursor=0;
    
    for (int i=0; i<sparkleCount; i++)
    { 
      scursor = i * valuesPerSparkle;

      myred = ( sparkles[ scursor + sparkle_color_red ] ) * ((sparkles[ scursor + sparkle_current_luminence] ) * 0.01);
      if (myred>255) myred=255;
      mygreen = ( sparkles[ scursor + sparkle_color_green ] ) * ((sparkles[ scursor + sparkle_current_luminence] ) * 0.01);
      if (mygreen>255) mygreen=255;
      myblue = ( sparkles[ scursor + sparkle_color_blue ] ) * ((sparkles[ scursor + sparkle_current_luminence] ) * 0.01);
      if (myblue>255) myblue=255;
      strip.setPixelColor(i, myred, mygreen, myblue);
      
      Log.Debug("r=%d g=%d b=%d lum=%d %d %d %d"CR,myred,mygreen,myblue, sparkles[ scursor + sparkle_current_luminence], sparkles[ scursor + sparkle_color_red ], sparkles[ scursor + sparkle_color_green ], sparkles[ scursor + sparkle_color_blue ]);
    }
    strip.show();
}


/* WavesFactory - instantiates rendering and motion engines
*/

void WavesFactory(){
}

/* DynamicMotionProvider - Motion and animation for sprites
*/

void DynamicMotionProvider(){
}

/* GridRenderProvider - Render Sprits to a PixelGrid

Sprite X, Y, Z (ignore Y, Z for the Demo)
Caret offset


Lights      0                   1                   2                   3
Pixels      0 1 2 3 4 5 6 7 8 9 0 1 2 3 4 5 6 7 8 9 0 1 2 3 4 5 6 7 8 9 0 1 2 3 4 5 6 7 8 9 

Linear Ramp

                              0 2 4 8 0 0 0 0 8 4 2 0
40                              x x x x x x x x x x
101         x x x x x x x x x x                     x x x x x x x x x x x x x x x x x x x x
                              0 8 6 2 0 0 0 0 2 6 8 0

Pointer within sprite

Known limitations/bugs:
Does not know the row boundary, so it simply rolls into the next row

*/

void GridRenderProvider(){
  
    Log.Info("GridRenderProvider()"CR);

    //Identify a PixelGrid and its dimensions
    // Turn this into an object, 
    // for now it's pixels: 160 pixels wide x 4 pixels tall, but we will only work with first row for now      
    int pixelRows = 4;
    int pixelRowSize = 160;
    int pixelTotal = ( pixelRows * pixelRowSize );
    if ( pixels == NULL ) pixels = (int *)malloc( pixelTotal );
    Log.Debug("pixel memory size is myInt1 %d"CR,pixelTotal);   
  
    // Fill PixelGrid with 101 background color
    memset(pixels, YellowColor, pixelTotal);

    Serial.println("h1");

    // Identify a Sprite and its location and pattern
    double perc = (double) sensorPercentage;
    int spriteX = pixelRowSize * (perc * 0.01); 

    Serial.println("h2");
    
    int spriteY = 0;
    int spriteZ = 0;
    int spriteRows = 1;
    int spriteRowSize = 10;
    int spriteTotal = spriteRows * spriteRowSize;

    Serial.println("h3");

    if (spriteMask == NULL) spriteMask = (int *)malloc( spriteTotal );

    Log.Debug("sprite memory size is myInt1 %d"CR,spriteTotal);   
    // Fill PixelGrid with blue sprite color
    memset(spriteMask, BlueColor, spriteTotal);

    // Plot PixelGrid with Sprite (40 color)

    // Identify sprite mask/effect, this is the Floating Ball of Rainbow effect
    // and these are the % transparency the lower the value the more of the background color you see
    spriteMask[0] = 0;
    spriteMask[1] = 20;
    spriteMask[2] = 40;
    spriteMask[3] = 80;
    spriteMask[4] = 100;
    spriteMask[5] = 100;
    spriteMask[6] = 80;
    spriteMask[7] = 40;
    spriteMask[8] = 20;
    spriteMask[9] = 0;
    
    /*
    spriteMask[0] = 100;
    spriteMask[1] = 100;
    spriteMask[2] = 100;
    spriteMask[3] = 100;
    spriteMask[4] = 100;
    spriteMask[5] = 100;
    spriteMask[6] = 100;
    spriteMask[7] = 100;
    spriteMask[8] = 100;
    spriteMask[9] = 100;
    */
    
    for (uint8_t n=0; n<spriteRowSize; n++)
    {
      double sm = spriteMask[n];
      pixels[ spriteX + n ] = ( YellowColor * ((100-sm)*0.01) ) + ( BlueColor * (sm * 0.01) );
    }
     
    // Print PixelGrid
    // Pixels      0 1 2 3 4 5 6 7 8 9 0 1 2 3 4 5 6 7 8 9 0 1 2 3 4 5 6 7 8 9 0 1 2 3 4 5 6 7 8 9 
    Log.Info("Pixels     ");
    for (uint8_t n=0; n<pixelRowSize; n++)
    {
      Log.Info("%d ",pixels[n]);      
    }
    Log.Info(CR);
 
}

/* DeviceRenderProvider - Render grid to hardware devices

Written for an Adafruit NeoPixel ring with 16 WS2812 units strung in series

Straight average of 10 pixels in a row for each WS2812 unit

*/

void DeviceRenderProvider(){

    int v;
    for (int p=0; p<16; p++)
    {
      v = 0;      
      for (int u=0; u<10; u++)
      {
        v = v + ( pixels[(p*10)+ u] * 0.10 );
      }
      strip.setPixelColor(p, v);
    }
  
    // Print WS2812 Device Status
    // Pixels      0 1 2 3 4 5 6 7 8 9 0 1 2 3 4 5 6
    Log.Info("Lights    ");
    for (int n=0; n<16; n++)
    {
      Log.Info("%d ", strip.getPixelColor(n));      
    }
    Log.Info(CR);
  
    strip.show();
  
  
  
  
}

/* WaveProperties - Values used to configure the Providers
*/

void WaveProperties(){
}


/* A utility method from the Adafruit_Neopixel library
   We will remove this in a future version of VOS.
   Input a value 0 to 255 to get a color value.
   The colours are a transition r - g - b - back to r.
*/
uint32_t Wheel(byte WheelPos) {
  if(WheelPos < 85) {
   return strip.Color(WheelPos * 3, 255 - WheelPos * 3, 0);
  } else if(WheelPos < 170) {
   WheelPos -= 85;
   return strip.Color(255 - WheelPos * 3, 0, WheelPos * 3);
  } else {
   WheelPos -= 170;
   return strip.Color(0, WheelPos * 3, 255 - WheelPos * 3);
  }
}

/*

void getDirectory()
{
      //pinMode(10, OUTPUT);     // change this to 53 on a mega

      if (!card.init(SPI_HALF_SPEED, chipSelect)) {
        Serial.println("initialization failed. Things to check:");
      } else 
      {
      Serial.println("Wiring is correct and a card is present.");
      }

      // print the type of card
      Serial.print("\nCard type: ");
      switch (card.type()) {
        case SD_CARD_TYPE_SD1:
          Serial.println("SD1");
          break;
        case SD_CARD_TYPE_SD2:
          Serial.println("SD2");
          break;
        case SD_CARD_TYPE_SDHC:
          Serial.println("SDHC");
          break;
        default:
          Serial.println("Unknown");
      }

      // Now we will try to open the 'volume'/'partition' - it should be FAT16 or FAT32
      if (!volume.init(card)) {
        Serial.println("Could not find FAT16/FAT32 partition.\nMake sure you've formatted the card");
        return;
      }

      // print the type and size of the first FAT-type volume
      uint32_t volumesize;
      Serial.print("\nVolume type is FAT");
      Serial.println(volume.fatType(), DEC);
      Serial.println();
    
      volumesize = volume.blocksPerCluster();    // clusters are collections of blocks
      volumesize *= volume.clusterCount();       // we'll have a lot of clusters
      volumesize *= 512;                            // SD card blocks are always 512 bytes
      Serial.print("Volume size (bytes): ");
      Serial.println(volumesize);
      Serial.print("Volume size (Kbytes): ");
      volumesize /= 1024;
      Serial.println(volumesize);
      Serial.print("Volume size (Mbytes): ");
      volumesize /= 1024;
      Serial.println(volumesize);    
    
      Serial.println("\nFiles found on the card (name, date and size in bytes): ");
      root.openRoot(volume);
    
      // list all files in the card with date and size
      root.ls(LS_R | LS_DATE | LS_SIZE);

      musicPlayer.setVolume(50,50);

      //if (! musicPlayer.useInterrupt(DREQ))
      //  Serial.println("DREQ pin is not an interrupt pin");

      // Alternately, we can just play an entire file at once
      // This doesn't happen in the background, instead, the entire
      // file is played and the program will continue when it's done!
      musicPlayer.playFullFile("FRANK.OGG");
    
      // Start playing a file, then we can do stuff while waiting for it to finish
      if (! musicPlayer.startPlayingFile("FRANK.OGG")) {
        Serial.println("Could not open file");
        return;
      }

      Serial.println("Started playing");
    
      while (musicPlayer.playingMusic) {
        // file is now playing in the 'background' so now's a good time
        // to do something else like handling LEDs or buttons :)
        Serial.print(".");
        delay(1000);
      }  
  
}

*/


