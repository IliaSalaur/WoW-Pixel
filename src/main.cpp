#include <Arduino.h>
#include <SimpleLED.h>
#include <ESP8266WiFi.h>
#include <FirebaseESP8266.h>
#include <GParser.h>
#include <bitmap.h>

int efNumb = 0;
uint32_t efChangeTimer = 0;

bool handle = 0;

SimpleLED<16, 8, D4> matrix;
void setup() {
  randomSeed(micros());
  Serial.begin(115200);
  Serial.setTimeout(20);
  //random16_add_entropy(ESP.getChipId());
  
  matrix.begin();
  matrix.drawText(String("Lol kek"), 20, -1, CRGB(random(0xffffff)), CRGB(0));
  //matrix.setEffect(ef);
  
  delay(3000);
}

void loop() {
  /*
  if(millis() - efChangeTimer >= 15000)
  {
    efChangeTimer = millis();
    if(efNumb >= 9)
    {
      matrix.drawText("lol kek", 7);
    }
    else matrix.setEffect(EffectsName(efNumb));
    efNumb++;
  }*/
  //!!!!!!!!!!!!!!!!!!!!!!! VIRTUAL MODE ON!!!!!!!!!!!!!!!!!!!
  matrix.handle();
}