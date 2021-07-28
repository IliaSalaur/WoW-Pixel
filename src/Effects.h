#ifndef MYEFFECTS_H
#define MYEFFECTS_H
#include <FastLED.h>
#include <XY_Func.h>
#include <SimpleArray.h>
#include <EffectsConfig.h>
#include <Fonts.h>

int runX = 13;

uint32_t getHEX(CRGB col)
{
    uint32_t raw = 0;
    raw = col.r;
    raw <<= 8;
    raw |= col.g;
    raw <<= 8;
    raw |= col.b;

    return raw;
}

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
    TEXT
};

class IEffect
{
protected:
    CRGB *_leds;
    uint16_t _leds_num;
    uint8_t _w = 0;
    uint8_t _h = 0;
public:
    virtual void show() = 0;
};

class Confetti : public IEffect
{
private:
    uint32_t _timer = 0;
public:
    Confetti(CRGB *leds, uint8_t w, uint8_t h)
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
                *(_leds + a) = CRGB(random(255), random(255), random(255));            
            }
        }
    }
};

class Off : public IEffect
{
public:
    Off(CRGB *leds, uint8_t w, uint8_t h)
    {
        _leds = leds;
        _leds_num = w * h;
        _w = w;
        _h = h;
    }
    
    void show() override
    {
        for (int i = 0; i < _leds_num; i++)
        {
            *(_leds + i) = CRGB::Black;
        }
        
    }
};

class Fire : public IEffect
{
private:
    
    int _pcnt = 0;

    uint8_t _hueStart = 5;

    uint8_t **matrixValue;
    uint8_t *line;

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

                    CRGB col = CHSV(
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
            CRGB col = CHSV(
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
    Fire(CRGB* leds, uint8_t w, uint8_t h)
    {
        _leds = leds;
        _leds_num = w * h;
        _w = w;
        _h = h;
        line = SimpleArrayBuilder<uint8_t>(w).build();
        matrixValue = Simple2dArrayBuilder<uint8_t>(w, h).build();
    }

    void show()
    {
        this->fireRoutine();
    }
};

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

      for (byte x = 0; x < _w; x++) {
        // заполняем случайно верхнюю строку
        // а также не даём двум блокам по вертикали вместе быть
        if (getHEX(_leds[XY(_w, _h, x, _h - 2)]) == 0 && (random(0, _scale) == 0))
          _leds[XY(_w, _h, x, testf)] = CRGB(14745599 - 1052688 * random(0, 4));
        else
          _leds[XY(_w, _h, x, testf)] = CRGB(0);
      }
    }

public:
    SnowFall(CRGB* leds, uint8_t w, uint8_t h)
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

class Lighters : public IEffect
{
private:
    int lightersPos[2][LIGHTERS_AM];
    int8_t lightersSpeed[2][LIGHTERS_AM];
    CHSV lightersColor[LIGHTERS_AM];
    byte loopCounter;

    int angle[LIGHTERS_AM];
    int speedV[LIGHTERS_AM];
    int8_t angleSpeed[LIGHTERS_AM];

    bool _loadingFlag;

    uint8_t _scale = 40;

    void lightersRoutine() {
      if (_loadingFlag == true) {
        _loadingFlag = false;
        randomSeed(millis());
        for (byte i = 0; i < LIGHTERS_AM; i++) {
          lightersPos[0][i] = random(0, _w * 10);
          lightersPos[1][i] = random(0, _h * 10);
          lightersSpeed[0][i] = random(-10, 10);
          lightersSpeed[1][i] = random(-10, 10);
          lightersColor[i] = CHSV(random(0, 255), 255, 255);
        }
      }
      //FastLED.clear();
      if (++loopCounter > 20) loopCounter = 0;
      for (byte i = 0; i < _scale; i++) {
        if (loopCounter == 0) {     // меняем скорость каждые 255 отрисовок
          lightersSpeed[0][i] += random(-3, 4);
          lightersSpeed[1][i] += random(-3, 4);
          lightersSpeed[0][i] = constrain(lightersSpeed[0][i], -20, 20);
          lightersSpeed[1][i] = constrain(lightersSpeed[1][i], -20, 20);
        }

        lightersPos[0][i] += lightersSpeed[0][i];
        lightersPos[1][i] += lightersSpeed[1][i];

        if (lightersPos[0][i] < 0) lightersPos[0][i] = (_w - 1) * 10;
        if (lightersPos[0][i] >= _h * 10) lightersPos[0][i] = 0;

        if (lightersPos[1][i] < 0) {
          lightersPos[1][i] = 0;
          lightersSpeed[1][i] = -lightersSpeed[1][i];
        }
        if (lightersPos[1][i] >= (_h - 1) * 10) {
          lightersPos[1][i] = (_w - 1) * 10;
          lightersSpeed[1][i] = -lightersSpeed[1][i];
        }
        _leds[XY(_w, _h, lightersPos[0][i] / 10, lightersPos[1][i] / 10)] =  lightersColor[i];
      }
    }  

public:
    Lighters(CRGB* leds, uint8_t w, uint8_t h)
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


class Matrix : public IEffect
{
private:
    uint8_t _scale = 40;

    void matrixRoutine()
    {
      for (int x = 0; x < _w; x++) {
        // заполняем случайно верхнюю строку
        CRGB thisColor = _leds[XY(_w, _h, x, 0)];
        //println(binary(thisColor.getHEX()));
        if (getHEX(thisColor) == 0)
        {
          //println("Hex 0");
          if (int(random(-1, _scale)) == 0)
          {
            _leds[XY(_w, _h, x, 0)] = CRGB(65280);
          } else
          {
            _leds[XY(_w, _h, x, 0)] = CRGB(0);
          }
        } else if (getHEX(thisColor) < 8192)
        {
          //println("Hex < 8192");
          _leds[XY(_w, _h, x, 0)] = CRGB(0);
        } else
        {
          //println("else");
          _leds[XY(_w, _h, x, 0)] = CRGB(getHEX(thisColor) - 8192);
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
    Matrix(CRGB* leds, uint8_t w, uint8_t h)
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

class RainbowVertical : public IEffect
{
private:
    int hue = 0;
    uint8_t _scale = 40;

    void rainbowVertical() {
        hue += 2;
        for (byte j = 0; j < _h; j++) {
            CHSV thisColor = CHSV(hue + j * _scale, 255, 255);
            for (byte i = 0; i < _w; i++){
                _leds[XY(_w, _h, i, j)] = thisColor;
            }
        }
    }
public:
    RainbowVertical(CRGB* leds, uint8_t w, uint8_t h)
    {
        _leds = leds;
        _w = w;
        _h = h;
    }

    void show()
    {
        static uint32_t timer = 0;
        if(millis() - timer >= RAINBOW_SHOW_PERIOD)
        {
            timer = millis();
            this->rainbowVertical();
        }
    }
};

class RainbowHorizontal : public IEffect
{
private:
    int hue = 0;
    uint8_t _scale = 40;

    void rainbowHorizontal() {
        hue += 2;
        for (byte i = 0; i < _w; i++) {
            CHSV thisColor = CHSV((hue + i * _scale), 255, 255);
            for (byte j = 0; j < _h; j++){
                _leds[XY(_w, _h, i, j)] = thisColor;   //leds[getPixelNumber(i, j)] = thisColor;
            }
        }
    }
public:
    RainbowHorizontal(CRGB* leds, uint8_t w, uint8_t h)
    {
        _leds = leds;
        _w = w;
        _h = h;
    }

    void show()
    {
        static uint32_t timer = 0;
        if(millis() - timer >= RAINBOW_SHOW_PERIOD)
        {
            timer = millis();
            this->rainbowHorizontal();
        }
    }
};

class Colors : public IEffect
{
private:
    int hue = 0;
    uint8_t _scale = 40;

    void colorsRoutine() {
        hue += _scale;
        for (int i = 0; i < _w * _h; i++) {
        _leds[i] = CHSV(hue, 255, 255);
    }
}
public:
    Colors(CRGB* leds, uint8_t w, uint8_t h)
    {
        _leds = leds;
        _w = w;
        _h = h;
    }

    void show()
    {
        static uint32_t timer = 0;
        if(millis() - timer >= COLORS_SHOW_PERIOD)
        {
            timer = millis();
            this->colorsRoutine();
        }
    }
};

class Text : public IEffect
{
private:
    String text;
    uint16_t _speed = 0;
    int _runX = 0; // it was unsigned...

    CRGB _letCol = CRGB(0xffffff), _backCol = CRGB(0x0);

    int x, y;

    void _drawLetter(char c, int x, int y)
    {
        int ic = c;
        for(int a = ((x < 0) ? -x:0); a < 5; a++)
        {
            for(int b = 0; b < 7; b++)
            {
            _leds[(XY(_w, _h, a + x, b + y))] = ((font5x7[ic * 5 + a] & (1 << b)) >> b == 1) ? _letCol : _backCol;
            }
        }
    }

    void _drawText()
    {
        for(int i = 0; i < _w * _h; i++)
        {
            _leds[i] = CRGB(0); 
        }

        for(int i = 0; i < text.length(); i++)
        {
            _drawLetter(text.charAt(i), i * 5 + i + _runX, 1);
        }
    }

public:
    Text(CRGB* leds, uint8_t w, uint8_t h, uint16_t speed = 0, String t = "")
    {
        _leds = leds;
        _w = w;
        _h = h;
        this->setText(t);
        this->setSpeed(speed);
    }

    void show() override
    {
        static uint32_t timer = 0;
        if(millis() - timer >= _speed && _speed != 0)
        {
            timer = millis();
            //_runX = (_runX-- * -1 >= text.length() * 5) ? text.length() + text.length() / 2 : _runX;            
            _runX--;
            if(_runX <= text.length() * -5)
            {
                _runX = runX;
            }
            
        }   
        this->_drawText();     
    }

    void setText(String t)
    {
        text = t;
        text += String("   ");
        Serial.println(text.length());
    }

    void setSpeed(uint16_t speed) // 0 - 1000, рекомендую от 5 до 15
    {
        _speed = (speed == 0) ? 0 : 1000/speed;
    }
};

class EffectFactory
{
public:
    EffectFactory(){}
    static IEffect *getEffect(EffectsName ef, CRGB *leds, const uint8_t w, const uint8_t h)
    {
        IEffect *efObj;
        switch(ef)
        {
        case CONFETTI:
            efObj = new Confetti(leds, w, h);
            break;
/*
        case OFF:
            efObj = new Off(leds, w, h);
            break;
*/
        case FIRE:
            efObj = new Fire(leds, w, h);
            break;

        case SNOWFALL:
            efObj = new SnowFall(leds, w, h);
            break;

        case LIGHTERS:
            efObj = new Lighters(leds, w, h);
            break;

        case MATRIX:
            efObj = new Matrix(leds, w, h);
            break;

        case COLORS:
            efObj = new Colors(leds, w, h);
            break;

        case RAINBOW_H:
            efObj = new RainbowHorizontal(leds, w, h);
            break;

        case RAINBOW_V:
            efObj = new RainbowVertical(leds, w, h);
            break;

        case TEXT:
            efObj = new Text(leds, w, h, 0U, String(""));
            break;
        }
        return efObj;
    }
};

#endif