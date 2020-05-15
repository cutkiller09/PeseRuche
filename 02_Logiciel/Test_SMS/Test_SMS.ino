#include <SPI.h>
#include <mcp2515.h>
/* Sending SMS with SoftwareSerial
In below code delay of 1 second is used after each command to give necessary time for SIM800 to respond to each command.
With this approach it is not possible to clearly identify any ERROR conditions because program will not ready the responses sent. 
Proper method of doing this is by checking each response against an expected value. 
This is already handled in most of the Arduino libraries including “Seeeduino” library we’ll be using below.
Hence, for this stage, 1 second delay is used for the sake of simplicity. 
/* Note : Replace 07194XXXXX with mobile number SMS should be sent to.  */

#include <SoftwareSerial.h>

//SIM800 TX is connected to Arduino D8
#define SIM800_TX_PIN 8
 
//SIM800 RX is connected to Arduino D9
#define SIM800_RX_PIN 9

#define DEBUG
#define ID 0xAA
#define KEY_REINIT 0x55
#define K_POS_ID_CAN_FRAME 7
#define K_POS_REINIT_CAN_FRAME 6

#ifdef DEBUG
char buffer[8];
#endif

struct can_frame canMsg_Read;
struct can_frame canMsg_Write;
struct can_frame canMsg;
MCP2515 mcp2515(10);
 
 
//Create software serial object to communicate with SIM800
SoftwareSerial serialSIM800(SIM800_TX_PIN,SIM800_RX_PIN);
 
void Send_Sms() {
  String SIM_PIN_CODE = String( "1234" );
  serialSIM800.print("AT+CPIN=");
  serialSIM800.println( SIM_PIN_CODE );
  Serial.println("Setup OK");
  
  //Begin serial comunication with Arduino and Arduino IDE (Serial Monitor)
  Serial.begin(9600);
  while(!Serial);
   
  //Being serial communication witj Arduino and SIM800
  serialSIM800.begin(9600);
  delay(1000);
   
  Serial.println("Setup Complete!");
  Serial.println("Sending SMS...");
   
  //Set SMS format to ASCII
  serialSIM800.write("AT+CMGF=1\r\n");
  delay(1000);
 
  //Send new SMS command and message number
  ////serialSIM800.write("AT+CMGS=\"07194XXXXX\"\r\n");
  serialSIM800.write("AT+CMGS=\"0627765133\"\r\n");  //Working
  delay(1000);
   
  //Send SMS content
  serialSIM800.write("poids : %i");
  delay(1000);
   
  //Send Ctrl+Z / ESC to denote SMS message is complete
  serialSIM800.write((char)26);
  delay(1000);
     
  Serial.println("SMS Sent!");
}   

/*************************************************************/
/*            Module d'initialisation du CAN                 */
/*************************************************************/
void Init_Can()
{
  SPI.begin();

  mcp2515.reset();
  mcp2515.setBitrate(CAN_125KBPS);
  mcp2515.setNormalMode();
} 

void Envoi_Can(char ID_CAN, char Reinit_Value)
{
  Serial.print("Envoi_Can:");

  canMsg.can_id  = 0x0F6;
  canMsg.can_dlc = 8;
  //memcpy(&canMsg.data[0],&Poids,8);
  canMsg.data[0] = 0;
  canMsg.data[K_POS_ID_CAN_FRAME] = (char)ID_CAN;
  canMsg.data[K_POS_REINIT_CAN_FRAME] = (char)Reinit_Value;


#ifdef DEBUG
  Serial.println("*********** Envoi_Can ***********");
  Serial.print("canMsg.data[0]:");
  Serial.println(canMsg.data[0], 1);
  Serial.print("canMsg.data[1]:");
  Serial.println(canMsg.data[1], 1);
  Serial.print("canMsg.data[2]:");
  Serial.println(canMsg.data[2], 1);
  Serial.print("canMsg.data[3]:");
  Serial.println(canMsg.data[3], 1);
  Serial.print("canMsg.data[4]:");
  Serial.println(canMsg.data[4], 1);
  Serial.print("canMsg.data[5]:");
  Serial.println(canMsg.data[5], 1);
  Serial.print("canMsg.data[6]:");
  Serial.println(canMsg.data[6], 1);
  Serial.print("canMsg.data[7]:");
  Serial.println(canMsg.data[7], 1);
#endif

  mcp2515.sendMessage(&canMsg);

  Serial.println("Messages sent");

  delay(100);

}

void Reinitalisation_Rucher() {
  Envoi_Can(0, KEY_REINIT);
  Serial.println("Reinitalisation_Rucher");
} 

/*************************************************************/
/*            Module de lecture de Trames CAN                */
/*************************************************************/
void Lecture_Can(float *f_poids_ruche)
{
  /*Init value*/
  unsigned char u8_Poids_high;
  unsigned char u8_Poids_low;  

  /* body */
  if (mcp2515.readMessage(&canMsg_Read) == MCP2515::ERROR_OK) {
#ifdef DEBUG
    Serial.println("*********** Lecture CAN ***********");
    Serial.print(canMsg_Read.can_id, HEX); // print ID
    Serial.print(" ");
    Serial.print(canMsg_Read.can_dlc, HEX); // print DLC
    Serial.print(" ");
#endif

    for (int i = 0; i < canMsg_Read.can_dlc; i++)  { // print the data

      //Serial.print(canMsg_Read.data[i],HEX);
      Serial.println(canMsg_Read.data[i], 1);
      //Serial.print(" "); 
    }

    u8_Poids_high = canMsg_Read.data[0];
    u8_Poids_low = canMsg_Read.data[1];

    *f_poids_ruche = u8_Poids_high *1000 + u8_Poids_low*10;
    *f_poids_ruche = *f_poids_ruche / 1000;
    
    Serial.print("*f_poids_ruche:");
    Serial.println(*f_poids_ruche, 1);

  }
}
//
//
///*************************************************************/
///*            Module d'envoi des données sur le NET          */
///*************************************************************/
//void envoi_Thingspeak_et_contenus(float f_poids)
//{
//        // CONFIGURATION EN MODE GPRS POUR TRANSMISSION D'UN MAIL              j=0;De plus, la tension dépend aussi de la température et de la consommation donc ça rend la chose encore plus dure.
//
////Pour être précis, il faut impérativement mesurer et intégrer le courant que la batterie débite.
//        //"AT+SAPBR=3,1,\"CONTYPE\",\"GPRS\"");
//        UART_1_PutString("AT+SAPBR=3,1,\"CONTYPE\",\"GPRS\"");  // mode GPRS  
//        UART_1_PutString("AT+SAPBR=3,1,\"APN\",\"free\"");  // APN du fournisseur carte SIM free  
//        UART_1_PutString("AT+SAPBR=1,1");  // on ouvre une session GPRS    
//        UART_1_PutString("AT+SAPBR=2,1");     
//        UART_1_PutString("AT+HTTPINIT");  // on ouvre une session GPRS  
//        UART_1_PutString("AT+HTTPPARA=\"CID\",1");   
//
//        ////////////////////////////////////////
//        /// ON ASSEMBLE LA TRAME A ENVOYER  ////
//        ////////////////////////////////////////
//        
//        j=0;
//        //           Il ne faut pas utiliser le https mais le http !
//        UART_1_PutString("AT+HTTPPARA=\"URL\",\"http://api.thingspeak.com/update?api_key=I5U9GBQNMZICFSSV");   
//        
//      
//        // Envoie de la température
//        UART_1_PutString("&field1="); 
//        lecture_temperature();
//        sprintf(buffer,"%d",temperature_entier);   // A REMPLACER par ID Ruche
//        UART_1_PutString(buffer); 
//        
//        // Envoie Poids Reference
//        UART_1_PutString("&field2="); 
//        //f_poids_ruche=52;
//        sprintf(buffer,"%d ",f_poids);    
//        UART_1_PutString(buffer); 
//        
//        
//        ////////////////////
//        // Fin de la trame
//        ////////////////////
//        UART_1_PutString("\"");   
//        UART_1_PutString("AT+HTTPACTION=0");  //  
//        UART_1_PutString("AT+HTTPTERM");  //   
//        UART_1_PutString("AT+SAPBR=0,1");  // on ferme la session GPRS :  
//}

/*************************************************************/
/*                        setup                              */
/*************************************************************/
void setup() {
  /* body */
  Serial.begin(38400);
  
  Init_Can();
  //Reinitalisation_Rucher();
  String SIM_PIN_CODE = String( "1234" );
  serialSIM800.print("AT+CPIN=");
  serialSIM800.println( SIM_PIN_CODE );
  Serial.println("Setup OK");
}


/*************************************************************/
/*                            loop                           */
/*************************************************************/
void loop() {
  float f_poids;
  
  
  Lecture_Can(&f_poids);
   Serial.print("f_poids:");
   Serial.println(f_poids, 1);
  //Envoi_Can(69, 51);
  //Simulation_PeseRuche_Test();
  Send_Sms();
} 