#ifndef PAINTER_H
#define PAINTER_H
#pragma once 

#include "IEffect.h"
class Painter : public IEffect
{
private:
    uint32_t buffer[128];

    void _setBuf()
    {
        for(int i = 0; i < _w * _h; i++)
        {
            _leds[i] = buffer[i];
        }
    }
public:
    Painter(uint32_t* leds, uint8_t w, uint8_t h) : buffer()
    {
        _w = w;
        _h = h;
        _leds = leds;

        this->_setBuf();
    }

    Painter()
    {
        this->_setBuf();
    }

    void draw(int n, uint32_t col)
    {
        buffer[n] = col;
    }

    void draw(int n, String colS)
    {
        uint32_t col = strtoul(colS.substring(1).c_str(), NULL, 16);
        this->draw(n, col);
    }

    void show()
    {
        this->_setBuf();
    }
};

#endif