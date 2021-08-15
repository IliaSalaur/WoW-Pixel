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
    }
};


#endif