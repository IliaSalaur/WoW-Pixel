#ifndef IEFFECT_H
#define IEFFECT_H
#pragma once

#include <Arduino.h>
#include <MatrixUtils.h>
#include "EffectsConfig.h"

enum EffectsName
{
    CONFETTI,
    //OFF,
    FIRE,
    SNOWFALL,
    LIGHTERS,
    MATRIX,
    COLORS,
    RAINBOW_H,
    RAINBOW_V,
    TEXT,
    PAINTER
};

class IEffect
{
protected:
    uint32_t *_leds = nullptr;
    uint16_t _leds_num = 0;

    uint8_t _w = 0;
    uint8_t _h = 0;
    //Adafruit_NeoPixel colUtils;
public:
    virtual void show() = 0;

    void setLeds(uint32_t * leds)
    {
        _leds = leds;
    }

    virtual void setWH(uint8_t w, uint8_t h)
    {
        _w = w;
        _h = h;
        _leds_num = w * h;
    }

    void setLED(uint16_t led, uint32_t col){
        if(_leds == nullptr) return;
        if(led >= _w * _h) return;
        _leds[led] = col;
    }

    void setLED(uint8_t x, uint8_t y, uint32_t col){
        if(_leds == nullptr) return;
        if(x > _w || y > _h) return;
        _leds[XY(_w, _h, x, y)] = col;
    }

    uint32_t getLED(uint16_t led){
        if(_leds == nullptr) return NULL_COLOR;
        if(led >= _w * _h) return NULL_COLOR;
        return _leds[led];
    }

    uint32_t getLED(uint8_t x, uint8_t y){
        if(_leds == nullptr) return NULL_COLOR;
        if(x > _w || y > _h) return NULL_COLOR;
        return _leds[XY(_w, _h, x, y)];
    }

};


#endif