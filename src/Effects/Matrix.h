#ifndef MATRIX_H
#define MATRIX_H
#pragma once 

#include "IEffect.h"

class Matrix : public IEffect   
{
private:
    uint8_t _scale = 40;

    void matrixRoutine()
    {
      for (int x = 0; x < _w; x++) {
        // заполняем случайно верхнюю строку
        uint32_t thisColor = _leds[XY(_w, _h, x, 0)];
        //println(binary(thisColor.getHEX())); -- кусок кода с Processing
        if (thisColor == 0)
        {
          //println("Hex 0");
          if (int(random(-1, _scale)) == 0)
          {
            _leds[XY(_w, _h, x, 0)] = 65280;
          } else
          {
            _leds[XY(_w, _h, x, 0)] = 0;
          }
        } else if (thisColor < 8192)
        {
          //println("Hex < 8192");
          _leds[XY(_w, _h, x, 0)] = 0;
        } else
        {
          //println("else");
          _leds[XY(_w, _h, x, 0)] = thisColor - 8192;
        }
      }

      // сдвигаем всё вниз
      for (int x = 0; x < _w; x++) {
        for (int y = _h - 1; y > 0; y--) {
          _leds[XY(_w, _h, x, y)] = _leds[XY(_w, _h, x, y - 1)];
        }
      }
    }
public:
    Matrix(uint32_t* leds, uint8_t w, uint8_t h)
    {
        _leds = leds;
        _w = w;
        _h = h;
    }

    void show() override
    {
        static uint32_t timer = 0;
        if(millis() - timer >= MATRIX_SHOW_PERIOD)
        {
            timer = millis();
            this->matrixRoutine();
        }
    }
};

#endif