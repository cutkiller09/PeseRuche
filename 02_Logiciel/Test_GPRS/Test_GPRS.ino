//SIM800_Test_OK_pour_Ctrl-z
#include <SoftwareSerial.h>
//Envoie une ligne tapée au SIM800
//En fin de SMS, le Ctrl-z indispensable est censuré par l'IDE Arduino
//on le remplace donc par le caratère "&".

//SIM800 TX is connected to Arduino D8
#define SIM800_TX_PIN 8
 
//SIM800 RX is connected to Arduino D9
#define SIM800_RX_PIN 9

int a = 0;
char c;//Reçoit le char tapé 
//Create software serial object to communicate with SIM800
SoftwareSerial serialSIM800(SIM800_TX_PIN, SIM800_RX_PIN);

void setup() {
  //Begin serial comunication with Arduino and Arduino IDE (Serial Monitor)
  Serial.begin(9600);
  while (!Serial);

  //Being serial communication witj Arduino and SIM800
    Serial.println("serialSIM800 not available");
  serialSIM800.begin(9600);
  delay(1000);
  Serial.println("Setup Complete!");
}

void loop()
{
  //Read SIM800 output (if available) and print it in Arduino IDE Serial Monitor
  if (serialSIM800.available()) {
    Serial.write(serialSIM800.read());
    Serial.println("serialSIM800 available");
  }
  else
  {
    //Serial.println("serialSIM800 not available");
  }
  //Read Arduino IDE Serial Monitor inputs (if available) and send them to SIM800
  if (Serial.available()) {
    c = Serial.read(); //& remplace Ctrl-z, fin de sms
    if (c == '&') c=(char)26;//Envoyer Ctrl-z
  Serial.println(" serialSIM800.write");
    serialSIM800.write(c);
  }
}
