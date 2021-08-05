#ifndef SIMPLELED
#define SIMPLELED

#define EFFECTS_NUM 2

#define EMULATE8x8

#include <FastLED.h>
#include <Effects.h>
#include <Fonts.h>
//#include <SimpleArray.h>
#include <VirtualMatrix.h>

#define DEBUG_MATRIX
#define ASYNC_MODE

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
        delay(200);
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
            #ifndef ASYNC_MODE
            FastLED.show();
            #endif
        }
        _ef = nullptr;
    }

    void drawPixelXY(int x, int y, CRGB color)
    {
        this->drawPixel(XY(_width, _height, x, y), color);
    }

    void handle()
    {
        if(_ef) 
        {
            _ef->show();                    
        }
        #ifdef DEBUG_MATRIX
            emulateLeds(_width, _height, _leds);
            //#error Virtual matrix active, switch it off
        #else
            
            FastLED.show();
        #endif
    }

    void setEffect(EffectsName ef)
    {
        //_ef = EffectFactory::getEffect(ef, _leds, 8, 8);
        FastLED.clear();
        this->clear();
        _ef = EffectFactory::getEffect(ef, _leds, _width, _height);//       Старая версия с использованием Enum и EffectFactory
        //_ef = ef;
        
    }

    void setEffect(IEffect * ef)
    {
        _ef = ef;
    }

    template<uint8_t _bitmapW, uint8_t _bitmapH>
    void drawBitmap(CRGB bitmap[_bitmapW][_bitmapH], uint8_t s_x, uint8_t s_y)
    {
        this->clear();
        //CRGB (*bitmap)[w][h] = (CRGB (*)[w][h]) pBitmap;//Кастим void* к CRGB *b[][]. Очень костыльный костыль который толком не завелся, так что юзаем template и не паримся. Вообще хотел использовать SimpleArray но, не сегодня 
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

    void drawText(String text, uint16_t speed, int scrollTimes = 0, CRGB lcol = CRGB(0xffffff), CRGB bcol = CRGB(0))
    {
        _ef = new Text(_leds, _width, _height, speed, scrollTimes, text, lcol, bcol);
    }
};


#endif 