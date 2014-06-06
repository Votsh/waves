#include <EEPROM.h>
#include <Logging.h>

byte value;

void setup()
{
  Log.Init(LOGLEVEL, 9600);

  int readOrWrite = 1;    // 1 = Read, 0 = Write
  int row = 0;
  
  if ( readOrWrite )
  {
    long startAdr = 8125;
    long endAdr = 8191;
    Serial.begin(9600);  // initialize serial  
    delay (2000);
    Serial.println("Begin EEPROM dump in CSV array format");
    delay (2000);
  
    Log.Info("byte pinoccioEEPROM[] = {"CR);
         
    for (int address=startAdr; address <= ( endAdr + 1 ); address++){ 
     value = EEPROM.read(address); 
     // Log.Info( "%x %x, ", address, value);
     Log.Info( "%d, ", value); 
     delay (10); // Arduino serial can be buggy without a little delay
     
     if ( row++ > 8 )
     {
       row = 0;
       Log.Info(" "CR); 
     }
     
    }
  
    Log.Info(" };");  
    Log.Info(CR"End EEPROM dump in CSV array format");  
  }
  else
  {
    Serial.begin(9600);  // initialize serial  
    delay (5000);    
    Serial.println("Write EEPROM");

    long startAdr = 8125;
    long endAdr = 8191;

    // Scout
    byte pinoccioEEPROM[] = {
      255, 0, 0, 255, 0, 101, 101, 55, 50, 50,  
      52, 50, 53, 57, 51, 97, 98, 54, 97, 98,  
      52, 54, 52, 100, 102, 99, 97, 57, 53, 100,  
      50, 50, 102, 98, 48, 97, 98, 54, 52, 100,  
      102, 99, 97, 57, 53, 100, 50, 50, 102, 98,  
      48, 97, 98, 0, 20, 12, 0, 2, 0, 161,  
      132, 30, 0, 232, 3, 1, 1, 101 };

    int i = 0;
    for (int address=startAdr; address <= ( endAdr + 1 ); address++){
      Log.Info(".");
      EEPROM.write(address, pinoccioEEPROM[ i++ ] ); 
      delay (400); // Arduino serial can be buggy without a little delay
    }
    
    Serial.println("Done writing.");
  }
}

void loop()
{
// nothing here
}
