#ifndef FIREEF_UNUSED_H
#define FIREEF_UNUSED_H

/*

class Fire : public IEffect // хоть у нас и так есть _w и _h придется юзать template. Добро пожаловать в с++ так сказать
{
private:
    uint8_t _centreX;
    uint8_t _centreY;

    void Fire2018_2() {
        static uint32_t x[NUM_LAYERS], y[NUM_LAYERS], z[NUM_LAYERS], scale_x[NUM_LAYERS], scale_y[NUM_LAYERS];
        //static uint8_t noise[NUM_LAYERS][_w][_h], heat[_h * _w] ;// Вот тут компилсятор жалуется на 'declaration of array with non-constant чёта там...'. По этому template
        static uint8_t ***noise = Simple3dArrayBuilder<uint8_t>(NUM_LAYERS, _w, _h).build();
        static uint8_t *heat = SimpleArrayBuilder<uint8_t>(_w * _h).build();
        uint16_t ctrl1 = inoise16(11 * millis(), 0, 0);
        uint16_t ctrl2 = inoise16(13 * millis(), 100000, 100000);
        uint16_t  ctrl = ((ctrl1 + ctrl2) / 2);
        
        uint16_t speed = 25;
        x[0] = 3 * ctrl * speed;
        y[0] = 20 * millis() * speed;
        z[0] = 5 * millis() * speed ;
        scale_x[0] = ctrl1 / 2;
        scale_y[0] = ctrl2 / 2;

        uint8_t layer = 0;
        for (uint8_t i = 0; i < _w; i++) {
            uint32_t ioffset = scale_x[layer] * (i - _centreX);
            for (uint8_t j = 0; j < _h; j++) {
            uint32_t joffset = scale_y[layer] * (j - _centreY);
            uint16_t data = ((inoise16(x[layer] + ioffset, y[layer] + joffset, z[layer])) + 1);
            noise[layer][i][j] = data >> 8;
            
            }
        }

        speed = 20;
        x[1] = 3 * ctrl * speed;
        y[1] = 20 * millis() * speed;
        z[1] = 5 * millis() * speed ;
        scale_x[1] = ctrl1 / 2;
        scale_y[1] = ctrl2 / 2;

        layer = 1;
        for (uint8_t i = 0; i < _w; i++) {
            uint32_t ioffset = scale_x[layer] * (i - _centreX);
            for (uint8_t j = 0; j < _h; j++) {
            uint32_t joffset = scale_y[layer] * (j - _centreY);
            uint16_t data = ((inoise16(x[layer] + ioffset, y[layer] + joffset, z[layer])) + 1);
            noise[layer][i][j] = data >> 8;
            }
        }

        for (uint8_t x = 0; x < _w; x++) {
            //heat[XY(_w, _h, x, 15)] =  noise[0][15 - x][7];
            heat[XY(_w, _h, x, _h-1)] = noise[0][_h - 1 - x][_centreY];
            
        }


        for (uint8_t y = 0; y < _h - 1; y++) {
            for (uint8_t x = 0; x < _w; x++) {
            heat[XY(_w, _h, x, y)] = heat[XY(_w, _h, x, y + 1)];
            }
        }

        //dim
        for (uint8_t y = 0; y < _h - 1; y++) {
            for (uint8_t x = 0; x < _w; x++) {
            uint8_t dim = noise[0][x][y];
            // high value = high flames
            dim = dim / 1.7;
            dim = 255 - dim;
            heat[XY(_w, _h, x, y)] = scale8(heat[XY(_w, _h, x, y)] , dim);
            }
        }

        for (uint8_t y = 0; y < _h; y++) {
            for (uint8_t x = 0; x < _w; x++) {
            // map the colors based on heatmap
            *(_leds + XY(_w, _h, x, y)) = CRGB( heat[XY(_w, _h, x, y)], 1 , 0);
            // dim the result based on 2nd noise layer
            (_leds + XY(_w, _h, x, y))->nscale8(noise[1][x][y]);
           }
        }
    }
public:
    Fire(CRGB *leds, uint8_t w, uint8_t h)
    {
        _leds = leds;
        _leds_num = w * h;
        _w = w;
        _h = h;
        _centreX = (_w / 2) - 1;
        _centreY = (_h / 2)- 1;
        uint16_t entropy = random(63999);
        random16_add_entropy(entropy);
    }

    void show() override
    {
        this->Fire2018_2();
    }
};

*/
#endif