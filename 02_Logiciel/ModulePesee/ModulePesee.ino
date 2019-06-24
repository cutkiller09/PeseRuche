
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
#include "Can.h"
#include "types.h"
#include <math.h>

/*--------------------------       DEFINITIONS       -------------------------*/

#define DEBUG 
  
/*--------------------------          STATIC         -------------------------*/

static bool b_Reinit=false;
static uint8 u8_ID_MAX;
HX711 scale;  
 

/*************************************************************/
/*            Module de Reinitialisation du rucher           */
/*************************************************************/
void Reinitialiser(uint8 *pu8_ID_CAN)
{
  /* Declarations */ 
  
  /* Init value */
  *pu8_ID_CAN=0;  // Mise à "0" du CAN ID
  b_Reinit=true;     // demande de Reinit
  
  /* Corps */
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
void Lecture_Poids(uint8 *pu8_Poids,uint8 *pu8_ID_CAN) 
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

  /* Si Demande de Reinit et Poids supérieur à 5 */
  if ((*pu8_Poids > 5)&&(b_Reinit != false))
  {
    *pu8_ID_CAN++;
    b_Reinit=false;
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
  uint8 u8_Poids_Moyen;
  static uint8 u8_ID_CAN;
  bool b_Demande_Reinit=false; 
  
  /*Init value*/
  u8_Poids_Moyen=0;
  u8_ID_CAN=0;
  
  /* Corps */
  Lecture_Poids(&u8_Poids_Moyen,&u8_ID_CAN);
  Envoi_Can(u8_Poids_Moyen,u8_ID_CAN);
  Lecture_Can(&u8_ID_CAN,&b_Demande_Reinit);

  /* Si demande de reinitialisation demandee */
  if (false != b_Demande_Reinit)
  {
        Reinitialiser(&u8_ID_CAN);    
  }
}
