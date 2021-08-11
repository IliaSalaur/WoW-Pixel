#ifndef MYEFFECTS_H
#define MYEFFECTS_H

//#include <ArduinoTrace.h>
#include <Adafruit_NeoPixel.h>
#include <MatrixUtils.h>
#include <EffectsConfig.h>
#include <Fonts.h>

//#define CRGB
//int sp = 2;

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
    uint32_t *_leds = nullptr;
    uint16_t _leds_num = 0;

    uint8_t _w = 0;
    uint8_t _h = 0;
    //Adafruit_NeoPixel colUtils;
public:
    virtual void show() = 0;

    void setLeds(uint32_t * leds)
    {
        _leds = leds;
    }

    void setWH(uint8_t w, uint8_t h)
    {
        _w = w;
        _h = h;
    }
};

 
class Confetti : public IEffect  
{
private:
    uint32_t _timer = 0;
public:
    Confetti(uint32_t *leds, uint8_t w, uint8_t h)
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
                *(_leds + a) = getHEX(random(255), random(255), random(255));            
            }
        }
    }
};

 
class Off : public IEffect   
{
public:
    Off(uint32_t *leds, uint8_t w, uint8_t h)
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
            *(_leds + i) = 0;
        }
        
    }
};
 
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

      for (uint8_t x = 0; x < _w; x++) {
        // заполняем случайно верхнюю строку
        // а также не даём двум блокам по вертикали вместе быть
        if (getHEX(_leds[XY(_w, _h, x, _h - 2)]) == 0 && (random(0, _scale) == 0))
          _leds[XY(_w, _h, x, testf)] = getHEX(14745599 - 1052688 * random(0, 4));
        else
          _leds[XY(_w, _h, x, testf)] = getHEX(0);
      }
    }

public:
    SnowFall(uint32_t* leds, uint8_t w, uint8_t h)
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

 
class Matrix : public IEffect   
{
private:
    uint8_t _scale = 40;

    void matrixRoutine()
    {
      for (int x = 0; x < _w; x++) {
        // заполняем случайно верхнюю строку
        uint32_t thisColor = _leds[XY(_w, _h, x, 0)];
        //println(binary(thisColor.getHEX())); -- кусок кода с Processing
        if (thisColor == 0)
        {
          //println("Hex 0");
          if (int(random(-1, _scale)) == 0)
          {
            _leds[XY(_w, _h, x, 0)] = 65280;
          } else
          {
            _leds[XY(_w, _h, x, 0)] = 0;
          }
        } else if (thisColor < 8192)
        {
          //println("Hex < 8192");
          _leds[XY(_w, _h, x, 0)] = 0;
        } else
        {
          //println("else");
          _leds[XY(_w, _h, x, 0)] = thisColor - 8192;
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
    Matrix(uint32_t* leds, uint8_t w, uint8_t h)
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
        for (uint8_t j = 0; j < _h; j++) {
            uint32_t thisColor = getHSV(hue + j * _scale, 255, 255);
            for (uint8_t i = 0; i < _w; i++){
                _leds[XY(_w, _h, i, j)] = thisColor;
            }
        }
    }
public:
    RainbowVertical(uint32_t* leds, uint8_t w, uint8_t h)
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
        for (uint8_t i = 0; i < _w; i++) {
            uint32_t thisColor = getHSV((hue + i * _scale), 255, 255);
            for (uint8_t j = 0; j < _h; j++){
                _leds[XY(_w, _h, i, j)] = thisColor;   //leds[getPixelNumber(i, j)] = thisColor;
            }
        }
    }
public:
    RainbowHorizontal(uint32_t* leds, uint8_t w, uint8_t h)
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
        _leds[i] = getHSV(hue, 255, 255);
    }
}
public:
    Colors(uint32_t* leds, uint8_t w, uint8_t h)
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
        case 15:
            //ic = (c >= 48 && c <= 57) ?  c - 48 : c;
            for(int a = ((x < 0) ? -x:0); a < 3; a++)
            {
                for(int b = 0; b < 5; b++)
                {
                    this->_setLeds(a + x, ((5 - b) + y),  ((font3x5Digits.getBytes(ic)[a] & (1 << b)) >> b == 1) ? _letCol : _backCol);
                    //_leds[(XY(_w, _h, a + x, (5 - b) + y))] = ((font3x5Digits.getBytes(ic)[a] & (1 << b)) >> b == 1) ? _letCol : _backCol;                    
                }
            }
            break;
        
        case 35:
            if(ic >= 208)
            {
                for(int a = ((x < 0) ? -x:0); a < 5; a++)
                {
                    for(int b = 0; b < 7; b++)
                    {
                        //_leds[(XY(_w, _h, a + x, b + y))] = ((font5x7Rus.getBytes(ic)[a] & (1 << b)) >> b == 1) ? _letCol : _backCol;
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
                        //_leds[(XY(_w, _h, a + x, b + y))] = ((font5x7.getBytes(ic)[a] & (1 << b)) >> b == 1) ? _letCol : _backCol;
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
                
                //_drawLetter(charIndex, j * 5 + j + _runX, 1, (_w * _h == 64) ? 3*5 : 5*7);    //(i + 1 < len && (charIndex >= 48 && charIndex <= 57) && (text[i + 1] >= 48 && text[i + 1] <= 57))
                //_drawLetter(charIndex, j * ((smallSpace) ? 1:5) + j + _runX, 1, (_w * _h == 64) ? 3*5 : 5*7);
                _drawLetter(charIndex, lastX + _runX, _y, (_w * _h == 64) ? 5*7 : 5*7);//3*5:5*7
                lastX += (smallSpace) ? 4:6;
                i += (rusChar) ? 1 : 0;
            }
        }
    }

    void _centerText()
    {
        if(text != String(" ") && _scrollTimes == 0)
        {
            _runX = (_w - int(text.length()) * ((_onlyDigits) ? 3:5)) / 2 - ((_onlyDigits == 1) ? ((_w == 8) ? 2 : 2):1);//1:2

        }
        DEBUG(String("_runX setted: ") + String(_runX))
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
        text = String(" ");
    }

    void onlyDigits(bool od)
    {
        _onlyDigits = od;
    }

    void show() override
    {
        static uint32_t timer = 0;
        static uint32_t frameTimer = 0; 
        /*
        21:34: Честно я понятия не имею почему этот баг возникает. 
        В Processing все работает четко хотя код оттуда я просто взял и ctrl+c ctrl+v.
        Но, в процесинг все это дело выполнялось 51 раз в секунду, здесь в 1000 раз быстрее (наверное)
        Так что добавляем фреймТаймер и молимся (несмотря на то что я атеист, лол)

        22:28:Не сработало 


        Итак, сейчас 2 часа ночи и я фиксанул этот еб*** баг. 
        Почему на Java все было нормально понятно не только лишь всем, лол.
        Как я это сделал и в чем заключался баг: Проблема была в том что если это условие
        (_runX-- * -1 >= text.length() * 5) ? text.length() + text.length() / 2 : _runX;
        было true то больше оно нигокда не становилось false, и по этому оно дальше не крутилось.
        Пофиксил немного изменив условие (_runX <= int(text.length() * -5)) ? _w : _runX - 1;
        В таком виде оно после true дает false (я гений епт, ну реально)
        И в этом была проблема. Вообще у програмистов часто такое бывает. 
        Мой рекорд - 5 часов на поиск одного неправильного символа. И это сейчас я весь такой прокачаный, 
        сразу знаю из-за чего что-то не работает, (о том что проблема была в формуле я понял на середине нашего тестирования) 
        а тогда то я вообще масленком был, я уже чуть ли не плакал пока искал ту ошибку. Но зато, когда 
        фиксишь тот самый баг испытаваешь огромный прилив дофамина, окситоцина, серотонина и прочих гормонов которые
        стимулируют центр удовольствия в мозге. Уот так уот.
        
        */
        if(millis() - frameTimer >= 20)
        {
            frameTimer = millis();
            if(_scrollTimes != 0 && _scrollTimes != _scrollCount)
            {
                if(millis() - timer >= _speed && _speed != 0)
                {DEBUG("MOVE start")
                    timer = millis();
                    //_runX = (_runX-- * -1 >= text.length() * 5) ? text.length() + text.length() / 2 : _runX;
                    //_runX = (_runX <= int(text.length() * -5)) ? _w : _runX - 1; // --- это фикс
                    if(_runX <= int(text.length()) * -5)
                    {
                        _runX = _w;
                        _scrollCount++;
                        DEBUG("Runx reset")                        
                    }
                    else
                    {
                        //_runX = _runX - 1;
                        _runX--;
                    }
                    DEBUG(_runX);
                    DEBUG(_scrollCount);
                    DEBUG(_scrollTimes);
                    DEBUG("MOVE stop")
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
        //text += String("   ");
        this->_centerText();
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
        DEBUG(String("Scroll setted: ") + String(_runX))
    }

    ~Text()
    {
        text.~String();
    }
};

class EffectFactory
{
public:
    EffectFactory(){}
    static IEffect *getEffect(EffectsName ef, uint32_t *leds, uint8_t w, uint8_t h)
    {
        switch(ef)
        {
        case CONFETTI:
            return new Confetti(leds, w, h);
            break;
/*
        case OFF:
            return new Off(leds, w, h);
            break;
*/
        case FIRE:
            return new Fire(leds, w, h);
            break;

        case SNOWFALL:
            return new SnowFall(leds, w, h);
            break;

        case LIGHTERS:
            return new Lighters(leds, w, h);
            break;

        case MATRIX:
            return new Matrix(leds, w, h);
            break;

        case COLORS:
            return new Colors(leds, w, h);
            break;

        case RAINBOW_H:
            return new RainbowHorizontal(leds, w, h);
            break;

        case RAINBOW_V:
            return new RainbowVertical(leds, w, h);
            break;

        case TEXT:
            return nullptr;
            break;
        }
    }
};

#endif