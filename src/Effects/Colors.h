#ifndef COLORS_H
#define COLORS_H
#pragma once 

#include "IEffect.h"

class Colors : public IEffect   
{
private:
    int hue = 0;
    uint8_t _scale = 40;

    void colorsRoutine() {
        hue += _scale;
        for (int i = 0; i < _w * _h; i++) {
        _leds[i] = getHSV(hue, 255, 255);
    }
}
public:
    Colors(uint32_t* leds, uint8_t w, uint8_t h)
    {
        _leds = leds;
        _w = w;
        _h = h;
    }

    void show()
    {
        static uint32_t timer = 0;
        if(millis() - timer >= COLORS_SHOW_PERIOD)
        {
            timer = millis();
            this->colorsRoutine();
        }
    }
};

#endif