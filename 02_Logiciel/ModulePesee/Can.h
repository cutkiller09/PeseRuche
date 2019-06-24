
/******************************************************************************/
/* SOURCE : Can.h                                                        
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
#include "HX711.h"
#include <SPI.h>
#include "types.h"
#include <mcp2515.h> 

/*--------------------------       DEFINITIONS       -------------------------*/ 

/******************************************************************************/
/*                             Interface CAN                                  */
/*                                                                            */
/* canMsg.data[K_POS_ID_CAN_FRAME] : 0x55 => Demande de reinitialisation      */
/* canMsg.data[K_POS_REINIT_CAN_FRAME] : ID CAN                               */
/******************************************************************************/
#define KEY_REINIT 0x55
#define K_POS_ID_CAN_FRAME 7
#define K_POS_REINIT_CAN_FRAME 6
   
void Init_Can() ; 
void Lecture_Can(uint8 *pu8_ID_CAN,bool *pb_Demande_Reinit);
void Envoi_Can(const uint8 u8_Poids, const uint8 u8_ID_CAN); 
 
