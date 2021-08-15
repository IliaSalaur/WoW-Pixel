#pragma once
#define DEBUG

#ifdef DEBUG
#define DEBUG(x) Serial.println(x);
#else 
#define DEBUG(x)
#endif