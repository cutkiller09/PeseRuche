#include "HX711.h"
#include <SPI.h>
#include <mcp2515.h>
#include <math.h>
//Optimisation de la consommation
#include <avr/power.h>
#include <avr/sleep.h>
#include <avr/wdt.h>
volatile int f_wdt=1;
volatile int u16_cpt_attente=0;
#define K_MAX_ATTENTE 20 // attente de K_MAX_ATTENTE fois 8 secondes
#define DEBUG
#define KEY_REINIT 0x55
#define K_RUCHE_ID_CAN_FRAME 1
#define K_POS_ID_CAN_FRAME 7
#define K_POS_REINIT_CAN_FRAME 6
#define K_SERIAL 38400  // vitesse du port série en bauds

#define K_OFFSET_A_VIDE 0 // 86 Offset à vide
#define K_POIDS_BRUTE (470668-K_OFFSET_A_VIDE) // valeur brute lue pour la valeur de reference donnee (ici 10kg)
#define K_POIDS_REFERENCE 11200 //11.2 kg 

static boolean b_Reinit = 0;
static unsigned int u8_ID_MAX;

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



/******************************************************************************/
/*                             Interface CAN                                  */
/*                                                                            */
/* canMsg.data[K_POS_ID_CAN_FRAME] : 0x55 => Demande de reinitialisation      */
/* canMsg.data[K_POS_REINIT_CAN_FRAME] : ID CAN                               */
/******************************************************************************/

#ifdef DEBUG
char buffer[8];
#endif

HX711 scale;

unsigned int u16_i=0;
float array_raw_value[10];
struct can_frame canMsg_Read;
struct can_frame canMsg_Write;
MCP2515 mcp2515(10);


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


/*************************************************************/
/*            Module de Reintialisation du rucher            */
/*************************************************************/
void Reinitialiser(unsigned int *pu8_ID_CAN) {
  /*Init value*/
  *pu8_ID_CAN = 0; // Mise à "0" du CAN ID
  b_Reinit = 1;   // demande de Reinit

  /* body */
}

/*************************************************************/
/*            Module de lecture de Trames CAN                */
/*************************************************************/
void Lecture_Can(unsigned int *pu8_ID_CAN)
{
  /*Init value*/

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
      /* Si demande de reinitialisation, alors tout reinitialiser */
      if (KEY_REINIT == canMsg_Read.data[K_POS_REINIT_CAN_FRAME])
      {
        Serial.print("Reinitialiser");
        Reinitialiser(pu8_ID_CAN);
#ifdef DEBUG
        Serial.print("Reinitialiser");
#endif
      }
      /* Si l'ID emis supérieur à l'ID actuel, alors on incremente l'ID */
      if (*pu8_ID_CAN < canMsg_Read.data[K_POS_ID_CAN_FRAME])
      {
        *pu8_ID_CAN = canMsg_Read.data[K_POS_ID_CAN_FRAME] + 1;

#ifdef DEBUG
        Serial.print("Incrementation u8_ID_CAN");
        Serial.print("*pu8_ID_CAN:");
        Serial.println(*pu8_ID_CAN, 1);
#endif
      }
    }

    Serial.println();
  }
}

/*************************************************************/
/*            Module d'envoi de Trames CAN                   */
/*************************************************************/
void Envoi_Can(const unsigned char u8_Poids_high,const unsigned char u8_Poids_low, const char ID_CAN)
{
  /*Init value*/

  /* body */
  canMsg_Write.can_id  = 0x0F6;
  canMsg_Write.can_dlc = 8;
  //memcpy(&canMsg_Write.data[0],&Poids,8);
  canMsg_Write.data[0] = u8_Poids_high;
  canMsg_Write.data[1] = u8_Poids_low;
  canMsg_Write.data[K_POS_ID_CAN_FRAME] = ID_CAN;


#ifdef DEBUG
  Serial.println("*********** Envoi CAN ***********");

  Serial.print("canMsg_Write.data[0]:");
  Serial.println(canMsg_Write.data[0], 1);
  Serial.print("canMsg_Write.data[1]:");
  Serial.println(canMsg_Write.data[1], 1);
  Serial.print("canMsg_Write.data[K_POS_REINIT_CAN_FRAME]:");
  Serial.println(canMsg_Write.data[K_POS_REINIT_CAN_FRAME], 1);
  Serial.print("canMsg_Write.data[K_POS_ID_CAN_FRAME]:");
  Serial.println(canMsg_Write.data[K_POS_ID_CAN_FRAME], 1);
#endif

  mcp2515.sendMessage(&canMsg_Write);

  delay(100);

}

/*************************************************************/
/*            Module d'envoi d'Initialisaion du Poids        */
/*************************************************************/
void Init_Poids()
{
  /*Init value*/ 
  float m = 0.01393321;
  int b = -510;
  float raw_value;
  float masse;
  float masse_kg;
  
  /* body */
  // parameter "gain" is ommited; the default value 128 is used by the library
  // HX711.DOUT  - pin #A1
  // HX711.PD_SCK - pin #A0
  scale.begin(A1, A0);

#ifdef DEBUG-2
  Serial.println("Before setting up the scale:");
  Serial.print("read: \t\t");
  Serial.println(scale.read());     // print a raw reading from the ADC

  Serial.print("read average: \t\t");
  Serial.println(scale.read_average(20));   // print the average of 20 readings from the ADC

  Serial.print("get value: \t\t");
  Serial.println(scale.get_value(5));   // print the average of 5 readings from the ADC minus the tare weight (not set yet)

  Serial.print("get units: \t\t");
  Serial.println(scale.get_units(5), 1);  // print the average of 5 readings from the ADC minus tare weight (not set) divided
  // by the SCALE parameter (not set yet)
#endif

  scale.set_scale();                      // this value is obtained by calibrating the scale with known weights; see the README for details
  //scale.set_scale(2280.f);                      // this value is obtained by calibrating the scale with known weights; see the README for details
   
  scale.tare();               // reset the scale to 0 

#ifdef DEBUG-2
  Serial.println("After setting up the scale:");

  Serial.print("read: \t\t");
  Serial.println(scale.read());                 // print a raw reading from the ADC

  Serial.print("read average: \t\t");
  Serial.println(scale.read_average(20));       // print the average of 20 readings from the ADC

  Serial.print("get value: \t\t");
  Serial.println(scale.get_value(5));   // print the average of 5 readings from the ADC minus the tare weight, set with tare()

  Serial.print("get units: \t\t");
  Serial.println(scale.get_units(5), 1);        // print the average of 5 readings from the ADC minus tare weight, divided
  // by the SCALE parameter set with set_scale

  Serial.println("Readings:");
#endif
}


/*************************************************************/
/*             Module de lecture du Poids                    */
/*************************************************************/
void Lecture_Poids(unsigned int *pu8_Poids_high,unsigned int *pu8_Poids_low, unsigned int *pu8_ID_CAN)
{
  /*Init value*/
  unsigned int u16_Poids = 0;
  unsigned char i;
  float average_raw_value=0;
  float masse;
  float masse_kg;
  float raw_value;

  /* body */
  
  raw_value = scale.get_units(10);
  array_raw_value[u16_i++]=raw_value; 
  Serial.print("raw_value:\t");
  Serial.println(raw_value, 1);  // print the average of 5 readings from the ADC minus tare weight (not set) divided
  if (10<=u16_i)
  {
    for (i=0;i<u16_i;i++)
    {
      average_raw_value=average_raw_value+array_raw_value[i];
    }
    average_raw_value=average_raw_value/u16_i;
    Serial.print("average_raw_value:\t");
    Serial.println(average_raw_value, 1);  // print the average of 5 readings from the ADC minus tare weight (not set) divided
    u16_i=0;
  }

  u16_Poids= (K_POIDS_REFERENCE*(raw_value+K_OFFSET_A_VIDE))/K_POIDS_BRUTE;
  Serial.print("u16_Poids:\t");
  Serial.println(u16_Poids, 1);  // print the average of 5 readings from the ADC minus tare weight (not set) divided 
  
  Serial.print("u16_Poids:\t");
  Serial.println(u16_Poids, 1);
#ifdef DEBUG
  Serial.print("one reading:\t");
  Serial.print(scale.get_units(), 1);
  Serial.print("\t| average:\t");
  Serial.println(u16_Poids, 1);
#endif
  if (u16_Poids > 0)
  {
    *pu8_Poids_high = u16_Poids/1000;
    *pu8_Poids_low = (u16_Poids%1000)/10;
  }
  else
  {
    *pu8_Poids_high = 0;
  }

#ifdef DEBUG
  Serial.print("*pu8_Poids_high:");
  Serial.println(*pu8_Poids_high, 1);
  Serial.print("*pu8_Poids_low:");
  Serial.println(*pu8_Poids_low, 1);
#endif

  /* Si Demande de Reinit et Poids supérieur à 0 */
  if ((*pu8_Poids_high > 5) && (b_Reinit != 0))
  {
    *pu8_ID_CAN++;
    b_Reinit = 0;
    Serial.print("Incrementation CAN ID");
  }

  scale.power_down();             // put the ADC in sleep mode
  delay(5000);
  scale.power_up();
}


/*************************************************************/
/*                        setup                              */
/*************************************************************/
void setup() {
  /*Init value*/
  u8_ID_MAX = 0;

  /* body */
  Serial.begin(K_SERIAL);
  Init_Poids();
  Init_Can(); 
  
  setup_watchdog(9);// sleep mode
}

/*************************************************************/
/*                            loop                           */
/*************************************************************/
void loop() {
  /*Init value*/
  unsigned int u8_Poids_Moyen_high;
  unsigned int u8_Poids_Moyen_low;
  static unsigned int u8_ID_CAN;

  /* body */
   if (f_wdt == 1)
   {
    f_wdt = 0; // Ne pas oublier d'initialiser le flag   
    Lecture_Poids(&u8_Poids_Moyen_high, &u8_Poids_Moyen_low, &u8_ID_CAN);
    Envoi_Can(u8_Poids_Moyen_high, u8_Poids_Moyen_low, K_RUCHE_ID_CAN_FRAME); // on fixe l'ID de la ruche à 2 !
    Lecture_Can(&u8_ID_CAN);
    enterSleep(); //Revenir en mode veille
  } 
  else  
  {
    /* Do nothing. */
  }
 
}
