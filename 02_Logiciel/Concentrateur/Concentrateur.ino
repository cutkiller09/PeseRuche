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
//Optimisation de la consommation
#include <avr/power.h>
#include <avr/sleep.h>
#include <avr/wdt.h>

#define K_MAX_ATTENTE 20 // attente de K_MAX_ATTENTE fois 8 secondes

//SIM800 TX is connected to Arduino D8
#define SIM800_TX_PIN 8
 
//SIM800 RX is connected to Arduino D9
#define SIM800_RX_PIN 9

#define DEBUG
#define ID 0xAA
#define KEY_REINIT 0x55
#define K_POS_ID_CAN_FRAME 7
#define K_POS_REINIT_CAN_FRAME 6

#define TIMEOUT 30000

#ifdef DEBUG
char buffer[8];
#endif

struct can_frame canMsg_Read;
struct can_frame canMsg_Write;
struct can_frame canMsg;
MCP2515 mcp2515(10);  

int8_t answer;
int onModulePin = 13;
char aux_str[100];
int x = 0;
int attempts;
char apn[] = "free"; 
char url[ ]="http://api.thingspeak.com/update?api_key=I5U9GBQNMZICFSSV";


unsigned long lastConnectionTime = 0;         // last time you connected to the server, in milliseconds
const unsigned long postingInterval = 15000L; // delay between updates, in milliseconds
volatile int f_wdt=1;
volatile int u16_cpt_attente=0;



void setup_watchdog(int ii) ;

// Watchdog Interrupt Service est exécité lors d'un timeout du WDT
ISR(WDT_vect) {
  u16_cpt_attente++;
  if (u16_cpt_attente>=K_MAX_ATTENTE)
  {  
    u16_cpt_attente=0;
    if(f_wdt == 0) {
     f_wdt = 1; // flag global 
     }
  }
  else
  {
    ;
  }
}

/******************************************************************************/
/*                             SLEEP MODE                            */ 
/******************************************************************************/

/*************************************************************/
/*                      SETUP WATCHDOG                       */
/*************************************************************/
// paramètre : 0=16ms, 1=32ms, 2=64ms, 3=128ms, 4=250ms, 5=500ms, 6=1 sec,7=2 sec, 8=4 sec, 9=8 sec
void setup_watchdog(int ii) 
{
 byte bb;
 int ww;
 if (ii > 9 ) ii=9;
 bb=ii & 7;
 if (ii > 7) bb|= (1<<5);
 bb|= (1<<WDCE);
 ww=bb;
 // Clear the reset flag
 MCUSR &= ~(1<<WDRF);
 // start timed sequence
 WDTCSR |= (1<<WDCE) | (1<<WDE);
 // set new watchdog timeout value
 WDTCSR = bb;
 WDTCSR |= _BV(WDIE);
}

/*************************************************************/
/*                      ENTER SLEEP                          */
/*************************************************************/
void enterSleep(void) {
 set_sleep_mode(SLEEP_MODE_PWR_DOWN);
 sleep_enable();
 sleep_mode(); //Entre dans le mode veille choisi

// Le programme va reprendre ici après le timeout du WDT

 sleep_disable(); // La 1ère chose à faire est de désactiver le mode veille
}
 
/////////////////////////////////////////////////////////////// 
//                SIM800 Internet Library                    //2
///////////////////////////////////////////////////////////////  

/*************************************************************/
/*                      serialSIM800                         */
/*************************************************************/
//Create software serial object to communicate with SIM800
SoftwareSerial serialSIM800(SIM800_TX_PIN,SIM800_RX_PIN);
 
void Send_Sms(unsigned char u8_ruche, float f_poids )  {
  unsigned int u16_test;
  String Data_SMS;
  
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
  Serial.write(serialSIM800.read());
  delay(1000);
 
  //Send new SMS command and message number
  ////serialSIM800.write("AT+CMGS=\"07194XXXXX\"\r\n");
  serialSIM800.write("AT+CMGS=\"0627765133\"\r\n");  //Working
  Serial.write(serialSIM800.read());
  delay(1000);
   
  //Send SMS content 
  Data_SMS = "Ruche #"+String(u8_ruche,1)+"\nPoids= "+String(f_poids,1)+" kg";
  serialSIM800.print(Data_SMS); 
  delay(1000);
   
  //Send Ctrl+Z / ESC to denote SMS message is complete
  serialSIM800.print((char)26); 
  delay(1000);

   
     
  Serial.println("SMS Sent!"); 
  
}   


/*************************************************************/
/*                     sendATcommand                         */
/*************************************************************/
int8_t sendATcommand(const char* ATcommand, const char* expected_answer1, unsigned int timeout) {

  uint8_t x = 0,  answer = 0;
  char response[100];
  unsigned long previous;

  memset(response, '\0', 100);    // Initialize the string

  delay(100);

  while (serialSIM800.available())
  { //Cleans the input buffer
    serialSIM800.read();
  } 

  Serial.println(ATcommand);    // Prints the AT command
  serialSIM800.write(ATcommand); // Sends the AT command 

  x = 0;
  previous = millis();

  // this loop waits for the answer
  do
  {
    ////if (Serial.available() != 0) {
    if (serialSIM800.available() != 0)
	{
      ////response[x] = Serial.read();
      response[x] = serialSIM800.read();
      serialSIM800.write(response);
      x++;
      // check if the desired answer is in the response of the module
      if (strstr(response, expected_answer1) != NULL)
      {
        answer = 1;
      }
    }
    // Waits for the asnwer with time out
  }
  while ((answer == 0) && ((millis() - previous) < timeout));

  return answer;
}

/*************************************************************/
/*                          power_on                         */
/*************************************************************/
void power_on()
{
  uint8_t answer = 0;

  Serial.println("On Power_on...");
  String SIM_PIN_CODE = String( "1234" );
  serialSIM800.print("AT+CPIN=");
  serialSIM800.println( SIM_PIN_CODE );
  Serial.println("Setup OK");

  // checks if the module is started
  answer = sendATcommand("AT\r\n", "OK\r\n", TIMEOUT);
  if (answer == 0)
  {
  Serial.println("ENCULE !!!!!");
    // power on pulse
    digitalWrite(onModulePin, HIGH);
    delay(3000);
    digitalWrite(onModulePin, LOW);

    // waits for an answer from the module
    while (answer == 0)
	{
      // Send AT every two seconds and wait for the answer
      answer = sendATcommand("AT\r\n", "OK\r\n", TIMEOUT);
      Serial.println("Trying connection with module...");
    }
  }
  else
  {
    Serial.println("cool, it is POWERED, GO AHAID MAN...");
  }
}

///*************************************************************/
///*                    restartPhoneActivity                   */
///*************************************************************/
//void restartPhoneActivity()
//{
//	do
//	{
//		sendATcommand("AT+CFUN=0\r\n", "OK\r\n", TIMEOUT);
//		delay(2000);
//		answer = sendATcommand("AT+CFUN=1\r\n", "Call Ready\r\n", TIMEOUT);
//	}while(answer == 0);
//}
//
///*************************************************************/
///*                    connectToNetwork                       */
///*************************************************************/
//void connectToNetwork()
//{
//	sendATcommand("AT+SAPBR=3,1,\"Contype\",\"GPRS\"\r\n", "OK\r\n", TIMEOUT);//sets Contype
//	snprintf(aux_str, sizeof(aux_str), "AT+SAPBR=3,1,\"APN\",\"%s\"\r\n", apn);//sets APN
//	sendATcommand(aux_str, "OK\r\n", TIMEOUT);
//	attempts = 0;//tries 3 times or gets on the loop until sendATcommand != 0
//	while (sendATcommand("AT+SAPBR=1,1\r\n", "OK\r\n", TIMEOUT) == 0)
//    {
//		delay(5000);
//		attempts = attempts + 1;
//		if(attempts > 2)
//		{
//			restartPhoneActivity();
//			attempts = 0;
//		}
//    } 
//}
//
///*************************************************************/
///*                    initHTTPSession                        */
///*************************************************************/
//void initHTTPSession()
//{
//	while (sendATcommand("AT+HTTPINIT\r\n", "OK\r\n", TIMEOUT) == 0)
//    {
//		restartPhoneActivity();
//		connectToNetwork();
//   }
//}
//
///*************************************************************/
///*                    HTTPRequest                            */
///*************************************************************/
//void HTTPRequest(unsigned char u8_Id_Ruche, float f_Masse_kg)
//{
//
//	////snprintf(aux_str, sizeof(aux_str), "AT+HTTPPARA=\"URL\",\"%s\"\"%f\"\r\n", url, light);
//	//float light = readLDR();
//	char sensorValue[6];
//	dtostrf(f_Masse_kg,5,2,sensorValue);
//	char ruchevalue[6];
//	dtostrf(u8_Id_Ruche,5,2,ruchevalue);
//	snprintf(aux_str, sizeof(aux_str), "AT+HTTPPARA=\"URL\",\"%s\%s\"\r\n", url, ruchevalue, sensorValue);
//	sendATcommand(aux_str, "OK\r\n", TIMEOUT);
//	delay(3000);
//	attempts = 0;//tries 3 times or gets on the loop until sendATcommand != 0
//	while (sendATcommand("AT+HTTPACTION=0\r\n", "+HTTPACTION: 0,200,", TIMEOUT) == 0)
//    {
//		delay(5000);
//		attempts = attempts + 1;
//		if(attempts > 2)
//		{
//			sendATcommand("AT+SAPBR=0,1\r\n", "OK\r\n", 2 * TIMEOUT);
//			sendATcommand("AT+HTTPTERM\r\n", "OK\r\n", TIMEOUT);
//			restartPhoneActivity();
//			connectToNetwork();
//			initHTTPSession();
//			////snprintf(aux_str, sizeof(aux_str), "AT+HTTPPARA=\"URL\",\"%s\"\r\n", url);
//			//light = readLDR(); 
//	    snprintf(aux_str, sizeof(aux_str), "AT+HTTPPARA=\"URL\",\"%s\%s\"\r\n", url, ruchevalue, sensorValue);
//			sendATcommand(aux_str, "OK\r\n", TIMEOUT);
//			attempts = 0;
//		}
//    }
//	Serial.println("Successfully uploaded");
//	// note the time that the connection was made
//    lastConnectionTime = millis();
//	
//}
///////////////////////////////////////////////////////////////


/////////////////////////////////////////////////////////////// 
//                 MPC5125    CAN Library                    //
/////////////////////////////////////////////////////////////// 
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

  canMsg.can_id  = 0x0F6;
  canMsg.can_dlc = 8;
  //memcpy(&canMsg.data[0],&Poids,8);
  canMsg.data[0] = 0;
  canMsg.data[K_POS_ID_CAN_FRAME] = (char)ID_CAN;
  canMsg.data[K_POS_REINIT_CAN_FRAME] = (char)Reinit_Value;


//#ifdef DEBUG
//  Serial.println("*********** Envoi_Can ***********");
//  Serial.print("canMsg.data[0]:");
//  Serial.println(canMsg.data[0], 1);
//  Serial.print("canMsg.data[1]:");
//  Serial.println(canMsg.data[1], 1);
//  Serial.print("canMsg.data[2]:");
//  Serial.println(canMsg.data[2], 1);
//  Serial.print("canMsg.data[3]:");
//  Serial.println(canMsg.data[3], 1);
//  Serial.print("canMsg.data[4]:");
//  Serial.println(canMsg.data[4], 1);
//  Serial.print("canMsg.data[5]:");
//  Serial.println(canMsg.data[5], 1);
//  Serial.print("canMsg.data[6]:");
//  Serial.println(canMsg.data[6], 1);
//  Serial.print("canMsg.data[7]:");
//  Serial.println(canMsg.data[7], 1);
//#endif

  mcp2515.sendMessage(&canMsg); 

  delay(100);

}

void Reinitalisation_Rucher() {
  Envoi_Can(0, KEY_REINIT);
  Serial.println("Reinitalisation_Rucher");
} 

/*************************************************************/
/*            Module de lecture de Trames CAN                */
/*************************************************************/
void Lecture_Can(unsigned char *pu8_ruche, float *pf_poids)
{
  /*Init value*/ 
  unsigned int u16_poids;

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

    u16_poids = canMsg_Read.data[0] *1000 + canMsg_Read.data[1]*10;  // u8_Poids_high *1000 + u8_Poids_low*10;
    *pf_poids = float((float)u16_poids / 1000.0);

    *pu8_ruche=0;
    
   Serial.print("*pf_poids :");
   Serial.println(*pf_poids, 1);

  }
}

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
  unsigned int answer=0; 
  
  /* body */
  //pinMode(onModulePin, OUTPUT);
  
  setup_watchdog(9);// sleep mode

  //Begin serial comunication with Arduino and Arduino IDE (Serial Monitor)
  Serial.begin(9600);
  //while(!Serial);
  //Being serial communication with Arduino and SIM800
  serialSIM800.begin(9600);
  delay(1000);
  
  Init_Can();

//  while (1) {  
//    Send_Sms();
    
    //String SIM_PIN_CODE = String( "1234" );
//    serialSIM800.print("AT+CPIN=");
//    serialSIM800.println( SIM_PIN_CODE );
//  
//    serialSIM800.print("AT");
//    serialSIM800.print("AT+CSCLK=0");
//    serialSIM800.print("AT+CFUN=1");
//    serialSIM800.print("AT");
//  }

  
  //Reinitalisation_Rucher();
//  String SIM_PIN_CODE = String( "1234" );
//  serialSIM800.print("AT+CPIN=");
//  serialSIM800.println( SIM_PIN_CODE );
//  Serial.println("Setup OK");
   
  Serial.println("Starting...");
  
  //power_on();

  delay(3000);
//
//  serialSIM800.print("AT");
//  serialSIM800.print("AT+CSCLK=0");
//  serialSIM800.print("AT+CFUN=1");
//  serialSIM800.print("AT");


//  Test
  
//  answer = sendATcommand("AT\r\n", "OK\r\n", TIMEOUT); //sortie  mise en veille profonde Dummie AT
//  if (answer == 0)
//  { 
//    Serial.println("Pas glop...");
//  }
//  else
//  {
//    Serial.println("Glop...");    
//  }
//      delay(100);
//  
//  answer = sendATcommand("AT+CGMI=0\r\n", "OK\r\n", TIMEOUT); //sortie  mise en veille profonde Dummie AT
//  if (answer == 0)
//  { 
//    Serial.println("Pas glop...");
//  }
//  else
//  {
//    Serial.println("Pas glop...");    
//  }
//      delay(100);
//  
//  answer = sendATcommand("AT+CSCLK=0\r\n", "OK\r\n", TIMEOUT); //sortie  mise en veille profonde Dummie AT
//  if (answer == 0)
//  { 
//    Serial.println("Pas glop...");
//  }
//  else
//  {
//    Serial.println("Pas glop...");    
//  }
//      delay(100);
//  
//  answer = sendATcommand("AT+CFUN=1\r\n", "OK\r\n", TIMEOUT); //sortie  mise en veille profonde Dummie AT
//  if (answer == 0)
//  { 
//    Serial.println("Pas glop...");
//  }
//  else
//  {
//    Serial.println("Glop...");    
//  } 
//      delay(4000); 
//
////  HTTPRequest(1, 5.5);
//  
//  
//  Serial.println("AT+HTTPPARA=\"URL\",\"www.castillolk.com.ve/WhiteList.txt\"\r\n");
//  
//  while( (sendATcommand("AT+CREG?\r\n", "+CREG: 0,1\r\n", 500) || 
//            sendATcommand("AT+CREG?\r\n", "+CREG: 0,5\r\n", 500)) == 0 );

  /* sendATcommand("AT+SAPBR=3,1,\"Contype\",\"GPRS\"\r\n", "OK\r\n", TIMEOUT);//sets Contype
  snprintf(aux_str, sizeof(aux_str), "AT+SAPBR=3,1,\"APN\",\"%s\"\r\n", apn);//sets APN
  sendATcommand(aux_str, "OK\r\n", TIMEOUT); */
//  
// connectToNetwork();
// initHTTPSession(); 
}


/*************************************************************/
/*                            loop                           */
/*************************************************************/
void loop() {
  float f_poids;
  unsigned char u8_ruche;

   if (f_wdt == 1)
   {
    Serial.print("GO");
    f_wdt = 0; // Ne pas oublier d'initialiser le flag   
      Lecture_Can(&u8_ruche, &f_poids);
    //   Serial.print("f_poids:");
    //   Serial.println(f_poids, 1);
      Envoi_Can(69, 51);
      //Simulation_PeseRuche_Test();
      
      Send_Sms(u8_ruche, f_poids);
      // if 15 seconds have passed since your last connection,
      // then connect again and send data
      if (millis() - lastConnectionTime > postingInterval) 
      {
        //HTTPRequest(1, f_poids);
      }
    enterSleep(); //Revenir en mode veille
   }
   else
   {
    ;// do nothing
   }
}
