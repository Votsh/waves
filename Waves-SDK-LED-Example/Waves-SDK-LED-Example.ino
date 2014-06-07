/**************************************************************************\
* Pinoccio Library                                                         *
* https://github.com/Pinoccio/library-pinoccio                             *
* Copyright (c) 2014, Pinoccio Inc. All rights reserved.                   *
* ------------------------------------------------------------------------ *
*  This program is free software; you can redistribute it and/or modify it *
*  under the terms of the MIT License as described in license.txt.         *
\**************************************************************************/
#include <SPI.h>
#include <Wire.h>
#include <Scout.h>
#include <GS.h>
#include <bitlash.h>
#include <lwm.h>
#include <js0n.h>

#include "version.h"

void setup() {
  Scout.setup(SKETCH_NAME, SKETCH_REVISION, SKETCH_BUILD);
  Serial.begin(9600);  // initialize serial  
  Serial.println("Wave LED Example");
  pinMode(21, OUTPUT);

}

void loop() {
  Scout.loop();
  // Add custom loop code here
  
Serial.println("LOW");
digitalWrite(21, LOW);
delay(1000);

Serial.println("HIGH");
digitalWrite(21, HIGH);
delay(1000);
  
  
}
