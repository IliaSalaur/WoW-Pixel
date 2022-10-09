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
#include "SimpleFirebase/StatePersistence.h"

#define USE_WM

const uint8_t WIDTH = 16;
const uint8_t HEIGHT = 16;

RTCStatePersist statePersist;
SimpleFirebase fb(DATABASE_URL, API_KEY, &statePersist);
SimpleLED<WIDTH, HEIGHT, D2> matrix;

std::shared_ptr<Text> text(new Text());
std::shared_ptr<Text> digits(new Text());
std::shared_ptr<Painter> painter(new Painter());

bool showBitmap = 1;

void caseCallback(const char* data)
{
  static int lastCase = -1;
  int caseNum = atoi(TinyJson::value(data).c_str());
  Fm("caseNum: %u, data:%s\n", caseNum, data);
  if(caseNum == lastCase) return;
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

  lastCase = caseNum;
}

void drawCallback(const char* data)
{
  int ledNum = atoi(TinyJson::path(data).c_str() + 9);
  //Fm("CB ledNum:%d\tpath: %s\n", ledNum, TinyJson::value(data).c_str());
  painter->draw(ledNum, TinyJson::value(data).c_str());
}

void initFB()
{
  bool con = 1;
  if(!fb.begin()){
    //WiFi.reconnect();
    con = 0;
  }
  Fm("Begin done, free heap: %u\t free stack: %u\n", ESP.getFreeHeap(), ESP.getFreeContStack())
  Serial.printf("Connect%s\n\n", con ? "ed":"ion failed");
  fb.beginStream(PATH);
  // fb.getClient().onDisconnect(initFB);
}

void configFB(){
  fb.on("/Matrix/l", drawCallback);
  fb.on("/Control/Text/Text", [](const char* data){text->setText(TinyJson::value(data).c_str());});
  fb.on("/Control/Text/Scroll", [](const char* data){text->setScrollTimes(atoi(TinyJson::value(data).c_str()));});
  fb.on("/Case/Case", caseCallback);
  fb.on("/Control/Brig", [](const char* data){matrix.setBrightness(atoi(TinyJson::value(data).c_str()));Fm("Brig: %u\n", atoi(TinyJson::value(data).c_str()))});
  fb.on("/Control/Text/Color", [](const char* data){text->setLetterColor(strtoul(TinyJson::value(data).c_str() + 1, NULL, 16));});
  fb.on("/Control/Text/BackColor", [](const char* data){text->setBackgroundColor(strtoul(TinyJson::value(data).c_str() + 1, NULL, 16));});
  fb.on("/Control/Digits/Color", [](const char* data){digits->setLetterColor(strtoul(TinyJson::value(data).c_str() + 1, NULL, 16));});
  fb.on("/Control/Digits/BackColor", [](const char* data){digits->setBackgroundColor(strtoul(TinyJson::value(data).c_str() + 1, NULL, 16));});
  fb.on("/Control/Digits/Text", [](const char* data){digits->setText(TinyJson::value(data).c_str());});

  fb.persistEntry("/Case/Case");
  fb.persistEntry("/Control/Brig");

  fb.persistEntry("/Control/Text/Text");
  fb.persistEntry("/Control/Text/Color");
  fb.persistEntry("/Control/Text/BackColor");
  fb.persistEntry("/Control/Text/Scroll");

  fb.persistEntry("/Control/Digits/Text");
  fb.persistEntry("/Control/Digits/Color");
  fb.persistEntry("/Control/Digits/BackColor");
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
  if(showBitmap)matrix.drawBitmap<8, 8>(wifiBitmap);
  SimpleWM wm;
  wm.whileConnecting([](){
    matrix.handle();
  });
  wm.begin(AP_NAME, AP_PASS);
  if(showBitmap)matrix.drawBitmap<8, 8>(okBitmap);
  //delay(1500);
#endif
  WiFi.setAutoReconnect(true);
  WiFi.persistent(true);
  //WiFi.onStationModeConnected(
  //   [](const WiFiEventStationModeConnected&){
      
  // );
}

void initMatrix()
{
  matrix.begin();
  delay(50);
}

void setup()
{
  initMatrix();

  Serial.begin(115200);
  Serial.setTimeout(20);
  delay(50);
  
  configFB();

  showBitmap = !fb.retrievePersistedState();
  if(showBitmap)
  {
    matrix.drawBitmap<8, 8>(monsterBitmap);
    delay(1500);
  }
  randomSeed(micros());

  digits->setY(0);//1:0
  digits->onlyDigits(true);

  initWiFi();
  initFB();  
}

void loop()
{
  static bool flag = 0;
  static uint32_t heap_tmr = 0;
  matrix.handle(); 
  fb.handle();
  if(Serial.available() && Serial.read() == 'd')
  {
    WiFi.reconnect();
  }

  if(!WiFi.isConnected() && !flag){
    Fm("WIFI DISCONNECTED\n");
    flag = 1;
  }
  if(WiFi.isConnected() && flag){
    Fm("WIFI CONNECTED\n");
    flag = 0;
  }

  if(millis() - heap_tmr >= 5000){
    heap_tmr = millis();
    Fm("%lu:  Free heap: %u\t free stack: %u\n", millis(), ESP.getFreeHeap(), ESP.getFreeContStack())
  }
}

