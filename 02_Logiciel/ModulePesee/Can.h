
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

#include "types.h"
#include <mcp2515.h> 

/*--------------------------       DEFINITIONS       -------------------------*/ 
typedef struct
{
  uint16 u16_Can_Id;
  uint8  u8_Poids; 
  bool  b_Demande_Reinit;   
} ts_trame_can;

/******************************************************************************/
/*                             Interface CAN                                  */
/*                                                                            */
/* canMsg.data[K_POS_ID_CAN_FRAME] : 0x55 => Demande de reinitialisation      */
/* canMsg.data[K_POS_REINIT_CAN_FRAME] : ID CAN                               */
/******************************************************************************/
#define KEY_REINIT 0x55
#define K_POS_ID_CAN_FRAME 7
#define K_POS_REINIT_CAN_FRAME 6
#define K_POS_POIDS_CAN_FRAME 0
   
void Init_Can() ; 
void Lecture_Can(ts_trame_can *ps_trame_can);
void Envoi_Can(ts_trame_can s_trame_can); 
 
