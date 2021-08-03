#ifndef VIRTUAL_MATRIX_H
#define VIRTUAL_MATRIX_H
#include <Arduino.h>
#include <FastLED.h>
#include <MatrixUtils.h>

#define EMLEDS_PERIOD 70
#define USE_EMLEDS_TIMER 1

void emulateLeds(uint8_t w, uint8_t h, CRGB* leds)
{
    static uint32_t timer = 0;

    if(USE_EMLEDS_TIMER)
    {
        if(millis() - timer >= EMLEDS_PERIOD)
        {
            timer = millis();
            String data;
            for(int i = 0; i < w * h; i++)
            {
                data += String(getHEX(leds[i]) / 16);
                if(i != w*h -1)
                {
                    data += ",";
                }        
            }
            data += ";";
            Serial.print(data);
        }
    }
}


#endif