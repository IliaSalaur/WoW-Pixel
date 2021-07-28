#ifndef SIMPLELED
#define SIMPLELED

#define EFFECTS_NUM 2

#define EMULATE8x8

#include <FastLED.h>
#include <Effects.h>
#include <Fonts.h>
//#include <SimpleArray.h>

template <uint8_t _width, uint8_t _height, int pin>
class SimpleLED
{
private:
    CRGB _leds[_width * _height];
    IEffect* _ef;
public:

    SimpleLED()
    {
        FastLED.addLeds<WS2812B, pin, GRB>(_leds, _width * _height);
        FastLED.setCorrection(LEDColorCorrection::TypicalLEDStrip); //pixelstring    
        FastLED.setBrightness(100);   
    }

    void begin()
    {
        FastLED.clear();
        delay(1500);
        FastLED.show();
    }

    void clear()
    {
        FastLED.clear();
        for(uint16_t i = 0; i < _width * _height; i++)
        {
            _leds[i] = CRGB(0);
        }
    }

    void drawPixel(int n, CRGB color)
    {
        if(n < _width * _height)
        {
            _leds[n] = color;
            FastLED.show();
        }
        _ef = nullptr;
    }

    void drawPixelXY(int x, int y, CRGB color)
    {
        if(x * y < _width * _height)
        {
            _leds[XY(_width, _height, x, y)] = color;
            FastLED.show();
        }
        _ef = nullptr;
    }

    void handle()
    {
        if(_ef) 
        {
            _ef->show();
            FastLED.show();
        }
    }

    void setEffect(EffectsName ef)
    {
        //_ef = EffectFactory::getEffect(ef, _leds, 8, 8);
        _ef = EffectFactory::getEffect(ef, _leds, _width, _height);//       Старая версия с использованием Enum и EffectFactory
        //_ef = ef;
        this->clear();
    }

    template<uint8_t _bitmapW, uint8_t _bitmapH>
    void drawBitmap(CRGB bitmap[_bitmapW][_bitmapH], uint8_t s_x, uint8_t s_y)
    {
        this->clear();
        //CRGB (*bitmap)[w][h] = (CRGB (*)[w][h]) pBitmap;//Кастим void* к CRGB *b[][]. Очень костыльный костыль который толком не завелся, так что юзаем template и не паримся
        for(int x = 0; x < _bitmapW; x++)
        {
            for(int y = 0; y < _bitmapH; y++)
            {
                _leds[XY(_width, _height, x + s_x, y + s_y)] = bitmap[x][y];
            }
        } 
        FastLED.show();
        _ef = nullptr;
    }

    void drawText(String text, uint16_t speed)
    {
        _ef = new Text(_leds, _width, _height, speed, text);
    }
};


#endif 