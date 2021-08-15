#ifndef TEXT_H
#define TEXT_H
#pragma once 

#include "IEffect.h"
#include <Fonts.h>

class Text : public IEffect   
{
private:
    String text = String(" ");
    uint16_t _speed =  TEXT_SPEED;
    bool _onlyDigits = 0;
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

    void _drawText()
    {
        for(int i = 0; i < _w * _h; i++)
        {
            _leds[i] = _backCol;
        }  
        
        int lastX = 0;
        int len = strlen(text.c_str());

        int chars[len];

        for(int i = 0; i < len; i++)
        {
            chars[i] = 0;
        }

        for (int i = 0, j = 0; i < len; i++, j++)
        {
            int charIndex = text[i];
            if(charIndex >= 208)
            {
                charIndex <<= 8;
                charIndex |= text.charAt(i + 1);                
                i++;
            }
            chars[j] = charIndex;
        }

        for(uint16_t i = 0; i < text.length(); i++)
        {
            _drawLetter(chars[i], lastX + _runX, _y, 5*7);
            //lastX += (smallSpace) ? 4:6;
            lastX += (i + 1 < len && (chars[i] >= 48 && chars[i] <= 57) && (text[i + 1] >= 48 && text[i + 1] <= 57)) ? 4:6;
        }
        
        /*
        for(uint16_t i = 0, j = 0; i < len; i++, j++)
        {
            int charIndex = text.charAt(i);
            if(1)
            {
                bool rusChar = 0;
                bool smallSpace = 0;
                if(charIndex >= 208)
                {
                    charIndex <<= 8;
                    charIndex |= text.charAt(i + 1);
                    rusChar = 1;
                }

                if(i + 1 < len)
                {
                    if((charIndex >= 48 && charIndex <= 57) && (text[i + 1] >= 48 && text[i + 1] <= 57))
                    {
                        smallSpace = 1;
                    }
                }
                
                _drawLetter(charIndex, lastX + _runX, _y, 5*7);//3*5:5*7
                lastX += (smallSpace) ? 4:6;
                i += (rusChar) ? 1 : 0;
            }
        }*/
    }

    void _centerText()
    {
        if(text != String(" ") && _scrollTimes == 0)
        {
            _runX = (_w - int(text.length()) * ((_onlyDigits) ? 4:5)) / 2;//1:2
            DEBUG(String("_runX setted: ") + String(_runX))
            DEBUG(String("Length: ") + String(text.length()))
            DEBUG(String("Width: ") + String(_w))
        }        
        else{
            DEBUG("Nope: ")
            DEBUG(text)
            DEBUG(_scrollTimes)
        }
    }

public:
    Text(uint32_t* leds, uint8_t w, uint8_t h, uint16_t speed = 0, int scrollTimes = 0, String t = "", uint32_t lcol = uint32_t(0xffffff), uint32_t bcol = uint32_t(0))
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

    Text()
    {
        _scrollTimes = 0;
        _runX = 0;
        this->setText(" ");
        this->setScrollTimes(0);
    }

    void setWH(uint8_t w, uint8_t h) override
    {
        _w = w;
        _h = h;

        this->_centerText();
    }

    void onlyDigits(bool od)
    {
        _onlyDigits = od;
    }

    void show() override
    {
        static uint32_t timer = 0;
        static uint32_t frameTimer = 0; 

        if(millis() - frameTimer >= 20)
        {
            frameTimer = millis();
            if(_scrollTimes != 0 && _scrollTimes != _scrollCount)
            {
                if(millis() - timer >= _speed && _speed != 0)
                {
                    //DEBUG("MOVE start")
                    timer = millis();
                    if(_runX <= int(text.length()) * -5)
                    {
                        _runX = _w;
                        _scrollCount++;
                        DEBUG("Runx reset")                        
                    }
                    else
                    {
                        _runX--;
                    }
                    //DEBUG(_runX);
                    //DEBUG(_scrollCount);
                    //DEBUG(_scrollTimes);
                    //DEBUG("MOVE stop")
                }
            }                        
            this->_drawText();
        }
    }

    void setText(String t)
    {
        text = t;
        int len = strlen(text.c_str());
        DEBUG(len);
        _scrollCount = (_scrollCount == _scrollTimes) ? 0 : _scrollCount;
        this->_centerText();
        this->setScrollTimes(_scrollTimes);

    }

    void setY(uint8_t y)
    {
        _y = y;
    }

    void setSpeed(uint16_t speed) // 0 - 1000, рекомендую от 50 до 300
    {
        _speed = speed;
    }

    void setLetterColor(uint32_t lcol)
    {
        _letCol = lcol;
    }

    void setBackgroundColor(uint32_t bcol)
    {
        _backCol = bcol;
    }

    void setScrollTimes(int n)
    {
        _scrollCount = 0;
        _scrollTimes = n;

        this->_centerText();
    }

    ~Text()
    {
        text.~String();
    }
};

#endif