#include <LowPower.h>

#include <SoftwareSerial.h>
SoftwareSerial SerialAT(9, 8); // RX, TX

#define TINY_GSM_MODEM_SIM800
#define TINY_GSM_RX_BUFFER 600
#include <TinyGsmClient.h>
//#include <ArduinoHttpClient.h>

const char apn[]  = "free";// Your GPRS credentials  orange, free,sfr,bouygue
const char user[] = "";// Leave empty, if missing user or pass
const char pass[] = "";

const char server[] = "IP.DE.MON.SER";  //ip de mon serveur;  Name of the server we want to connect to
const int  port     = 80;
// Path to download (this is the bit after the hostname in the URL)
char resource[32] = "/dossier/w.php?o=1&v=2&t=3";  // le dossier avec le fichier php

const int Uanemo = 0;
const int Ugirouette = 1;
const int alim_capteurs = 9;  //base du NPN pour alimenter anemo
const int rst_sim800L = 8;  // pin pour resetter la sim800L
int vitesse;   //String for the first Paramter vitesse
int orientation;   //String for the second Paramter (e.g. orientation)
int i = 0;
const int numReadings = 5;
int readings[numReadings];
int readings2[numReadings];// the readings from the analog input
int readIndex = 0;              // the index of the current reading
int total = 0;
int total2 = 0;// the running total
int vitMoy = 0;
int orienMoy = 0; // the average
enum { Mesure, transmission, veille, relance} phaseDeTravail;
unsigned long timeout;

TinyGsm modem(SerialAT);
TinyGsmClient client(modem);
//HttpClient http(client, server, port);

void setup() {
  SerialAT.begin(9600); 
  String SIM_PIN_CODE = String( "1234" );
  SerialAT.print("AT+CPIN=");
  SerialAT.println( SIM_PIN_CODE );
  Serial.println("Setup OK");
}

void loop() { 
      SerialAT.println("AT");    //sortie  mise en veille profonde Dummie AT
      delay(100);
      SerialAT.println("AT+CSCLK=0");     // sortie veille
      delay(100);
      SerialAT.println("AT+CFUN=1");
      delay(4000);
      modem.init();
      modem.simUnlock("1234");  //code pin de la sim
      delay(7000);
      if (!modem.waitForNetwork()) {
        delay(5000);
        return;
      }
      if (!modem.gprsConnect(apn, user, pass)) {
        delay(5000);
        return;
      }
//      if (!client.connect(server, port)) {
//        delay(10000);
//        return;
//      }
//
//      // Make a HTTP GET request:
//      client.print(String("GET ") + resource + " HTTP/1.0\r\n");
//      client.print(String("Host: ") + server + "\r\n");
//      client.print("Connection: close\r\n\r\n");
//      delay(100);
//      client.stop();
      modem.gprsDisconnect();
      delay(1000);
      SerialAT.println("AT+CFUN=0");    // mise en veille profonde du
      delay(500);
      SerialAT.println("AT+CSCLK=2");   // module GPRS
      delay(500); 

} 
