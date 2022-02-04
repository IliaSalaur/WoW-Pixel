#ifndef DEBUG_WOWPIXEL_H
#define DEBUG_WOWPIXEL_H

//#define DEBUG_WOWPIXEL

#ifdef DEBUG_WOWPIXEL
#define DEBUG(x) Serial.println(x);
#else 
#define DEBUG(x)
#endif

#endif