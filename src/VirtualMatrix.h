#ifndef VIRTUAL_MATRIX_H
#define VIRTUAL_MATRIX_H
#include <Arduino.h>
#include <FastLED.h>
#include <MatrixUtils.h>

void emulateLeds(uint8_t w, uint8_t h, CRGB* leds)
{
    String data;
    for(int i = 0; i < w * h; i++)
    {
        data += String(getHEX(leds[i]));
        if(i != w*h -1)
        {
            data += ",";
        }        
    }
    data += ";";

    Serial.print(data);
}

#endif