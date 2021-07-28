#ifndef XYFUNC_H
#define XYFUNC_H

#define SERPENTINE_LAYOUT 1
/*
1:
    X-
 Y  0- 1- 2 - 3
 |            |
    7- 6- 5 - 4
    |
    8- 9- 10-11
              |
    15-14-13-12

0:
    0- 1- 2- 3
             |
    ----------
    |
    4- 5- 6- 7       
*/

#define V_FLIP 0
/*
1:
    12-13-14-15
    8- 9- 10-11

0:
    0-1-2-3
    4-5-6-7
*/
#define H_FLIP 1
/*
1:
    3-2-1-0
    7-6-5-4

0:
    0-1-2-3
    4-5-6-7
*/

#include <Arduino.h>

uint8_t XY (uint8_t _w, uint8_t _h, uint8_t x, uint8_t y)
{
    if(x < _w && y < _h)
    {
        return ((y % 2 == 0) ? y * _w + _w - x - 1:(y * _w) + x);
    }
    //Serial.println("Out of bounds");
    return (_h * _w - 1);
}
#endif