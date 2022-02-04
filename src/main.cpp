#include <Arduino.h>
#include <Debug.h>
#include "Config.h"
#include "SimpleFirebase/SimpleFirebase.h"
#include <SimpleLED.h>
#include <ESP8266WiFi.h>
#include <GParser.h>
#include <bitmap.h>
#include <SimpleWM.h>
#include "SimpleFirebase/TinyJsonFinal.h"

#define USE_WM

SimpleFirebase fb("wowpixel-test1-default-rtdb.firebaseio.com", "qpSkriFCNOO1ovNWkiFYKiLAl2DDBLpeDl8ps6TO");
SimpleLED<16, 8, D2> matrix;

std::shared_ptr<Text> text(new Text());
std::shared_ptr<Text> digits(new Text());
std::shared_ptr<Painter> painter(new Painter());

void caseCallback(const char* data)
{
  int caseNum = atoi(TinyJson::value(data).c_str());
  DEBUG(data)
  DEBUG(caseNum)
  switch(caseNum)
  {
  case 0:
    matrix.setEffect(painter);
    break;

  case 1:
    matrix.setEffect(text);
    break;

  case 2:
    matrix.setEffect(digits);
    break;

  default:
    matrix.setEffect(EffectsName((caseNum <= 17 && caseNum >=10) ? caseNum - 10 : 0));
    break;
  }
}

void drawCallback(const char* data)
{
  int ledNum = atoi(TinyJson::path(data).c_str() + 11);
  painter->draw(ledNum, TinyJson::value(data).c_str());
  DEBUG(String("DrawCallback: ") + String(ledNum)/* + String(" ") + String(colHex)*/)
}

void initFB()
{
  fb.on("/Matrix/Led", drawCallback);
  fb.on("/Control/Text/Text", [](const char* data){text->setText(TinyJson::value(data).c_str());});
  fb.on("/Control/Text/Scroll", [](const char* data){text->setScrollTimes(atoi(TinyJson::value(data).c_str()));});
  fb.on("/Case/Case", caseCallback);
  fb.on("/Control/Brig", [](const char* data){matrix.setBrightness(atoi(TinyJson::value(data).c_str()));});
  fb.on("/Control/Text/Color", [](const char* data){text->setLetterColor(strtoul(TinyJson::value(data).c_str() + 1, NULL, 16));});
  fb.on("/Control/Text/BackColor", [](const char* data){text->setBackgroundColor(strtoul(TinyJson::value(data).c_str() + 1, NULL, 16));});
  fb.on("/Control/Digits/Color", [](const char* data){digits->setLetterColor(strtoul(TinyJson::value(data).c_str() + 1, NULL, 16));});
  fb.on("/Control/Digits/BackColor", [](const char* data){digits->setBackgroundColor(strtoul(TinyJson::value(data).c_str() + 1, NULL, 16));});
  fb.on("/Control/Digits/Text", [](const char* data){digits->setText(TinyJson::value(data).c_str());});
  Serial.println(fb.begin());
  fb.beginStream(PATH);
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
  matrix.drawBitmap<8, 8>(wifiBitmap);
  SimpleWM wm;
  wm.begin(AP_NAME, AP_PASS);
  matrix.drawBitmap<8, 8>(okBitmap);
  //delay(1500);
#endif
  WiFi.setAutoReconnect(true);
  WiFi.persistent(true);
}

void initMatrix()
{
  matrix.begin();
  delay(700);
  matrix.drawBitmap<8, 8>(monsterBitmap);
  delay(2000);
}

void setup()
{
  initMatrix();

  Serial.begin(115200);
  Serial.setTimeout(20);
  delay(50);
  DEBUG("Serial setuped ")
  

  randomSeed(micros());

  digits->setY((NUM_LEDS == 64) ? 0:0);//1:0
  digits->onlyDigits(true);

  initWiFi();
  delay(200);
  initFB();  
}

void loop()
{
  matrix.handle(); 
  fb.handle();
}

