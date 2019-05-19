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
const char *ssid      = "REP MIXTO QUENTE";
const char *password  = "advinha32";
int port = 80;
String serverAddress_http = "http://10.0.0.108";
String serverAddress = "10.0.0.108";
int set_command = 1;

String autoRegisterUsername = "iPhone Bergamini";
String autoRegisterPassword = "12345678";
const int ssidAddress[] = {0, 40};
const int wiFipasswordAddress[] = {40, 80};
const int isWiFiConfigured[] = {80, 120};
const int deviceUsernameAddress[] = {120, 160};
const int devicePasswordAddress[] = {160, 200};
const int isAutoRegisterNeeded[] = {200, 240};

String jwt;
String wiFiConfigured;
String autoRegisterNecessary;

//String user
boolean isLoginNeeded = true;

//-------------------------------------------------------------------------------------------------------------------------
void standard_connect()
{
   for (uint8_t t = 4; t > 0; t--) {
    Serial.printf("[SETUP] WAIT %d...\n", t);
    Serial.flush();
    delay(1000);
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
            int loop = 1;
            while(loop)
            {
              Serial.println("Conectado na rede definitiva\n");
              delay(1000);
              //Liga Green LED
              //Desliga Red LED
              //grava dados na EEPROM
              if ((WiFi.status() == WL_CONNECTED))
                loop = 1;
              else
                loop = 0;
            }
          }
           else
          {
            Serial.println("Nao foi possivel conectar na rede definitiva\n");
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
  standard_connect();
  
}

void loop() {
  // wait for WiFi connection
  //if ((WiFiMulti.run() == WL_CONNECTED)) {
  setup_connect();
}
/*
void definitive_network(const char* ssid_new, const char* psk_new)
{
  WiFi.mode(WIFI_STA);
  WiFi.begin(ssid_new, psk_new);
  if ((WiFi.status() == WL_CONNECTED)) {
    while(1){
      Serial.println("Conectado na rede definitiva\n");
      delay(1000);
      //Liga Green LED
      //Desliga Red LED
    }
  }
  else
  {
    Serial.println("Nao foi possivel conectar na rede definitiva\n");
    delay(100);
    standard_connect();
    setup_connect();
    //Liga Red LED
    //Desliga Green LED
  }
} */
void eraseEEPROM() {

  EEPROM.begin(512);
  // write a 0 to all 512 bytes of the EEPROM
  for (int i = 0; i < 512; i++) {
    EEPROM.write(i, 0);
  }

  // turn the LED on when we're done
  pinMode(13, OUTPUT);
  digitalWrite(13, HIGH);
  EEPROM.end();
}

void EEPROM_ESP8266_GRABAR(String buffer, int N) {
  EEPROM.begin(512); delay(10);
  for (int L = 0; L < 40; ++L) {
    EEPROM.write(N + L, buffer[L]);
  }
  EEPROM.commit();
}

//
String EEPROM_ESP8266_LEER(int min, int max) {
  EEPROM.begin(512); delay(10); String buffer;
  for (int L = min; L < max; ++L){
    //if (isAlphaNumeric(EEPROM.read(L)))
      buffer += char(EEPROM.read(L));
  }
  return buffer;
}
