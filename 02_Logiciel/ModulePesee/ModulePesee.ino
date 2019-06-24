
/******************************************************************************/
/* SOURCE : ModulePesee                                                         
/******************************************************************************/
/* DESCRIPTION  
 *  
 *  Ce module permet de peser une ruche et envoyer les données sur le CAN
 *  
/******************************************************************************/
/* UTILISATION   
 *  
 *  A la premiere utilisation, l'utilisateur devra faire une demande de 
 *  reinitialisation du rucher aprés avoir positionné tous les modules
 *  Puis le positionnement de chaque ruche definira l'identifiant de la ruche
 *  ex : premiere ruche posée => 1, deuxieme => 2, etc...
 *  
/******************************************************************************/
/* HISTORIQUE
 *    DATE        : 22/06/19
 *    DESCRIPTIF  : Creation
 *    AUTEUR      : SLO
 *    VERSION     : 1.0
 *     
/******************************************************************************/

/*--------------------------        INCLUDES         -------------------------*/

#include "HX711.h"
#include <SPI.h>
#include <mcp2515.h>
#include <math.h>

/*--------------------------       DEFINITIONS       -------------------------*/

#define DEBUG


/******************************************************************************/
/*                             Interface CAN                                  */
/*                                                                            */
/* canMsg.data[K_POS_ID_CAN_FRAME] : 0x55 => Demande de reinitialisation      */
/* canMsg.data[K_POS_REINIT_CAN_FRAME] : ID CAN                               */
/******************************************************************************/
#define KEY_REINIT 0x55
#define K_POS_ID_CAN_FRAME 7
#define K_POS_REINIT_CAN_FRAME 6
  
/*--------------------------          STATIC         -------------------------*/

static boolean b_Reinit=0;
static unsigned int u8_ID_MAX;
HX711 scale;

struct can_frame canMsg_Read; 
struct can_frame canMsg_Write; 
MCP2515 mcp2515(10);
   
#ifdef DEBUG
char buffer[8]; 
#endif 

/*************************************************************/
/*            Module d'initialisation du CAN                 */
/*************************************************************/
void Init_Can() 
{   
  /* Declarations */ 
  
  /*Init value*/
  
  /* Corps */
  SPI.begin();
  
  mcp2515.reset();
  mcp2515.setBitrate(CAN_125KBPS);
  mcp2515.setNormalMode(); 
}

/*************************************************************/
/*            Module de Reinitialisation du rucher           */
/*************************************************************/
void Reinitialiser(unsigned int *pu8_ID_CAN)
{
  /* Declarations */ 
  
  /* Init value */
  *pu8_ID_CAN=0;  // Mise à "0" du CAN ID
  b_Reinit=1;     // demande de Reinit
  
  /* Corps */
}

/*************************************************************/
/*            Module de lecture de Trames CAN                */
/*************************************************************/
void Lecture_Can(unsigned int *pu8_ID_CAN)
{ 
  /* Declarations */ 
  
  /*Init value*/
  
  /* Corps */
  if (mcp2515.readMessage(&canMsg_Read) == MCP2515::ERROR_OK) {
#ifdef DEBUG 
    Serial.println("*********** Lecture CAN ***********"); 
    Serial.print(canMsg_Read.can_id, HEX); // print ID
    Serial.print(" "); 
    Serial.print(canMsg_Read.can_dlc, HEX); // print DLC
    Serial.print(" ");
#endif
    
    for (int i = 0; i<canMsg_Read.can_dlc; i++)  {  // print the data
        
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
        if (*pu8_ID_CAN<canMsg_Read.data[K_POS_ID_CAN_FRAME])
        {
          *pu8_ID_CAN=canMsg_Read.data[K_POS_ID_CAN_FRAME]+1;
        
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
void Envoi_Can(const unsigned int u8_Poids, const char ID_CAN) 
{ 
  /* Declarations */ 
  
  /*Init value*/
  
  /* Corps */
  canMsg_Write.can_id  = 0x0F6;
  canMsg_Write.can_dlc = 8;
  //memcpy(&canMsg_Write.data[0],&Poids,8);  
  canMsg_Write.data[0]=(char)u8_Poids;
  canMsg_Write.data[K_POS_ID_CAN_FRAME]=ID_CAN;
 

#ifdef DEBUG
  Serial.println("*********** Envoi CAN ***********"); 
  
  Serial.print("canMsg_Write.data[0]:");  
  Serial.println(canMsg_Write.data[0], 1); 
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
  /* Declarations */ 
  
  /*Init value*/
  
  /* Corps */
  // parameter "gain" is ommited; the default value 128 is used by the library
  // HX711.DOUT  - pin #A1
  // HX711.PD_SCK - pin #A0
  scale.begin(A1, A0);

#ifdef DEBUG
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

  scale.set_scale(2280.f);                      // this value is obtained by calibrating the scale with known weights; see the README for details
  scale.tare();               // reset the scale to 0

#ifdef DEBUG
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
void Lecture_Poids(unsigned int *pu8_Poids,unsigned int *pu8_ID_CAN) 
{
  /* Declarations */ 
  
  /*Init value*/
  float f_Poids=0;
  
  /* Corps */
  f_Poids=scale.get_units(10);
#ifdef DEBUG
  Serial.print("one reading:\t");
  Serial.print(scale.get_units(), 1);
  Serial.print("\t| average:\t");
  Serial.println(f_Poids, 1);
#endif
  if (f_Poids>0)
  {
    *pu8_Poids=(unsigned char)f_Poids;
  }
  else
  {
    *pu8_Poids=0;
  }
  
#ifdef DEBUG
  Serial.print("*pu8_Poids:");  
  Serial.println(*pu8_Poids, 1);
#endif

  /* Si Demande de Reinit et Poids supérieur à 0 */
  if ((*pu8_Poids > 5)&&(b_Reinit != 0))
  {
    *pu8_ID_CAN++;
    b_Reinit=0;
    Serial.print("Incrementation CAN ID");    
  }

  scale.power_down();             // put the ADC in sleep mode
  delay(5000);
  scale.power_up();
}


/*************************************************************/
/*                        setup                              */
/*************************************************************/
void setup() 
{
  /* Declarations */ 
  
  /* Init value */
  u8_ID_MAX=0;

  /* Corps */
  Serial.begin(38400);
  Init_Poids();
  Init_Can(); 
} 

/*************************************************************/
/*                            loop                           */
/*************************************************************/
void loop() 
{
  /* Declarations */ 
  unsigned int u8_Poids_Moyen;
  static unsigned int u8_ID_CAN;
  
  /*Init value*/
  u8_Poids_Moyen=0;
  u8_ID_CAN=0;
  
  /* Corps */
  Lecture_Poids(&u8_Poids_Moyen,&u8_ID_CAN);
  Envoi_Can(u8_Poids_Moyen,u8_ID_CAN);
  Lecture_Can(&u8_ID_CAN);
}
