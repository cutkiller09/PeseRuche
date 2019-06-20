#include <SigFox.h>   // must include the SigFox library

void setup() {
  Serial.begin(9600); 
  while(!Serial) {}; //waits for Serial to be available
  if (!SigFox.begin()) { //ensures SigFox is connected
    Serial.println("Shield error or not present!");
    return;
  }
  Serial.print("sending");
  Serial.print(SigFox.internalTemperature());
  SigFox.begin();
  SigFox.beginPacket();
  SigFox.print(SigFox.internalTemperature()); //content of the packet
  
  SigFox.endPacket();
  Serial.print("sent");
}
void loop() {
  while(1);
}
