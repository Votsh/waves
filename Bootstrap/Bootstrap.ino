#include <SPI.h>        // Serial Device Interface 
#include <Wire.h>       // TWI/I2C library for Wiring & Arduino
#include <Scout.h>      // Pinoccio Scout APIs, including setup and loop
#include <GS.h>         // HAL driver to talk with Gainspan GS1011 WiFi module
#include <bitlash.h>    // Tiny language interpreter that provides a serial port 
                        // shell environment for bit banging and hardware hacking.
#include <lwm.h>        // Atmel Light Weight Mesh radio network
#include <js0n.h>       // Javascript Object Notation JSON library 

void setup() {
  Scout.setup();
  // Add custom setup code here
}

void loop() {
  Scout.loop();
  // Add custom loop code here
}
