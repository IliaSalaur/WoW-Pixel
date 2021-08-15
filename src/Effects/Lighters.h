#ifndef LIGHTERS_H
#define LIGHTERS_H
#pragma once 

#include "IEffect.h"

class Lighters : public IEffect   
{
private:
    int lightersPos[2][LIGHTERS_AM];
    int8_t lightersSpeed[2][LIGHTERS_AM];
    uint32_t lightersColor[LIGHTERS_AM];
    uint8_t loopCounter;

    int angle[LIGHTERS_AM];
    int speedV[LIGHTERS_AM];
    int8_t angleSpeed[LIGHTERS_AM];

    bool _loadingFlag;

    uint8_t _scale = 40;

    void lightersRoutine() {
      if (_loadingFlag == true) {
        _loadingFlag = false;
        randomSeed(millis());
        for (uint8_t i = 0; i < LIGHTERS_AM; i++) {
          lightersPos[0][i] = random(0, _w * ((_w / _h == 1) ? 10 : 5));//10  
          lightersPos[1][i] = random(0, _h * 10);
          lightersSpeed[0][i] = random(-10, 10);
          lightersSpeed[1][i] = random(-10, 10);
          lightersColor[i] = getHSV(random(0, 255), 255, 255);
        }
      }
      //FastLED.clear();
      if (++loopCounter > 20) loopCounter = 0;
      for (uint8_t i = 0; i < _scale; i++) {
        if (loopCounter == 0) {     // меняем скорость каждые 255 отрисовок
          lightersSpeed[0][i] += random(-3, 4);
          lightersSpeed[1][i] += random(-3, 4);
          lightersSpeed[0][i] = constrain(lightersSpeed[0][i], -20, 20);
          lightersSpeed[1][i] = constrain(lightersSpeed[1][i], -20, 20);
        }

        lightersPos[0][i] += lightersSpeed[0][i];
        lightersPos[1][i] += lightersSpeed[1][i];

        if (lightersPos[0][i] < 0) lightersPos[0][i] = (_w - 1) * ((_w / _h == 1) ? 10 : 5); //10
        if (lightersPos[0][i] >= _h * 10) lightersPos[0][i] = 0;

        if (lightersPos[1][i] < 0) {
          lightersPos[1][i] = 0;
          lightersSpeed[1][i] = -lightersSpeed[1][i];
        }
        if (lightersPos[1][i] >= (_h - 1) * 10) {
          lightersPos[1][i] = (_w - 1) * ((_w / _h == 1) ? 10 : 5);//10
          lightersSpeed[1][i] = -lightersSpeed[1][i];
        }
        _leds[XY(_w, _h, lightersPos[0][i] / ((_w / _h == 1) ? 10 : 5)/*10*/, lightersPos[1][i] / 10)] =  lightersColor[i];
      }
    }  

public:
    Lighters(uint32_t* leds, uint8_t w, uint8_t h)
    {
        _leds = leds;
        _w = w;
        _h = h;
        _loadingFlag = true;
    }

    void show() override
    {
        static uint32_t timer = 0;
        if(millis() - timer >= LIGHTERS_SHOW_PERIOD)
        {
            timer = millis();
            this->lightersRoutine();
        }
    }
};

#endif