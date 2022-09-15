#ifndef RAINBOW_EF_H
#define RAINBOW_EF_H
#pragma once 

#include "IEffect.h"

class RainbowVertical : public IEffect
{
private:
    int hue = 0;
    uint8_t _scale = 40;

    void rainbowVertical() {
        hue += 2;
        for (uint8_t j = 0; j < _h; j++) {
            uint32_t thisColor = getHSV(hue + j * _scale, 255, 255);
            for (uint8_t i = 0; i < _w; i++){
                setLED(i, j, thisColor);
            }
        }
    }
public:
    RainbowVertical(uint32_t* leds, uint8_t w, uint8_t h)
    {
        _leds = leds;
        _w = w;
        _h = h;
    }

    void show()
    {
        static uint32_t timer = 0;
        if(millis() - timer >= RAINBOW_SHOW_PERIOD)
        {
            timer = millis();
            this->rainbowVertical();
        }
    }
};

 
class RainbowHorizontal : public IEffect
{
private:
    int hue = 0;
    uint8_t _scale = 40;

    void rainbowHorizontal() {
        hue += 2;
        for (uint8_t i = 0; i < _w; i++) {
            uint32_t thisColor = getHSV((hue + i * _scale), 255, 255);
            for (uint8_t j = 0; j < _h; j++){
                setLED(i, j, thisColor);   //leds[getPixelNumber(i, j)] = thisColor;
            }
        }
    }
public:
    RainbowHorizontal(uint32_t* leds, uint8_t w, uint8_t h)
    {
        _leds = leds;
        _w = w;
        _h = h;
    }

    void show()
    {  
        static uint32_t timer = 0;
        if(millis() - timer >= RAINBOW_SHOW_PERIOD)
        {
            timer = millis();
            this->rainbowHorizontal();
        }
    }
};

#endif