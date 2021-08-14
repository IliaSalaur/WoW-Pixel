#ifndef BITMAP_H
#define BITMAP_H
#include <MatrixUtils.h>
#include <pgmspace.h>

#define BITMAP_SIZE 64
const uint32_t monsterBitmap[8][8] = {
    {getHEX(0),getHEX(0),getHEX(0),getHEX(218,97,228),getHEX(214,99,226),getHEX(214,44,228),getHEX(214,39,228),getHEX(0),},
    {getHEX(0),getHEX(220,150,236),getHEX(223,151,237),getHEX(216,97,225),getHEX(0),getHEX(214,49,229),getHEX(218,41,235),getHEX(220,33,236),},
    {getHEX(215,150,230),getHEX(0),getHEX(212,102,223),getHEX(213,97,224),getHEX(0),getHEX(216,48,231),getHEX(216,39,229),getHEX(0),},
    {getHEX(0),getHEX(0),getHEX(215,98,228),getHEX(213,98,225),getHEX(215,99,226),getHEX(215,42,230),getHEX(217,35,232),getHEX(0),},
    {getHEX(0),getHEX(0),getHEX(214,99,226),getHEX(216,96,229),getHEX(213,102,223),getHEX(214,41,229),getHEX(215,35,230),getHEX(0),},
    {getHEX(0),getHEX(0),getHEX(215,105,226),getHEX(214,95,225),getHEX(0),getHEX(213,43,228),getHEX(217,35,232),getHEX(0),},
    {getHEX(0),getHEX(223,151,237),getHEX(222,150,236),getHEX(215,99,226),getHEX(0),getHEX(212,49,226),getHEX(211,46,228),getHEX(211,31,226),},
    {getHEX(221,152,235),getHEX(0),getHEX(0),getHEX(216,97,227),getHEX(217,96,227),getHEX(214,45,226),getHEX(213,36,230),getHEX(0),},
};

const uint32_t wifiBitmap[8][8] = {
    {getHEX(0),getHEX(0),getHEX(0),getHEX(0,115,254),getHEX(0),getHEX(0),getHEX(0),getHEX(0),},
    {getHEX(0),getHEX(0),getHEX(0,114,255),getHEX(0),getHEX(0),getHEX(1,112,253),getHEX(0),getHEX(0),},
    {getHEX(0),getHEX(1,104,235),getHEX(0,114,251),getHEX(0),getHEX(2,114,252),getHEX(0),getHEX(0),getHEX(0),},
    {getHEX(0),getHEX(1,105,236),getHEX(0,111,252),getHEX(0),getHEX(0,112,247),getHEX(0),getHEX(2,112,249),getHEX(1,112,253),},
    {getHEX(0),getHEX(1,108,240),getHEX(0,111,252),getHEX(0),getHEX(1,114,255),getHEX(0),getHEX(3,110,250),getHEX(2,113,255),},
    {getHEX(0),getHEX(1,108,238),getHEX(0,114,249),getHEX(0),getHEX(2,113,255),getHEX(0),getHEX(0),getHEX(0),},
    {getHEX(0),getHEX(0),getHEX(1,114,252),getHEX(0),getHEX(0),getHEX(0,110,247),getHEX(0),getHEX(0),},
    {getHEX(0),getHEX(0),getHEX(0),getHEX(0,112,255),getHEX(0),getHEX(0),getHEX(0),getHEX(0),},
};

const uint32_t okBitmap[8][8] = {   
    {getHEX(0),getHEX(0),getHEX(0),getHEX(0),getHEX(76,234,0),getHEX(80,236,2),getHEX(0),getHEX(0),},
    {getHEX(0),getHEX(0),getHEX(0),getHEX(0),getHEX(73,236,7),getHEX(71,238,2),getHEX(78,236,1),getHEX(0),},
    {getHEX(0),getHEX(0),getHEX(0),getHEX(0),getHEX(0),getHEX(75,236,0),getHEX(73,236,5),getHEX(77,238,2),},
    {getHEX(0),getHEX(0),getHEX(0),getHEX(0),getHEX(0),getHEX(75,237,3),getHEX(78,236,1),getHEX(79,236,0),},
    {getHEX(0),getHEX(0),getHEX(0),getHEX(0),getHEX(76,237,1),getHEX(78,236,1),getHEX(78,237,0),getHEX(0),},
    {getHEX(0),getHEX(0),getHEX(75,235,3),getHEX(73,235,4),getHEX(75,236,2),getHEX(0),getHEX(0),getHEX(0),},
    {getHEX(79,235,4),getHEX(73,236,7),getHEX(78,236,3),getHEX(74,235,1),getHEX(0),getHEX(0),getHEX(0),getHEX(0),},
    {getHEX(76,237,0),getHEX(76,237,1),getHEX(77,232,4),getHEX(0),getHEX(0),getHEX(0),getHEX(0),getHEX(0),},
};

#endif