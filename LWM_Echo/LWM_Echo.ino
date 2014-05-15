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

#include <Logging.h>    // Log errors, warnings, info, thanks to mrRobot@web.de

/*
  Examples for using the Atmel proprietary Light Weight Mesh and Pinoccio APIs to communicate
  messages over a LWM radio network
  by Frank Cohen, fcohen@votsh.com
  (c) 2014 Frank Cohen
  Distributed under a free GPL v3 license

  This example uses:
  Send a message, https://github.com/Pinoccio/library-pinoccio/blob/master/src/Shell.cpp#L2164-L2186
  Listen for ack/confirmation, https://github.com/Pinoccio/library-pinoccio/blob/master/src/Shell.cpp#L2188-L2243
  Register a callback to handle received messages,
  https://github.com/Pinoccio/library-pinoccio/blob/master/src/Shell.cpp#L335
  Receive a message, https://github.com/Pinoccio/library-pinoccio/blob/master/src/Shell.cpp#L2119-L2162

  A simple example is at, https://github.com/Pinoccio/library-atmel-lwm/blob/master/examples/NWK_Ping/NWK_Ping.ino

  I recommend you look at these docs:
  http://www.atmel.com/Images/Atmel-42028-Lightweight-Mesh-Developer-Guide_Application-Note_AVR2130.pdf
  describes all of the mesh library stuff that our code uses.
  For instance, what the NWK_DataReq_t type is, etc.

  Thank you, Eric of the Pinoccio team, for the help on this.
*/

#define LOGLEVEL LOG_LEVEL_DEBUG

#define destination 2      // Remote Scout running the LWM_Echo sketch

byte counter = 0;          // Counts how many messages sent and received
long time = 0;
int messageNumber = 0;
char outmsg[250];

void setup() {
  Scout.setup();

  Serial.begin(115200);
  Log.Init(LOGLEVEL, 115200);
  Log.Info(CR"Pinoccio Light Weight Mesh (LWM) Simple Messaging Example"CR);
  Log.Info("by Frank Cohen of Votsh.com"CR);

  Log.Info("Scout temperature is %d"CR, Pinoccio.getTemperature());

  /* Typical reports look like this:
   Scout: print mesh.report();
   {"type":"mesh","scoutid":2,"troopid":11,"routes":0,"channel":20,"rate":"250 kb/s","power":"3.5 dBm"}
   Lead: print mesh.report();
   {"type":"mesh","scoutid":1,"troopid":11,"routes":0,"channel":20,"rate":"250 kb/s","power":"3.5 dBm"}
  */

  Log.Debug("Scout Address %i"CR, Scout.getAddress());
  Log.Debug("Scout PanID %i"CR, Scout.getPanId());

  /*
  Scouts set the mesh network settings. If running this outside of a Scout use:
  SYS_Init();
  NWK_SetAddr(meshAddress);
  NWK_SetPanId(0x01);
  PHY_SetChannel(0x1a);
  PHY_SetRxState(true);
  NWK_OpenEndpoint(1, receiveMessage);
  */

  // Sets a mesh listener (similar to Scout.meshListen) to receive LWM messages
  NWK_OpenEndpoint(Scout.getAddress(), receiveMessage);

  time = millis();
}

void loop() {
  Scout.loop();      // If outside of a Scount use: SYS_TaskHandler();

  // If we are the first Scout, then send a message every 10 seconds
  if ( (Scout.getAddress() == 1 ) && ( ( time + 10000 ) < millis() ) )
  {
    time = millis();

    Log.Debug("Hello from %i number %i"CR, Scout.getAddress(), messageNumber  );

    messageNumber++;

    strcpy( outmsg, "Hello " );
    
    char rv[100];
    sprintf(rv,"%d", messageNumber);
    strcat( outmsg, rv );
    strcat( outmsg, "\0" );
    
    Log.Debug("msg: %s, strlen=%d"CR, outmsg, strlen(outmsg) );

    int status = sendMsg( outmsg, destination);
  }

  delay(500);
}

/*
Send a message across the LWM to another Scout running this Echo sketch
*/

static void sendMsg( const char * data, int destination_mesh_id ) {

  Log.Debug("sendMsg to %i, length = %d, sending: %s"CR, destination_mesh_id, strlen(data) + 1, data );

  // we just leak for now
  NWK_DataReq_t *message = (NWK_DataReq_t*) malloc( sizeof( NWK_DataReq_t ) );

  message->dstAddr = destination_mesh_id;
  message->dstEndpoint = 1;
  message->srcEndpoint = 1;
  message->options = 0;   // Example: NWK_OPT_ACK_REQUEST|NWK_OPT_ENABLE_SECURITY
  message->data = (uint8_t *) data;
  message->size = strlen( data ) + 1 ;
  message->confirm = sendConfirm;
  NWK_DataReq(message);
  
  return 0;
}

/*
Event handler to receive messages
*/

static bool receiveMessage(NWK_DataInd_t *ind) {

  char *recdata = (char*)ind->data;
  
  Log.Debug("Received message of %d bytes: %s", ind->size, recdata );
  Log.Debug(" from %d ", ind->srcAddr );
  Log.Debug(" lqi %d ", ind->lqi );
  Log.Debug(" rssi %d "CR, abs(ind->rssi) );

  NWK_SetAckControl(abs(ind->rssi));

  return true;
}

// only needed if you want to do something once you receive the Ack packet back from the receiver
static void sendConfirm( NWK_DataReq_t *req ) {
  if (NWK_SUCCESS_STATUS == req->status)
  {
    Log.Debug("Sent successfully."CR);
  }
  else
  {
    Log.Debug("sendConfirm error: %d"CR, req->status);
  }
}

