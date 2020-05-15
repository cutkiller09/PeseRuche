// Importation des librairies
#include <SoftwareSerial.h>

SoftwareSerial sim800l(7,8);          //RX, TXnboolean data;

void setup()
{
    // Démarrage du modem
    sim800l.begin(9600);
    // Initialisation de la communication série 
    Serial.begin(9600);
    delay(500);
    data = true;
}

void loop() 
{
    if(data==true)
    {
        getdata();
        data = false;
    }
    if (sim800l.available()>0) 
    {
        Serial.write(sim800l.read());
    } 
}

void getdata()
{
    Serial.println("Get data HTTP");
    //set up GPRS, connection type
    sim800l.print("AT+SAPBR=3,1,"CONTYPE","GPRS"r");
    delay(1000);
    // set the APN
    sim800l.print("AT+SAPBR=3,1,"APN","free"r");
    delay(1000);
    //tries to connect to GPRS
    sim800l.print("AT+SAPBR=1,1r");
    delay(2000);
    // initialise embedded HTTP ruttine
    sim800l.print("AT+HTTPINITr");
    delay(2000);
    // set the HTTP session
    sim800l.print("AT+HTTPPARA="CID",1r");
    delay(1000); 
    Serial.println("Enter the URL of the website: ");
    char url[100];
    readSerial(url);  // votre URL
    Serial.println(url);
    // set the HTTP URL
    sim800l.print("AT+HTTPPARA="URL","");
    sim800l.print(url);
    sim800l.print(""r");
    delay(1000);
    // start the session
    sim800l.print("AT+HTTPACTION=0r");
    delay(10000);
    // read the data of the HTTP server
    sim800l.print("AT+HTTPREADr");
    delay(1000);
    // terminate the HTTP server
    sim800l.print("AT+HTTPTERMr");
    delay(100);
    sim800l.println();
}

int readSerial(char result[]) 
{
    int i = 0;
    while (1) 
    {
        while (Serial.available() > 0) 
        {
            char inChar = Serial.read();
            if (inChar == 'n') 
            {
                result[i] = '';
                Serial.flush();
                return 0;
            }
            if (inChar != 'r') 
            {
                result[i] = inChar;
                i++;
            }
        }
    }
}