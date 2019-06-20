#include <SigFox.h>

bool value = 1;

void setup() {
  Serial.begin(9600);
  if (!SigFox.begin()) {
  Serial.println("Unable to init the Atmel ATA8520 Sigfox chipset");
  return;
  }
  
  SigFox.debug();
  Serial.println("Device ID\t" + SigFox.ID());
  delay(100);
  SigFox.sendBit(value);
}  

void loop(){}
