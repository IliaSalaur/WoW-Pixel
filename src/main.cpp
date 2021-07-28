#include <Arduino.h>
#include <SimpleLED.h>
#include <ESP8266WiFi.h>
#include <FirebaseESP8266.h>
#include <GParser.h>
#include <bitmap.h>

SimpleLED<16, 16> matrix(D4);
void setup() {
  randomSeed(micros());
  Serial.begin(115200);
  Serial.setTimeout(20);
  //random16_add_entropy(ESP.getChipId());
  
  matrix.begin();
  
  Serial.println("s");
}

void loop() {
  if(Serial.available())
  {
    int i = Serial.parseInt();
    if(i > 9)
    {
      matrix.drawText(String("kek"), 0);
    }
    else{
    matrix.setEffect(EffectsName(i));
    }
    while(Serial.available()) Serial.read();
  }
  matrix.handle();
}