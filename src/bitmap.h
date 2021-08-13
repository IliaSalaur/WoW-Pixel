#ifndef BITMAP_H
#define BITMAP_H
#include <MatrixUtils.h>
#include <pgmspace.h>

#define BITMAP_SIZE 64
const uint32_t monsterBitmap[] = 
{
    0,  0,   getHEX(255, 180, 252),  0,  0,  0,  0,   getHEX(255, 180, 252),  0,   getHEX(255, 180, 252),  0,  0,  0,  0,   getHEX(255, 180, 252),  0,  0,   getHEX(255, 180, 252),   getHEX(255, 120, 245),   getHEX(255, 120, 245),   getHEX(255, 120, 245),   getHEX(255, 120, 245),   getHEX(255, 180, 252),  0,   getHEX(255, 120, 245),   getHEX(255, 120, 245),   getHEX(255, 120, 245),   getHEX(255, 120, 245),   getHEX(255, 120, 245),   getHEX(255, 120, 245),   getHEX(255, 120, 245),   getHEX(255, 120, 245),   getHEX(255, 120, 245),  0,  0,   getHEX(255, 120, 245),   getHEX(255, 120, 245),  0,  0,   getHEX(255, 120, 245),   getHEX(233, 0, 214),   getHEX(233, 0, 214),   getHEX(233, 0, 214),   getHEX(233, 0, 214),   getHEX(233, 0, 214),   getHEX(233, 0, 214),   getHEX(233, 0, 214),   getHEX(233, 0, 214),   getHEX(233, 0, 214),   getHEX(233, 0, 214),   getHEX(233, 0, 214),   getHEX(233, 0, 214),   getHEX(233, 0, 214),   getHEX(233, 0, 214),   getHEX(233, 0, 214),   getHEX(233, 0, 214),  0,   getHEX(233, 0, 214),  0,  0,  0,  0,   getHEX(233, 0, 214),  0, 
};

const uint32_t wifiBitmap[] = 
{
    0,  0,  0,  0,  0,  0,  0,  0,  0,  0,   getHEX(74, 132, 215),   getHEX(74, 132, 215),   getHEX(74, 132, 215),   getHEX(74, 132, 215),  0,  0,  0,   getHEX(74, 132, 215),   getHEX(74, 132, 215),   getHEX(74, 132, 215),   getHEX(74, 132, 215),   getHEX(74, 132, 215),   getHEX(74, 132, 215),  0,   getHEX(74, 132, 215),  0,  0,  0,  0,  0,  0,   getHEX(74, 132, 215),  0,  0,   getHEX(74, 132, 215),   getHEX(74, 132, 215),   getHEX(74, 132, 215),   getHEX(74, 132, 215),  0,  0,  0,   getHEX(74, 132, 215),  0,  0,  0,  0,   getHEX(74, 132, 215),  0,  0,  0,  0,   getHEX(74, 132, 215),   getHEX(74, 132, 215),  0,  0,  0,  0,  0,  0,   getHEX(74, 132, 215),   getHEX(74, 132, 215),  0,  0,  0, 
};

const uint32_t okBitmap[] = 
{   
    getHEX(0, 255, 0),   getHEX(0, 255, 0),  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,   getHEX(0, 255, 0),   getHEX(0, 255, 0),   getHEX(0, 255, 0),   getHEX(0, 255, 0),   getHEX(0, 255, 0),  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,   getHEX(0, 255, 0),   getHEX(0, 255, 0),  0,  0,  0,   getHEX(0, 255, 0),   getHEX(0, 255, 0),  0,  0,   getHEX(0, 255, 0),   getHEX(0, 255, 0),   getHEX(0, 255, 0),   getHEX(0, 255, 0),   getHEX(0, 255, 0),   getHEX(0, 255, 0),   getHEX(0, 255, 0),  0,  0,  0,  0,  0,  0,   getHEX(0, 255, 0),   getHEX(0, 255, 0),   getHEX(0, 255, 0),   getHEX(0, 255, 0),  0,  0,  0,   getHEX(0, 255, 0),   getHEX(0, 255, 0),  0,  0,  0,  0, 
};
#endif