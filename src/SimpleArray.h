#ifndef SIMPLEARRAY_H
#define SIMPLEARRAY_H
#include <Arduino.h>
/*
Итак, в этом классе мы пере изобретём массивы.
Самая большая проблема массива это невозможность его объявления при использовании не константных аргументов.
Короче, при помощи переменной объявить массив не получится.
По этому, сделаем динамическое выделение памяти при помощи malloc
*/

template<typename T>
class SimpleArrayBuilder
{
private:
    T *_arr;

public:
    SimpleArrayBuilder(size_t size)
    {
        _arr = (T*) malloc(size * sizeof(T));
    }

    T* build()
    {
        Serial.println("Array builded");
        return _arr;
    }
};

template<typename T>
class Simple2dArrayBuilder // То что творится в этом классе лучше никогда не видеть, поймут это только тру с кодеры
{
private:
    T **_arr;

public:
    Simple2dArrayBuilder(size_t _w, size_t _h)
    {
        _arr = (T**)malloc(_w * sizeof(T*));

        for(int d2 = 0; d2 < _h; d2++)
        {
            _arr[d2] = (T*)malloc(_h * sizeof(T));
        }
        
    }

    T** build()
    {
        Serial.println("3dArray builded");
        return _arr;
    }
};

template<typename T>
class Simple3dArrayBuilder // То что творится в этом классе лучше никогда не видеть, поймут это только тру с кодеры
{
private:
    T ***_arr;

public:
    Simple3dArrayBuilder(size_t _w, size_t _h, size_t _d)
    {
        _arr = (T***)malloc(_w * sizeof(T**));
        for(int d1 = 0; d1 < _w; d1++)
        {
            _arr[d1] = (T**) malloc(_h * sizeof(T*));
            for(int d2 = 0; d2 < _h; d2++)
            {
                _arr[d1][d2] = (T*)malloc(_d * sizeof(T));
            }
        }
    }

    T*** build()
    {
        Serial.println("3dArray builded");
        return _arr;
    }
};

#endif