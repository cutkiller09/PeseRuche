
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

#include "Can.h"
#include "Poids.h"
#include "types.h"
#include <math.h>

/*--------------------------       DEFINITIONS       -------------------------*/

#define DEBUG 
  
/*--------------------------          STATIC         -------------------------*/
static uint8 u8_CAN_ID_MAX;
static ts_trame_can s_trame_can_emise;
static ts_trame_can s_trame_can_recue;
 

/*************************************************************/
/*            Module de Reinitialisation du rucher           */
/*************************************************************/
void Gerer_Reinitialisation(ts_trame_can s_trame_can_recue,ts_trame_can *ps_trame_can_emise)
{
  /* Declarations */ 
  static bool b_Raz_Id=false;
  
  /* Init value */
  
  /* Si demande de reinitialisation du CAN-ID recu par l'utilisateur */
  if (false != s_trame_can_recue.b_Demande_Reinit)
  {
    ps_trame_can_emise->u16_Can_Id=0; // Mise à "0" du CAN ID
    b_Raz_Id=true;                    // demande de Reinit
    u8_CAN_ID_MAX=1;
  }
  
  /* Si le CAN ID recu d'une trame est superieur a notre CAN-ID                           */
  /* Alors cela veut dire qu'une ruche a deja doné son ID et que nous avons l'ID suivant  */
  if (u8_CAN_ID_MAX<s_trame_can_recue.u16_Can_Id)
      {
        u8_CAN_ID_MAX=s_trame_can_recue.u16_Can_Id+1;
      
#ifdef DEBUG
        Serial.print("Incrementation u8_CAN_ID_MAX");  
        Serial.print("u8_CAN_ID_MAX:");  
        Serial.println(u8_CAN_ID_MAX, 1);
#endif 
      }
   
  /* Si Demande de Reinit et Poids supérieur à 5 */
  if ((ps_trame_can_emise->u8_Poids > 5)&&(b_Raz_Id != false))
  {
    ps_trame_can_emise->u16_Can_Id=u8_CAN_ID_MAX;
    b_Raz_Id=false;
    Serial.print("Mise à jour du CAN ID");    
  }
  
  /* Corps */
} 

/*************************************************************/
/*                        setup                              */
/*************************************************************/
void setup() 
{
  /* Declarations */ 
  
  /* Init value */
  u8_CAN_ID_MAX=0;
  
  s_trame_can_emise.u16_Can_Id=0;
  s_trame_can_emise.u8_Poids=0;
  s_trame_can_emise.b_Demande_Reinit=false;
  
  s_trame_can_recue.u16_Can_Id=0;
  s_trame_can_recue.u8_Poids=0;
  s_trame_can_recue.b_Demande_Reinit=false;

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
  bool b_Demande_Reinit=false; 
  
  /*Init value*/ 
  
  /* Corps */
  Lecture_Poids(&s_trame_can_emise.u8_Poids);

  Envoi_Can(s_trame_can_emise);
  Lecture_Can(&s_trame_can_recue);
  Gerer_Reinitialisation(s_trame_can_recue,&s_trame_can_emise); 
}
