#ifndef EF_FACTORY_H
#define EF_FACTORY_H
#pragma once

#include "Fire.h"
#include "Colors.h"
#include "Snowfall.h"
#include "Matrix.h"
#include "Rainbow.h"
#include "Painter.h"
#include "Text.h"
#include "Lighters.h"
#include "Confetti.h"

class EffectFactory
{
public:
    EffectFactory(){}
    static IEffect *getEffect(EffectsName ef, uint32_t *leds, uint8_t w, uint8_t h)
    {
        switch(ef)
        {
        case CONFETTI:
            return new Confetti(leds, w, h);
            break;
/*
        case OFF:
            return new Off(leds, w, h);
            break;
*/
        case FIRE:
            return new Fire(leds, w, h);
            break;

        case SNOWFALL:
            return new SnowFall(leds, w, h);
            break;

        case LIGHTERS:
            return new Lighters(leds, w, h);
            break;

        case MATRIX:
            return new Matrix(leds, w, h);
            break;

        case COLORS:
            return new Colors(leds, w, h);
            break;

        case RAINBOW_H:
            return new RainbowHorizontal(leds, w, h);
            break;

        case RAINBOW_V:
            return new RainbowVertical(leds, w, h);
            break;

        default:
            return nullptr;
            break;
        }
        return nullptr;
    }   
};

#endif