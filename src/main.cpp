#include <Arduino.h>
#include <SimpleLED.h>
#include <ESP8266WiFi.h>
#include <FirebaseESP8266.h>
#include <GParser.h>
#include <bitmap.h>
#include <SimpleFirebase.h>
//#include <WiFiManager.h>
#include "addons/TokenHelper.h"
#include "addons/RTDBHelper.h"

#define WIFI_SSID "hinev1"
#define WIFI_PASSWORD "069052345"

#define API_KEY "AIzaSyCiMqSEMR44_G3LngmUJpW_8cs7DxQiyzo"

#define DATABASE_URL "https://wowpixel-test1-default-rtdb.firebaseio.com/" //<databaseName>.firebaseio.com or <databaseName>.<region>.firebasedatabase.app

#define USER_EMAIL "salaur.ilie@gmail.com"
#define USER_PASSWORD "12345678"

FirebaseAuth auth;
FirebaseConfig config;

SimpleFirebase fb;
SimpleLED<8, 8, D4> matrix;
uint32_t leds[64];

String text = "";
int scrollN = 0;
uint32_t textCol = 0xffffff;
uint32_t backCol = 0x0;

void caseCallback(PathData data)
{
  int caseNum = data.data.toInt();
  switch(caseNum)
  {
  case 0:
    for(int i = 0; i < 64; i++)
    {
      matrix.drawPixel(i, leds[i]);
    }
    break;

  case 1:
    matrix.drawText(text, TEXT_SPEED, scrollN, textCol, backCol);
    break;

  default:
    matrix.setEffect(EffectsName((caseNum <= 17 && caseNum >=10) ? caseNum - 10 : 0));
    break;
  }
}

void brightnessCallback(PathData data)
{
  matrix.setBrightness(data.data.toInt());
}

void textCallback(PathData data)
{  
  DEBUG("Text callback called")   
  text = data.data;
  DEBUG(text) 
}

void scrollCallback(PathData data)
{
  DEBUG("Scroll callback called")
  scrollN = data.data.toInt();
  DEBUG(scrollN)
}

void drawCallback(PathData data)
{
  int ledNum = data.path.substring(11).toInt();
  uint32_t colHex = strtoul(data.data.substring(1).c_str(), NULL, 16);

  matrix.drawPixel(ledNum, colHex);
  leds[ledNum] = colHex;
  DEBUG(String("DrawCallback: ") + String(ledNum) + String(" ") + String(colHex))
}

void textColCallback(PathData data)
{
  textCol = strtoul(data.data.substring(1).c_str(), NULL, 16);
  DEBUG("Text color changed");
}

void backColCallback(PathData data)
{
  backCol = strtoul(data.data.substring(1).c_str(), NULL, 16);
  DEBUG("Back color changed");
}

void setup()
{
  randomSeed(micros());
  Serial.begin(115200);
  Serial.setTimeout(20);
  delay(50);

  WiFi.begin(WIFI_SSID, WIFI_PASSWORD);
  Serial.print("Connecting to Wi-Fi");
  while (WiFi.status() != WL_CONNECTED)
  {
    Serial.print(".");
    delay(300);
  }
  Serial.println();
  Serial.print("Connected with IP: ");
  Serial.println(WiFi.localIP());
  Serial.println();

  Serial.printf("Firebase Client v%s\n\n", FIREBASE_CLIENT_VERSION);

  config.api_key = API_KEY;
  auth.user.email = USER_EMAIL;
  auth.user.password = USER_PASSWORD;
  config.database_url = DATABASE_URL;
  config.token_status_callback = tokenStatusCallback; 

  fb.on(String("/Matrix/"), drawCallback);
  fb.on(String("/Control/Text/Text"), textCallback);
  fb.on(String("/Control/Text/Scroll"), scrollCallback);
  fb.on(String("/Case/Case"), caseCallback);
  fb.on(String("/Control/Brig"), brightnessCallback);
  fb.on(String("/Control/Text/Color"), textColCallback);
  fb.on(String("/Control/Text/BackColor"), backColCallback);


  fb.begin(&config, &auth);
  matrix.begin();

  //delay(3000);
}

void loop()
{
  fb.handle();
  matrix.handle();
  if(Serial.available())
  {
    delInt = Serial.parseInt();
    while (Serial.available())
    {
      Serial.read();
    }
    
  }
}