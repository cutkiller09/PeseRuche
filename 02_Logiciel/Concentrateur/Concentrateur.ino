
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

#include <SPI.h>
#include <mcp2515.h>

/*--------------------------       DEFINITIONS       -------------------------*/

#define DEBUG
#define ID 3

/******************************************************************************/
/*                             Interface CAN                                  */
/*                                                                            */
/* canMsg.data[K_POS_ID_CAN_FRAME] : 0x55 => Demande de reinitialisation      */
/* canMsg.data[K_POS_REINIT_CAN_FRAME] : ID CAN                               */
/******************************************************************************/
#define KEY_REINIT 0x55
#define K_POS_ID_CAN_FRAME 7
#define K_POS_REINIT_CAN_FRAME 6

#ifdef DEBUG
char buffer[8]; 
#endif

struct can_frame canMsg;
MCP2515 mcp2515(10);

/*--------------------------          STATIC         -------------------------*/

/*************************************************************/
/*            Module d'initialisation du CAN                 */
/*************************************************************/
void Init_Can() 
{   
  Serial.begin(115200);
  SPI.begin();
  
  mcp2515.reset();
  mcp2515.setBitrate(CAN_125KBPS);
  mcp2515.setNormalMode();
  
  Serial.println("------- CAN Read ----------");
  Serial.println("ID  DLC   DATA");
}


/*************************************************************/
/*            Module d'envoi sur le CAN                      */
/*************************************************************/
void Envoi_Can(char ID_CAN, char Reinit_Value) 
{
Serial.print("Envoi_Can:");  
  
  canMsg.can_id  = 0x0F6;
  canMsg.can_dlc = 8;
  //memcpy(&canMsg.data[0],&Poids,8);  
  canMsg.data[0]=0;
  canMsg.data[K_POS_ID_CAN_FRAME]=(char)ID_CAN;
  canMsg.data[K_POS_REINIT_CAN_FRAME]=(char)Reinit_Value;
 

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

/*************************************************************/
/*                  Reinitalisation_Rucher                   */
/*************************************************************/
void Reinitalisation_Rucher(){
  Envoi_Can(0,KEY_REINIT);
  Serial.println("Reinitalisation_Rucher");
}

/*************************************************************/
/*              Simulation_PeseRuche_Test                    */
/*************************************************************/
void Simulation_PeseRuche_Test(){  
  Envoi_Can(ID,0);
}

/*************************************************************/
/*                       Lecture_Can                         */
/*************************************************************/
void Lecture_Can()
{ 
  if (mcp2515.readMessage(&canMsg) == MCP2515::ERROR_OK) {
      
  Serial.println("*********** Lecture_Can ***********");
    Serial.print(canMsg.can_id, HEX); // print ID
    Serial.print(" "); 
    Serial.print(canMsg.can_dlc, HEX); // print DLC
    Serial.print(" ");
    
    for (int i = 0; i<canMsg.can_dlc; i++)  {  // print the data
        
      //Serial.print(canMsg.data[i],HEX);
      Serial.println(canMsg.data[i], 1);
      //Serial.print(" ");

    }

    Serial.println();      
  }
}

/*************************************************************/
/*                          Setup                            */
/*************************************************************/
void setup() {
  Init_Can();
  Reinitalisation_Rucher();
}

/*************************************************************/
/*                          Loop                             */
/*************************************************************/
void loop() {
  Lecture_Can();
  Simulation_PeseRuche_Test();
}
