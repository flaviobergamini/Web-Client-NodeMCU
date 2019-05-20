#include <Arduino.h>
#include <string.h>

#include <ESP8266WiFi.h>
//#include <ESP8266WiFiMulti.h>
#include <ESP8266HTTPClient.h>
//#define USE_SERIAL Serial
#include <WiFiClient.h>
#include <EEPROM.h>
#include <ArduinoJson.h>
WiFiClient client;
//ESP8266WiFiMulti WiFiMulti;

//-------------------------------------------------------------------------------------------------------------------------
const char *ssid_new;
const char *password_new;
const char *ssid      = "REP MIXTO QUENTE";              //SSID inicialmente com rede padrão
const char *password  = "advinha32";                     //Password inicialmente com rede padrão
int port = 80;
String serverAddress_http = "http://10.0.0.108";        //Endereço do servidor pelo protocolo HTTP
int set_command = 1;



//-------------------------------------------------------------------------------------------------------------------------
void logica()   //Desenvolver a logica necessária para a aplicação
{
  for(int i = 0; i < 5; i ++)
  {
    digitalWrite(D3, HIGH);
    delay(200);
    digitalWrite(D3, LOW);
    delay(200);
  }
  
}

void standard_connect()
{
   for (uint8_t t = 4; t > 0; t--) {
    Serial.printf("[SETUP] WAIT %d...\n", t);
    Serial.flush();
    delay(800);
  }

  WiFi.mode(WIFI_STA);
  //WiFiMulti.addAP("REP MIXTO QUENTE", "advinha32");
  WiFi.begin(ssid, password);
}

void setup_connect()
{
  switch(set_command)
  {
    case 1:
            if ((WiFi.status() == WL_CONNECTED)) {
        
            HTTPClient http;
        
           // Serial.print("[HTTP] begin...\n");
            http.begin(serverAddress_http); //HTTP
        
           // Serial.print("[HTTP] GET...\n");
            int httpCode = http.GET();
        
            if (httpCode > 0) {
              // HTTP header has been send and Server response header has been handled
              Serial.printf("[HTTP] GET... code: %d\n", httpCode);
        
              // file found at server
              if (httpCode == HTTP_CODE_OK) {
                String pay = http.getString();
                //Serial.println("payload em string\n");
                Serial.println(pay);
                DynamicJsonDocument doc(1024);
                deserializeJson(doc, pay);
                ssid_new = doc["username"];
                password_new = doc["password"];
                Serial.println(ssid_new);
                Serial.println(password_new);
                set_command = 2;
                setup_connect();
              }
                
            } 
            else {
              Serial.printf("[HTTP] GET... failed, error: %s\n", http.errorToString(httpCode).c_str());
            }
            http.end();
          }
          delay(10000);
    break;
    case 2:
          ssid = ssid_new;
          password = password_new;
          standard_connect();
          delay(1000);
          if ((WiFi.status() == WL_CONNECTED)) {
            int loop1 = 1;
            int loop2 = 1;
            Serial.println("Apagando buffer da EEPROM\n");
            //eraseEEPROM();
            Serial.println("Escrevendo na EEPROM\n");
            WriteEEPROM(ssid, password);

            while(loop1)
            { 
               logica();     //Desenvolver a logica necessária para a aplicação
              Serial.println("Conectado na rede definitiva\n");
              digitalWrite(D1,LOW);
              digitalWrite(D0,HIGH);
              digitalWrite(D4,LOW);
              delay(1000);
              while(loop2 == 1)
              {
                ReadEEPROM(ssid, password);
                loop2 = 2;
              }
              if ((WiFi.status() == WL_CONNECTED))
                loop1 = 1;
              else
                loop1 = 0;
            }
          }
           else
          {
            Serial.println("Nao foi possivel conectar na rede definitiva\n");
            digitalWrite(D1,HIGH);
            digitalWrite(D0,LOW);
            digitalWrite(D4,LOW);
            standard_connect();
            setup_connect();
            //Liga Red LED
            //Desliga Green LED
            delay(1000);
          }
      break;
  }
}  

void setup() {

  Serial.begin(115200);
  EEPROM.begin(1024);
  standard_connect();
  pinMode(D0, OUTPUT);
  pinMode(D1, OUTPUT);
  pinMode(D3, OUTPUT);
  pinMode(D4, OUTPUT);
  digitalWrite(D0,LOW);
  digitalWrite(D1,LOW);
  digitalWrite(D3,LOW);
  digitalWrite(D4,LOW);
  
}

void loop() {
  // wait for WiFi connection
  //if ((WiFiMulti.run() == WL_CONNECTED)) {
  setup_connect();
}

void eraseEEPROM() {
  EEPROM.begin(1024);
  for (int i = 0; i < 1024; i++) {
    EEPROM.write(i, 0);
  }

  // turn the LED on when we're done
  pinMode(D4, OUTPUT);
  digitalWrite(D4, HIGH);
  EEPROM.end();
}

void WriteEEPROM(String ssid, String password)
{
  EEPROM.begin(1024);
  int ssidlen = ssid.length();
  int passlen = password.length();
 
  Serial.println("writing eeprom ssid:");
          for (int i = 0; i < ssidlen; ++i)
            {
              EEPROM.write(i, ssid[i]);
              Serial.print("Wrote: ");
              Serial.println(ssid[i]); 
            }

  Serial.println("writing eeprom password:");
          for (int i = 0; i < passlen; ++i)
            {
              EEPROM.write((i+ssidlen), password[i]);
              Serial.print("Wrote: ");
              Serial.println(password[i]); 
            }
  EEPROM.end();
}

void ReadEEPROM(String ssid, String password)
{
  EEPROM.begin(1024);
  int ssidlen = ssid.length();
  int passlen = password.length();

  Serial.println("Reading EEPROM ssid");
  String esid;
  for (int i = 0; i < ssidlen; ++i)
    {
      esid += char(EEPROM.read(i));
    }
    //esid.trim();
  Serial.println(esid.length());
  Serial.print("SSID saida: ");
  Serial.println(esid);
  
  Serial.println("\n");
  
  Serial.println("Lendo password EEPROM");
  String passq;
  for (int i = (ssidlen); i < (ssidlen+passlen); ++i)
  {
    passq +=char(EEPROM.read(i));
  }
  Serial.println("Tamanho da password: ");
  Serial.println(passq.length());
  Serial.println("Password saida: ");
  Serial.println(passq);
  EEPROM.end();
}
