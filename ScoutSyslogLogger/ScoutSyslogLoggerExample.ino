#include <Arduino.h>    // Arduino main libraries
#include <SPI.h>        // Serial Device Interface 
#include <Wire.h>       // TWI/I2C library for Wiring & Arduino
#include <Scout.h>      // Pinoccio Scout APIs, including setup and loop, mostly hardware related
#include <GS.h>         // HAL driver to talk with Gainspan GS1011 WiFi module
#include <bitlash.h>    // Tiny language interpreter that provides a serial port 
                        // shell environment for bit banging and hardware hacking.
#include <lwm.h>        // Atmel Light Weight Mesh radio network
#include <js0n.h>       // Javascript Object Notation JSON library 
#include "key/key.h"    // Key utilities

#include "ScoutSyslogLogger.h"    // Web-based log4j-style logger for Pinoccio Scouts

#define baudrate 115200

byte counter = 0;          // Counts how many log messages sent
long time = 0;

void setup() {
  Scout.setup();      // Initializes Pinoccio Scout libraries

  // See ScoutSyslogLogger.h for definitions
  // ip address of syslog server
  // port of syslog server, normally 514
  // driver LOG_WIFI, LOG_BLE or others
  // consoleOption If LOG_CONSOLE then logs to the Serial monitor too
  // facility syslog identifier
  // machine Scout name
  // procid Process number, default is 0
  // appname App name

  Log.setup( 
    LOG_LEVEL_DEBUG,
    baudrate,
    54,209,5,236,
    514,
    LOG_WIFI,
    LOG_SERIAL,
    user,
    "wave2",
    0,
    "ScoutSyslogLoggerExample" );  
}

void loop() {
  Scout.loop();      // Pinoccio Scout libraries process their tasks
  
  Log.handleMessages();
  
  if ( ( time + 10000 ) < millis() )
  {
    time = millis();

    int r = rand() % 4;
    if ( r == 0 )
    {
      Log.Error( "Houston, we have a %s", "problem" );      
    }    
    if ( r == 1 )
    {
      Log.Info( "Hull breech detected on deck %d", 4 );      
    }
    if ( r == 2 )
    {
      Log.Debug( "Though the program be but %x lines of code, someday it will have to be maintained.", 3 );      
    }
    if ( r == 3 )
    {
      Log.Verbose( "I can take any truth, just do not lie to me." );
    }
    
  }

}
