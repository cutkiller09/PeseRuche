#include "HX711.h"
#include <SPI.h>
#include <mcp2515.h>
#include <math.h>

#define DEBUG
#define KEY_REINIT 0x55
#define K_POS_ID_CAN_FRAME 7
#define K_POS_REINIT_CAN_FRAME 6
  
static boolean b_Reinit=0;
static unsigned int u8_ID_MAX;
  
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
void Reinitialiser(unsigned int *pu8_ID_CAN){
  /*Init value*/
  *pu8_ID_CAN=0;  // Mise à "0" du CAN ID
  b_Reinit=1;     // demande de Reinit
  
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
  /*Init value*/
  
  /* body */
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
  /*Init value*/
  
  /* body */
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
  /*Init value*/
  float f_Poids=0;
  
  /* body */
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
void setup() {
  /*Init value*/
  u8_ID_MAX=0;

  /* body */
  Serial.begin(38400);
  Init_Poids();
  Init_Can(); 
} 

/*************************************************************/
/*                            loop                           */
/*************************************************************/
void loop() {
  /*Init value*/
  unsigned int u8_Poids_Moyen;
  static unsigned int u8_ID_CAN;
  
  /* body */
  Lecture_Poids(&u8_Poids_Moyen,&u8_ID_CAN);
  Envoi_Can(u8_Poids_Moyen,u8_ID_CAN);
  Lecture_Can(&u8_ID_CAN);
}
