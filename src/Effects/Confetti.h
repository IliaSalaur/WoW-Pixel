#ifndef CONFETTI_H
#define CONFETTI_H
#pragma once 

#include "IEffect.h"

class Confetti : public IEffect  
{
private:
    uint32_t _timer = 0;
public:
    Confetti(uint32_t *leds, uint8_t w, uint8_t h)
    {
        _leds = leds;
        _leds_num = w * h;
        _w = w;
        _h = h;
    }
    
    void show() override
    {
        if(millis() - _timer >= CONFETTI_SHOW_PERIOD)
        {
            _timer = millis();
            for(int a = 0; a < _leds_num; a++)
            {
                *(_leds + a) = getHEX(random(255), random(255), random(255));            
            }
        }
    }
};

#endif