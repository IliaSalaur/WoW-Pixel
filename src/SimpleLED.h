#ifndef SIMPLELED
#define SIMPLELED

#include <Adafruit_NeoPixel.h>
#include <Effects.h>
#include <VirtualMatrix.h>

#define DEBUG_MATRIX
#define ASYNC_MODE

template <uint8_t _width, uint8_t _height, int pin>
class SimpleLED
{
private:
    uint32_t _leds[_width * _height];
    IEffect* _ef;
    Adafruit_NeoPixel *_matrix;

    void _setPixels()
    {
        for(int i = 0; i < _width * _height; i++)
        {
            _matrix->setPixelColor(i, _leds[i]);
        }
    }
public:

    SimpleLED()
    {
        _matrix = new Adafruit_NeoPixel(_width * _height, pin, NEO_GRB + NEO_KHZ800);
    }

    void begin()
    {
        _matrix->begin();
        _matrix->clear();
    }

    void clear()
    {
        _matrix->clear();
        for(uint16_t i = 0; i < _width * _height; i++)
        {
            _leds[i] = 0;
        }
    }

    void drawPixel(int n, uint32_t color)
    {
        if(n < _width * _height)
        {
            _leds[n] = color;
            _matrix->setPixelColor(n, color);
            #ifndef ASYNC_MODE
            _matrix->show();
            #endif
        }
        _ef = nullptr;
    }

    void drawPixelXY(int x, int y, uint32_t color)
    {
        this->drawPixel(XY(_width, _height, x, y), color);
    }

    void handle()
    {
        if(_ef) 
        {
            _ef->show();                    
        }
            this->_setPixels();
        #ifdef DEBUG_MATRIX
            emulateLeds(_width, _height, _leds);
            //#error Virtual matrix active, switch it off
        #else
            //_matrix.show();
            _matrix->show();
        #endif
    }

    void setEffect(EffectsName ef)
    {
        //_ef = EffectFactory::getEffect(ef, _leds, 8, 8);
        _matrix->clear();
        this->clear();
        _ef = EffectFactory::getEffect(ef, _leds, _width, _height);//       Старая версия с использованием Enum и EffectFactory
        //_ef = ef;
        
    }

    void setEffect(IEffect * ef)
    {
        _ef = ef;
        _ef->setLeds(_leds);
        _ef->setWH(_width, _height);
    }

    template<uint8_t _bitmapW, uint8_t _bitmapH>
    void drawBitmap(uint32_t bitmap[_bitmapW][_bitmapH], uint8_t s_x, uint8_t s_y)
    {
        this->clear();
        for(int x = 0; x < _bitmapW; x++)
        {
            for(int y = 0; y < _bitmapH; y++)
            {
                _leds[XY(_width, _height, x + s_x, y + s_y)] = bitmap[x][y];
            }
        } 
        _matrix->show();
        _ef = nullptr;
    }

    void setBrightness(uint8_t brig)
    {
        _matrix->setBrightness(brig);
    }
/*
    void drawText(String text, uint16_t speed, int scrollTimes = 0, uint32_t lcol = 0xffffff, uint32_t bcol = 0)
    {
        _ef = new Text(_leds, _width, _height, speed, scrollTimes, text, lcol, bcol);
    }*/
};


#endif 