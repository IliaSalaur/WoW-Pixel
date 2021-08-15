#ifndef SNOWFALL_H
#define SNOWFALL_H
#pragma once 

#include "IEffect.h"

class SnowFall : public IEffect   
{
private:
    uint8_t _scale = 40;
    
    void snowRoutine() {
      // сдвигаем всё вниз
      int testf = 0;
      for (int x = 0; x < _w; x++) {
        for (int y = _h - 1; y > testf; y--) {
          _leds[XY(_w, _h, x, y)] = _leds[XY(_w, _h, x, y - 1)];
        }
      }

      for (uint8_t x = 0; x < _w; x++) {
        // заполняем случайно верхнюю строку
        // а также не даём двум блокам по вертикали вместе быть
        if (getHEX(_leds[XY(_w, _h, x, _h - 2)]) == 0 && (random(0, _scale) == 0))
          _leds[XY(_w, _h, x, testf)] = getHEX(14745599 - 1052688 * random(0, 4));
        else
          _leds[XY(_w, _h, x, testf)] = getHEX(0);
      }
    }

public:
    SnowFall(uint32_t* leds, uint8_t w, uint8_t h)
    {
        _w = w;
        _h = h;
        _leds = leds;
    }

    void show() override
    {
        static uint32_t timer = 0;
        if(millis() - timer >= SNOWFALL_SHOW_PERIOD)
        {
            timer = millis();
            this->snowRoutine();
        }        
    }
};

#endif