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

public:
    static T* build(size_t size)
    { 
        T* _arr;
        _arr = (T*) malloc(size * sizeof(T));
        //Serial.println("Array builded");
        return _arr;
    }

    static void stop(T* _arr, size_t size)
    {
        free(_arr);
    }
};

template<typename T>
class Simple2dArrayBuilder // То что творится в этом классе лучше никогда не видеть, поймут это только тру C кодеры
{
private:

public:
    Simple2dArrayBuilder(){}
    
    static T** build(size_t _w, size_t _h)
    {
        T** _arr;
        _arr = (T**)malloc(_w * sizeof(T*));

        for(size_t d2 = 0; d2 < _h; d2++)
        {
            _arr[d2] = (T*)malloc(_h * sizeof(T));
        }
        ////Serial.println("2dArray builded");
        return _arr;
    }
    //template<typename T>
    static void stop(T** arr, size_t _w, size_t _h)
    {
        for(size_t d2 = 0; d2 < _h; d2++)
        {
            free(arr[d2]);
        }
        free(arr);
    }
};

template<typename T>
class Simple3dArrayBuilder // То что творится в этом классе лучше никогда не видеть, поймут это только тру с кодеры
{
private:

public:
    Simple3dArrayBuilder(){}

    static T*** build(size_t _w, size_t _h, size_t _d)
    {
        T ***_arr;
        //Serial.println("3dArray builded");
        _arr = (T***)malloc(_w * sizeof(T**));
        for(int d1 = 0; d1 < _w; d1++)
        {
            _arr[d1] = (T**) malloc(_h * sizeof(T*));
            for(int d2 = 0; d2 < _h; d2++)
            {
                _arr[d1][d2] = (T*)malloc(_d * sizeof(T));
            }
        }
        return _arr;
    }

    static void stop(T*** _arr, size_t w, size_t h, size_t d)
    {
        for(int d1 = 0; d1 < w; d1++)
        {                        
            for(int d2 = 0; d2 < h; d2++)
            {
                free(_arr[d1][d2]);
            }
            free(_arr[d1]);
        }
        free(_arr);
    }
};

#endif