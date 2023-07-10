#include <Arduino.h>
#include "config.hpp"
struct temperature_t;

typedef struct temperature_t
{
    float tempBuf;
    short tick;
    int avgTemp;
    int init()
    {
      tick = 0;
	    tempBuf = 0;
	    tick = 0;
	    avgTemp = 0;
      return 0;
    };
} Temperature;

float TmpGetTemperature();
int TmpGetAvgTemperature();
int TmpInit();
