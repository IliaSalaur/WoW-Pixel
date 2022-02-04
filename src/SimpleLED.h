#ifndef SIMPLELED
#define SIMPLELED

#include <Adafruit_NeoPixel.h>
#include <Effects/EffectsFactory.h>
#include <VirtualMatrix.h>


//#define DEBUG_CORRECTION
#define ASYNC_MODE

template <uint8_t _width, uint8_t _height, int pin>
class SimpleLED
{
private:
    uint32_t _leds[_width * _height];
    std::shared_ptr<IEffect> _ef;
    //IEffect* _ef;
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
        _ef.reset();
    }

    void begin()
    {
        _matrix->begin();
        _matrix->clear();
        #ifdef DEBUG_CORRECTION
        DEBUG(_matrix->rOffset)
        DEBUG(_matrix->gOffset)
        DEBUG(_matrix->bOffset)
        #endif
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
        _ef.reset();
    }

    void drawPixelXY(int x, int y, uint32_t color)
    {
        this->drawPixel(XY(_width, _height, x, y), color);
    }

    void handle()
    {
        static uint32_t fps_timer = 0;
        if(_ef) 
        {
            _ef->show();                    
        }
        if(millis() - fps_timer >= FPS_PERIOD)
        {
            this->_setPixels();
#ifdef DEBUG_MATRIX
            emulateLeds(_width, _height, _leds);
#warning (Virtual matrix active, switch it off)
#else            
            _matrix->show();
#endif
        }            
    }

    void setEffect(EffectsName ef)
    {
        _matrix->clear();
        this->clear();   
        _ef.reset(EffectFactory::getEffect(ef, _leds, _width, _height));//       Старая версия с использованием Enum и EffectFactory
        
    }

    void setEffect(std::shared_ptr<IEffect> ef)
    {
        _ef = ef;
        _ef->setLeds(_leds);
        _ef->setWH(_width, _height);
    }

    template<uint8_t _bitmapW, uint8_t _bitmapH>
    void drawBitmap(const uint32_t bitmap[_bitmapW][_bitmapH], uint8_t s_x, uint8_t s_y)
    {
        this->clear();
        for(int x = 0; x < _bitmapW; x++)
        {
            for(int y = 0; y < _bitmapH; y++)
            {
                _leds[XY(_width, _height, x + s_x, y + s_y)] = bitmap[x][y];
            }
        } 
        this->handle();
        _ef.reset();
    }

    template<uint8_t _bitmapW, uint8_t _bitmapH>
    void drawBitmap(const uint32_t bitmap[_bitmapW][_bitmapH])
    {
        int s_x = (_width -_bitmapW) / 2;
        int s_y = _height - _bitmapH;   
        this->clear();
        for(int x = 0; x < _bitmapW; x++)
        {
            for(int y = 0; y < _bitmapH; y++)
            {
                if(s_x + x < _width && s_y + y < _height)
                {
                    _leds[XY(_width, _height, x + s_x, y + s_y)] = bitmap[x][y];
                }                
            }
        } 
        this->handle();
        _ef.reset();
    }

    void drawBitmap(const uint32_t bitmap[64])
    {
        this->clear();
        DEBUG("drawing bitmap const")
        for(int i = 0; i < 64; i++)
        {
            _leds[i] = bitmap[i];
        }

        this->handle();
        _ef.reset();
    }

    void drawBitmap(uint32_t bitmap[64])
    {
        this->clear();
        DEBUG("drawing bitmap ")
        for(int i = 0; i < 64; i++)
        {
            _leds[i] = bitmap[i];
        }

        this->handle();
        _ef.reset();
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