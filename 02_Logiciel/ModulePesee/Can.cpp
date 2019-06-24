
/******************************************************************************/
/* SOURCE : Can.c                                                        
/******************************************************************************/
/* DESCRIPTION  
 *  
 *  Ce module permet de communiquer sur le CAN
 *  
/******************************************************************************/
/* UTILISATION   
 *  
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
 
#include "Can.h"
#include "types.h"

/*--------------------------       DEFINITIONS       -------------------------*/

#define DEBUG


/******************************************************************************/
/*                             Interface CAN                                  */
/*                                                                            */
/* canMsg.data[K_POS_ID_CAN_FRAME] : 0x55 => Demande de reinitialisation      */
/* canMsg.data[K_POS_REINIT_CAN_FRAME] : ID CAN                               */
/******************************************************************************/
#define KEY_REINIT                0x55
#define K_POS_ID_CAN_FRAME        7
#define K_POS_REINIT_CAN_FRAME    6
#define K_STANDARD_FRAME_FORMAT   0x0F6
#define K_DLC                     8
  
/*--------------------------          STATIC         -------------------------*/ 
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
/*            Module de lecture de Trames CAN                */
/*************************************************************/
void Lecture_Can(ts_trame_can *ps_trame_can)
{ 
  /* Declarations */ 
  
  /*Init value*/
  
  /* Corps */
  if (mcp2515.readMessage(&canMsg_Read) == MCP2515::ERROR_OK)
  {
    
      /* Si demande de reinitialisation, alors tout reinitialiser */
      ps_trame_can->b_Demande_Reinit= canMsg_Read.data[K_POS_REINIT_CAN_FRAME];
      ps_trame_can->u16_Can_Id=canMsg_Read.data[K_POS_ID_CAN_FRAME];
      
#ifdef DEBUG 
    Serial.println("*********** Lecture CAN ***********"); 
    Serial.print(canMsg_Read.can_id, HEX); // print ID
    Serial.print(" "); 
    Serial.print(canMsg_Read.can_dlc, HEX); // print DLC
    Serial.print(" ");
    
    for (int i = 0; i<canMsg_Read.can_dlc; i++)  
    {  // print the data
        
      //Serial.print(canMsg_Read.data[i],HEX);
      Serial.println(canMsg_Read.data[i], 1);
      //Serial.print(" "); 
  
    }

    Serial.println();      
#endif
  }
}

/*************************************************************/
/*            Module d'envoi de Trames CAN                   */
/*************************************************************/
void Envoi_Can(ts_trame_can s_trame_can)
{ 
  /* Declarations */ 
  
  /*Init value*/
  
  /* Corps */
  canMsg_Write.can_id  = K_STANDARD_FRAME_FORMAT;
  canMsg_Write.can_dlc = K_DLC;
  //memcpy(&canMsg_Write.data[0],&Poids,8);  
  canMsg_Write.data[K_POS_POIDS_CAN_FRAME]=(char)s_trame_can.u8_Poids;
  canMsg_Write.data[K_POS_ID_CAN_FRAME]=s_trame_can.u16_Can_Id;
  canMsg_Write.data[K_POS_ID_CAN_FRAME]=s_trame_can.b_Demande_Reinit;
  
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
 
