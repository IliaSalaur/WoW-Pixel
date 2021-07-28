#include <Arduino.h>
#include <SimpleLED.h>
#include <ESP8266WiFi.h>
#include <FirebaseESP8266.h>
#include <GParser.h>
#include <bitmap.h>

int efNumb = 0;
uint32_t efChangeTimer = 0;

SimpleLED<16, 16, D4> matrix;
void setup() {
  randomSeed(micros());
  Serial.begin(115200);
  Serial.setTimeout(20);
  //random16_add_entropy(ESP.getChipId());
  
  matrix.begin();
  
  Serial.println("s");
}

void loop() {
  if(millis() - efChangeTimer >= 15000)
  {
    efChangeTimer = millis();
    if(efNumb >= 9)
    {
      matrix.drawText("lol kek", 7);
    }
    else matrix.setEffect(EffectsName(efNumb));
    efNumb++;
  }
  matrix.handle();
}