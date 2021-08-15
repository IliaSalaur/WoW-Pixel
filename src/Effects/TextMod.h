#ifndef TEXTMOD_H
#define TEXTMOD_H
#pragma once 

#include "IEffect.h"
#include <Fonts.h>

class TextMod : public IEffect
{
private:
    String text = String(" ");
    uint16_t _speed =  TEXT_SPEED;
    int _runX = 0;
    int _scrollCount = 0;
    int _scrollTimes = 0;
    uint8_t _y = 1;

    uint32_t _letCol = 0xffffff, _backCol = 0;

    void _setLeds(int x, int y, uint32_t col)
    {
        if(x < _w && y < _h)
        {
            _leds[XY(_w, _h, x, y)] = col;                  
        }
    }

    void _drawLetter(int c, int x, int y, int font)
    {        
        int ic = c;
        switch (font)
        {
        case 35:
            if(ic >= 208)
            {
                for(int a = ((x < 0) ? -x:0); a < 5; a++)
                {
                    for(int b = 0; b < 7; b++)
                    {                        
                        this->_setLeds(a + x, b + y,  ((font5x7Rus.getBytes(ic)[a] & (1 << b)) >> b == 1) ? _letCol : _backCol);
                    }
                }
            }
            else
            {
                for(int a = ((x < 0) ? -x:0); a < 5; a++)
                {
                    for(int b = 0; b < 7; b++)
                    {
                        this->_setLeds(a + x, b + y,  ((font5x7.getBytes(ic)[a] & (1 << b)) >> b == 1) ? _letCol : _backCol);
                    }
                }
            }            
            break;
        }        
    }
public:
    TextMod(uint32_t* leds, uint8_t w, uint8_t h, uint16_t speed = 0, int scrollTimes = 0, String t = "", uint32_t lcol = uint32_t(0xffffff), uint32_t bcol = uint32_t(0))
    {
        _leds = leds;
        _w = w;
        _h = h;
        _letCol = lcol;
        _backCol = bcol;
        _scrollTimes = scrollTimes;
        this->setText(t);
        this->setSpeed(speed);
    }

    TextMod()
    {
        text = String(" ");
        this->setScrollTimes(0);
    }

    void setText(String t)
    {
        text = t;
    }

    void setSpeed(uint16_t speed)
    {
        _speed = speed;
    }

    void setScrollTimes(int sct)
    {
        _scrollTimes = sct;
    }
};

#endif