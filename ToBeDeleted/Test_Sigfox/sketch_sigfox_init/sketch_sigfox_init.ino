#include <SigFox.h>   // must include the SigFox library
  
void setup() {
  Serial.begin(9600); 
  while(!Serial) {}; //waits for Serial to be available
  if (!SigFox.begin()) { //ensures SigFox is connected
    Serial.println("Shield error or not present!");
    return;
  }
  
  String version = SigFox.SigVersion();
  String ID = SigFox.ID();
  String PAC = SigFox.PAC();

  Serial.println("ID  = " + ID);
  Serial.println("PAC = " + PAC);

  delay(100);

  // Send the module to the deepest sleep
  SigFox.end();
}

void loop() {
}
