#include <SPI.h>
#include <Wire.h>
#include <Scout.h>
#include <GS.h>
#include <bitlash.h>
#include <lwm.h>
#include <js0n.h>
#include <SD.h>
#include <Logging.h>
#include <Adafruit_NeoPixel_Votsh.h>
#include "InterpolationTable.h"
#include "version.h"

#define BAUDRATE 115200

//#define LOGLEVEL LOG_LEVEL_DEBUG
#define LOGLEVEL LOG_LEVEL_INFOS
//#define LOGLEVEL LOG_LEVEL_ERRORS

// Hardware defines
#define WS2812_PIN 8    // Using D8 on Pinoccio Scout for now
#define SD_CS      8    // SD card chip select line
#define NUM_PIXELS 16   

Adafruit_NeoPixel_Votsh strip = Adafruit_NeoPixel_Votsh(NUM_PIXELS, WS2812_PIN, NEO_GRB + NEO_KHZ800);  

// File definitions
#define FILE_NAME  "wave.txt"
#define WAVE_VERSION    1
#define MAX_INTENSITY   255
#define MACHINE_TYPE    1


// Memory structures
struct Header {
  char hello[4];                // always should be "WAVE"
  unsigned int wave_version;   // File version
  unsigned int max_intensity;   // maximum pixel intensity (how bright a colour can be - currently 255)
  unsigned int machine_type;    // What type of wave machine this show is for
  unsigned int totalchunks;     // Total number of chunks in the show file
  unsigned long totalduration;  // Total duration of the show in millisecondse
  unsigned long firstchunksize; // Size of the first chunk in bytes
};

struct LEDLocation {
  unsigned long addr;            // address offset relative to chunk start
  unsigned int entries;          // number of entries for LED in this chunk
  unsigned int currententry;     // current LED entry being processed by the show maker
};

struct LEDEntry {
  unsigned int timeoffset;      // Time offset inside current chunk of this paricular LED value
  byte red;                     // Red LED intensity (0-255)
  byte green;                   // Green LED intensity (0-255)
  byte blue;                    // Blue LED intensity (0-255)
};

struct Chunk {
  unsigned long time;           // Time in milliseconds of this chunk relative to start of show
  unsigned long duration;       // Chunk duration in milliseconds
  unsigned long nextchunk;      // File pointer to the next chunk
  unsigned long nextchunksize;  // Size of the next chunk in bytes
  byte data[];                  // Bucket to hold all of the data
};

File myFile;
Header *myheader;
Chunk *mychunk, *mynextchunk;   // working copy and next chunk, these are pointers we we'll dynamically allocate using malloc() later
unsigned long mychunksize;  // track how big the current chunk size is

unsigned long showstart;

void errHandler(char *message)
{
  Log.Error(message); 
  while(1);     // Since we don't have a way of recovering from errors, stop.
}

void SDTest(void)
{
  
  Log.Info("\Checking SD card properties...\n");

  Sd2Card card;
  SdVolume volume;
  SdFile root;

  // Set the MCU's hardware Slave Select pin to an output.  Required for SD library to work
  pinMode(SS, OUTPUT);
  // Use the SD card's real chip select pin to talk to it.
  if (!card.init(SPI_HALF_SPEED, SD_CS)) {
    errHandler("SD Initialization failed\n");
  } else {
    Log.Info("Wiring is correct and a card is present.\n");
  }

  // print the type of card
  Log.Info("Card type: ");
  switch (card.type()) {
    case SD_CARD_TYPE_SD1:
      Log.Info("SD1\n");
      break;
    case SD_CARD_TYPE_SD2:
      Log.Info("SD2\n");
      break;
    case SD_CARD_TYPE_SDHC:
      Log.Info("SDHC\n");
      break;
    default:
      errHandler("Unknown SD card type\n");
  }

  // Now we will try to open the 'volume'/'partition' - it should be FAT16 or FAT32
  if (!volume.init(card)) {
    errHandler("Could not find FAT16/FAT32 partition.\nMake sure you've formatted the card\n");
  }

  // print the type and size of the first FAT-type volume
  uint32_t volumesize;
  Log.Info("Volume type is FAT%d\n", volume.fatType());

  volumesize = volume.blocksPerCluster();    // clusters are collections of blocks
  volumesize *= volume.clusterCount();       // we'll have a lot of clusters
  volumesize *= 512;                            // SD card blocks are always 512 bytes
  Log.Info("Volume size (Mbytes): %l\n", volumesize/1024/1024);

  Log.Info("Files found on the card (name, date and size in bytes): \n");
  root.openRoot(volume);

  // list all files in the card with date and size
  root.ls(LS_R | LS_DATE | LS_SIZE);

  root.close();
}  

void WAVE_GetHeader(void)
{
  
  myheader = (Header*)malloc(sizeof(Header));
  if (myheader == NULL) { 
    errHandler("Couldn't allocate header!\n");
  }
  Log.Info("Header at address %d\n", myheader);

  Log.Info("Initialising SD card\n");
  if (!SD.begin(SD_CS)) {
    errHandler("SD Initialization failed!\n");
  }

  Log.Info("Opening %s\n", FILE_NAME);

  myFile = SD.open(FILE_NAME, FILE_READ);
  if (!(myFile))
  {
    errHandler("Error opening file for reading!\n");
  }

  Log.Info("Reading file %s:\n", FILE_NAME);

  for(byte i=0;i<sizeof(Header);i++) {
    if (!myFile.available())
    {
      errHandler("Unexpected EOF!\n");
    }    
    *((byte*)myheader+i) = (byte)myFile.read();
    Log.Debug("%d ", *((byte*)myheader+i));
  }
  Log.Debug("\n");

  if ((myheader->hello[0] == 'W') && (myheader->hello[1] == 'A') && (myheader->hello[2] == 'V') && (myheader->hello[3] == 'E')) {
    Log.Info("WAVE header detected\n");
  }
  else
  {
    errHandler("WAVE header not found!\n");
  }
  
  if (myheader->wave_version != WAVE_VERSION) { errHandler("File Version incorrect!\n"); }
  if (myheader->max_intensity != MAX_INTENSITY) { errHandler("File Max intensity value incorrect!\n"); }
  if (myheader->machine_type != MACHINE_TYPE) { errHandler("File machine type incorrect!\n"); }
  
  Log.Info("Max intensity: %d, Machine type: %d, Total chunks: %d, Total duration: %l, First chunk size: %l\n", myheader->max_intensity, myheader->machine_type, myheader->totalchunks, myheader->totalduration, myheader->firstchunksize);
}

void WAVE_GetChunk(void)
{
  if (mychunk == NULL)
  {
    Log.Info("First run - allocating chunk\n");
    mychunk = (Chunk*)malloc(myheader->firstchunksize);
    if (mychunk == NULL) { 
      errHandler("Couldn't allocate chunk!\n");
    }
    mychunksize = myheader->firstchunksize;
    
    myFile.seek(sizeof(Header));
    
    for(unsigned long i=0;i<mychunksize;i++) {
      if (!myFile.available())
      {
        errHandler("Unexpected EOF!\n");
      }
      *((byte*)mychunk+i) = (byte)myFile.read();
      Log.Debug("%X ", *((byte*)mychunk+i));
    }  
    Log.Debug("\n");
    
    Log.Info("Finished reading first chunk\n");

  }
  else
  {
    // See if there is another chunk to fetch after this one
    if ((mychunk->nextchunk) && (mychunk->nextchunksize)) 
    {
      mynextchunk = (Chunk*)malloc(mychunk->nextchunksize);
      
      if (mynextchunk == NULL) { 
        errHandler("Couldn't allocate next chunk!\n");
      }
      Log.Info("Next chunk at address %d, reading...\n", mynextchunk);
      
      // seek to next chunk in file
      myFile.seek(mychunk->nextchunk);
      
      for(unsigned long i=0;i<mychunk->nextchunksize;i++) 
      {
        if (!myFile.available())
        {
          errHandler("Unexpected EOF!\n");
        }
        *((byte*)mynextchunk+i) = (byte)myFile.read();
        Log.Debug("%X ", *((byte*)mynextchunk+i));
      }
      Log.Debug("\n");
    }
    else
    {
      Log.Info("No more chunks, the show is finished!\n");
      for(byte i=0;i<NUM_PIXELS;i++)
      {
        strip.setPixelColor(i, 0,0,0);
      }
      strip.show();
      errHandler("Show finished\n");
    }
    
         
    mychunksize = mychunk->nextchunksize;  // prep for the size of the next chunk
    free(mychunk);                         // done using chunk, let's free it
    mychunk = mynextchunk;                 // our pointer becomes the address of the next chunk
        
  }

  Log.Info("Chunk at address %d size %d bytes\n", mychunk, mychunksize);

  Log.Info("Chunk time: %l, Chunk Duration: %l, Nextchunk: %l, Nextchunk size: %l\n", mychunk->time, mychunk->duration, mychunk->nextchunk, mychunk->nextchunksize);    
  
}

void setup() {
  Scout.setup(SKETCH_NAME, SKETCH_REVISION, SKETCH_BUILD);

  Log.Init(LOGLEVEL, BAUDRATE);
  Log.Info("---------------------------------\n");
  Log.Info("VOS Waves Interpolation Demo\n");
  Log.Info("---------------------------------\n");
  
  pinMode(WS2812_PIN, OUTPUT);
  strip.begin();
  Log.Info("\nStarted WS2812 lights\n");
  
  
  SDTest();
 
  WAVE_GetHeader();
  
  showstart = millis() + 2000;  // TODO - replace with starting instructions from mesh network

}

void loop() {
  Scout.loop();

  if (millis() < showstart)
  {
    Log.Info("Waiting for show start in %lms time\n", showstart-millis());
    
    while (millis() < showstart); // wait for show start
    
    Log.Info("Starting show!\n");
  }

  WAVE_GetChunk();  // retrieve our next chunk from the SD file

  // Set up arrays to track contents of each block of LED entries, and current pointer for each LED
  LEDLocation location[NUM_PIXELS];

  for(int i=0; i<NUM_PIXELS;i++)
  {
    Log.Debug("Copying location %d into location table.. need to copy %d bytes\n", i, sizeof(location[i].addr) + sizeof(location[i].entries));
    Log.Debug("Base address %d\n", (byte*)mychunk->data + i*6);
    
    for(byte j=0;j<sizeof(location[i].addr) + sizeof(location[i].entries);j++)  // Copy the entry address and number of entries into our struct
    {
      *(((byte*)&location[i])+j) = *((byte*)mychunk->data + i*6+j);  
    }
    location[i].currententry = 0;    // Initialise counter to the first entry
    
    Log.Debug("LED%d Location: %l, entries: %d\n", i, location[i].addr, location[i].entries);
    for (byte j=0;j<location[i].entries;j++)
    {
      LEDEntry* thisentry = (LEDEntry*)((byte*)mychunk + location[i].addr + j * sizeof(LEDEntry));
      Log.Debug("LED%d entry %d: R=%d G=%d B=%d @ t=%d\n", i, j, thisentry->red, thisentry->green, thisentry->blue, thisentry->timeoffset);
    }
  }
 
  unsigned long chunkstart = showstart + mychunk->time;
  unsigned long chunkend = chunkstart + mychunk->duration;

  while(millis() < chunkend)
  {
    // work out the time offsets and p value for our interpolation table
    unsigned int toffset = millis() - chunkstart;  // intra-chunk time offset in milliseconds
    
    // for each LED
    for(byte i=0;i<16;i++)
    {
      // Retrieve our current and next entries - can be optimised further if we don't care about logging
      LEDEntry* thisentry = (LEDEntry*)((byte*)mychunk + location[i].addr + location[i].currententry * sizeof(LEDEntry));
      LEDEntry* nextentry = (LEDEntry*)((byte*)mychunk + location[i].addr + location[i].currententry * sizeof(LEDEntry)) + 1;
      Log.Debug("toffset %d LED%d this entry at address %d with values t%d R%d G%d B%d \n", toffset, i, thisentry, thisentry->timeoffset, thisentry->red, thisentry->green, thisentry->blue);
      Log.Debug("toffset %d LED%d next entry at address %d with values t%d R%d G%d B%d \n", toffset, i, nextentry, nextentry->timeoffset, nextentry->red, nextentry->green, nextentry->blue);

      // If we have reached/passed the time offset for the next entry, make it the current entry      
      if (toffset >= nextentry->timeoffset)
      {
        Log.Debug("LED%d at next transition\n", i);
         // increment LED entry index
        location[i].currententry++;
        nextentry = (LEDEntry*)((byte*)mychunk + location[i].addr + location[i].currententry * sizeof(LEDEntry)) + 1;
        thisentry = (LEDEntry*)((byte*)mychunk + location[i].addr + location[i].currententry * sizeof(LEDEntry));
      }

      // work out how far through this transition we are      
      unsigned int poffset = ((unsigned long)toffset - thisentry->timeoffset) * INTERPROWS / (nextentry->timeoffset - thisentry->timeoffset);  // percentage offset expressed as an index to the interpolation table
      if (poffset == -1) {  // yes, i know it's unsigned.
        poffset = INTERPROWS-1;
        Log.Info("Instantaneous transition detected (tcurrent = tnext)\n");
      }
      if (poffset >= INTERPROWS)
      {
        Log.Info("Error calculating interpolation table lookup LED%d entry %d, poffset %d\n", i, location[i].currententry, poffset);
        Log.Info("toffset %d LED%d this entry at address %d with values t%d R%d G%d B%d \n", toffset, i, thisentry, thisentry->timeoffset, thisentry->red, thisentry->green, thisentry->blue);
        Log.Info("toffset %d LED%d next entry at address %d with values t%d R%d G%d B%d \n", toffset, i, nextentry, nextentry->timeoffset, nextentry->red, nextentry->green, nextentry->blue);
        poffset = INTERPROWS-1;
      }
      // set the LED pixel colour based on the interpolation table as a percentage of the current entry RGB and the next entry RGB
      byte pixred = interp(thisentry->red,nextentry->red,poffset);
      byte pixgreen = interp(thisentry->green,nextentry->green,poffset);
      byte pixblue = interp(thisentry->blue,nextentry->blue,poffset);

      Log.Debug("LED%d current entry %d R=%d G=%d B=%d @ %d -> R=%d G=%d B=%d @ %d, currently: R=%d G=%d B=%d poffset %d\n", i, location[i].currententry, thisentry->red, thisentry->green, 
        thisentry->blue, thisentry->timeoffset, nextentry->red, nextentry->green, nextentry->blue, nextentry->timeoffset, pixred, pixgreen, pixblue, poffset);

      // TODO - map from row/column sequence in the compiled document to actual pixel position in the string
      
      strip.setPixelColor(i, pixred, pixgreen, pixblue);
    }    

    strip.show();  // output data to pixelss
    
  }

}
