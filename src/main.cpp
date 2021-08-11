#include <Arduino.h>
#include "Config.h"
#include <SimpleFirebase.h>
#include <SimpleLED.h>
#include <ESP8266WiFi.h>
#include <GParser.h>
#include <bitmap.h>
#include "addons/TokenHelper.h"
#include "addons/RTDBHelper.h"
#include <SimpleWM.h>

#define USE_WM
#ifdef USE_WM
SimpleWM wm;
#endif

#define NUM_LEDS 128

FirebaseAuth auth;
FirebaseConfig config;
SimpleFirebase fb;
SimpleLED<16, 8, D4> matrix;
uint32_t leds[NUM_LEDS];

Text text;
Text digits;

int caseNum = 0;

void caseCallback(PathData data)
{
  caseNum = data.data.toInt();
  switch(caseNum)
  {
  case 0:
    for(int i = 0; i < NUM_LEDS; i++)
    {
      matrix.drawPixel(i, leds[i]);
    }
    break;

  case 1:
    matrix.setEffect(&text);
    break;

  case 2:
    matrix.setEffect(&digits);
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
  text.setText(data.data);
  DEBUG(data.data) 
}

void digitsCallback(PathData data)
{  
  DEBUG("Digits callback called")   
  digits.setText(data.data);
  DEBUG(data.data) 
}

void digitsColCallback(PathData data)
{
  digits.setLetterColor(strtoul(data.data.substring(1).c_str(), NULL, 16));
  DEBUG("Digits color changed");
}

void digitsBackColCallback(PathData data)
{
  digits.setBackgroundColor(strtoul(data.data.substring(1).c_str(), NULL, 16));
  DEBUG("Digits Back color changed");
}

void scrollCallback(PathData data)
{
  DEBUG("Scroll callback called")
  text.setScrollTimes(data.data.toInt());
  DEBUG(data.data.toInt())
}

void drawCallback(PathData data)
{
  int ledNum = data.path.substring(11).toInt();
  uint32_t colHex = strtoul(data.data.substring(1).c_str(), NULL, 16);

  if(caseNum == 0) matrix.drawPixel(ledNum, colHex);
  
  leds[ledNum] = colHex;
  DEBUG(String("DrawCallback: ") + String(ledNum) + String(" ") + String(colHex))
}

void textColCallback(PathData data)
{
  text.setLetterColor(strtoul(data.data.substring(1).c_str(), NULL, 16));
  DEBUG("Text color changed");
}

void backColCallback(PathData data)
{
  text.setBackgroundColor(strtoul(data.data.substring(1).c_str(), NULL, 16));
  DEBUG("Back color changed");
}

void initFB()
{
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
  fb.on(String("/Control/Digits/Color"), digitsColCallback);
  fb.on(String("/Control/Digits/BackColor"), digitsBackColCallback);
  fb.on(String("/Control/Digits/Text"), digitsCallback);
  fb.begin(&config, &auth);

  Serial.printf("Firebase Client v%s\n\n", FIREBASE_CLIENT_VERSION);
}

void initWiFi()
{
#ifndef USE_WM
  WiFi.mode(WIFI_STA);
  WiFi.begin(WIFI_SSID, WIFI_PASSWORD);
  while(!WiFi.isConnected()){Serial.print("-");delay(300);}
  Serial.println();
  Serial.print("Connected with IP: ");
  Serial.println(WiFi.localIP());
  Serial.println();
#else
  wm.begin(AP_NAME);
#endif
}

void setup()
{
  randomSeed(micros());
  Serial.begin(115200);
  Serial.setTimeout(20);
  delay(50);

  digits.setY((NUM_LEDS == 64) ? 1:0);
  digits.onlyDigits(1);

  initWiFi();
  delay(100);
  initFB();
  matrix.begin();
}

void loop()
{
  fb.handle();
  matrix.handle(); 
/*
  if(Serial.available())
  {
    sp = Serial.parseInt();
    while (Serial.available())
    {
      Serial.read();
    }
    
  }*/
}