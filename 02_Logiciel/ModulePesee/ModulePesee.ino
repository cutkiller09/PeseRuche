#include "HX711.h"
#include <SPI.h>
#include <mcp2515.h>
#include <math.h>

#define DEBUG
#define KEY_REINIT 0x55
#define DEBUG
#define K_POS_ID_CAN_FRAME 7
#define K_POS_REINIT_CAN_FRAME 6
 

static boolean b_Reinit=0;
  
/******************/
/* Interface CAN  */
// canMsg.data[K_POS_ID_CAN_FRAME] : 0x55 => Demande de reinitialisation
// canMsg.data[K_POS_REINIT_CAN_FRAME] : ID CAN

#ifdef DEBUG
char buffer[8]; 
#endif

HX711 scale;

struct can_frame canMsg_Read; 
struct can_frame canMsg_Write; 
MCP2515 mcp2515(10);


void Init_Can() 
{   
  SPI.begin();
  
  mcp2515.reset();
  mcp2515.setBitrate(CAN_125KBPS);
  mcp2515.setNormalMode();
  
  Serial.println("Example: Write to CAN");
}

void Reinitialiser(unsigned int *pu8_ID_CAN){
  *pu8_ID_CAN=0;
  b_Reinit=1;
}


void Lecture_Can(unsigned int *pu8_ID_CAN)
{ 
  if (mcp2515.readMessage(&canMsg_Read) == MCP2515::ERROR_OK) {
    Serial.print("Lecture CAN");   
    Serial.print(canMsg_Read.can_id, HEX); // print ID
    Serial.print(" "); 
    Serial.print(canMsg_Read.can_dlc, HEX); // print DLC
    Serial.print(" ");
    
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
      if (0 != canMsg_Read.data[K_POS_ID_CAN_FRAME])
      {
        if (*pu8_ID_CAN<canMsg_Read.data[K_POS_ID_CAN_FRAME])
        {
          *pu8_ID_CAN=canMsg_Read.data[K_POS_ID_CAN_FRAME]+1;
          Serial.print("Incrementation u8_ID_CAN");  
        }
        
#ifdef DEBUG
  Serial.print("*pu8_ID_CAN:");  
  Serial.println(*pu8_ID_CAN, 1);
#endif
      }
    }

    Serial.println();      
  }
}

void Envoi_Can(const unsigned int u8_Poids, const char ID_CAN) 
{

#ifdef DEBUG
  Serial.print("u8_Poids:");  
  Serial.println(u8_Poids, 1);
#endif
  
  canMsg_Write.can_id  = 0x0F6;
  canMsg_Write.can_dlc = 8;
  //memcpy(&canMsg_Write.data[0],&Poids,8);  
  canMsg_Write.data[0]=(char)u8_Poids;
  canMsg_Write.data[K_POS_ID_CAN_FRAME]=ID_CAN;
 

#ifdef DEBUG
    Serial.print("Ecriture CAN");    
  
  Serial.print("canMsg_Write.data[0]:");  
  Serial.println(canMsg_Write.data[0], 1); 
  Serial.print("canMsg_Write.data[K_POS_REINIT_CAN_FRAME]:");  
  Serial.println(canMsg_Write.data[K_POS_REINIT_CAN_FRAME], 1);
  Serial.print("canMsg_Write.data[K_POS_ID_CAN_FRAME]:");  
  Serial.println(canMsg_Write.data[K_POS_ID_CAN_FRAME], 1); 
#endif 
  
  mcp2515.sendMessage(&canMsg_Write); 

  Serial.println("Messages sent");
  
  delay(100);

}

void Init_Poids()
{
  Serial.println("HX711 Demo");

  Serial.println("Initializing the scale");
  // parameter "gain" is ommited; the default value 128 is used by the library
  // HX711.DOUT  - pin #A1
  // HX711.PD_SCK - pin #A0
  scale.begin(A1, A0);

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

  scale.set_scale(2280.f);                      // this value is obtained by calibrating the scale with known weights; see the README for details
  scale.tare();               // reset the scale to 0

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
} 

void setup() {
  Serial.begin(38400);
  Init_Poids();
  Init_Can(); 
}


void Lecture_Poids(unsigned int *pu8_Poids,unsigned int *pu8_ID_CAN) {
  float f_Poids=0;
  
  f_Poids=scale.get_units(10);
  Serial.print("one reading:\t");
  Serial.print(scale.get_units(), 1);
  Serial.print("\t| average:\t");
  Serial.println(f_Poids, 1);
  if (f_Poids>0)
  {
    *pu8_Poids=(unsigned char)f_Poids;
  }
  else
  {
    *pu8_Poids=0;
  }
  
  Serial.print("*pu8_Poids:");  
  Serial.println(*pu8_Poids, 1);

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


void loop() {
  unsigned int u8_Poids_Moyen;
  static unsigned int u8_ID_CAN;
  
  Lecture_Poids(&u8_Poids_Moyen,&u8_ID_CAN);
  Envoi_Can(u8_Poids_Moyen,u8_ID_CAN);
  Lecture_Can(&u8_ID_CAN);
}
