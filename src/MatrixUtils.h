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

uint32_t getHEX(uint8_t r, uint8_t g, uint8_t b)
{
    uint32_t raw = 0;
    raw = r;
    raw <<= 8;
    raw |= g;
    raw <<= 8;
    raw |= b;

    return raw;
}

uint32_t getHEX(uint32_t hex)
{
    return hex;
}

uint32_t getHSV(int hue, int saturation, int value)
{
  long _r = 0, _g = 0, _b = 0;
    long val = value;
    long sat = saturation;

    long h = ((24 * hue / 17) / 60) % 6;
    long vmin = (long)val - val * sat / 255;
    long a = (long)val * sat / 255 * (hue * 24 / 17 % 60) / 60;
    long vinc = vmin + a;
    long vdec = val - a;
  switch (int(h)) {
  case 0: 
    _r = val; 
    _g = vinc; 
    _b = vmin; 
    break;
  case 1: 
    _r = vdec; 
    _g = val; 
    _b = vmin; 
    break;
  case 2: 
    _r = vmin; 
    _g = val; 
    _b = vinc; 
    break;
  case 3: 
    _r = vmin; 
    _g = vdec; 
    _b = val; 
    break;
  case 4: 
    _r = vinc; 
    _g = vmin; 
    _b = val; 
    break;
  case 5: 
    _r = val; 
    _g = vmin; 
    _b = vdec; 
    break;
  }  
  return getHEX(_r, _g, _b);
}

uint8_t XY (uint8_t _w, uint8_t _h, uint8_t x, uint8_t y)
{
  if(x < _w && y < _h)
  {
      return ((y % 2 == 0) ? y * _w + _w - x - 1:(y * _w) + x);
  }
  //Serial.println("Out of bounds");
  //return (_h * _w - 1);
  return 0;    
}
#endif