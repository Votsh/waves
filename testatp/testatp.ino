#include "Arduino.h"
#include "ATP.h"
#include "Logging.h"
#include <SoftwareSerial.h>
#include <SD.h>
#include <SPI.h>

ATP atp = ATP();

void setup() {
  Serial.begin(9600);
    atp.begin("XBEE");

    delay(3000);

    //atp.initiateTransferRequest();

    atp.addNode(100);
    atp.print();


}

void loop() {
  // put your main code here, to run repeatedly:

}
