#ifndef FIRE_H
#define FIRE_H
#pragma once 

#include "IEffect.h"

class Fire : public IEffect   
{
private:
    
    int _pcnt = 0;

    uint8_t _hueStart = 2;

    uint8_t matrixValue[8][16];
    uint8_t line[16];

    int hueMask[8][16]  = {//8*16 
        {0, 0, 0, 1, 1, 0, 0, 0, 0, 0, 0, 1, 1, 0, 0, 0 },
        {0, 0, 1, 5, 5, 1, 0, 0, 0, 0, 1, 5, 5, 1, 0, 0 },
        {0, 1, 5, 8, 8, 5, 1, 0, 0, 1, 5, 8, 8, 5, 1, 0 },
        {1, 5, 11, 11, 11, 11, 5, 1, 1, 5, 11, 11, 11, 11, 5, 1 },
        {1, 5, 11, 13, 13, 13, 5, 1, 1, 5, 11, 13, 13, 13, 5, 1 },
        {1, 8, 13, 16, 19, 16, 8, 1, 1, 8, 13, 16, 19, 16, 8, 1 },
        {1, 8, 13, 19, 25, 19, 8, 1, 1, 8, 13, 19, 25, 19, 8, 1 }, 
        {1, 11, 19, 25, 25, 22, 11, 1, 1, 11, 19, 25, 25, 22, 11, 1 },
    };

    int valueMask[8][16]  = { //8*16
        {255, 192, 160, 128, 128, 160, 192, 255, 255, 192, 160, 128, 128, 160, 192, 255},
        {255, 160, 128, 96, 96, 128, 160, 255, 255, 160, 128, 96, 96, 128, 160, 255},
        {192, 128, 96, 64, 64, 96, 128, 192, 192, 128, 96, 64, 64, 96, 128, 192},
        {160, 96, 64, 32, 32, 64, 96, 160, 160, 96, 64, 32, 32, 64, 96, 160},
        {128, 64, 32, 0, 0, 32, 64, 128, 128, 64, 32, 0, 0, 32, 64, 128},
        {96, 32, 0, 0, 0, 0, 32, 96, 96, 32, 0, 0, 0, 0, 32, 96 },
        {64, 0, 0, 0, 0, 0, 0, 64, 64, 0, 0, 0, 0, 0, 0, 64 },
        {32, 0, 0, 0, 0, 0, 0, 32, 32, 0, 0, 0, 0, 0, 0, 32 },
    };

    void drawFrame(int pcnt) {
        int nextv;
        //each row interpolates with the one before it
        for (int y = 0; y < 7 ; y++) {
            for (int x = 0; x < _w; x++) {
                int newX = x;
                if (x > _w - 1) newX = x - (_w - 1);
                if (y < 8) {
                    nextv =
                    (((100 - pcnt) * matrixValue[y][newX]
                    + pcnt * matrixValue[y + 1][newX]) / 100)
                    - valueMask[y][newX];

                    uint32_t col = getHSV(
                        int(_hueStart * 2.5 + hueMask[y][newX]), // H
                        255, // S
                    (int)max(0, nextv) // V
                    );

                    _leds[XY(_w, _h, x, y)] = col;
                }                    
            }
        }

        //first row interpolates with the "next" line
        for (int x = 0; x < _w; x++) {
            int newX = x;
            if (x > _w - 1) newX = x - (_w - 1);
            uint32_t col = getHSV(
                int(_hueStart * 2.5 + hueMask[7][newX]), // H
                255, // S
                (int)(((100 - pcnt) * matrixValue[_h - 1][newX] + pcnt * line[newX]) / 100) // V
            );
            _leds[XY(_w, _h, newX, _h - 1)] = col;
        }
    }

    void generateLine() {
        for (int x = 0; x < _w; x++) {
        line[x] = random(64, 255);
        }
    }

    void shiftUp() {
        for (int y = 0; y < _h - 1; y++) {
            for (int x = 0; x < _w; x++) {
                int newX = x;
                if (x > (_w - 1)) newX = x - (_w - 1);
                if (y > 7) continue;
                //if (y > (_h / 2 - 2)) continue;
                matrixValue[y][newX] = matrixValue[y + 1][newX];
            }
        }

        for (int x = 0; x < _w; x++) {
            int newX = x;
            if (x > (_w - 1)) newX = x - (_w - 1);
            matrixValue[_h - 1][newX] = line[newX];
        }
    }

    void fireRoutine()
    {
        static uint32_t fireTimer = 0;
        if (millis() - fireTimer > FIRE_SHOW_PERIOD)
        {
            fireTimer = millis();
            if (_pcnt >= 100) {
                shiftUp();
                generateLine();
                _pcnt = 0;
            }
            drawFrame(_pcnt);
            _pcnt += 30;
        }
    }    


public:
    Fire(uint32_t* leds, uint8_t w, uint8_t h)
    {
        _leds = leds;
        _leds_num = w * h;
        _w = w;
        _h = h;

        //line = SimpleArrayBuilder<uint8_t>::build(w);
        //matrixValue = Simple2dArrayBuilder<uint8_t>().build(_h, _w);
        //Simple2dArrayBuilder<uint8_t>::build(_h, _w, matrixValue);
        //SimpleArrayBuilder<uint8_t>::build(_w, line);
    }

    void show()
    {
        this->fireRoutine();
    }

    ~Fire()
    {
        //Simple2dArrayBuilder<uint8_t>::stop(matrixValue, _h, _w);
        //SimpleArrayBuilder<uint8_t>::stop(line, _w);
    }
};

#endif