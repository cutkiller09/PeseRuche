#include <SigFox.h>
#include <SPI.h>
#include <mcp2515.h>

struct can_frame canMsg;
//MCP2515 mcp2515(10);

#define DEBUG 1

void setup() {

   if (DEBUG){
    Serial.begin(9600);
    while (!Serial) {};
  }
  
  // Initialize the SigFox module
  if (!SigFox.begin()) {
    if (DEBUG){
      Serial.println("Sigfox module unavailable !");
    }
    return;
  }

  //SPI.begin();
  Serial.println("------- SPI.begin ----------");
  
  //mcp2515.reset();
  Serial.println("------- mcp2515.reset ----------");
  //mcp2515.setBitrate(CAN_125KBPS);
  Serial.println("------- mcp2515.setBitrate(CAN_125KBPS) ----------");
  //mcp2515.setNormalMode();
  Serial.println("------- mcp2515.setNormalMode----------");
  
  Serial.println("------- CAN Read ----------");
  Serial.println("ID  DLC   DATA"); 

  
  // If we wanto to debug the application, print the device ID to easily find it in the backend
  if (DEBUG){
    SigFox.debug();
    Serial.println("ID  = " + SigFox.ID()); 
    short valA = 7700;
    //short valB = 128;
    //float valC = 654.32;
    SigFox.beginPacket();
    SigFox.write(valA);
    //SigFox.write(valB);
    //SigFox.write(valC);
    SigFox.endPacket();    
  }

  delay(100); 
}  

void loop(){}
 
//
//
//void setup() {
//  Serial.begin(115200);
//  SPI.begin();
//  
//  mcp2515.reset();
//  mcp2515.setBitrate(CAN_125KBPS);
//  mcp2515.setNormalMode();
//  
//  Serial.println("------- CAN Read ----------");
//  Serial.println("ID  DLC   DATA");
//}
//
//void loop() {
//  
//  if (mcp2515.readMessage(&canMsg) == MCP2515::ERROR_OK) {
//      
//    Serial.print(canMsg.can_id, HEX); // print ID
//    Serial.print(" "); 
//    Serial.print(canMsg.can_dlc, HEX); // print DLC
//    Serial.print(" ");
//    
//    for (int i = 0; i<canMsg.can_dlc; i++)  {  // print the data
//        
//      Serial.print(canMsg.data[i],HEX);
//      Serial.print(" ");
//
//    }
//
//    Serial.println();      
//  }
//
//}
